/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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
#include "netswitchwidget.h"

#include "widgets/switchwidget.h"

using namespace dcc;
using namespace widgets;

namespace DCC_NAMESPACE {
namespace network {

NetSwitchWidget::NetSwitchWidget(QFrame *parent)
    : SwitchWidget(parent)
{
    addBackground();
}

SwitchWidget *NetSwitchWidget::switchWidget()
{
    return this;
}

}
}
