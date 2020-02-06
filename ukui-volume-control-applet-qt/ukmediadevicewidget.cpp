/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#include "ukmediadevicewidget.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>

UkmediaDeviceSlider::UkmediaDeviceSlider(QWidget *panet)
{

}

UkmediaDeviceSlider::~UkmediaDeviceSlider()
{

}

UkmediaDeviceWidget::UkmediaDeviceWidget(QWidget *parent) : QWidget (parent)
{
    //初始化设备界面
    deviceWidgetInit();
    this->setMinimumSize(360,260);
    this->setMaximumSize(360,500);
    this->setStyleSheet("QWidget{background-color: rgba(21,26,30,90%);"
                        "border-radius: 2px;}");
}

void UkmediaDeviceWidget::deviceWidgetInit()
{
    const QSize iconSize(32,32);
    QWidget *deviceWidget = new QWidget(this);
    QWidget *outputWidget = new QWidget(this);
    QWidget *outputSliderWidget = new QWidget(outputWidget);
    QWidget *outputDisplayWidget = new QWidget(outputWidget);

    QWidget *inputSliderWidget = new QWidget(this);
    QWidget *inputDisplayWidget = new QWidget(this);
    QWidget *inputWidget = new QWidget(this);

    outputWidget->setFixedSize(296,52);
    outputDisplayWidget->setFixedSize(246,46);
    outputSliderWidget->setFixedSize(246,32);
    outputDeviceLabel = new QLabel(tr("Output Device"),this);
    outputVolumeLabel = new QLabel("0",outputSliderWidget);
    outputDeviceDisplayLabel = new QLabel(tr("Speaker Realtek Audio"),outputDisplayWidget);
    outputDeviceBtn = new QPushButton(outputWidget);
    outputDeviceSlider = new UkmediaDeviceSlider(outputSliderWidget);

    inputDeviceLabel = new QLabel(tr("Input Device"));
    inputVolumeLabel = new QLabel("0",inputSliderWidget);
    inputDeviceDisplayLabel = new QLabel(tr("Microphone"),inputDisplayWidget);
    inputDeviceBtn = new QPushButton(this);
    inputDeviceSlider = new UkmediaDeviceSlider(inputSliderWidget);

    //设置输入输出音量图标
    outputDeviceBtn->setFixedSize(iconSize);
    outputDeviceBtn->setIconSize(iconSize);
    inputDeviceBtn->setFixedSize(iconSize);
    inputDeviceBtn->setIconSize(iconSize);
    outputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audiocard.svg"));
    inputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-input-microphone.svg"));
    //设置滑动条的范围和取向
    outputDeviceLabel->setFixedSize(78,20);
    outputDeviceDisplayLabel->setFixedSize(220,14);
    outputVolumeLabel->setFixedHeight(10);
    outputDeviceSlider->setRange(0,100);
    outputDeviceSlider->setOrientation(Qt::Horizontal);
    outputDeviceSlider->setFixedSize(220,20);
    inputDeviceSlider->setRange(0,100);
    inputDeviceSlider->setOrientation(Qt::Horizontal);
    inputDeviceSlider->setFixedSize(220,20);
    //布局
    QHBoxLayout *hlayout1 = new QHBoxLayout(outputSliderWidget);
    QHBoxLayout *hlayout2 = new QHBoxLayout;
    QHBoxLayout *hlayout3 = new QHBoxLayout;
    QHBoxLayout *hlayout4 = new QHBoxLayout;

    QVBoxLayout *vlayout1 = new QVBoxLayout;
    QVBoxLayout *vlayout2 = new QVBoxLayout;
    QVBoxLayout *vlayout3 = new QVBoxLayout;

    QSpacerItem *item1 = new QSpacerItem(2,30,QSizePolicy::Minimum, QSizePolicy::Fixed);
    QSpacerItem *item2 = new QSpacerItem(2,30,QSizePolicy::Minimum, QSizePolicy::Fixed);
    QSpacerItem *item3 = new QSpacerItem(2,30, QSizePolicy::Minimum, QSizePolicy::Fixed);
    QSpacerItem *item4 = new QSpacerItem(2,30,QSizePolicy::Expanding, QSizePolicy::Minimum);

    //输出设备布局outputWidget
    hlayout1->addWidget(outputDeviceSlider);
    hlayout1->addWidget(outputVolumeLabel);
    hlayout1->setSpacing(0);
    outputSliderWidget->setLayout(hlayout1);
    outputSliderWidget->layout()->setContentsMargins(0,0,0,0);

    vlayout1->addWidget(outputDeviceDisplayLabel);
    vlayout1->addWidget(outputSliderWidget);
    vlayout1->setSpacing(0);
//    vlayout1->setMargin(0);
    vlayout1->setContentsMargins(0,0,0,0);
    outputDisplayWidget->setLayout(vlayout1);
    outputDisplayWidget->layout()->setContentsMargins(0,0,0,0);
    outputDeviceLabel->move(20,23);
    outputWidget->move(60,72);
    outputDisplayWidget->move(50,0);

    //输入设备布局 inputWidget
    hlayout3->addWidget(inputDeviceSlider);
    hlayout3->addWidget(inputVolumeLabel);
    hlayout3->setSpacing(20);
    inputSliderWidget->setLayout(hlayout3);

    vlayout2->addWidget(inputDeviceDisplayLabel);
    vlayout2->addWidget(inputSliderWidget);
    inputDisplayWidget->setLayout(vlayout2);

    hlayout4->addWidget(inputDeviceBtn);
    hlayout4->addWidget(inputDisplayWidget);
    inputWidget->setLayout(hlayout4);
//    inputWidget->move(40,100);

    //整体布局
    vlayout3->addWidget(outputDeviceLabel);
    vlayout3->addItem(item1);
    vlayout3->addWidget(outputWidget);
//    vlayout3->addItem(item2);
    vlayout3->addWidget(inputDeviceLabel);
//    vlayout3->addItem(item3);
    vlayout3->addWidget(inputWidget);
//    vlayout3->addItem(item4);
    deviceWidget->setLayout(vlayout3);
    vlayout3->setSpacing(0);
    vlayout3->setMargin(0);
    deviceWidget->layout()->setContentsMargins(20,22,0,20);
//    deviceWidget->move(40,0);

    outputVolumeLabel->setStyleSheet("QLabel{background:transparent;"
                                         "border:0px;color:#ffffff;"
                                         "font-size:14px;}");
    outputDeviceLabel->setStyleSheet("QLabel{background:transparent;"
                                     "border:0px;color:#ffffff;"
                                     "font-family:Noto Sans CJK SC;"
                                     "font-weight:400;"
                                     "color:rgba(255,255,255,1);"
                                     "line-height:34px;"
                                     "opacity:0.97;"
                                     "font-size:20px;}");
    outputDeviceDisplayLabel->setStyleSheet("QLabel{background:transparent;"
                                         "border:0px;color:#ffffff;"
                                         "font-size:14px;}");
    inputDeviceLabel->setStyleSheet("QLabel{background:transparent;"
                                     "border:0px;color:#ffffff;"
                                     "font-size:18spx;}");

    inputDeviceDisplayLabel->setStyleSheet("QLabel{background:transparent;"
                                         "border:0px;color:#ffffff;"
                                         "font-size:14px;}");
    inputVolumeLabel->setStyleSheet("QLabel{background:transparent;"
                                         "border:0px;color:#ffffff;"
                                         "font-size:14px;}");
}

UkmediaDeviceWidget::~UkmediaDeviceWidget()
{

}
