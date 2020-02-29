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
//#include <QStyleOption>
//#include <QPainter>

//void ApplicationVolumeWidget::paintEvent(QPaintEvent *event)
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

ApplicationVolumeWidget::ApplicationVolumeWidget(QWidget *parent) : QWidget (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    applicationLabel = new QLabel(tr("Application Volume"),this);
    noAppLabel = new QLabel(tr("No application is currently playing or recording audio"),this);
    gridlayout = new QGridLayout(this);
    app_volume_list = new QStringList;
    applicationLabel->setFixedSize(160,18);
    noAppLabel->setFixedSize(300,14);
    applicationLabel->move(18,23);
    noAppLabel->move(60,153);
    this->setMinimumSize(358,320);
    this->setMaximumSize(358,500);
    //设置样式
    applicationLabel->setStyleSheet("QLabel{background:transparent;"
                                          "border:0px;"
                                          "color:#ffffff;"
                                          "font-size:18px;}");
    noAppLabel->setStyleSheet("QLabel{width:210px;"
                              "height:14px;"
                              "font-size:14px;"
                              "color:rgba(255,255,255,0.57);"
                              "line-height:28px;}");
}

ApplicationVolumeWidget::~ApplicationVolumeWidget()
{
    delete  app_volume_list;
}
