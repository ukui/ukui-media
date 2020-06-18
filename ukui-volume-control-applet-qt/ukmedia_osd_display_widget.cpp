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
#include "ukmedia_osd_display_widget.h"
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>

UkmediaOsdDisplayWidget::UkmediaOsdDisplayWidget(QWidget *parent) : QWidget (parent)
{
    osdWidget = new QWidget(this);
    iconButton = new QPushButton(this);
    iconButton->setFocusPolicy(Qt::NoFocus);
    UkmediaOsdDisplayWidgetInit();
    this->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Popup);
}

/*!
 * \brief
 * \details
 * 通过dbus获取任务栏位置
 */
int UkmediaOsdDisplayWidget::getPanelPosition(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelPosition", str);
    if (reply < 0)
        return 0;
    return reply;
}

/*!
 * \brief
 * \details
 * 通过dbus获取任务栏高度
 */
int UkmediaOsdDisplayWidget::getPanelHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    if (reply <= 0)
        return 46;
    return reply;
}

void UkmediaOsdDisplayWidget::UkmediaOsdDisplayWidgetInit()
{
    int positionX;
    int positionY;
    double scale;
    int screenWidth = qApp->primaryScreen()->size().width();
    int screenHeight = qApp->primaryScreen()->size().height();
    double width ,height;
    double temp;
    QVBoxLayout *hLayout = new QVBoxLayout();
    hLayout->addWidget(iconButton);
    osdWidget->setLayout(hLayout);
    width = screenWidth/640.0;
    height = screenHeight/480.0;
    scale = width > height ? height:width;
    temp = scale > 1 ? scale : 1;
    double size = temp * 130;
    int margin = (0.35*size)/2;
    osdWidget->layout()->setContentsMargins(margin,margin,margin,margin);
    this->setFixedSize(size,size);
    this->iconButton->setFixedSize(size*0.65,size*0.65);
    QSize iconSize(size*0.65,size*0.65);
    this->iconButton->setIconSize(iconSize);
    positionX = (screenWidth - this->osdWidget->width())/2;
    positionY = (screenHeight/2) + (screenHeight/2 - this->osdWidget->height())/2;

    this->move(positionX,positionY);
//    this->setStyleSheet("QWidget{background:rgba(255,0,0,0.7);}");
    this->osdWidget->setStyleSheet("QWidget{background:rgba(255,0,0,0);}");
    this->iconButton->setStyleSheet("QPushButton::hover{background:rgba(255,255,255,0);"
                                    "border-radius:4px;}"
                                    "QPushButton::pressed{background:rgba(61,107,229,0);"
                                    "border-radius:4px;padding-left:0px;}");
    this->setWindowOpacity(0.7);
}

void UkmediaOsdDisplayWidget::UkmediaOsdSetIcon(QString iconStr)
{
    QIcon icon = QIcon::fromTheme(iconStr);
    this->iconButton->setIcon(icon);
}

void UkmediaOsdDisplayWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    int rounded;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x13,0x14,0xB2)));
    p.setPen(Qt::NoPen);
    QPainterPath path;
    opt.rect.adjust(0,0,0,0);
    rounded = this->height()/10;
    path.addRoundedRect(opt.rect,rounded,rounded);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,rounded,rounded);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//    QWidget::paintEvent(event);
}


UkmediaOsdDisplayWidget::~UkmediaOsdDisplayWidget()
{

}
