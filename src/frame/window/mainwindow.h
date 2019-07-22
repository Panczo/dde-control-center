/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "navigation/navmodel.h"
#include "interfaces/moduleinterface.h"

#include <DMainWindow>

#include <QStack>

DWIDGET_USE_NAMESPACE
QT_BEGIN_NAMESPACE
class QHBoxLayout;
QT_END_NAMESPACE
class NavWinView;
class NavModel;

using namespace dcc;

class MainWindow : public DMainWindow, public FrameProxyInterface
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    void pushWidget(QWidget *widget);
    void popWidget(void);
    void popAllWidgets(void);

private:
    void tryLoadModule(NavModel::ModuleType type = NavModel::ModuleType::Default);
    void mouseDoubleClickEvent(QMouseEvent *event) override;

public:
    void pushWidget(ModuleInterface * const inter, ContentWidget * const w);
    void setFrameAutoHide(ModuleInterface * const inter, const bool autoHide);
    void setModuleVisible(ModuleInterface * const inter, const bool visible);
    void showModulePage(const QString &module, const QString &page, bool animation);

private:
    QHBoxLayout *m_contentLayout;
    QHBoxLayout *m_rightContentLayout;
    NavWinView *m_navView;
    QWidget *m_rightView;
    NavModel *m_navModel;
    QStack<QWidget *> m_contentStack;
    NavModel::ModuleType m_navModelType;

Q_SIGNALS:
    void moduleVisibleChanged(const QString &module, bool visible);

private Q_SLOTS:
    void onFirstItemClick(const QModelIndex &index);
    void loadModule(ModuleInterface *const module);
};

#endif // MAINWINDOW_H
