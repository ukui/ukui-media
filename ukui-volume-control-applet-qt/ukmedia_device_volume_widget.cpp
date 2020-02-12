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
#include "ukmedia_device_volume_widget.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QDebug>

UkmediaDeviceSlider::UkmediaDeviceSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaDeviceSlider::~UkmediaDeviceSlider()
{

}

UkmediaDeviceWidget::UkmediaDeviceWidget(QWidget *parent) : QWidget (parent)
{
    //初始化设备界面
    deviceWidget = new QWidget(this);
    outputWidget = new QWidget(deviceWidget);
    outputSliderWidget = new QWidget(outputWidget);
    outputDisplayWidget = new QWidget(outputWidget);

    inputWidget = new QWidget(deviceWidget);
    inputSliderWidget = new QWidget(inputWidget);
    inputDisplayWidget = new QWidget(inputWidget);

    inputDeviceLabel = new QLabel(tr("Input Device"),this);
    inputVolumeLabel = new QLabel("0",inputSliderWidget);
    inputDeviceDisplayLabel = new QLabel(tr("Microphone"),inputDisplayWidget);
    inputDeviceBtn = new QPushButton(inputWidget);
    inputDeviceSlider = new UkmediaDeviceSlider(inputSliderWidget);

    outputWidget->setFixedSize(296,52);
    outputDisplayWidget->setFixedSize(246,46);
    outputSliderWidget->setFixedSize(246,32);

    outputDeviceLabel = new QLabel(tr("Output Device"),this);
    outputVolumeLabel = new QLabel("0",outputSliderWidget);
    outputDeviceDisplayLabel = new QLabel(tr("Speaker Realtek Audio"),outputDisplayWidget);
    outputDeviceBtn = new QPushButton(outputWidget);
    outputDeviceSlider = new UkmediaDeviceSlider(outputSliderWidget);
    noInputDeviceLabel = new QLabel(tr("Input device can not be detected"),this);

    noInputWidgetInit();

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
    outputDeviceDisplayLabel->setStyleSheet("QLabel{width:126px;"
                                            "height:14px;"
                                            "font-size:14px;"
                                            "color:rgba(255,255,255,0.57);"
                                            "line-height:28px;}");
    inputDeviceLabel->setStyleSheet("QLabel{background:transparent;"
                                     "border:0px;color:#ffffff;"
                                     "font-size:20px;}");
    inputDeviceDisplayLabel->setStyleSheet("QLabel{width:126px;"
                                           "height:14px;"
                                           "font-size:14px;"
                                           "color:rgba(255,255,255,0.57);"
                                           "line-height:28px;}");
    inputVolumeLabel->setStyleSheet("QLabel{background:transparent;"
                                         "border:0px;color:#ffffff;"
                                         "font-size:14px;}");
}

void UkmediaDeviceWidget::deviceWidgetInit()
{
    const QSize iconSize(32,32);
    inputWidget->setFixedSize(296,52);
    inputDisplayWidget->setFixedSize(246,46);
    inputSliderWidget->setFixedSize(246,32);

    //设置输入输出音量图标
    outputDeviceBtn->setFixedSize(iconSize);
    outputDeviceBtn->setIconSize(iconSize);
    outputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audiocard.svg"));
    inputDeviceBtn->setFixedSize(iconSize);
    inputDeviceBtn->setIconSize(iconSize);
    inputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-input-microphone.svg"));
    //设置滑动条的范围和取向
    inputDeviceLabel->setFixedSize(78,20);
    inputDeviceDisplayLabel->setFixedSize(220,14);
    inputVolumeLabel->setFixedHeight(16);

    outputDeviceLabel->setFixedSize(78,20);
    outputDeviceDisplayLabel->setFixedSize(220,14);
    outputVolumeLabel->setFixedHeight(16);

    outputDeviceSlider->setRange(0,100);
    outputDeviceSlider->setOrientation(Qt::Horizontal);
    outputDeviceSlider->setFixedSize(220,20);
    inputDeviceSlider->setRange(0,100);
    inputDeviceSlider->setOrientation(Qt::Horizontal);
    inputDeviceSlider->setFixedSize(220,20);
    //布局
    QHBoxLayout *hlayout1 = new QHBoxLayout(outputSliderWidget);
    QHBoxLayout *hlayout2 = new QHBoxLayout;

    QVBoxLayout *vlayout1 = new QVBoxLayout;
    QVBoxLayout *vlayout2 = new QVBoxLayout;
    QVBoxLayout *vlayout3 = new QVBoxLayout;

    //输出设备布局outputWidget
    hlayout1->addWidget(outputDeviceSlider);
    hlayout1->addWidget(outputVolumeLabel);
    hlayout1->setSpacing(10);
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
    outputWidget->move(20,72);
    outputDisplayWidget->move(50,0);

    //输入设备布局 inputWidget
    hlayout2->addWidget(inputDeviceSlider);
    hlayout2->addWidget(inputVolumeLabel);
    hlayout2->setSpacing(10);
    inputSliderWidget->setLayout(hlayout2);
    inputSliderWidget->layout()->setContentsMargins(0,0,0,0);

    vlayout2->addWidget(inputDeviceDisplayLabel);
    vlayout2->addWidget(inputSliderWidget);
    vlayout2->setContentsMargins(0,0,0,0);
    vlayout2->setSpacing(0);
    inputDisplayWidget->setLayout(vlayout2);

    inputDeviceLabel->move(20,154);
    inputWidget->move(20,204);
    inputDisplayWidget->move(50,0);

    //整体布局
    deviceWidget->setFixedSize(400,320);
}

void UkmediaDeviceWidget::noInputWidgetInit()
{
    //隐藏inputDeviceWidget
    inputDeviceLabel->hide();
    inputWidget->hide();
    inputSliderWidget->hide();
    inputDisplayWidget->hide();
    noInputDeviceLabel->show();
    QSize iconSize(32,32);
    //设置输入输出音量图标
    outputDeviceBtn->setFixedSize(iconSize);
    outputDeviceBtn->setIconSize(iconSize);
    outputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audiocard.svg"));
  //设置滑动条的范围和取向
    outputDeviceLabel->setFixedSize(78,20);
    outputDeviceDisplayLabel->setFixedSize(220,14);
    outputVolumeLabel->setFixedHeight(16);

    outputDeviceSlider->setRange(0,100);
    outputDeviceSlider->setOrientation(Qt::Horizontal);
    outputDeviceSlider->setFixedSize(220,20);

    deviceWidget->setFixedSize(360,320);

    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    //输出设备布局outputWidget
    hlayout->addWidget(outputDeviceSlider);
    hlayout->addWidget(outputVolumeLabel);
    hlayout->setSpacing(10);
    outputSliderWidget->setLayout(hlayout);
    outputSliderWidget->layout()->setContentsMargins(0,0,0,0);

    vlayout->addWidget(outputDeviceDisplayLabel);
    vlayout->addWidget(outputSliderWidget);
    vlayout->setSpacing(0);
    outputDisplayWidget->setLayout(vlayout);
    outputDisplayWidget->layout()->setContentsMargins(0,0,0,0);

    outputDeviceLabel->move(20,22);

    outputWidget->move(20,72);
    outputDisplayWidget->move(50,0);
    noInputDeviceLabel->move(20,154);
    noInputDeviceLabel->setStyleSheet("QLabel{width:126px;"
                                      "height:14px;"
                                      "font-size:14px;"
                                      "color:rgba(255,255,255,0.57);"
                                      "line-height:28px;}");
}

void UkmediaDeviceWidget::inputWidgetShow()
{
    //设置noinputlabel隐藏
    noInputDeviceLabel->hide();
    const QSize iconSize(32,32);
    inputWidget->setFixedSize(296,52);
    inputDisplayWidget->setFixedSize(246,46);
    inputSliderWidget->setFixedSize(246,32);

    //设置输入输出音量图标
    inputDeviceBtn->setFixedSize(iconSize);
    inputDeviceBtn->setIconSize(iconSize);
    inputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-input-microphone.svg"));
    //设置滑动条的范围和取向
    inputDeviceLabel->setFixedSize(78,20);
    inputDeviceDisplayLabel->setFixedSize(220,14);
    inputVolumeLabel->setFixedHeight(16);

    inputDeviceSlider->setRange(0,100);
    inputDeviceSlider->setOrientation(Qt::Horizontal);
    inputDeviceSlider->setFixedSize(220,20);
    //布局
    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    //输入设备布局 inputWidget
    hlayout->addWidget(inputDeviceSlider);
    hlayout->addWidget(inputVolumeLabel);
    hlayout->setSpacing(10);
    inputSliderWidget->setLayout(hlayout);
    inputSliderWidget->layout()->setContentsMargins(0,0,0,0);

    vlayout->addWidget(inputDeviceDisplayLabel);
    vlayout->addWidget(inputSliderWidget);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);
    inputDisplayWidget->setLayout(vlayout);

    inputDeviceLabel->move(20,154);
    inputWidget->move(20,204);
    inputDisplayWidget->move(50,0);
    inputDeviceLabel->show();
    inputSliderWidget->show();
    inputDisplayWidget->show();
    inputWidget->show();
}

UkmediaDeviceWidget::~UkmediaDeviceWidget()
{

}
