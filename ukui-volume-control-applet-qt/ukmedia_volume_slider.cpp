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
    QSlider::initStyleOption(option);
}

UkmediaVolumeSlider::~UkmediaVolumeSlider()
{

}
