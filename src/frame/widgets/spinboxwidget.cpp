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

#include "widgets/spinboxwidget.h"

#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace dcc;
using namespace dcc::widgets;

SpinBoxWidget::SpinBoxWidget(QWidget *parent)
    : SettingsItem(parent)
    , m_defaultVal(0)
    , m_title(new QLabel)
    , m_spinBox(new QSpinBox)
    , m_resetBtn(new DIconButton(this))
{
    m_spinBox->setContextMenuPolicy(Qt::NoContextMenu);
    m_spinBox->setButtonSymbols(QAbstractSpinBox::PlusMinus);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    m_title->setFixedWidth(110);
    mainLayout->addWidget(m_spinBox);
    m_resetBtn->setIcon(QIcon::fromTheme("dcc_reset"));
    m_resetBtn->setIconSize(QSize(16, 16));
    m_resetBtn->setFixedSize(36, 36);
    mainLayout->addWidget(m_resetBtn);

    connect(m_resetBtn, &DIconButton::clicked, [this] {
        m_spinBox->setValue(m_defaultVal);
    });
    setLayout(mainLayout);
}

void SpinBoxWidget::setDefaultVal(int defaultVal)
{
    m_defaultVal = defaultVal;
    m_spinBox->setValue(m_defaultVal);
}

void SpinBoxWidget::setTitle(const QString &title)
{
    m_title->setText(title);
    m_title->setWordWrap(true);
}
