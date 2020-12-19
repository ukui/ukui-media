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
#ifndef UKMEDIADEVICEWIDGET_H
#define UKMEDIADEVICEWIDGET_H
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QEvent>
#include <QMouseEvent>
#include <QCoreApplication>
#include "ukmedia_volume_slider.h"


class UkmediaDeviceWidget:public QWidget
{
    Q_OBJECT
public:
    UkmediaDeviceWidget(QWidget *parent = nullptr);
    ~UkmediaDeviceWidget();
    void noInputWidgetInit();
    void inputWidgetShow();
    void inputWidgetHide();
    friend class  DeviceSwitchWidget;
private:
    UkuiButtonDrawSvg *outputMuteBtn;
    QLabel *outputDeviceLabel;
    QLabel *outputDeviceDisplayLabel;
    QPushButton *outputDeviceBtn;
    UkmediaVolumeSlider *outputDeviceSlider;
    QWidget *inputWidget;
    QWidget *deviceWidget;
    QWidget *outputWidget;
    QWidget *outputSliderWidget;
    QWidget *inputSliderWidget;
    QLabel *outputDisplayVolumeLabel;

    QLabel *displayOutputLabel;
    QLabel *inputDeviceLabel;
    UkuiButtonDrawSvg *inputMuteButton;
    QLabel *inputDeviceDisplayLabel;
    QLabel *noInputDeviceLabel;
    QPushButton *inputDeviceBtn;
    UkmediaVolumeSlider *inputDeviceSlider;
Q_SIGNALS:
    void mouse_wheel_signal(bool step);
protected:
//    void wheelEvent(QWheelEvent *event);
//    void paintEvent(QPaintEvent *event);

};

#endif // UKMEDIADEVICEWIDGET_H
