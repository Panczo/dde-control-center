/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#pragma once

#include "window/namespace.h"

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>

namespace DCC_NAMESPACE {
namespace personalization {
class PersonalizationList : public QWidget
{
    Q_OBJECT
public:
    explicit PersonalizationList(QWidget *parent = 0);

Q_SIGNALS:
    void requestShowGeneral();
    void requestShowIconTheme();
    void requestShowCursorTheme();
    void requestShowFonts();
public Q_SLOTS:
    void onCategoryClicked(const QModelIndex &index);

private:
    QListView  *m_categoryListView;
    QStandardItemModel *m_model;
    QVBoxLayout *m_centralLayout;
};

}
}
