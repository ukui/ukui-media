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
#include "customstyle.h"
UkmediaDeviceWidget::UkmediaDeviceWidget(QWidget *parent) : QWidget (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    //初始化设备界面
    displayOutputLabel = new QLabel(this);
    deviceWidget = new QWidget(this);
    outputWidget = new QWidget(deviceWidget);
    outputSliderWidget = new QWidget(outputWidget);
    inputWidget = new QWidget(deviceWidget);
    inputSliderWidget = new QWidget(inputWidget);

    inputDeviceLabel = new QLabel(tr("Input Device"),this);
    inputDeviceDisplayLabel = new QLabel(tr("Microphone"),inputWidget);
    inputDeviceBtn = new QPushButton(inputWidget);
    inputDeviceSlider = new UkmediaVolumeSlider(inputSliderWidget);
    inputMuteButton = new QPushButton(inputSliderWidget);

    outputWidget->setFixedSize(340,60);
    outputSliderWidget->setFixedSize(306,32);
    outputMuteBtn = new QPushButton(outputSliderWidget);
    outputDeviceLabel = new QLabel(tr("Output Device"),this);
    outputDeviceDisplayLabel = new QLabel(tr("Speaker Realtek Audio"),outputWidget);
    outputDeviceBtn = new QPushButton(outputWidget);
    outputDeviceSlider = new UkmediaVolumeSlider(outputSliderWidget);
    noInputDeviceLabel = new QLabel(tr("Input device can not be detected"),this);
    outputDisplayVolumeLabel = new QLabel(this);

    inputDeviceSlider->setStyle(new CustomStyle());
    outputDeviceSlider->setStyle(new CustomStyle());
    outputMuteBtn->setFixedSize(24,24);
    inputMuteButton->setFixedSize(24,24);
    QSize iconSize(24,24);
    outputMuteBtn->setIconSize(iconSize);
    inputMuteButton->setIconSize(iconSize);
    inputDeviceBtn->setFocusPolicy(Qt::NoFocus);
    outputDeviceBtn->setFocusPolicy(Qt::NoFocus);
    noInputWidgetInit();
    inputMuteButton->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                                   "padding-left:0px;}");
    outputMuteBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                "padding-left:0px;}");
    inputDeviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                "padding-left:0px;}");
    outputDeviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                 "padding-left:0px;}");
    outputDeviceSlider->setStyleSheet("QSlider::groove:horizontal {border: 0px solid #bbb;}"
                                      "QSlider::sub-page:horizontal {"
                                      "background: rgb(107,142,235);"
                                      "border-radius: 2px;"
                                      "margin-top:9px;"
                                      "margin-bottom:9px;}"
                                      "QSlider::add-page:horizontal {"
                                      "background:  rgba(52,70,80,90%);"
                                      "border: 0px solid #777;"
                                      "border-radius: 2px;"
                                      "margin-top:9px;"
                                      "margin-bottom:9px;}"
                                      "QSlider::handle:horizontal {"
                                      "width: 20px;"
                                      "height: 20px;"
                                      "background: rgb(61,107,229);"
                                      "border-radius:10px;}");

    outputDeviceLabel->setStyleSheet("QLabel{background:transparent;"
                                     "border:0px;color:#ffffff;"
                                     "font-family:Noto Sans CJK SC;"
                                     "font-weight:400;"
                                     "color:rgba(255,255,255,0.97);"
                                     "line-height:34px;"
                                     "font-size:20px;}");
    outputDeviceDisplayLabel->setStyleSheet("font-size:14px;font-family:Noto Sans CJK SC;"
                                            "font-weight:400;"
                                            "color:rgba(255,255,255,0.91);"
                                            "line-height:28px;");
    inputDeviceLabel->setStyleSheet("QLabel{background:transparent;"
                                    "border:0px;color:#ffffff;"
                                    "font-family:Noto Sans CJK SC;"
                                    "font-weight:400;"
                                    "color:rgba(255,255,255,0.97);"
                                    "line-height:34px;"
                                    "font-size:20px;}");
    inputDeviceDisplayLabel->setStyleSheet("QLabel{font-family:Noto Sans CJK SC;"
                                           "font-size:14px;"
                                           "color:rgba(255,255,255,0.91);"
                                           "line-height:28px;}");
}



void UkmediaDeviceWidget::noInputWidgetInit()
{
    //隐藏inputDeviceWidget
    inputDeviceLabel->hide();
    inputWidget->hide();
    inputSliderWidget->hide();
    noInputDeviceLabel->show();
    QSize iconSize(32,32);
    //设置输入输出音量图标
    outputDeviceBtn->setFixedSize(iconSize);
    outputDeviceBtn->setIconSize(iconSize);
    outputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audiocard.svg"));
  //设置滑动条的范围和取向
    outputDeviceLabel->setFixedSize(140,20);
    outputDeviceDisplayLabel->setFixedSize(220,16);

    outputDeviceSlider->setRange(0,100);
    outputDeviceSlider->setOrientation(Qt::Horizontal);
    outputDeviceSlider->setFixedSize(220,22);
    deviceWidget->setFixedSize(358,320);

    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    QSpacerItem *item1 = new QSpacerItem(18,20);
    QSpacerItem *item2 = new QSpacerItem(12,20);

    //输出设备布局outputWidget
    hlayout->addWidget(outputDeviceBtn);
    hlayout->addItem(item1);
    hlayout->addWidget(outputDeviceSlider);
    hlayout->addItem(item2);
    hlayout->addWidget(outputMuteBtn);
    hlayout->setSpacing(0);
    outputSliderWidget->setLayout(hlayout);
    outputSliderWidget->layout()->setContentsMargins(0,0,0,0);

    vlayout->addWidget(outputDeviceDisplayLabel);
    vlayout->addWidget(outputSliderWidget);
    vlayout->setSpacing(12);
    outputWidget->setLayout(vlayout);
    outputWidget->layout()->setContentsMargins(0,0,0,0);

    outputDeviceLabel->move(18,22);
    outputWidget->move(18,62);
    noInputDeviceLabel->move(18,154);
    noInputDeviceLabel->setStyleSheet("QLabel{width:126px;"
                                      "height:14px;"
                                      "font-family:Noto Sans CJK SC;"
                                      "font-size:14px;"
                                      "color:rgba(255,255,255,0.57);"
                                      "line-height:28px;}");
}

void UkmediaDeviceWidget::inputWidgetShow()
{
    //设置noinputlabel隐藏
    noInputDeviceLabel->hide();
    const QSize iconSize(32,32);
    inputWidget->setFixedSize(340,60);
    inputSliderWidget->setFixedSize(306,32);

    //设置输入输出音量图标
    inputDeviceBtn->setFixedSize(iconSize);
    inputDeviceBtn->setIconSize(iconSize);
    inputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-input-microphone.svg"));
    //设置滑动条的范围和取向
    inputDeviceLabel->setFixedSize(140,20);
    inputDeviceDisplayLabel->setFixedSize(220,14);

    inputDeviceSlider->setRange(0,100);
    inputDeviceSlider->setOrientation(Qt::Horizontal);
    inputDeviceSlider->setFixedSize(220,22);
    //布局
    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;

    //输入设备布局 inputWidget
    hlayout->addWidget(inputDeviceBtn);
    hlayout->addItem(new QSpacerItem(18,20));
    hlayout->addWidget(inputDeviceSlider);
    hlayout->addItem(new QSpacerItem(12,20));
    hlayout->addWidget(inputMuteButton);
    hlayout->setSpacing(0);
    inputSliderWidget->setLayout(hlayout);
    inputSliderWidget->layout()->setContentsMargins(0,0,0,0);

    vlayout->addWidget(inputDeviceDisplayLabel);
    vlayout->addWidget(inputSliderWidget);
    vlayout->setSpacing(14);
    vlayout->setContentsMargins(0,0,0,6);
    inputWidget->setLayout(vlayout);

    inputDeviceLabel->move(18,154);
    inputWidget->move(18,194);

    inputDeviceLabel->show();
    inputSliderWidget->show();
    inputWidget->show();
}

void UkmediaDeviceWidget::inputWidgetHide()
{
    //隐藏inputDeviceWidget
    inputDeviceLabel->hide();
    inputWidget->hide();
    inputSliderWidget->hide();
    noInputDeviceLabel->show();
}


UkmediaDeviceWidget::~UkmediaDeviceWidget()
{

}
