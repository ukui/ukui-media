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
#include "ukmedia_device_volume_widget.h"
#include "ukmedia_application_volume_widget.h"
#include "ukmedia_mini_master_volume_widget.h"
#include "ukmedia_osd_display_widget.h"

#include <QMenu>
#include <QCheckBox>
#include <QWidgetAction>
#include <QFrame>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusObjectPath>
#include <QDBusReply>
#include <QSystemTrayIcon>
#include <QProcess>
#include <QGSettings>
#include <QMediaPlayer>
#include <QTimer>
#include <QTime>
extern "C" {
#include <libmatemixer/matemixer.h>
#include <gio/gio.h>
#include <dconf/dconf.h>
#include <canberra.h>
}

#define SOUND_MODE_SCRIPTS "/usr/share/ukui-media/scripts/detection_output_mode.sh"
#define UKUI_THEME_SETTING "org.ukui.style"
#define UKUI_TRANSPARENCY_SETTING "org.ukui.control-center.personalise"
#define UKUI_THEME_NAME "style-name"
#define UKUI_THEME_WHITE "ukui-default"
#define UKUI_THEME_BLACK "ukui-dark"

//为平板模式提供设置音量值
#define UKUI_VOLUME_BRIGHTNESS_GSETTING_ID "org.ukui.quick-operation.panel"
#define UKUI_VOLUME_KEY "volumesize"

#define UKUI_PANEL_SETTING "org.ukui.panel.settings"
#define dsdasdasdasda
#define VERSION "1.12.1"
#define GVC_APPLET_DBUS_NAME    "org.mate.VolumeControlApplet"
#define KEY_SOUNDS_SCHEMA   "org.mate.sound"
#define TIMER_TIMEOUT   (2*1000)

class UkmediaTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    UkmediaTrayIcon(QWidget *parent = nullptr);
    ~UkmediaTrayIcon();

Q_SIGNALS:
    void wheelRollEventSignal(bool);

protected:
    bool event(QEvent *e) ;
};

class MyTimer : public QObject
{
  Q_OBJECT

public:
  MyTimer(QObject* parent = NULL);
  ~MyTimer();
  void  handleTimeout();  //超时处理函数
  virtual void timerEvent( QTimerEvent *event);
private:
  int m_nTimerID;
Q_SIGNALS:
  void timeOut();
};

class DeviceSwitchWidget:public QWidget
{
    Q_OBJECT
public:
    DeviceSwitchWidget(QWidget *parent = nullptr);
    ~DeviceSwitchWidget();
    void get_window_nameAndid();
    QList<char *> listExistsPath();
    QString findFreePath();
    void addValue(QString name,QString filename);
    void deviceSwitchWidgetInit();
    void systemTrayMenuInit();
    void showWindow();
    void hideWindow();
    void updateMicrophoneIcon(int volume,bool status);
    void updateSystemTrayIcon(int volume,bool status);
    int getPanelPosition(QString str);
    int getPanelHeight(QString str);
    void miniWidgetShow();
    void advancedWidgetShow();

    int getInputVolume();
    int getOutputVolume();
    bool getInputMuteStatus();
    bool getOutputMuteStatus();

    void inputVolumeDarkThemeImage(int value,bool status);
    void outputVolumeDarkThemeImage(int value,bool status);
    void drawImagColorFromTheme(UkuiButtonDrawSvg *button, QString iconStr);
    void themeChangeIcons();

    QString getAppName(QString desktopfp);
    QString getAppIcon(QString desktopfp);
    static void list_device(DeviceSwitchWidget *w,MateMixerContext *context);
    static void gvc_stream_status_icon_set_control (DeviceSwitchWidget *w,MateMixerStreamControl *control);
    static void context_set_property(DeviceSwitchWidget *object);
    static void on_context_state_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w);
    static void on_context_stream_added (MateMixerContext *context,const gchar *name,DeviceSwitchWidget  *w);
    static void on_context_stream_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);
    static void remove_stream (DeviceSwitchWidget *w, const gchar *name);
    static void add_stream (DeviceSwitchWidget *w, MateMixerStream *stream,MateMixerContext *context);
    static void add_application_control (DeviceSwitchWidget *w, MateMixerStreamControl *control,const gchar *name);
    static void on_stream_control_added (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget  *w);
    static void on_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w);
    static void remove_application_control (DeviceSwitchWidget *w,const gchar *name);
    static void add_app_to_appwidget(DeviceSwitchWidget *w,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control);
    static void on_context_stored_control_added (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);
    static void update_app_volume (MateMixerStreamControl *control, QString *pspec ,DeviceSwitchWidget *w);

    static void app_volume_mute (MateMixerStreamControl *control, QString *pspec ,DeviceSwitchWidget *w);
    static void on_context_device_added (MateMixerContext *context, const gchar *name, DeviceSwitchWidget *w);
    static void add_device (DeviceSwitchWidget *w, MateMixerDevice *device);
    static void on_context_device_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);

    static void on_context_default_input_stream_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w);
    static void set_input_stream (DeviceSwitchWidget *w, MateMixerStream *stream);
    static void on_stream_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w);

    static void on_context_default_output_stream_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w);
    static void on_context_stored_control_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w);
    static void set_context(DeviceSwitchWidget *w,MateMixerContext *context);

    static void update_icon_input (DeviceSwitchWidget *w,MateMixerStream *stream);
    static void update_icon_output (DeviceSwitchWidget *w,MateMixerContext *contetx);
    static void on_stream_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w);
    static void update_output_settings (DeviceSwitchWidget *w,MateMixerStreamControl *control);

    static void on_key_changed (GSettings *settings,gchar *key,DeviceSwitchWidget *w);
    static void set_output_stream (DeviceSwitchWidget *w, MateMixerStream *stream);
    static void update_output_stream_list(DeviceSwitchWidget *w,MateMixerStream *stream);

    static void bar_set_stream (DeviceSwitchWidget *w,MateMixerStream *stream);
    static void bar_set_stream_control (DeviceSwitchWidget *w,MateMixerDirection direction,MateMixerStreamControl *control);

    static void on_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w);
    void init_widget_action(QWidget* wid, QString iconstr, QString textstr);

    static void update_input_settings (DeviceSwitchWidget *w,MateMixerStreamControl *control);
    static void on_input_stream_control_added (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w);
    static void on_input_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w);
    static gboolean update_default_input_stream (DeviceSwitchWidget *w);
    friend class UkmediaSystemTrayIcon;
Q_SIGNALS:
    void app_volume_changed(bool isMute,int volume,QString app_name,QString appBtnName);
    void mouse_middle_clicked_signal();
    void mouse_wheel_signal(bool step);
    void app_name_signal(QString app_name);
    void system_muted_signal(bool status);
//    void appvolume_mute_change_mastervolume_status();
private Q_SLOTS:
    void deviceButtonClickedSlot();
    void appVolumeButtonClickedSlot();
    void activatedSystemTrayIconSlot(QSystemTrayIcon::ActivationReason reason);
    void jumpControlPanelSlot();
    void miniToAdvancedWidget();
    void advancedToMiniWidget();
    void deviceComboxIndexChanged(QString str);
    void moveAdvanceSwitchBtnSlot();
    void miniMastrerSliderChangedSlot(int value);
    void advancedSystemSliderChangedSlot(int value);
    void outputDeviceSliderChangedSlot(int value);
    void devWidgetMuteButtonClickedSlot();
    void miniWidgetMuteButtonClickedSlot();
    void appWidgetMuteButtonCLickedSlot();
    void muteCheckBoxReleasedSlot();
    void actionMuteTriggeredSLot();
    void mouseMeddleClickedTraySlot();
    void trayWheelRollEventSlot(bool step);
    void miniWidgetWheelSlot(bool step);
    void miniWidgetKeyboardPressedSlot(int volumeGain);
    void primaryScreenChangedSlot(QScreen *screen);
    void inputWidgetMuteButtonClicked();
    void inputWidgetSliderChangedSlot(int value);
    void ukuiThemeChangedSlot(const QString &themeStr);
    void osdDisplayWidgetHide();
private:

    QPushButton *deviceBtn;
    QPushButton *appVolumeBtn;
    UkuiMediaButton *switchToMiniBtn;
    QRect trayRect;
    UkmediaDeviceWidget *devWidget;
    ApplicationVolumeWidget *appWidget;
    UkmediaMiniMasterVolumeWidget *miniWidget;
    UkmediaOsdDisplayWidget *osdWidget;
    MateMixerStream *stream;
    MateMixerStream *input;
    MateMixerContext *context;
    MateMixerStreamControl *control;
    MateMixerStreamControl *m_pOutputBarStreamControl;
    MateMixerStreamControl *m_pInputBarStreamControl;

    QStringList *soundlist;
    QStringList *appBtnNameList;
    QStringList *device_name_list;
    QStringList *device_display_name_list;
    QStringList *output_stream_list;
    QStringList *input_stream_list;
    QStringList *stream_control_list;
    QStringList *app_name_list;

    MyTimer *timer;
    QMenu *menu;
    QGSettings *m_pThemeSetting;
    QGSettings *m_pTransparencySetting;

    QAction *m_pMuteAction;
    QAction *m_pSoundPreferenceAction;
    QString outputControlName;
    GSettings *sound_settings;
    UkmediaTrayIcon *soundSystemTrayIcon;
    QProcess *m_process;
    QString mThemeName;
    bool firstEnterSystem = true;
    ca_context *caContext;

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);//重写窗口事件
};

#endif // DEICESWITCHWIDGET_H
