#include "quickcontrolpanel.h"
#include "basicsettingspage.h"
#include "quickswitchbutton.h"
#include "vpn/vpncontrolpage.h"
#include "display/displaycontrolpage.h"
#include "wifi/wifipage.h"
#include "bluetooth/bluetoothlist.h"

#include "network/networkmodel.h"
#include "network/networkworker.h"

#include "display/displaymodel.h"
#include "display/displayworker.h"

#include "bluetooth/bluetoothmodel.h"
#include "bluetooth/bluetoothworker.h"

#include <QVBoxLayout>

using namespace dcc;
using dcc::network::NetworkModel;
using dcc::network::NetworkWorker;
using dcc::network::NetworkDevice;
using dcc::display::DisplayModel;
using dcc::display::DisplayWorker;
using dcc::bluetooth::BluetoothModel;
using dcc::bluetooth::BluetoothWorker;

QuickControlPanel::QuickControlPanel(QWidget *parent)
    : QWidget(parent),

      m_itemStack(new QStackedLayout)
{
    m_networkModel = new NetworkModel(this);
    m_networkWorker = new NetworkWorker(m_networkModel, this);

    m_displayModel = new DisplayModel(this);
    m_displayWorker = new DisplayWorker(m_displayModel, this);

    WifiPage *wifiPage = new WifiPage(m_networkModel);

    m_bluetoothModel = new BluetoothModel(this);
    m_bluetoothWorker = new BluetoothWorker(m_bluetoothModel);
    m_bluetoothWorker->activate();

    BluetoothList *bluetoothList = new BluetoothList(m_bluetoothModel);

    DisplayControlPage *displayPage = new DisplayControlPage(m_displayModel);

    VpnControlPage *vpnPage = new VpnControlPage(m_networkModel);

    m_itemStack->addWidget(new BasicSettingsPage);
    m_itemStack->addWidget(bluetoothList);
    m_itemStack->addWidget(vpnPage);
    m_itemStack->addWidget(wifiPage);
    m_itemStack->addWidget(displayPage);

    QuickSwitchButton *btSwitch = new QuickSwitchButton(1, "bluetooth");
    QuickSwitchButton *vpnSwitch = new QuickSwitchButton(2, "VPN");
    m_wifiSwitch = new QuickSwitchButton(3, "wifi");
    QuickSwitchButton *displaySwitch = new QuickSwitchButton(4, "display");
    QuickSwitchButton *detailSwitch = new QuickSwitchButton(0, "all_settings");

    btSwitch->setObjectName("QuickSwitchBluetooth");
    btSwitch->setAccessibleName("QuickSwitchBluetooth");
    vpnSwitch->setObjectName("QuickSwitchVPN");
    vpnSwitch->setAccessibleName("QuickSwitchVPN");
    m_wifiSwitch->setObjectName("QuickSwitchWiFi");
    m_wifiSwitch->setAccessibleName("QuickSwitchWiFi");
    displaySwitch->setObjectName("QuickSwitchDisplay");
    displaySwitch->setAccessibleName("QuickSwitchDisplay");
    displaySwitch->setCheckable(false);
    displaySwitch->setChecked(true);
    detailSwitch->setObjectName("QuickSwitchAllSettings");
    detailSwitch->setAccessibleName("QuickSwitchAllSettings");
    detailSwitch->setCheckable(false);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(btSwitch);
    btnsLayout->addWidget(vpnSwitch);
    btnsLayout->addWidget(m_wifiSwitch);
    btnsLayout->addWidget(displaySwitch);
    btnsLayout->addWidget(detailSwitch);
    btnsLayout->setContentsMargins(0, 15, 0, 15);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(m_itemStack);
    mainLayout->addLayout(btnsLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(btSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(vpnSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(m_wifiSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(displaySwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(detailSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(detailSwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::requestDetailConfig);

    connect(m_networkModel, &NetworkModel::vpnEnabledChanged, vpnSwitch, &QuickSwitchButton::setChecked);
    connect(vpnSwitch, &QuickSwitchButton::checkedChanged, m_networkWorker, &NetworkWorker::setVpnEnable);
    connect(vpnPage, &VpnControlPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(vpnPage, &VpnControlPage::requestDisconnect, m_networkWorker, &NetworkWorker::deactiveConnection);

    connect(m_networkModel, &NetworkModel::deviceEnableChanged, this, &QuickControlPanel::onNetworkDeviceEnableChanged);
    connect(m_wifiSwitch, &QuickSwitchButton::checkedChanged, this, &QuickControlPanel::onWirelessButtonClicked);
    connect(wifiPage, &WifiPage::requestDeviceApList, m_networkWorker, &NetworkWorker::queryAccessPoints);
    connect(wifiPage, &WifiPage::requestActivateAccessPoint, m_networkWorker, &NetworkWorker::activateAccessPoint);
    connect(wifiPage, &WifiPage::requestDeactivateConnection, m_networkWorker, &NetworkWorker::deactiveConnection);

    connect(displayPage, &DisplayControlPage::requestOnlyMonitor, [=](const QString &name) { m_displayWorker->switchMode(SINGLE_MODE, name); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestDuplicateMode, [=] { m_displayWorker->switchMode(MERGE_MODE); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestExtendMode, [=] { m_displayWorker->switchMode(EXTEND_MODE); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestCustom, [=] { emit requestPage("display", QString()); });

    connect(bluetoothList, &BluetoothList::requestConnect, m_bluetoothWorker, &bluetooth::BluetoothWorker::connectDevice);

    vpnSwitch->setChecked(m_networkModel->vpnEnabled());
    onNetworkDeviceEnableChanged();
}

void QuickControlPanel::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);

    m_itemStack->setCurrentIndex(0);
}

void QuickControlPanel::onNetworkDeviceEnableChanged()
{
    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() == NetworkDevice::Wireless && dev->enabled())
        {
            m_wifiSwitch->setChecked(true);
            return;
        }
    }

    m_wifiSwitch->setChecked(false);
}

void QuickControlPanel::onWirelessButtonClicked()
{
    const bool enable = m_wifiSwitch->checked();

    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() == NetworkDevice::Wireless)
            m_networkWorker->setDeviceEnable(dev->path(), enable);
    }
}
