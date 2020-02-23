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

class UkmediaDeviceSlider : public QSlider
{
    Q_OBJECT
public:
    UkmediaDeviceSlider(QWidget *parent = nullptr);
    ~UkmediaDeviceSlider();

protected:
    void mousePressEvent(QMouseEvent *ev)
    {
        //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
        QSlider::mousePressEvent(ev);
        //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
        double pos = ev->pos().x() / (double)width();
        setValue(pos *(maximum() - minimum()) + minimum());
        //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
        QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
        QCoreApplication::sendEvent(parentWidget(), &evEvent);
    }
};

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
    QLabel *outputDeviceLabel;
    QLabel *outputVolumeLabel;
    QLabel *outputDeviceDisplayLabel;
    QPushButton *outputDeviceBtn;
    UkmediaDeviceSlider *outputDeviceSlider;
    QWidget *inputWidget;
    QWidget *deviceWidget;
    QWidget *outputWidget;
    QWidget *outputSliderWidget;
    QWidget *outputDisplayWidget;
    QWidget *inputSliderWidget;
    QWidget *inputDisplayWidget;

    QLabel *inputDeviceLabel;
    QLabel *inputVolumeLabel;
    QLabel *inputDeviceDisplayLabel;
    QLabel *noInputDeviceLabel;
    QPushButton *inputDeviceBtn;
    UkmediaDeviceSlider *inputDeviceSlider;

};

#endif // UKMEDIADEVICEWIDGET_H
