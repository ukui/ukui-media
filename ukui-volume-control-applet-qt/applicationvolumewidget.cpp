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
#include "applicationvolumewidget.h"
#include <QDebug>
#include <QScrollBar>
ApplicationVolumeWidget::ApplicationVolumeWidget(QWidget *parent) : QWidget (parent)
{
    applicationLabel = new QLabel(tr("Application Volume"),this);
    noAppLabel = new QLabel(tr("No application is currently playing or recording audio"),this);
//    qDebug() << 10;
    gridlayout = new QGridLayout(this);
    app_volume_list = new QStringList;
    applicationLabel->setFixedSize(160,16);
    noAppLabel->setFixedSize(300,14);
    applicationLabel->move(20,23);
    noAppLabel->move(60,123);
    qDebug() << "0000000000000000000" << gridlayout->geometry();
    this->setFixedSize(340,500);
    //设置样式
    applicationLabel->setStyleSheet("QLabel{background:transparent;"
                                          "border:0px;"
                                          "color:#ffffff;"
                                          "font-size:18px;}");
    noAppLabel->setStyleSheet("QLabel{background:transparent;"
                                "border:0px;"
                                "color:#ffffff;"
                                "font-size:14px;}");
}

ApplicationVolumeWidget::~ApplicationVolumeWidget()
{
    delete  app_volume_list;
}
