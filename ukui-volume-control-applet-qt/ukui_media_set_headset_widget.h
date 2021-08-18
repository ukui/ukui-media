/*
 * Copyright (C) 2021 KylinSoft Co., Ltd.
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
#ifndef UKUIMEDIASETHEADSETWIDGET_H
#define UKUIMEDIASETHEADSETWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QPushButton>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusObjectPath>
#include <QDBusReply>
#include <alsa/asoundlib.h>
#include "customstyle.h"

//设置声卡的key controls
#define LEVEL_BASIC (1<<0)
#define LEVEL_ID (1<<2)
#define HW_CARD "hw:0"

class UkmediaHeadsetButtonWidget : public QWidget
{
    Q_OBJECT
public:
   UkmediaHeadsetButtonWidget(QString icon,QString text);
   ~UkmediaHeadsetButtonWidget();

private:
   QLabel *iconLabel;
   QLabel *textLabel;
};

class UkuiMediaSetHeadsetWidget : public QWidget
{
    Q_OBJECT
public:
    UkuiMediaSetHeadsetWidget(QWidget *parent = nullptr);
    ~UkuiMediaSetHeadsetWidget();
    void initSetHeadsetWidget();
    int cset(char * name, char *card, char *c, int roflag, int keep_handle);
    void showControlId(snd_ctl_elem_id_t *id);
    int showControl(const char *space, snd_hctl_elem_t *elem,int level);
    void showWindow();
    friend class DeviceSwitchWidget;
private:
    QToolButton *headphoneIconButton;
    QToolButton *headsetIconButton;
    QToolButton *microphoneIconButton;
    QLabel *headphoneLabel;
    QLabel *headsetLabel;
    QLabel *microphoneLabel;

    QLabel *selectSoundDeviceLabel;
    QPushButton *soundSettingButton;
    QPushButton *cancelButton;

    bool isShow = false;
private Q_SLOTS:
    void headphoneButtonClickedSlot();
    void headsetButtonClickedSlot();
    void microphoneButtonClickedSlot();
    void soundSettingButtonClickedSlot();
    void cancelButtonClickedSlot();
protected:
    void paintEvent(QPaintEvent *event);
};


#endif // UKUIMEDIASETHEADSETWIDGET_H
