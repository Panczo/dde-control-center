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
 *             listenerri <listenerri@gmail.com>
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

#include "pppoepage.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/translucentframe.h"
#include "widgets/loadingnextpagewidget.h"
#include "widgets/switchwidget.h"

#include <networkmodel.h>
#include <wireddevice.h>
#include <DHiDPIHelper>
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

PppoePage::PppoePage(QWidget *parent)
    : ContentWidget(parent)
      , m_createBtn(new QPushButton)
      , m_lvsettings(new DListView)
      , m_modelSettings(new QStandardItemModel)
{
    m_createBtn->setText(tr("Create PPPoE Connection"));

    m_lvsettings->setModel(m_modelSettings);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_lvsettings);
    mainLayout->addWidget(m_createBtn);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("PPP"));

    connect(m_createBtn, &QPushButton::clicked, this, &PppoePage::createPPPoEConnection);
    connect(m_lvsettings, &QListView::clicked, this, [this](const QModelIndex &idx) {
        this->onPPPoESelected(idx.data(UuidRole).toString());
    });
}

PppoePage::~PppoePage()
{
    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void PppoePage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::connectionListChanged, this, &PppoePage::onConnectionListChanged);
    connect(model, &NetworkModel::activeConnectionsChanged, this, &PppoePage::onActiveConnectionChanged);

    onConnectionListChanged();
}

void PppoePage::createPPPoEConnection()
{
    m_editPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::PppoeConnection, "/");
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &PppoePage::requestFrameKeepAutoHide);

    Q_EMIT requestNextPage(m_editPage);
}

void PppoePage::onConnectionListChanged()
{
    m_items.clear();
    m_modelSettings->clear();

    for (const auto &pppoe : m_model->pppoes())
    {
        const auto name = pppoe.value("Id").toString();
        const auto uuid = pppoe.value("Uuid").toString();

        DStandardItem *it = new DStandardItem();
        it->setText(name);
        it->setData(uuid, UuidRole);
        it->setCheckable(true);

        DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignRight, QSize(24, 24), QSize(), true);
        editaction->setIcon(QIcon::fromTheme("arrow-right"));
        connect(editaction, &QAction::triggered, [this, uuid] {
            this->onConnectionDetailClicked(uuid);
        });
        it->setActionList(Qt::Edge::RightEdge, {editaction});
        m_items[uuid] = it;

        m_modelSettings->appendRow(it);
    }

    onActiveConnectionChanged(m_model->activeConns());
}

void PppoePage::onConnectionDetailClicked(const QString &connectionUuid)
{
    m_editPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::PppoeConnection, "/", connectionUuid);
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &PppoePage::requestFrameKeepAutoHide);

    Q_EMIT requestNextPage(m_editPage);
}

void PppoePage::onPPPoESelected(const QString &connectionUuid)
{
    Q_EMIT requestActivateConnection("/", connectionUuid);
}

void PppoePage::onActiveConnectionChanged(const QList<QJsonObject> &conns)
{
    for (int i = 0; i < m_modelSettings->rowCount(); ++i) {
        m_modelSettings->item(i)->setCheckState(Qt::CheckState::Unchecked);
    }
    for (const QJsonObject &connObj : conns) {
        const QString &uuid = connObj.value("Uuid").toString();
        if (!m_items.contains(uuid)) {
            continue;
        }
        // the State of Active Connection
        // 0:Unknow, 1:Activating, 2:Activated, 3:Deactivating, 4:Deactivated
        int state = m_model->activeConnObjectByUuid(uuid).value("State").toInt(0);
        if(state == 2) {
            m_items[uuid]->setCheckState(Qt::CheckState::Checked);
            //w->setLoading(false);
        } else if(state == 1) {
            //TODO: connecting indicator?
            //w->setLoading(true);
        } else {
            //w->setLoading(false);
        }
    }
}