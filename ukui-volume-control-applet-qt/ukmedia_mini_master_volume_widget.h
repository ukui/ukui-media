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
#ifndef UKMEDIAMINIMASTERVOLUMEWIDGET_H
#define UKMEDIAMINIMASTERVOLUMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QKeyEvent>
#include "ukmedia_volume_slider.h"
#include "customstyle.h"

class UkmediaMiniMasterVolumeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaMiniMasterVolumeWidget(QWidget *parent = nullptr);
    ~UkmediaMiniMasterVolumeWidget();
    friend class DeviceSwitchWidget;
Q_SIGNALS:
    void mouse_wheel_signal(bool step);
    void keyboard_pressed_signal(int volumeGain);
private Q_SLOTS:
    void moveMiniSwitchBtnSlot();
private:
    QWidget *masterWidget;
    UkmediaVolumeSlider *masterVolumeSlider;
    QLabel *displayVolumeLabel;
//    QLabel *deviceLabel;
    QPushButton *muteBtn;
    UkuiMediaButton *switchBtn;

    QPushButton *deviceBtn;
    QLabel *deviceLabel;
    QComboBox *deviceCombox;
protected:
    bool event(QEvent *event);//重写窗口事件
    void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // UKMEDIAMINIMASTERVOLUMEWIDGET_H
