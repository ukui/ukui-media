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
#include "scrollwitget.h"
#include <QVBoxLayout>
#include <QDebug>
ScrollWitget::ScrollWitget(QWidget *parent) : QWidget (parent)
{
    area = new QScrollArea(this);

    area->setFixedSize(360,260);

    area->move(0,0);
    this->setFixedSize(360,300);
//    QVBoxLayout *layout = new QVBoxLayout(this);
//    layout->addWidget(scrollBar);
//    this->setLayout(layout);
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
