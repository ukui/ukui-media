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
#ifndef DEICESWITCHWIDGET_H
#define DEICESWITCHWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "scrollwitget.h"
#include "ukmediadevicewidget.h"
#include "applicationvolumewidget.h"
extern "C" {
#include <libmatemixer/matemixer.h>
#include <gtk/gtk.h>
#include <canberra-gtk.h>
#include <gio/gio.h>
}
#define MATE_DESKTOP_USE_UNSTABLE_API
#define VERSION "1.12.1"
#define GVC_APPLET_DBUS_NAME    "org.mate.VolumeControlApplet"
#define KEY_SOUNDS_SCHEMA   "org.mate.sound"

class DeviceSwitchWidget:public QWidget
{
    Q_OBJECT
public:
    DeviceSwitchWidget(QWidget *parent = nullptr);
    ~DeviceSwitchWidget();
    void deviceSwitchWidgetInit();
    static void list_device(DeviceSwitchWidget *w,MateMixerContext *context);

    static void gvc_stream_status_icon_set_control (UkmediaDeviceWidget *w,MateMixerStreamControl *control);
    static void context_set_property(DeviceSwitchWidget *object);//guint prop_id,const GValue *value,GParamSpec *pspec);
    static void on_context_state_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w);

    static void on_context_stream_added (MateMixerContext *context,const gchar *name,DeviceSwitchWidget  *w);
    static void on_context_stream_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);
    static void remove_stream (DeviceSwitchWidget *w, const gchar *name);

    static void add_stream (DeviceSwitchWidget *w, MateMixerStream *stream,MateMixerContext *context);
    static void add_application_control (DeviceSwitchWidget *w, MateMixerStreamControl *control);
    static void on_stream_control_added (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget  *w);
    static void on_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w);
    static void remove_application_control (DeviceSwitchWidget *w,const gchar *name);
    static void add_app_to_tableview(DeviceSwitchWidget *w,int appnum,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control);
    static void on_context_stored_control_added (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);
    static void update_app_volume (MateMixerStreamControl *control, GParamSpec *pspec ,DeviceSwitchWidget *w);

    static void on_context_device_added (MateMixerContext *context, const gchar *name, DeviceSwitchWidget *w);
    static void add_device (DeviceSwitchWidget *w, MateMixerDevice *device);

    static void on_context_device_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);

    static void on_context_default_input_stream_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w);
    static void set_input_stream (DeviceSwitchWidget *w, MateMixerStream *stream);
    static void on_stream_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w);

    static void on_context_default_output_stream_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w);

    static void on_context_stored_control_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);
    static void set_context(DeviceSwitchWidget *w,MateMixerContext *context);

    static void update_icon_input (UkmediaDeviceWidget *w,MateMixerContext *context);
    static void update_icon_output (UkmediaDeviceWidget *w,MateMixerContext *contetx);
    static void on_stream_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,UkmediaDeviceWidget *w);

    static void update_output_settings (DeviceSwitchWidget *w,MateMixerStreamControl *control);

    static void on_key_changed (GSettings *settings,gchar *key,DeviceSwitchWidget *w);


    static void set_output_stream (DeviceSwitchWidget *w, MateMixerStream *stream);
    static void update_output_stream_list(DeviceSwitchWidget *w,MateMixerStream *stream);

    static void bar_set_stream (DeviceSwitchWidget *w,MateMixerStream *stream);
    static void bar_set_stream_control (DeviceSwitchWidget *w,MateMixerStreamControl *control);
Q_SIGNALS:
    void app_volume_changed(bool is_mute,int volume,const gchar *app_name);

private Q_SLOTS:
    void app_slider_changed_slot(int volume);
    void app_volume_changed_slot(bool is_mute,int volume,const gchar *app_name);
    void output_volume_slider_changed_slot(int volume);
    void input_volume_slider_changed_slot(int volume);

public Q_SLOTS:
    void device_button_clicked_slot();
    void appvolume_button_clicked_slot();

private:
    QPushButton *deviceBtn;
    QPushButton *appVolumeBtn;

    ScrollWitget *devScrollWidget;
    ScrollWitget *appScrollWidget;
    QScrollArea *devArea;
    UkmediaDeviceWidget *devWidget;
    ApplicationVolumeWidget *appWidget;
    MateMixerStream *stream;
    MateMixerContext *context;
    MateMixerStreamControl *control;

    QStringList *soundlist;
    QStringList *device_name_list;
    QStringList *device_display_name_list;
    QStringList *output_stream_list;
    QStringList *input_stream_list;

    QStringList *stream_control_list;

    GSettings *sound_settings;

};

#endif // DEICESWITCHWIDGET_H