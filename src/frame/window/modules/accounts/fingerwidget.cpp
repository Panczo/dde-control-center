/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "fingerwidget.h"
#include "widgets/titlelabel.h"

#include <DFontSizeManager>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

FingerWidget::FingerWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_listGrp(new SettingsGroup(nullptr, SettingsGroup::GroupBackground))
    , m_clearBtn(nullptr)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    m_clearBtn = new DCommandLinkButton(tr("Delete fingerprint"));
    TitleLabel *fingetitleLabel = new TitleLabel(tr("Fingerprint Password"));

    m_listGrp->setSpacing(1);
    m_listGrp->setContentsMargins(0, 0, 0, 0);
    m_listGrp->layout()->setMargin(0);
    m_listGrp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *headLayout = new QHBoxLayout;
    headLayout->setSpacing(0);
    headLayout->setContentsMargins(10, 0, 10, 0);
    headLayout->addWidget(fingetitleLabel, 0, Qt::AlignLeft);
    headLayout->addWidget(m_clearBtn, 0, Qt::AlignRight);

    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setSpacing(1);
    mainContentLayout->setMargin(0);
    mainContentLayout->addLayout(headLayout);
    mainContentLayout->addSpacing(10);
    mainContentLayout->addWidget(m_listGrp);
    setLayout(mainContentLayout);

    //设置字体大小
    DFontSizeManager::instance()->bind(m_clearBtn, DFontSizeManager::T8);

    connect(m_clearBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestCleanThumbs(m_curUser);
    });
}

FingerWidget::~FingerWidget()
{

}

void FingerWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    connect(model, &FingerModel::thumbsListChanged, this, &FingerWidget::onThumbsListChanged);
    onThumbsListChanged(model->thumbsList());
}

void FingerWidget::onThumbsListChanged(const QList<dcc::accounts::FingerModel::UserThumbs> &thumbs)
{
    QStringList thumb = thumbsLists;
    bool isAddFingeBtn = true;
    m_listGrp->clear();

    for (int n = 0; n < 10 && n < thumbs.size(); ++n) {
        auto u = thumbs.at(n);
        if (u.username != m_curUser->name()) {
            continue;
        }

        int i = 1; // 记录指纹列表项编号
        qDebug() << "user thumb count: " << u.userThumbs.size();
        for (const QString &title : u.userThumbs) {
            AccounntFingeItem *item = new AccounntFingeItem(this);
            QString finger = tr("Fingerprint") + QString::number(i++);
            item->setTitle(finger);
            item->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            DFontSizeManager::instance()->bind(item, DFontSizeManager::T6);
            m_listGrp->appendItem(item);
            thumb.removeOne(title);
            qDebug() << "onThumbsListChanged: " << finger;
        }

        if (!thumb.isEmpty()) {
            m_notUseThumb = thumb.first();
        }

        if (i == 11) {
            isAddFingeBtn = false;
        }
    }

    m_clearBtn -> setVisible(m_listGrp->itemCount());

    if (!thumb.isEmpty()) {
        m_notUseThumb = thumb.first();
    }
    if (isAddFingeBtn) {
        addFingerButton();
    }
}

void FingerWidget::addFingerButton()
{
    AccounntFingeItem *addfingeItem = new AccounntFingeItem;
    DCommandLinkButton *addBtn = new DCommandLinkButton(tr("Add fingerprint"));
    addfingeItem->setTitle("");
    addfingeItem->appendItem(addBtn);
    m_listGrp->insertItem(m_listGrp->itemCount(), addfingeItem);
    addfingeItem->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    DFontSizeManager::instance()->bind(addBtn, DFontSizeManager::T7);
    connect(addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestAddThumbs(m_curUser->name(), m_notUseThumb);
    });
}
