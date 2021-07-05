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
#include "ukmedia_custom_sound.h"
#include "ukui_media_set_headset_widget.h"

#include <QMenu>
#include <QCheckBox>
#include <QWidgetAction>
#include <QMessageBox>
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
#include <QFrame>
#include <QDesktopWidget>
#include <QMap>

extern "C" {
#include <libmatemixer/matemixer.h>
#include <gio/gio.h>
#include <dconf/dconf.h>
#include <canberra.h>
#include <glib/gmain.h>
#include <pulse/ext-stream-restore.h>
#include <pulse/glib-mainloop.h>
#include <pulse/error.h>
#include <pulse/subscribe.h>
#include <pulse/introspect.h>
#include <pulse/pulseaudio.h>
}
#include <alsa/asoundlib.h>
#include <set>

#define UKUI_THEME_SETTING "org.ukui.style"
#define UKUI_TRANSPARENCY_SETTING "org.ukui.control-center.personalise"
#define UKUI_THEME_NAME "style-name"
#define UKUI_THEME_WHITE "ukui-default"
#define UKUI_THEME_BLACK "ukui-dark"
#define PORT_NUM 5

//为平板模式提供设置音量值
#define UKUI_VOLUME_BRIGHTNESS_GSETTING_ID "org.ukui.quick-operation.panel"
#define UKUI_VOLUME_KEY "volumesize"
#define UKUI_VOLUME_STATE "soundstate"

#define UKUI_PANEL_SETTING "org.ukui.panel.settings"
#define dsdasdasdasda
#define VERSION "1.12.1"
#define GVC_APPLET_DBUS_NAME    "org.mate.VolumeControlApplet"
#define KEY_SOUNDS_SCHEMA   "org.ukui.sound"
#define TIMER_TIMEOUT   (2*1000)

#define DBUS_NAME       "org.ukui.SettingsDaemon"
#define DBUS_PATH       "/org/ukui/SettingsDaemon/wayland"
#define DBUS_INTERFACE  "org.ukui.SettingsDaemon.wayland"

//第一次运行时初始化音量
#define UKUI_AUDIO_SCHEMA "org.ukui.audio"  //定制音量
#define FIRST_RUN "first-run" //是否第一次运行
#define HEADPHONE_OUTPUT_VOLUME "headphone-output-volume"
#define OUTPUT_VOLUME "output-volume"
#define MIC_VOLUME "mic-volume"
#define HDMI_VOLUME "hdmi-volume"
#define PLUGIN_INIT_VOLUME "plugin-init-volume"
#define INTEL_MIC "intel-mic"
#define REAR_MIC "rear-mic"
#define HEADSET_MIC "headset-mic"
#define LINEIN "linein"
#define SPEAKER "speaker"
#define HDMI "hdmi"
#define HEADPHONES_1 "headphones-1"
#define HEADPHONES_2 "headphones-2"

typedef struct {
        const pa_card_port_info *headphones;
        const pa_card_port_info *headsetmic;
        const pa_card_port_info *headphonemic;
        const pa_card_port_info *internalmic;
        const pa_card_port_info *internalspk;
} headset_ports;

class PortInfo {
public:

      QByteArray name;
      QByteArray description;
      uint32_t priority;
      int available;
      int direction;
      int64_t latency_offset;
      std::vector<QByteArray> profiles;

};

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
    Q_CLASSINFO("D-Bus Interface", "org.ukui.media")//ukui-media的dbus借口
public:
    DeviceSwitchWidget(QWidget *parent = nullptr);
    ~DeviceSwitchWidget();

    void get_window_nameAndid();
    void pulseDisconnectMseeageBox();
    QList<char *> listExistsPath();
    QString findFreePath();
    void addValue(QString name,QString filename);
    void deviceSwitchWidgetInit();
    void systemTrayMenuInit();
    void showWindow();
    void hideWindow();
    void updateSystemTrayIcon(int volume,bool status);
    MateMixerSwitch * findDeviceProfileSwitch(DeviceSwitchWidget *w,MateMixerDevice *device);
    static void onOutputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,DeviceSwitchWidget *w);
    static void onDeviceProfileActiveOptionNotify (MateMixerDeviceSwitch *swtch,GParamSpec *pspec,DeviceSwitchWidget *w);
    static void onInputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,DeviceSwitchWidget *w);
    void osdWidgetShow(const gchar *portName);
    void updateOutputDeviceLabel();
    void updateInputDeviceLabel();
    void setOutputLabelDummyOutput();

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
    void setVolumeSettingValue(int value);
    MateMixerSwitch* findStreamPortSwitch (DeviceSwitchWidget *widget,MateMixerStream *stream);

    QPixmap drawDarkColoredPixmap(const QPixmap &source);
    QPixmap drawLightColoredPixmap(const QPixmap &source);
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
    static void add_application_control (DeviceSwitchWidget *w, MateMixerStreamControl *control,const gchar *name,MateMixerDirection direction);
    static void on_stream_control_added (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget  *w);
    static void on_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w);
    static void remove_application_control (DeviceSwitchWidget *w,const gchar *name);
    static void add_app_to_appwidget(DeviceSwitchWidget *w,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control,MateMixerDirection direction);

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
    static void on_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w);
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

    int getScreenGeometry(QString methodName);
    void initSystemVolume();
    void initHuaweiAudio(DeviceSwitchWidget *w);

    //记录端口改变
    pa_context* get_context(void);
    void show_error(const char *txt);
    static void context_state_callback(pa_context *c, void *userdata);
    static gboolean connect_to_pulse(gpointer userdata);
    void setConnectingMessage(const char *string);
    static void ext_stream_restore_read_cb(pa_context *,const pa_ext_stream_restore_info *i,int eol,void *userdata);
    static void ext_stream_restore_subscribe_cb(pa_context *c, void *userdata);
    static void subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata);
    void updateCard(const pa_card_info &info);
    static void card_cb(pa_context *, const pa_card_info *i, int eol, void *userdata);

    static void updatePorts(DeviceSwitchWidget *w,const pa_card_info &info, std::map<QByteArray, PortInfo> &ports);

    void notifySend(QString arg,QString appName , QString appIconName,QString deviceName);

    //添加拔插headset提示
    static void check_audio_device_selection_needed (DeviceSwitchWidget *widget,MateMixerStreamControl *control,const pa_card_info *info);
    static gboolean verify_alsa_card (int cardindex,gboolean *headsetmic,gboolean *headphonemic);
    static headset_ports *get_headset_ports (MateMixerStreamControl *control,const pa_card_info *c);
    void free_priv_port_names (MateMixerStreamControl    *control);

    friend class UkmediaSystemTrayIcon;
Q_SIGNALS:
    void mouse_middle_clicked_signal();
    void mouse_wheel_signal(bool step);
    void app_name_signal(QString app_name);
    void theme_change();
    void font_change();
//    void appvolume_mute_change_mastervolume_status();
private Q_SLOTS:

    void priScreenChanged(int x, int y, int width, int height);
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
    void fontSizeChangedSlot(const QString &themeStr);
    void osdDisplayWidgetHide();
    void volumeSettingChangedSlot();
    void handleTimeout();
    void setHeadsetPort(QString );

private:

    QPushButton *deviceBtn;
    QPushButton *appVolumeBtn;
    UkuiMediaButton *switchToMiniBtn;
    QRect trayRect;
    UkmediaDeviceWidget *devWidget;
    ApplicationVolumeWidget *appWidget;
    UkmediaMiniMasterVolumeWidget *miniWidget;
    UkmediaOsdDisplayWidget *osdWidget;
    UkuiMediaSetHeadsetWidget *headsetWidget;

    MateMixerStream *stream;
    MateMixerStream *input;
    MateMixerContext *context;
    MateMixerStreamControl *control;
    MateMixerStreamControl *currentControl = nullptr;
    MateMixerSwitch *privOutputSwitch = nullptr;
    MateMixerStreamControl *m_pOutputBarStreamControl;
    MateMixerStreamControl *m_pInputBarStreamControl;

    QLine *dividLine;
    QStringList *eventList;
    QStringList *eventIdNameList;
    QStringList *soundlist;
    QStringList *appBtnNameList;
    QStringList *device_name_list;
    QStringList *device_display_name_list;
    QStringList *output_stream_list;
    QStringList *input_stream_list;
    QStringList *stream_control_list;
    QStringList *app_name_list;
    QStringList *m_pOutputPortList;
    QStringList judgetAppList;
    QVector<QStringList> appDisplayVector;  //存放app音量滑动条名称和按钮名称
    QVector<QMap<QString,QStringList>> appControlVector;  //QMap<应用名，(control1, control2)>  例如：<奇安信，(pulse-output-control-1,pulse-output-control-2)>


    QFrame *dividerFrame;
    MyTimer *timer;
    QMenu *menu;
    GSettings *m_pSoundSettings;
    QGSettings *m_pThemeSetting;
    QGSettings *m_pTransparencySetting;
    QGSettings *m_pVolumeSetting;
    QGSettings *m_pFontSetting;
    QGSettings *m_pInitSystemVolumeSetting;

    QAction *m_pMuteAction;
    QAction *m_pSoundPreferenceAction;
    QString outputControlName;
    GSettings *sound_settings;
    UkmediaTrayIcon *soundSystemTrayIcon;
    QProcess *m_process;
    QString mThemeName;
    bool firstEnterSystem = true;
    ca_context *caContext;
    bool setOutputVolume = false;
    bool setInputVolume = false;
    bool firstLoad = true;

    QDBusInterface  *mDbusXrandInter;

    void plug_IconChange(MateMixerSwitchOption *outputActivePort);

    int tag = 0;
    const gchar *isInputPortSame ;

    QTimer *m_pTimer;
    QTimer *m_pTimer2;
    bool mouseReleaseState = false;
    bool mousePress = false;
    CustomSound *customSoundFile;
    QDBusInterface *m_areaInterface;

    QByteArray role;
    QByteArray device;
    pa_channel_map channelMap;
    pa_cvolume volume;
    pa_context* m_paContext ;
    pa_mainloop_api* api;
    pa_ext_stream_restore_info info;

    int callBackCount = 0;
    bool firstEntry = true;
    bool hasSinks;
    bool hasSources;
    bool setBluezProfile = false;   //当输入切换到非蓝牙输入时需要将连接的蓝牙设备的profile设置成a2dp-sink
    QString bluezDeviceName;
    QByteArray activeProfile;
    QByteArray noInOutProfile;
    QByteArray lastActiveProfile;
    std::map<QByteArray, PortInfo> ports;
    std::vector< std::pair<QByteArray,QByteArray> > profiles;
    QMap<int, QString> cardMap;
    QMap<int, QString> outputPortNameMap;
    QMap<int, QString> inputPortNameMap;
    QMap<int, QString> outputPortLabelMap;
    QMap<int, QString> currentOutputPortLabelMap;
    QMap<int, QString> currentInputPortLabelMap;
    QMap<int, QString> inputPortLabelMap;
    QMap<QString, QString> profileNameMap;
    QMap<QString, QString> inputPortProfileNameMap;
    QMap<int, QList<QString>> cardProfileMap;
    QMap<int, QMap<QString,int>> cardProfilePriorityMap;
    QMap<QString,QString> inputCardStreamMap;
    QMap<QString,QString> outputCardStreamMap;



    int      headset_card;
    gboolean has_headsetmic;
    gboolean has_headphonemic;
    gboolean headset_plugged_in;
    char    *headphones_name;
    char    *headsetmic_name;
    char    *headphonemic_name;
    char    *internalspk_name;
    char    *internalmic_name;

    bool    shortcutMute = false;

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);//重写窗口事件

//    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // DEICESWITCHWIDGET_H
