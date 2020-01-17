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
#ifndef UKMEDIA_CONTROL_WIDGET_H
#define UKMEDIA_CONTROL_WIDGET_H

#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QEvent>
#include <QCoreApplication>
#include <QApplication>
#include <QMouseEvent>
#include <QDebug>
#include <QWidget>
#include <QWheelEvent>
#include <QGSettings/QGSettings>
extern "C" {
#include <libmatemixer/matemixer.h>
#include <gtk/gtk.h>
#include <canberra-gtk.h>
}
#define MATE_DESKTOP_USE_UNSTABLE_API
#define VERSION "1.12.1"
#define GVC_APPLET_DBUS_NAME    "org.mate.VolumeControlApplet"
#define KEY_SOUNDS_SCHEMA   "org.mate.sound"

typedef enum {
    SYSTEMTRAYICON_UNKNOW,  //未知的托盘图标类型
    SYSTEMTRAYICON_MICROPHONE, //麦克风托盘图标类型
    SYSTEMTRAYICON_VOLUME   //声音托盘图标类型
}SystemTrayIconType;

extern SystemTrayIconType trayIconType;

class UkmediaSlider : public QSlider
{
    Q_OBJECT
public:
    UkmediaSlider(QWidget *parent = nullptr);
    ~UkmediaSlider();

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

class UkmediaControlWidget : public QWidget
{
    Q_OBJECT
public:
    UkmediaControlWidget(QWidget *parent = nullptr);
    ~UkmediaControlWidget();
    void dockWidgetInit();
    void mute();
    void setFocus();
    void scrollUp();
    void scrollDown();

    int getIpVolume();
    bool getIpMuteStatus();
    void setIpVolume(int volume);

    int getOpVolume();
    bool getOpMuteStatus();
    void setOpVolume(int volume);

    void mateMixerInit();
    //声音输出托盘栏设置
    void outputVolumeNotify();
    void outputVolumeChanged();
    void getDefaultOutputStream();

    //麦克风托盘栏设置
    void inputVolumeNotify();
    void inputVolumeChanged();
    void getDefaultInputStream();

    void setIpSystemTrayIconVolume();
    void setOpSystemTrayIconVolume();

    void muteWidget(int volume,bool status);
    void setIpMuteButtonIcon(int volume);
    void setOpMuteButtonIcon(int volume);

    static void outputControlVolumeNotify(MateMixerStreamControl *control,
                                          GParamSpec *pspec, UkmediaControlWidget *p);
    static void inputControlVolumeNotify(MateMixerStreamControl *control,
                                         GParamSpec *pspec, UkmediaControlWidget *p);
    static void on_context_stream_added(MateMixerContext *context,
                                        const gchar      *name,
                                        UkmediaControlWidget *w);
    static void on_context_device_added(MateMixerContext *context, const gchar *name, UkmediaControlWidget *dialog);
    static void add_device (UkmediaControlWidget *dialog, MateMixerDevice *device);

    static void on_device_profile_active_option_notify (MateMixerDeviceSwitch *swtch,GParamSpec *pspec,UkmediaControlWidget *dialog);

    static MateMixerSwitch *find_device_profile_switch (MateMixerDevice *device);

    friend class UkmediaSystemTrayIcon;
    friend class UkmediaSystemTrayWidget ;
    friend class UkmediaIpSystemTrayWidget;
private:
    QPushButton *m_muteButton;
    QLabel *m_displayVolumeValue;
    UkmediaSlider *m_volumeSlider;
    MateMixerContext *ukuiContext;
    MateMixerStream *inputStream;
    MateMixerStream *outputStream;
    MateMixerStreamControl *outputControl;
    MateMixerStreamControl *inputControl;

Q_SIGNALS:
    void emitVolume(int);
    void valueChangedSignal(int);
    void micMixerMuteSignal(bool,guint);
    void soundMixerMuteSignal(bool,guint);
    void sliderSystemTrayIcon(SystemTrayIconType type);
    void updateSystemTrayIconSignal(int volume,SystemTrayIconType type,bool status);

public Q_SLOTS:
    void muteButtonClicked();
    void acceptOpVolume(int);
    void acceptIpVolume(int);
    void acceptOpMixerMute(bool,guint);
    void acceptIpMixerMute(bool,guint);
    void volumeSliderChanged(int volume);
    void acceptOpSystemTrayIconRoll(SystemTrayIconType type);
    void acceptIpSystemTrayIconRoll(SystemTrayIconType type);
    void acceptOpSystemTrayIconTriggered(SystemTrayIconType type);
    void acceptIpSystemTrayIconTriggered(SystemTrayIconType type);

protected:
//    bool event(QEvent *event);//重写窗口事件
};

#endif // UKMEDIA_CONTROL_WIDGET_H
