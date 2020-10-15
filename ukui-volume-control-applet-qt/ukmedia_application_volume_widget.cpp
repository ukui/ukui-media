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
#include <QStyleOption>
#include <QPainter>
ApplicationVolumeWidget::ApplicationVolumeWidget(QWidget *parent) : QWidget (parent)
{
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
    this->setAttribute(Qt::WA_TranslucentBackground);
}

//void ApplicationVolumeWidget::paintEvent(QPaintEvent *event)
//{
//    QStyleOption opt;
//    opt.init(this);
//    QPainter p(this);
////    double transparence = transparency * 255;
////    p.setBrush(this->palette().base());
//    p.setBrush(QBrush(QColor(19, 19, 20, 0)));
//    p.setPen(Qt::NoPen);
//    QPainterPath path;
//    opt.rect.adjust(0,0,0,0);
//    path.addRoundedRect(opt.rect,6,6);
//    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    p.drawRoundedRect(opt.rect,6,6);
//    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
////    QWidget::paintEvent(event);
//}

ApplicationVolumeWidget::~ApplicationVolumeWidget()
{
    delete  app_volume_list;
}
