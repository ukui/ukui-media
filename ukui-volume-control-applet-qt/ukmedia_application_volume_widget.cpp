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

ApplicationVolumeWidget::ApplicationVolumeWidget(QWidget *parent) : QWidget (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    applicationLabel = new QLabel(tr("Application Volume"),this);
//    noAppLabel = new QLabel(tr("No application is currently playing or recording audio"),displayAppVolumeWidget);
    app_volume_list = new QStringList;
//    appArea = new QScrollArea(this);
//    displayAppVolumeWidget = new QWidget(appArea);
//    appArea->setWidget(displayAppVolumeWidget);
//    gridlayout = new QGridLayout(displayAppVolumeWidget);

//    appArea->setFixedSize(358,288);
//    appArea->move(0,72);
//    displayAppVolumeWidget->resize(358,288);
//    displayAppVolumeWidget->move(0,72);
    applicationLabel->setFixedSize(160,18);
    applicationLabel->move(18,23);

    this->setFixedSize(358,320);

//    displayAppVolumeWidget->setObjectName("displayAppVolumeWidget");
//    displayAppVolumeWidget->setStyleSheet("QWidget#displayAppVolumeWidget{background:rgb(14,19,22);}");
    //设置样式
    applicationLabel->setStyleSheet("QLabel{background:transparent;"
                                    "border:0px;"
                                    "color:#ffffff;"
                                    "font-family:Noto Sans CJK SC;"
                                    "font-size:18px;}");


}

ApplicationVolumeWidget::~ApplicationVolumeWidget()
{
    delete  app_volume_list;
}
