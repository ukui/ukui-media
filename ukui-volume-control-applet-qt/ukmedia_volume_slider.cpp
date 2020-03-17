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
#include "ukmedia_volume_slider.h"
#include <QPalette>
#include <QPainter>
#include <QWidget>
#include <QStyleOptionComplex>
#include <QDebug>

DisplayerMode displayMode = MINI_MODE;

MyCustomSlider::MyCustomSlider(QWidget *parent):QSlider(parent)
{
    this->setFixedSize(220,50);
    m_displayLabel=new QLabel(this);
    m_displayLabel->setFixedSize(QSize(42,30));
    //设置游标背景为白色
    m_displayLabel->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::white);
    m_displayLabel->setPalette(palette);

    m_displayLabel->setAlignment(Qt::AlignCenter);

    m_displayLabel->setVisible(false);
    m_displayLabel->move(0,3);
    m_displayLabel->setStyleSheet("QLabel{background:rgba(26,26,26,0.7);"
                                  "border:1px solid rgba(255, 255, 255, 0.2);"
                                  "border-radius:6px;}");
    setStyleSheet("QSlider::groove:horizontal {border: 0px solid #bbb;}"
                                          "QSlider::sub-page:horizontal {"
                                          "background: rgb(107,142,235);"
                                          "border-radius: 2px;"
                                          "margin-top:30px;"
                                          "margin-bottom:0px;}"
                                          "QSlider::add-page:horizontal {"
                                          "background:  rgba(52,70,80,90%);"
                                          "border: 0px solid #777;"
                                          "border-radius: 2px;"
                                          "margin-top:30px;"
                                          "margin-bottom:0px;}"
                                          "QSlider::handle:horizontal {"
                                          "width: 20px;"
                                          "height: 20px;"
                                          "margin-top:30px;"
                                          "margin-bottom:0px;"
                                          "background: rgb(61,107,229);"
                                          "border-radius:10px;}");
}

MyCustomSlider::~MyCustomSlider()
{

}

void MyCustomSlider::mousePressEvent(QMouseEvent *event)
{
    if(!m_displayLabel->isVisible())
      {
          m_displayLabel->setVisible(true);
          QString percent = QString::number(this->value());
          percent.append("%");
          m_displayLabel->setText(percent);
      }
//    Q_EMIT sliderMousePressSignal(this->value());
    QSlider::mousePressEvent(event);
}

void MyCustomSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_displayLabel->isVisible())
    {
        m_displayLabel->setVisible(false);
    }
//    Q_EMIT sliderMouseReleaseSignal();
    QSlider::mouseReleaseEvent(event);
}

void MyCustomSlider::mouseMoveEvent(QMouseEvent *event)
{
    QString percent = QString::number(this->value());
    percent.append("%");
    m_displayLabel->setText(percent);
    m_displayLabel->move((this->width()-m_displayLabel->width())*this->value()/(this->maximum()-this->minimum()),0);
//    qDebug() << "鼠标移动";
//    Q_EMIT sliderMouseMoveSignal(this->value(),this->width());
    QSlider::mouseMoveEvent(event);
}

UkuiMediaButton::UkuiMediaButton(QWidget *parent)
{
    Q_UNUSED(parent);
    this->setFixedSize(36,36);
}
UkuiMediaButton::~UkuiMediaButton()
{

}

void UkuiMediaButton::mouseMoveEvent(QMouseEvent *e)
{
   QToolButton::mouseMoveEvent(e);
}

void UkuiMediaButton::mousePressEvent(QMouseEvent *e)
{
    qDebug()<<"UkuiMediaButton::mousePressEvent----";
    if (displayMode == MINI_MODE) {
        Q_EMIT moveMiniSwitchBtnSignale();
        this->setFixedSize(34,34);
        QSize iconSize(14,14);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/complete-module.svg"));
    }
    else {
        this->setFixedSize(34,34);
        QSize iconSize(14,14);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/mini-module.svg"));
        Q_EMIT moveAdvanceSwitchBtnSignal();
    }
    QToolButton::mousePressEvent(e);
}

void UkuiMediaButton::mouseReleaseEvent(QMouseEvent *e)
{
    qDebug() << "UkuiMediaButton::mouseReleaseEvent";
    if (displayMode == MINI_MODE) {

        Q_EMIT miniToAdvanceSignal();
        this->setFixedSize(36,36);
        QSize iconSize(16,16);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/complete-module.svg"));
    }
    else {
        Q_EMIT advanceToMiniSignal();
        this->setFixedSize(36,36);
        QSize iconSize(16,16);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/mini-module.svg"));
    }
    QToolButton::mouseReleaseEvent(e);
}

UkmediaVolumeSlider::UkmediaVolumeSlider(QWidget *parent)
{
    Q_UNUSED(parent);  
}

void UkmediaVolumeSlider::initStyleOption(QStyleOptionSlider *option)
{
    qDebug() << "init style option";
    QSlider::initStyleOption(option);
}

UkmediaVolumeSlider::~UkmediaVolumeSlider()
{

}
