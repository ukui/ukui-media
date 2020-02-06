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
    this->setFixedSize(360,260);
    this->setStyleSheet("QWidget{background-color: rgba(21,26,30,90%);"
                        "border-radius: 2px;}");
}

void UkmediaDeviceWidget::deviceWidgetInit()
{
    const QSize iconSize(30,30);
    QWidget *deviceWidget = new QWidget(this);
    QWidget *outputSliderWidget = new QWidget(this);
    QWidget *outputDisplayWidget = new QWidget(this);
    QWidget *outputWidget = new QWidget(this);

    QWidget *inputSliderWidget = new QWidget(this);
    QWidget *inputDisplayWidget = new QWidget(this);
    QWidget *inputWidget = new QWidget(this);

    outputDeviceLabel = new QLabel(tr("Output Device"));
    outputVolumeLabel = new QLabel("0",outputSliderWidget);
    outputDeviceDisplayLabel = new QLabel(tr("Speaker Realtek Audio"),outputDisplayWidget);
    outputDeviceBtn = new QPushButton(this);
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
    QHBoxLayout *hlayout5 = new QHBoxLayout;

    QVBoxLayout *vlayout1 = new QVBoxLayout;
    QVBoxLayout *vlayout2 = new QVBoxLayout;
    QVBoxLayout *vlayout3 = new QVBoxLayout;
    QVBoxLayout *vlayout4 = new QVBoxLayout;
    QVBoxLayout *vlayout5 = new QVBoxLayout;

    //输出设备布局outputWidget
    hlayout1->addWidget(outputDeviceSlider);
    hlayout1->addWidget(outputVolumeLabel);
    hlayout1->setSpacing(20);
    outputSliderWidget->setLayout(hlayout1);

    vlayout1->addWidget(outputDeviceDisplayLabel);
    vlayout1->addWidget(outputSliderWidget);
    outputDisplayWidget->setLayout(vlayout1);

    hlayout2->addWidget(outputDeviceBtn);
    hlayout2->addWidget(outputDisplayWidget);
    outputWidget->setLayout(hlayout2);

//    outputWidget->move(40,0);

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
    vlayout3->addWidget(outputWidget);
    vlayout3->addWidget(inputDeviceLabel);
    vlayout3->addWidget(inputWidget);
    deviceWidget->setLayout(vlayout3);
//    deviceWidget->move(40,0);
}

UkmediaDeviceWidget::~UkmediaDeviceWidget()
{

}
