/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "brightnesspage.h"
#include "modules/display/displaymodel.h"
#include "modules/display/monitor.h"
#include "modules/display/brightnessitem.h"

#include "widgets/labels/tipslabel.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"

#include <QLabel>

using namespace dcc::widgets;
using namespace dcc::display;

namespace DCC_NAMESPACE {

namespace display {

BrightnessPage::BrightnessPage(QWidget *parent)
    : QWidget(parent)
    ,m_centralLayout(new QVBoxLayout)
    ,m_nightTips(new QLabel)
{
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(10);
    m_centralLayout->addSpacing(10);

    m_nightTips = new QLabel(tr("The screen tone will be auto adjusted by help of figuring out your location to protect eyes"));
    m_nightTips->setWordWrap(true);
    m_nightTips->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_nightTips->adjustSize();
    m_nightTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_centralLayout->addWidget(m_nightTips);

    m_nightShift = new SwitchWidget;
    m_nightShift->setTitle(tr("Night Shift"));
    m_centralLayout->addWidget(m_nightShift);

    m_autoLightMode = new SwitchWidget;
    m_autoLightMode->setTitle(tr("Auto Brightness"));
    m_centralLayout->addWidget(m_autoLightMode);

    setMinimumWidth(300);

    setLayout(m_centralLayout);
}

void BrightnessPage::setMode(DisplayModel* model)
{
    m_displayModel = model;

    connect(m_autoLightMode, &SwitchWidget::checkedChanged, this, &BrightnessPage::requestAmbientLightAdjustBrightness);
    connect(m_displayModel, &DisplayModel::nightModeChanged, m_nightShift, &SwitchWidget::setChecked);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, m_nightShift, &SwitchWidget::setVisible);
    connect(m_displayModel, &DisplayModel::redshiftVaildChanged, m_nightTips, &QLabel::setVisible);

    connect(m_nightShift, &SwitchWidget::checkedChanged, this, &BrightnessPage::requestSetNightMode);
    connect(m_displayModel, &DisplayModel::redshiftSettingChanged, m_nightShift, &SwitchWidget::setDisabled);

    connect(m_displayModel, &DisplayModel::autoLightAdjustVaildChanged, m_autoLightMode, &SwitchWidget::setVisible);
    connect(m_displayModel, &DisplayModel::autoLightAdjustSettingChanged, m_autoLightMode, &SwitchWidget::setChecked);

    m_autoLightMode->setVisible(model->autoLightAdjustIsValid());
    m_autoLightMode->setChecked(model->isAudtoLightAdjust());

    m_nightShift->setVisible(model->redshiftIsValid());
    m_nightTips->setVisible(model->redshiftIsValid());

    m_nightShift->setChecked(model->isNightMode());
    m_nightShift->setDisabled(model->redshiftSetting());

    addSlider();
    m_centralLayout->addStretch(1);
}

void BrightnessPage::addSlider()
{
    auto monList = m_displayModel->monitorList();

    QString title = tr("Display scaling for all monitors");
    for (int i = 0; i < monList.size(); ++i)
    {
        monList[i]->brightness();


        //单独显示每个亮度调节名
        title = 1 == monList.size() ? title : tr("Display scaling for %1").arg(monList[i]->name());

        TitledSliderItem *slideritem = new TitledSliderItem(title);
        DCCSlider *slider = slideritem->slider();
        slider->setRange(0, 100);
        slider->setType(DCCSlider::Vernier);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTickInterval(1);
        slider->setSliderPosition(int(monList[i]->brightness()*100));
        slider->setPageStep(1);

        connect(slider, &DCCSlider::sliderMoved, this, [=](int pos){
            double val = pos/100.0;
            Q_EMIT requestSetMonitorBrightness(monList[i],val);
        });

        m_centralLayout->addWidget(slideritem);
    }
}

}

}