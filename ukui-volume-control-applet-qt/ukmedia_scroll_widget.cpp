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
#include "ukmedia_scroll_widget.h"
#include <QVBoxLayout>
#include <QDebug>
//#include <QStyleOption>
//#include <QPainter>

//void ScrollWitget::paintEvent(QPaintEvent *event)
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


ScrollWitget::ScrollWitget(QWidget *parent) : QWidget (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    area = new QScrollArea(this);

    area->setFixedSize(358,320);
    area->move(0,0);
//    this->resize(358,320);

    area->setStyleSheet("QScrollArea{border:none;}");
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    area->viewport()->setStyleSheet("background-color:transparent;");
    area->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{margin:0px 0px 0px 0px;background:transparent;border:0px;width:2px;height:100px;}"
                                                    "QScrollBar::up-arrow:vertical{height:0px;}"
                                                    "QScrollBar::sub-line:vertical{border:0px solid;height:0px}"
                                                    "QScrollBar::sub-page:vertical{background:transparent;height:20px;}"
                                                    "QScrollBar::handle:vertical{background-color:rgba(255,255,255,0.25);"
                                                    "opacity:0.25;border-radius:1px;width:2px;}"
                                                    "QScrollBar::handle:vertical:hover{background-color:#3593b5;}"
                                                    "QScrollBar::handle:vertical:pressed{background-color:#3593b5;}"
                                                    "QScrollBar::add-page:vertical{background:transparent;height:20px;}"
                                                    "QScrollBar::add-line:vertical{border:0px solid;height:0px}"
                                                    "QScrollBar::down-arrow:vertical{height:0px;}");

}

ScrollWitget::~ScrollWitget()
{
    delete scrollBar;
}
