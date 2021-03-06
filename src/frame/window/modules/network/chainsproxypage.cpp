/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "chainsproxypage.h"
#include "widgets/comboxwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"
#include "widgets/buttontuple.h"

#include <DTipLabel>

#include <QComboBox>
#include <QHBoxLayout>
#include <QRegularExpression>

#include <networkmodel.h>

using namespace dcc;
using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;
using namespace dde::network;

ChainsProxyPage::ChainsProxyPage(QWidget *parent) : ContentWidget(parent)
{
    setTitle(tr("Application Proxy"));

    m_proxyType = new ComboxWidget;
    m_proxyType->setTitle(tr("Proxy Type"));

    QComboBox *cb = m_proxyType->comboBox();
    cb->addItem("http");
    cb->addItem("socks4");
    cb->addItem("socks5");

    m_addr = new LineEditWidget;
    m_addr->setTitle(tr("IP Address"));
    m_addr->setPlaceholderText(tr("Required"));

    m_port = new LineEditWidget;
    m_port->setTitle(tr("Port"));
    m_port->setPlaceholderText(tr("Required"));

    m_username = new LineEditWidget;
    m_username->setTitle(tr("Username"));
    m_username->setPlaceholderText(tr("Optional"));

    m_password = new LineEditWidget;
    m_password->setTitle(tr("Password"));
    m_password->setPlaceholderText(tr("Optional"));
    m_password->textEdit()->setEchoMode(QLineEdit::Password);

    SettingsGroup *grp = new SettingsGroup;
    grp->appendItem(m_proxyType);
    grp->appendItem(m_addr);
    grp->appendItem(m_port);
    grp->appendItem(m_username);
    grp->appendItem(m_password);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addSpacing(20);
    layout->addLayout(topLayout);
    layout->addWidget(grp);

    DTipLabel *tip = new DTipLabel(tr("Check \"Use a proxy\" in application context menu in Launcher after configured"));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignLeft);
    layout->addWidget(tip);

    ButtonTuple *btns = new ButtonTuple(ButtonTuple::Save);
    btns->leftButton()->setText(tr("Cancel"));
    btns->rightButton()->setText(tr("Save"));

    layout->addStretch();

    layout->addWidget(btns, 0, Qt::AlignBottom);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);

    setContent(w);

    connect(btns->leftButton(), &QPushButton::clicked, this, &ChainsProxyPage::back);
    connect(btns->rightButton(), &QPushButton::clicked, this, &ChainsProxyPage::onCheckValue);
}

void ChainsProxyPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::chainsTypeChanged, m_proxyType->comboBox(), &QComboBox::setCurrentText);
    connect(model, &NetworkModel::chainsAddrChanged, m_addr, &LineEditWidget::setText);
    connect(model, &NetworkModel::chainsPortChanged, this, [ = ](const uint port) {
        m_port->setText(QString::number(port));
    });
    connect(model, &NetworkModel::chainsUsernameChanged, m_username, &LineEditWidget::setText);
    connect(model, &NetworkModel::chainsPasswdChanged, m_password, &LineEditWidget::setText);

    ProxyConfig config = model->getChainsProxy();

    m_proxyType->comboBox()->setCurrentText(config.type);
    m_addr->setText(config.url);
    m_port->setText(QString::number(config.port));
    m_username->setText(config.username);
    m_password->setText(config.password);
}

void ChainsProxyPage::onCheckValue()
{
    m_addr->setIsErr(false);
    m_port->setIsErr(false);
    m_username->setIsErr(false);
    m_password->setIsErr(false);

    // if address and port is empty,remove config file
    if (m_addr->text().isEmpty() && m_port->text().isEmpty()) {
        ProxyConfig config;
        config.port = 0;
        config.url.clear();

        Q_EMIT requestSet(config);
        Q_EMIT back();
        return;
    }

    const QString &addr = m_addr->text();
    if (addr.isEmpty() || !isIPV4(addr)) {
        m_addr->setIsErr(true);
        m_addr->dTextEdit()->showAlertMessage(tr("Invalid IP address"), this, 2000);
        return;
    }

    bool ok = true;
    const uint port = m_port->text().toUInt(&ok);
    if (!ok) {
        m_port->setIsErr(true);
        m_port->dTextEdit()->showAlertMessage(tr("Invalid port"), this, 2000);
        return;
    }

    const QString &username = m_username->text();
    const QString &password = m_password->text();

    ProxyConfig config;
    config.type = m_proxyType->comboBox()->currentText();
    config.url = addr;
    config.port = port;
    config.username = username;
    config.password = password;

    Q_EMIT requestSet(config);
    Q_EMIT back();
}

bool ChainsProxyPage::isIPV4(const QString &ipv4)
{
    QRegularExpression reg("(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = reg.match(ipv4);
    bool hasMatch = match.hasMatch();

    if (!hasMatch)
        return false;

    for (int i(1); i != 5; i++) {
        const int n = match.captured(i).toInt();

        if (n < 0 || n > 255)
            return false;
    }

    return true;
}
