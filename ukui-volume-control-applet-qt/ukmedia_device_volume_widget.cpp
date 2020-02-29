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
//#include <QStyleOption>
//#include <QPainter>

//void UkmediaDeviceWidget::paintEvent(QPaintEvent *event)
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
//    p.setBrush(QBrush(QColor(0x00,0xFF,0xFF,0x59)));
//                      p.setPen(Qt::NoPen);
//            p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//            p.drawRoundedRect(opt.rect,15,15);
////            style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
////    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//    QWidget::paintEvent(event);
//}


UkmediaDeviceSlider::UkmediaDeviceSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaDeviceSlider::~UkmediaDeviceSlider()
{

}

UkmediaDeviceWidget::UkmediaDeviceWidget(QWidget *parent) : QWidget (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
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

    outputWidget->setFixedSize(304,52);
    outputDisplayWidget->setFixedSize(252,52);
    outputSliderWidget->setFixedSize(252,22);

    outputDeviceLabel = new QLabel(tr("Output Device"),this);
    outputVolumeLabel = new QLabel("0",outputSliderWidget);
    outputDeviceDisplayLabel = new QLabel(tr("Speaker Realtek Audio"),outputDisplayWidget);
    outputDeviceBtn = new QPushButton(outputWidget);
    outputDeviceSlider = new UkmediaDeviceSlider(outputSliderWidget);
    noInputDeviceLabel = new QLabel(tr("Input device can not be detected"),this);

    inputDeviceBtn->setFocusPolicy(Qt::NoFocus);
    outputDeviceBtn->setFocusPolicy(Qt::NoFocus);
    inputDeviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                "padding-left:0px;}");
    outputDeviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                 "padding-left:0px;}");

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
    outputDeviceSlider->setFixedSize(220,22);

    deviceWidget->setFixedSize(358,320);

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
    vlayout->setSpacing(10);
    outputDisplayWidget->setLayout(vlayout);
    outputDisplayWidget->layout()->setContentsMargins(0,0,0,6);

    outputDeviceLabel->move(18,22);

    outputWidget->move(18,72);
    outputDisplayWidget->move(50,0);
    noInputDeviceLabel->move(18,154);
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
    inputWidget->setFixedSize(304,52);
    inputDisplayWidget->setFixedSize(252,52);
    inputSliderWidget->setFixedSize(252,22);

    //设置输入输出音量图标
    inputDeviceBtn->setFixedSize(iconSize);
    inputDeviceBtn->setIconSize(iconSize);
    inputDeviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-input-microphone.svg"));
    //设置滑动条的范围和取向
    inputDeviceLabel->setFixedSize(78,20);
    inputDeviceDisplayLabel->setFixedSize(220,14);
    inputVolumeLabel->setFixedSize(24,14);

    inputDeviceSlider->setRange(0,100);
    inputDeviceSlider->setOrientation(Qt::Horizontal);
    inputDeviceSlider->setFixedSize(220,22);
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
    vlayout->setSpacing(10);
    vlayout->setContentsMargins(0,0,0,6);
    inputDisplayWidget->setLayout(vlayout);

    inputDeviceLabel->move(18,154);
    inputWidget->move(18,204);
    inputDisplayWidget->move(50,0);
    inputDeviceLabel->show();
    inputSliderWidget->show();
    inputDisplayWidget->show();
    inputWidget->show();
}

void UkmediaDeviceWidget::inputWidgetHide()
{
    //隐藏inputDeviceWidget
    inputDeviceLabel->hide();
    inputWidget->hide();
    inputSliderWidget->hide();
    inputDisplayWidget->hide();
    noInputDeviceLabel->show();
}

UkmediaDeviceWidget::~UkmediaDeviceWidget()
{

}
