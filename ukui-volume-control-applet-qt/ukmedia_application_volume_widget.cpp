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
#include "ukmedia_application_volume_widget.h"
#include <QDebug>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QSpacerItem>
ApplicationVolumeWidget::ApplicationVolumeWidget(QWidget *parent) : QWidget (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    upWidget = new QWidget(this);
    applicationLabel = new QLabel(tr("Application Volume"),this);
    app_volume_list = new QStringList;
    systemVolumeLabel = new QLabel(tr("System Volume"),this);
    systemVolumeBtn = new UkuiButtonDrawSvg(this);
    systemVolumeSlider = new UkmediaVolumeSlider(this);
    systemVolumeDisplayLabel = new QLabel(this);
    systemVolumeWidget = new QWidget(this);
    systemVolumeSliderWidget = new QWidget();

    systemVolumeSlider->setOrientation(Qt::Horizontal);
    systemVolumeSlider->setRange(0,100);
    upWidget->setFixedSize(358,140);
    systemVolumeWidget->setFixedSize(312,60);
    systemVolumeSliderWidget->setFixedSize(312,32);
    systemVolumeLabel->setFixedSize(220,16);
    systemVolumeBtn->setFixedSize(32,32);
    systemVolumeSlider->setFixedSize(220,22);
    systemVolumeDisplayLabel->setFixedSize(32,32);
    applicationLabel->setFixedSize(200,20);
    QSize iconSize(32,32);
    systemVolumeBtn->setIconSize(iconSize);

    upWidget->move(0,0);
    systemVolumeWidget->move(18,62);
//    systemVolumeSliderWidget->move(18,90);
    applicationLabel->move(18,22);
    this->setFixedSize(358,320);

    QSpacerItem *item1 = new QSpacerItem(18,20,QSizePolicy::Fixed);
    QSpacerItem *item2 = new QSpacerItem(10,20,QSizePolicy::Fixed);
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(systemVolumeBtn);
    hlayout->addItem(item1);
    hlayout->addWidget(systemVolumeSlider);
    hlayout->addItem(item2);
    hlayout->addWidget(systemVolumeDisplayLabel);
    systemVolumeSliderWidget->setLayout(hlayout);
    hlayout->setSpacing(0);
    systemVolumeSliderWidget->layout()->setContentsMargins(0,0,0,0);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(systemVolumeLabel);
    vLayout->addItem(new QSpacerItem(20,12,QSizePolicy::Fixed,QSizePolicy::Fixed));
    vLayout->addWidget(systemVolumeSliderWidget);
    vLayout->setSpacing(0);
    systemVolumeWidget->setLayout(vLayout);
    systemVolumeWidget->layout()->setContentsMargins(0,0,0,0);

    systemVolumeLabel->setStyleSheet("font-size:14px;"
                                     "font-weight:400;"
                                     "color:rgba(255,255,255,0.91);"
                                     "line-height:28px;");
    systemVolumeBtn->setStyleSheet("QPushButton{background:transparent;border:0px;}");
    systemVolumeSlider->setStyleSheet("QSlider::groove:horizontal {border: 0px solid #bbb;}"
                                      "QSlider::sub-page:horizontal {"
                                      "background: rgb(107,142,235);"
                                      "border-radius: 2px;"
                                      "margin-top:9px;"
                                      "margin-bottom:9px;}"
                                      "QSlider::add-page:horizontal {"
                                      "background:  rgba(255,255,255,0.1);"
                                      "border: 0px solid #777;"
                                      "border-radius: 2px;"
                                      "margin-top:9px;"
                                      "margin-bottom:9px;}"
                                      "QSlider::handle:horizontal {"
                                      "width: 20px;"
                                      "height: 20px;"
                                      "background: rgb(61,107,229);"
                                      "border-radius:10px;}");
    systemVolumeDisplayLabel->setStyleSheet("font-size:18px;font-family:Noto Sans CJK SC;"
                                            "font-weight:400;"
                                            "color:rgba(255,255,255,0.91);"
                                            "line-height:28px;");
    //设置样式
    applicationLabel->setStyleSheet("QLabel{background:transparent;"
                                    "border:0px;"
                                    "color:rgba(255,255,255,0.97);"
                                    "font-weight:400;"
                                    "line-height:34px;"
                                    "font-family:Noto Sans CJK SC;"
                                    "font-size:20px;}");

    upWidget->setStyleSheet("QWidget{border-bottom: 1px solid rgba(255,255,255,0.08); }");
}

ApplicationVolumeWidget::~ApplicationVolumeWidget()
{
    delete  app_volume_list;
}
