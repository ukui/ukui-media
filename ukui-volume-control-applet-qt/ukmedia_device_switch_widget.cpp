/*
 * Copyright (C) 2020 Tianjin KYLIN Information Technology Co., Ltd.
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
#include "ukmedia_device_switch_widget.h"
#include "ukmedia_monitor_window_thread.h"
extern "C" {
#include <glib-object.h>
#include <glib.h>
#include <gio/gio.h>
#include <gobject/gparamspecs.h>
#include <glib/gi18n.h>
#include <dconf/dconf.h>
}
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStringList>
#include <QSpacerItem>
#include <QListView>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QProcess>
#include <QApplication>
#include <QSvgRenderer>
#include <QScrollBar>
#include <QtCore/qmath.h>
#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QFrame>

#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.media.sound"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/sound/keybindings/"
#define KEY_SOUNDS_SCHEMA   "org.ukui.sound"
#define EVENT_SOUNDS_KEY "event-sounds"
#define MAX_CUSTOM_SHORTCUTS 1000

#define FILENAME_KEY "filename"
#define NAME_KEY "name"

double transparency = 0.8;
typedef enum {
    DEVICE_VOLUME_BUTTON,
    APP_VOLUME_BUTTON
}ButtonType;

ButtonType btnType = DEVICE_VOLUME_BUTTON;
guint appnum = 0;
int app_count = 0;
//bool isShow = true;
extern DisplayerMode displayMode ;
QString application_name;

struct profile_prio_compare {
    bool operator() (pa_card_profile_info2 const * const lhs, pa_card_profile_info2 const * const rhs) const {

        if (lhs->priority == rhs->priority)
            return strcmp(lhs->name, rhs->name) > 0;

        return lhs->priority > rhs->priority;
    }
};

UkmediaTrayIcon::UkmediaTrayIcon(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaTrayIcon::~UkmediaTrayIcon()
{

}

/*!
 * \brief
 * \details
 * 处理托盘的滚动事件
 */
bool UkmediaTrayIcon::event(QEvent *event)
{
    bool value = false;
    QWheelEvent *e = static_cast<QWheelEvent *>(event);
    if (event->type() == QEvent::Wheel) {
        if (e->delta() > 0) {
            value = true;
        }
        else if (e->delta() < 0) {
            value = false;
        }
        Q_EMIT  wheelRollEventSignal(value);
    }
    return QSystemTrayIcon::event(e);

}

/*!
 * \brief
 * \details
 * 绘制窗体的颜色及圆角
 */
void DeviceSwitchWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    double transparence = transparency * 255;
    QColor color = palette().color(QPalette::Base);
    color.setAlpha(transparence);
    QBrush brush = QBrush(color);
    p.setBrush(brush);
    p.setPen(Qt::NoPen);
    QPainterPath path;
    opt.rect.adjust(0,0,0,0);

    path.addRoundedRect(opt.rect,6,6);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*!
 * \brief
 * \details
 * 显示窗体
 */
void DeviceSwitchWidget::showWindow()
{
    this->show();
    //    isShow = false;
}

/*!
 * \brief
 * \details
 * 隐藏窗体
 */
void DeviceSwitchWidget::hideWindow()
{
    switch (displayMode) {
    case MINI_MODE:
        miniWidget->hide();
        break;
    case ADVANCED_MODE:
        this->hide();
        break;
    default:
        break;
    }
}


gboolean DeviceSwitchWidget::connect_to_pulse(gpointer userdata)
{
    //连接到pulseaudio
    pa_glib_mainloop *m = pa_glib_mainloop_new(g_main_context_default());
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);
    w->api = pa_glib_mainloop_get_api(m);

    pa_proplist *proplist = pa_proplist_new();
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_NAME, QObject::tr("PulseAudio Volume Control").toUtf8().constData());
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ID, "org.PulseAudio.pavucontrol");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_ICON_NAME, "audio-card");
    pa_proplist_sets(proplist, PA_PROP_APPLICATION_VERSION, "PACKAGE_VERSION");
    w->m_paContext = pa_context_new_with_proplist(w->api, nullptr, proplist);
    g_assert(w->m_paContext);

    pa_proplist_free(proplist);

    pa_context_set_state_callback(w->m_paContext, context_state_callback, w);
    if (pa_context_connect(w->m_paContext, nullptr, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        if (pa_context_errno(w->m_paContext) == PA_ERR_INVALID) {
            /*w->setConnectingMessage(QObject::tr("Connection to PulseAudio failed. Automatic retry in 5s\n\n"
                "In this case this is likely because PULSE_SERVER in the Environment/X11 Root Window Properties\n"
                "or default-server in client.conf is misconfigured.\n"
                "This situation can also arrise when PulseAudio crashed and left stale details in the X11 Root Window.\n"
                "If this is the case, then PulseAudio should autospawn again, or if this is not configured you should\n"
                "run start-pulseaudio-x11 manually.").toUtf8().constData());*/
            qFatal("connect pulseaudio failed")  ;
        }
        else {
            g_timeout_add_seconds(5,connect_to_pulse,w);
        }
    }

    return false;
}

DeviceSwitchWidget::DeviceSwitchWidget(QWidget *parent) : QWidget (parent)
{
    QDBusConnection::sessionBus().unregisterService("org.ukui.media");
    QDBusConnection::sessionBus().registerService("org.ukui.media");
    QDBusConnection::sessionBus().registerObject("/", this,QDBusConnection :: ExportAllSlots | QDBusConnection :: ExportAllSignals);
    qDebug()<<"dbus bind success!";
    eventList = new QStringList;
    eventIdNameList = new QStringList;
    eventList->append("window-close");
    eventList->append("system-setting");
    eventList->append("volume-changed");
    eventList->append("alert-sound");
    eventIdNameList->append("gudou");
    eventIdNameList->append("gudou");
    eventIdNameList->append("gudou");
    eventIdNameList->append("gudou");

    for (int i=0;i<eventList->count();i++) {
//        getValue();
        addValue(eventList->at(i),eventIdNameList->at(i));
    }

    //创建声音初始化记录文件
    customSoundFile = new CustomSound();
    customSoundFile->createAudioFile();

    //
    unsigned int uid = getuid();
    QString objpath = "/org/freedesktop/Accounts/User"+QString::number(uid);
    m_areaInterface = new QDBusInterface("org.freedesktop.Accounts",
                                         objpath,
                                         "org.freedesktop.Accounts.User",
                                         QDBusConnection::systemBus());

    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup);
    mThemeName = UKUI_THEME_WHITE;
    devWidget = new UkmediaDeviceWidget(this);
    connect(devWidget->outputDeviceSlider,&UkmediaVolumeSlider::silderPressSignal,this,[=](){
        mousePress = true;
        mouseReleaseState = false;
    });
    connect(devWidget->outputDeviceSlider,&UkmediaVolumeSlider::silderReleaseSignal,this,[=](){
        mouseReleaseState = true;
    });
    appWidget = new ApplicationVolumeWidget(this);//appScrollWidget->area);
    connect(appWidget->systemVolumeSlider,&UkmediaVolumeSlider::silderPressSignal,this,[=](){
        mousePress = true;
        mouseReleaseState = false;
    });
    connect(appWidget->systemVolumeSlider,&UkmediaVolumeSlider::silderReleaseSignal,this,[=](){
        mouseReleaseState = true;
    });
    miniWidget = new UkmediaMiniMasterVolumeWidget();

    connect(miniWidget->masterVolumeSlider,&UkmediaVolumeSlider::silderPressSignal,this,[=](){
        mousePress = true;
        mouseReleaseState = false;
    });
    connect(miniWidget->masterVolumeSlider,&UkmediaVolumeSlider::silderReleaseSignal,this,[=](){
        mouseReleaseState = true;
    });
    osdWidget = new UkmediaOsdDisplayWidget();
    headsetWidget = new UkuiMediaSetHeadsetWidget;
    soundSystemTrayIcon = new UkmediaTrayIcon(this);

    //为系统托盘图标添加菜单静音和声音首选项
    soundSystemTrayIcon->setToolTip(tr("Output volume control"));
#if (QT_VERSION <= QT_VERSION_CHECK(5,6,1))
    m_pMuteAction = new QAction(QIcon(""),tr("Mute"),this);
    m_pSoundPreferenceAction = new QAction(tr("Sound preference(S)"),this);
#elif (QT_VERSION > QT_VERSION_CHECK(5,6,1))
    m_pMuteAction = new QAction(QIcon(""),tr("Mute"));
    m_pSoundPreferenceAction = new QAction(tr("Sound preference(S)"));
#endif
    QString settingsIconStr = "document-page-setup-symbolic";
    QIcon settingsIcon = QIcon::fromTheme(settingsIconStr);
    m_pSoundPreferenceAction->setIcon(settingsIcon);

    soundSystemTrayIcon->setVisible(true);
    m_pInitSystemVolumeSetting = new QGSettings(UKUI_AUDIO_SCHEMA);
    dividerFrame = new QFrame(this);
    dividerFrame->setFrameShape(QFrame::NoFrame);
    dividerFrame->setFrameStyle(QFrame::VLine);
    dividerFrame->setFixedSize(1,320);
    dividerFrame->setParent(this);
    QPalette palette = dividerFrame->palette();
    QColor color = QColor(255,255,255);
    color.setAlphaF(0.08);
    palette.setColor(QPalette::WindowText, color);
    dividerFrame->setPalette(palette);
    dividerFrame->move(40,0);
    appWidget->appArea = new QScrollArea(appWidget);
    appWidget->displayAppVolumeWidget = new UkuiApplicationWidget(appWidget->appArea);
    appWidget->appArea->setWidget(appWidget->displayAppVolumeWidget);
    QPalette palette1 = appWidget->appArea->palette();
    palette1.setColor(QPalette::Window, QColor(0x00,0xff,0x00,0x00));  //改变appArea背景色透明
    appWidget->appArea->setPalette(palette1);
    appWidget->appArea->verticalScrollBar()->setProperty("drawScrollBarGroove",false);
//    appWidget->appArea->verticalScrollBar()->setFixedSize(2,70);       //设置垂直滑动条大小和背景色，因为设置完不美观，暂时注释掉
//    appWidget->appArea->verticalScrollBar()->setBackgroundRole(QPalette::Light);
    appWidget->displayAppVolumeWidget->setFixedWidth(340);
    appWidget->m_pVlayout = new QVBoxLayout(appWidget->displayAppVolumeWidget);

    appWidget->displayAppVolumeWidget->setAttribute(Qt::WA_TranslucentBackground);
    appWidget->appArea->setFixedSize(358,168);
    appWidget->appArea->move(0,143);

    appWidget->displayAppVolumeWidget->move(0,143);

    switchToMiniBtn = new UkuiMediaButton(this);
    switchToMiniBtn->setParent(this);

    switchToMiniBtn->setFlat(true);
    switchToMiniBtn->setCheckable(false);
    switchToMiniBtn->setProperty("useIconHighlightEffect",true);
    switchToMiniBtn->setProperty("iconHighlightEffectMode",true);
    
    switchToMiniBtn->setToolTip(tr("Go Into Mini Mode"));
    QSize switchSize(16,16);
    switchToMiniBtn->setIconSize(switchSize);
    switchToMiniBtn->setFixedSize(36,36);
    switchToMiniBtn->move(361,6);
    switchToMiniBtn->setIcon(QIcon("/usr/share/ukui-media/img/mini-module.svg"));

    output_stream_list = new QStringList;
    input_stream_list = new QStringList;
    device_name_list = new QStringList;
    device_display_name_list = new QStringList;
    stream_control_list = new QStringList;
    app_name_list = new QStringList;
    appBtnNameList = new QStringList;
    m_pOutputPortList = new QStringList;

    //监听主屏改变信号
    mDbusXrandInter = new QDBusInterface(DBUS_NAME,
                                         DBUS_PATH,
                                         DBUS_INTERFACE,
                                         QDBusConnection::sessionBus());
//    connect(mDbusXrandInter, SIGNAL(screenPrimaryChanged(int,int,int,int)),
//            this, SLOT(priScreenChanged(int,int,int,int)));

    ca_context_create(&caContext);
    /*!
     * \brief
     * \details
     * mixer初始化
     */
    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }
    /*!
     * \brief
     * \details
     * 创建context，这是重要的一环，因为声音的音量值、设备名称等各种信息
     * 都可以直接或间接的通过context获得
     */
    context = mate_mixer_context_new();
    mate_mixer_context_set_app_name (context,_("Ukui Volume Control App"));//设置app名
    mate_mixer_context_set_app_id(context, GVC_APPLET_DBUS_NAME);
    mate_mixer_context_set_app_version(context,VERSION);
    mate_mixer_context_set_app_icon(context,"ukuimedia-volume-control");

    /*!
     * \brief
     * \details
     * 打开context，如果context打开失败将导致无法加载声音
     */
    if G_UNLIKELY (mate_mixer_context_open(context) == FALSE) {
        qFatal("Failed to connect to a sound system");
    }

    appWidget->setFixedSize(358,320);
    devWidget->setFixedSize(358,320);

    devWidget->move(42,0);
    appWidget->move(42,0);
    this->setFixedSize(400,320);
    devWidget->show();
    appWidget->hide();

    /*!
     * \brief
     * \details
     * 菜单及设备界面的初始化
     */
    systemTrayMenuInit();
    deviceSwitchWidgetInit();

    connect(switchToMiniBtn,SIGNAL(moveAdvanceSwitchBtnSignal()),this,SLOT(moveAdvanceSwitchBtnSlot()));
    /*!
     * \brief
     * \details
     * mini模式下切换设备，为了减少用户使用的复杂度，暂时禁用此功能
     */
    /*connect(miniWidget->deviceCombox,SIGNAL(currentIndexChanged(QString)),this,SLOT(deviceComboxIndexChanged(QString)));*/
    context_set_property(this);
    g_signal_connect (G_OBJECT (context),
                      "notify::state",
                      G_CALLBACK (on_context_state_notify),
                      this);

    //获取声音gsettings值
    m_pSoundSettings = g_settings_new (KEY_SOUNDS_SCHEMA);

    //检测系统主题
    if (QGSettings::isSchemaInstalled(UKUI_THEME_SETTING)){
        m_pThemeSetting = new QGSettings(UKUI_THEME_SETTING);
        m_pFontSetting = new QGSettings(UKUI_THEME_SETTING);
        QString fontType;
        if (m_pThemeSetting->keys().contains("styleName")) {
            mThemeName = m_pThemeSetting->get(UKUI_THEME_NAME).toString();
        }
        if (m_pFontSetting->keys().contains("systemFont")) {
            fontType = m_pFontSetting->get("systemFont").toString();
        }
        if (m_pFontSetting->keys().contains("systemFontSize")) {
            int font = m_pFontSetting->get("system-font-size").toInt();
            QFont fontSize(fontType,font);
            devWidget->outputDeviceDisplayLabel->setFont(fontSize);
            appWidget->systemVolumeLabel->setFont(fontSize);
            devWidget->inputDeviceDisplayLabel->setFont(fontSize);
        }
        connect(m_pFontSetting,SIGNAL(changed(const QString &)),this,SLOT(fontSizeChangedSlot(const QString &)));
        connect(m_pThemeSetting, SIGNAL(changed(const QString &)),this,SLOT(ukuiThemeChangedSlot(const QString &)));
    }

    //获取透明度
    if (QGSettings::isSchemaInstalled(UKUI_TRANSPARENCY_SETTING)){
        m_pTransparencySetting = new QGSettings(UKUI_TRANSPARENCY_SETTING);
        if (m_pTransparencySetting->keys().contains("transparency")) {
            transparency = m_pTransparencySetting->get("transparency").toInt();
        }
    }

    //给侧边栏提供音量之设置
    if (QGSettings::isSchemaInstalled(UKUI_VOLUME_BRIGHTNESS_GSETTING_ID)) {
        m_pVolumeSetting = new QGSettings(UKUI_VOLUME_BRIGHTNESS_GSETTING_ID);

        connect(m_pVolumeSetting,SIGNAL(changed(const QString &)),this,SLOT(volumeSettingChangedSlot()));
    }

    //如果为第一次运行需要关闭dp对应的配置文件
    if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){
        m_pInitSystemVolumeSetting = new QGSettings(UKUI_AUDIO_SCHEMA);
        if (m_pInitSystemVolumeSetting->keys().contains("firstRun")) {
            bool isFirstRun = m_pInitSystemVolumeSetting->get(FIRST_RUN).toBool();
            if (isFirstRun) {
                //system("/usr/lib/pulse-13.99.1/scripts/kylin_hdmi_sound_toggle.sh");
                m_pInitSystemVolumeSetting->set(FIRST_RUN,false);
            }
                // initSystemVolume();
        }
    }

    UkmediaMonitorWindowThread *m_pThread = new UkmediaMonitorWindowThread();
    m_pThread->start();

    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    m_pTimer2 = new QTimer(this);
    connect(m_pTimer2, &QTimer::timeout, this, [=](){
        if(mouseReleaseState){
            gint retval;
            const gchar *desc = "Volume Changed";
            QString filenameStr;
            QList<char *> existsPath = this->listExistsPath();
            for (char * path : existsPath) {
                char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
                char * allpath = strcat(prepath, path);
                const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
                const QByteArray bba(allpath);
                if(QGSettings::isSchemaInstalled(ba))
                {
                    QGSettings * settings = new QGSettings(ba, bba);
                    filenameStr = settings->get(FILENAME_KEY).toString();
                    QString nameStr = settings->get(NAME_KEY).toString();
                    if (nameStr == "volume-changed") {
                        break;
                    }
                }
            }
            const QByteArray text = filenameStr.toLocal8Bit();
            const gchar *id = text.data();
            const gchar *eventId =id;
            if(desc){
                retval = ca_context_play (this->caContext, 0,
                                          CA_PROP_EVENT_ID, eventId,
                                          CA_PROP_EVENT_DESCRIPTION, desc, NULL);
            }
            m_pTimer->stop();
            mousePress = false;
            mouseReleaseState = false;
        }
        else{
            m_pTimer2->start(100);
        }
    });
    /*!
     * \brief
     * \details
     * mini模式下,当滑动条值改变时更改系统音量
     */
    connect(miniWidget->masterVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(miniMastrerSliderChangedSlot(int)));

    /*!
     * \brief
     * \details
     * 完整模式下,应用音量节面，当滑动条值改变时更改系统音量
     */
    connect(appWidget->systemVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT( advancedSystemSliderChangedSlot(int)));
    /*!
     * \brief
     * \details
     * 完整模式下,系统音量界面，当滑动条值改变时更改系统音量
     */
    connect(devWidget->outputDeviceSlider,SIGNAL(valueChanged(int)),this,SLOT(outputDeviceSliderChangedSlot(int)));
    /*!
     * \brief
     * \details
     * mini模式切换到完整模式
     */
    connect(miniWidget->switchBtn,SIGNAL(miniToAdvanceSignal()),this,SLOT(miniToAdvancedWidget()));
    /*!
     * \brief
     * \details
     * 完整模式切换到mini模式
     */
    connect(switchToMiniBtn,SIGNAL(advanceToMiniSignal()),this,SLOT(advancedToMiniWidget()));
    /*!
     * \brief
     * \details
     * 完整模式下,系统音量和应用音量的切换
     */
    connect(deviceBtn,SIGNAL(clicked()),this,SLOT(deviceButtonClickedSlot()));
    connect(appVolumeBtn,SIGNAL(clicked()),this,SLOT(appVolumeButtonClickedSlot()));
    /*!
     * \brief
     * \details
     * 一系列的静音控制功能
     */
    connect(devWidget->outputMuteBtn,SIGNAL(clicked()),this,SLOT(devWidgetMuteButtonClickedSlot()));
    connect(miniWidget->muteBtn,SIGNAL(clicked()),this,SLOT(miniWidgetMuteButtonClickedSlot()));
    connect(appWidget->systemVolumeBtn,SIGNAL(clicked()),this,SLOT(appWidgetMuteButtonCLickedSlot()));
    connect(m_pMuteAction,SIGNAL(triggered()),this,SLOT(actionMuteTriggeredSLot()));
    /*!
     * \brief
     * \details
     * 鼠标中间滚轮键设置音量静音
     */
    connect(this,SIGNAL(mouse_middle_clicked_signal()),this,SLOT(mouseMeddleClickedTraySlot()));
    /*!
     * \brief
     * \details
     * 当无任何控制音量的界面显示时，鼠标滚轮向上滚动音量托盘图标增加音量
     * 向下滚动减少音量值
     */
    connect(soundSystemTrayIcon,SIGNAL(wheelRollEventSignal(bool)),this,SLOT(trayWheelRollEventSlot(bool)));
    /*!
     * \brief
     * \details
     * mini界面显示时，鼠标滚轮控制音量
     */
    connect(miniWidget,SIGNAL(mouse_wheel_signal(bool)),this,SLOT(miniWidgetWheelSlot(bool)));
    /*!
     * \brief
     * \details
     * mini模式下,上下左右键控制音量值
     */
    connect(miniWidget,SIGNAL(keyboard_pressed_signal(int)),this,SLOT(miniWidgetKeyboardPressedSlot(int)));
    /*!
     * \brief
     * \details
     * 完整模式下，输入音量的静音功能
     */
    connect(devWidget->inputMuteButton,SIGNAL(clicked()),this,SLOT(inputWidgetMuteButtonClicked()));
    connect(devWidget->inputDeviceSlider,SIGNAL(valueChanged(int)),this,SLOT(inputWidgetSliderChangedSlot(int)));

    //主屏改变
    connect(qApp,SIGNAL(primaryScreenChanged(QScreen *)),this,SLOT(primaryScreenChangedSlot(QScreen *)));

    connect(soundSystemTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),\
            this,SLOT(activatedSystemTrayIconSlot(QSystemTrayIcon::ActivationReason)));
    connect(m_pSoundPreferenceAction,SIGNAL(triggered()),this,SLOT(jumpControlPanelSlot()));

    QDBusConnection::sessionBus().connect( QString(), "/", "org.ukui.media", "DbusSignalHeadsetJack", this, SLOT(setHeadsetPort(QString)));

    trayRect = soundSystemTrayIcon->geometry();
    appWidget->displayAppVolumeWidget->setLayout(appWidget->m_pVlayout);

    this->setObjectName("mainWidget");
    appWidget->setObjectName("appWidget");
    appWidget->appArea->setFrameShape(QFrame::NoFrame);//bjc去掉appArea的边框
//    appWidget->appArea->setStyleSheet("QScrollArea{border:none;}");
    appWidget->appArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    appWidget->appArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    appWidget->appArea->setAttribute(Qt::WA_TranslucentBackground);

    if (appnum <= 0) {
        appWidget->upWidget->hide();
    }
    else {
        appWidget->upWidget->show();
    }

}

/*!
 * \brief
 * \details
 * 初始化音量
 */
void DeviceSwitchWidget::initSystemVolume()
{
    //是否初始化音量
    if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){
        if (m_pInitSystemVolumeSetting->keys().contains("headphoneOutputVolume")) {
            int headphoneVolume = m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
            qDebug() << "init headphone volume "  << headphoneVolume ;
        }
        if (m_pInitSystemVolumeSetting->keys().contains("outputVolume")) {
            int outputVolume = m_pInitSystemVolumeSetting->get(OUTPUT_VOLUME).toInt();
            devWidget->outputDeviceSlider->setValue(outputVolume);
            qDebug() << "init speaker volume "  << outputVolume;
        }
        if (m_pInitSystemVolumeSetting->keys().contains("micVolume")) {
            int micVolume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
            devWidget->inputDeviceSlider->setValue(micVolume);
            qDebug() << "init mic volume"  << micVolume;
        }
        if (m_pInitSystemVolumeSetting->keys().contains("pluginInitVolume")) {
            bool neddInit = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toBool();
            //            devWidget->inputDeviceSlider->setValue();

        }
    }
}

void DeviceSwitchWidget::initHuaweiAudio(DeviceSwitchWidget *w)
{
//    //使用 pactl list sinks 命令判断是耳机还是板载扬声器
//    //获取当前语言
//    bool isSpeaker = true;
//    QString language("信宿 #");
//    QString actPort("活动端口：");
//    if(w->m_areaInterface->property("Language").toString() == "en_US"){
//        language = "Sink #";
//        actPort = "Active Port:";
//    }else{
//        language = "信宿 #";
//        actPort = "活动端口：";
//    }
//    QProcess process;
//    process.start("pactl list sinks");
//    process.waitForFinished();
//    QString sinkMsg = process.readAllStandardOutput();
//    QStringList sinkMsgList =sinkMsg.split(language);
//    QString activePort("analog-output-speaker-huawei");
//    for(int i=0;i<sinkMsgList.size();i++){
//        QString curSink = sinkMsgList.at(i);
//        if(curSink.contains("RUNNING") && !curSink.contains("histen_sink")){//处于运行状态但名称不是‘histen_sink’的sink
//            if(curSink.contains(actPort + "analog-output-headphones-huawei")){
//                isSpeaker = false;
//                activePort = "analog-output-headphones-huawei"; //活动端口为耳机
//                break;
//            }
//            else if(curSink.contains(actPort + "analog-output-speaker-huawei")){
//                isSpeaker = true;
//                activePort = "analog-output-speaker-huawei";  //活动端口是扬声器
//                break;
//            }
//        }
//    }
//    //根据活动端初始化音量
//    if(isSpeaker){
//        if(!w->customSoundFile->isExist(activePort)){  //histen端口的话，使用当前活动端口作为初始化依据
//            if (w->m_pInitSystemVolumeSetting->keys().contains("outputVolume")) {
//                int outputVolume = w->m_pInitSystemVolumeSetting->get(OUTPUT_VOLUME).toInt();
//                w->miniWidget->masterVolumeSlider->setValue(outputVolume);
//                w->customSoundFile->addXmlNode(activePort,false);
//                qDebug()<<"初始化扬声器";
//            }
//        }
//    }
//    else{
//        if(!w->customSoundFile->isExist(activePort)){
//            if (w->m_pInitSystemVolumeSetting->keys().contains("headphoneOutputVolume"))
//            {
//                int outputVolume = w->m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
//                w->miniWidget->masterVolumeSlider->setValue(outputVolume);
//                w->customSoundFile->addXmlNode(activePort,false);
//                qDebug()<<"初始化耳机";
//            }
//        }
//    }
}

/*!
 * \brief
 * \details
 * 菜单的初始化
 */
void DeviceSwitchWidget::systemTrayMenuInit()
{
    menu = new QMenu();
    menu->setAttribute(Qt::WA_NoMouseReplay);
    soundSystemTrayIcon->setContextMenu(menu);
    //设置右键菜单
    menu->addAction(m_pMuteAction);
    menu->addAction(m_pSoundPreferenceAction);
}

/*!
 * \brief
 * \details
 * mini模式下，滑动条控制输出音量
 */
void DeviceSwitchWidget::miniMastrerSliderChangedSlot(int value)
{
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    MateMixerStreamControl *control;
    if (MATE_MIXER_IS_STREAM(stream))
        control = mate_mixer_stream_get_default_control(stream);
    else
        qDebug()<<"miniMastrerSliderChangedSlot:stream invalid!";
    if(!MATE_MIXER_IS_STREAM_CONTROL(control))
        qDebug()<<"miniMastrerSliderChangedSlot:control invalid!";
    QString percent;
    percent = QString::number(value);
    //音量值改变时添加提示音
    if (firstEnterSystem != true) {
        mate_mixer_stream_control_set_mute(control,FALSE);
    }
    int volume = value*65536/100;
    qDebug() << "set output volume:" << value << mate_mixer_stream_control_get_name(control);
    bool ret = mate_mixer_stream_control_set_volume(control,guint(volume));
    //异常处理
    if(!ret){
        qDebug()<<"miniMastrerSliderChangedSlot:异常处理";
        int volume = mate_mixer_stream_control_get_volume(control);
        QString cmd = "pactl set-sink-volume "+ QString(mate_mixer_stream_control_get_name(control)) +" "+ QString::number(volume,10);
        system(cmd.toLocal8Bit().data());
    }
    appWidget->systemVolumeSlider->blockSignals(true);
    devWidget->outputDeviceSlider->blockSignals(true);
    miniWidget->displayVolumeLabel->setText(percent+"%");
    appWidget->systemVolumeSlider->setValue(value);
    devWidget->outputDeviceSlider->setValue(value);
    appWidget->systemVolumeSlider->blockSignals(false);
    devWidget->outputDeviceSlider->blockSignals(false);
    themeChangeIcons();
    firstEnterSystem = false;
}

/*!
 * \brief
 * \details
 * 完整模式下，应用音量选项中系统音量控制输出音量值
 */
void DeviceSwitchWidget::advancedSystemSliderChangedSlot(int value)
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    QString percent;
    percent = QString::number(value);
    if (firstEnterSystem != true) {
        mate_mixer_stream_control_set_mute(control,FALSE);
    }
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(control,guint(volume));
    appWidget->systemVolumeSlider->blockSignals(true);
    miniWidget->masterVolumeSlider->blockSignals(true);
    miniWidget->masterVolumeSlider->setValue(value);
    appWidget->systemVolumeDisplayLabel->setText(percent+"%");
    appWidget->systemVolumeSlider->blockSignals(false);
    miniWidget->masterVolumeSlider->blockSignals(false);
}

/*!
 * \brief
 * \details
 * 完整模式下，设备音量中滑动条改变控制输出音量
 */
void DeviceSwitchWidget::outputDeviceSliderChangedSlot(int value)
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    QString percent;

    percent = QString::number(value);
    if (firstEnterSystem != true) {
        mate_mixer_stream_control_set_mute(control,FALSE);
    }
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(control,guint(volume));
    miniWidget->masterVolumeSlider->setValue(value);
}

/*!
 * \brief
 * \details
 * 完整模式下，点击静音按钮控制输出音量的静音状态
 */
void DeviceSwitchWidget::devWidgetMuteButtonClickedSlot()
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
    }
    else {
        status =true;
        QString muteActionIconStr = "object-select-symbolic";
        QIcon muteActionIcon = QIcon::fromTheme(muteActionIconStr);
        m_pMuteAction->setIcon(muteActionIcon);
        mate_mixer_stream_control_set_mute(control,status);
    }
    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * mini模式下静音按钮的控制静音状态
 */
void DeviceSwitchWidget::miniWidgetMuteButtonClickedSlot()
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
    }
    else {
        status =true;
        QString muteActionIconStr = "object-select-symbolic";
        QIcon muteActionIcon = QIcon::fromTheme(muteActionIconStr);
        m_pMuteAction->setIcon(muteActionIcon);
        mate_mixer_stream_control_set_mute(control,status);
    }

    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 完整模式中应用音量的静音控制
 */
void DeviceSwitchWidget::appWidgetMuteButtonCLickedSlot()
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
    }
    else {
        status =true;
        QString muteActionIconStr = "object-select-symbolic";
        QIcon muteActionIcon = QIcon::fromTheme(muteActionIconStr);
        m_pMuteAction->setIcon(muteActionIcon);
        mate_mixer_stream_control_set_mute(control,status);
    }
    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 菜单静音勾选框设置静音
 */
void DeviceSwitchWidget::muteCheckBoxReleasedSlot()
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
    }
    else {
        status =true;
        QString muteActionIconStr = "object-select-symbolic";
        QIcon muteActionIcon = QIcon::fromTheme(muteActionIconStr);
        m_pMuteAction->setIcon(muteActionIcon);
        mate_mixer_stream_control_set_mute(control,status);
    }
    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 右键菜单的静音选项点击
 */
void DeviceSwitchWidget::actionMuteTriggeredSLot()
{
    bool isMute = false;
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (MATE_MIXER_IS_STREAM(stream)) {
        control = mate_mixer_stream_get_default_control(stream);
    }
    else{
        qDebug()<<"actionMuteTriggeredSLot:stream Invalid";
    }
    if(!MATE_MIXER_IS_STREAM_CONTROL(control)){
        qDebug()<<"actionMuteTriggeredSLot:control Invalid";
    }
    isMute = mate_mixer_stream_control_get_mute(control);
    int opVolume = int(mate_mixer_stream_control_get_volume(control));
    opVolume = int(opVolume*100/65536.0 + 0.5);
    if (isMute) {
        m_pMuteAction->setIcon(QIcon(""));
        bool ret = mate_mixer_stream_control_set_mute(control,FALSE);
        //异常处理：针对偶现蓝牙静音不生效的问题
        if(!ret){
            qDebug()<<"Exception handling -- Unmute";
            //使用命令重新设置音量
            //pactl set-sink-mute bluez_sink.E3_11_30_92_5E_C8.a2dp_sink true
            QString cmd = "pactl set-sink-mute "+ QString(mate_mixer_stream_control_get_name(control)) +" false";
            system(cmd.toLocal8Bit().data());
        }
    }
    else {
        QString muteActionIconStr = "object-select-symbolic";
        QIcon muteActionIcon = QIcon::fromTheme(muteActionIconStr);
        m_pMuteAction->setIcon(muteActionIcon);
        bool ret2 = mate_mixer_stream_control_set_mute(control,TRUE);
        //异常处理：针对偶现蓝牙静音不生效的问题
        if(!ret2){
            qDebug()<<"Exception handling -- Mute";
            //使用命令重新设置音量
            //pactl set-sink-mute bluez_sink.E3_11_30_92_5E_C8.a2dp_sink true
            QString cmd = "pactl set-sink-mute "+ QString(mate_mixer_stream_control_get_name(control)) +" true";
            system(cmd.toLocal8Bit().data());
        }
    }
    isMute = mate_mixer_stream_control_get_mute(control);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 鼠标滚轮点击托盘图标，设置输出音量的静音状态
 */
void DeviceSwitchWidget::mouseMeddleClickedTraySlot()
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    bool isMute = mate_mixer_stream_control_get_mute(control);
    int opVolume = int(mate_mixer_stream_control_get_volume(control));
    opVolume = int(opVolume*100/65536.0 + 0.5);
    if (isMute) {
        mate_mixer_stream_control_set_mute(control,FALSE);
    }
    else {
        mate_mixer_stream_control_set_mute(control,TRUE);
    }
    isMute = mate_mixer_stream_control_get_mute(control);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 鼠标滚轮在声音托盘上滚动设置输出音量值
 */
void DeviceSwitchWidget::trayWheelRollEventSlot(bool step)
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    if (step) {
        miniWidget->masterVolumeSlider->setValue(volume+5);
        devWidget->outputDeviceSlider->setValue(volume+5);
    }
    else {
        miniWidget->masterVolumeSlider->setValue(volume-5);
        devWidget->outputDeviceSlider->setValue(volume-5);
    }
}

/*!
 * \brief
 * \details
 * mini界面显示时，在窗口外滚动鼠标滚轮控制音量值
 */
void DeviceSwitchWidget::miniWidgetWheelSlot(bool step)
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    if (step) {
        miniWidget->masterVolumeSlider->setValue(volume+5);
    }
    else {
        miniWidget->masterVolumeSlider->setValue(volume-5);
    }
}

/*!
 * \brief
 * \details
 * mini模式下上下左右键控制音量
 */
void DeviceSwitchWidget::miniWidgetKeyboardPressedSlot(int volumeGain)
{
    MateMixerStreamControl *control;
    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    miniWidget->masterVolumeSlider->setValue(volume+volumeGain);
}


/*!
 * \brief
 * \details
 * 点击菜单中声音设置跳转到控制面板的声音控制
 */
void DeviceSwitchWidget::jumpControlPanelSlot()
{
    m_process = new QProcess(0);
    m_process->start("ukui-control-center -s");
    m_process->waitForStarted();
    return;
}

/*!
 * \brief
 * \details
 * 按下时按钮偏移1px，达到动画效果
 */
void DeviceSwitchWidget::moveAdvanceSwitchBtnSlot()
{
    switchToMiniBtn->move(362,7);
}

/*!
 * \brief
 * \details
 * mini切换到完整模式
 */
void DeviceSwitchWidget::miniToAdvancedWidget()
{
    miniWidget->switchBtn->resize(34,34);
    QSize iconSize(14,14);
    miniWidget->switchBtn->setIconSize(iconSize);
    miniWidget->switchBtn->setIcon(QIcon("/usr/share/ukui-media/img/complete-module.svg"));
    advancedWidgetShow();

    miniWidget->hide();
    displayMode = ADVANCED_MODE;
}

/*!
 * \brief
 * \details
 * 从完整模式切换到mini模式
 */
void DeviceSwitchWidget::advancedToMiniWidget()
{
    miniWidgetShow();
    switchToMiniBtn->hide();
    this->hide();
    switchToMiniBtn->isCheckable();
    displayMode = MINI_MODE;
}

/*!
 * \brief
 * \details
 * mini模式下切换输出设备，由于考虑到用户使用的简便性，禁用此功能
 */
void DeviceSwitchWidget::deviceComboxIndexChanged(QString str)
{
    MateMixerBackendFlags flags;
    int index = miniWidget->deviceCombox->findText(str);
    if (index == -1)
        return;
    const QString str1 =  output_stream_list->at(index);
    const gchar *name = str1.toLocal8Bit();
    stream = mate_mixer_context_get_stream(context,name);
    if (G_UNLIKELY (stream == nullptr)) {
        g_warn_if_reached ();
        return;
    }
    flags = mate_mixer_context_get_backend_flags (context);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_OUTPUT_STREAM) {
        mate_mixer_context_set_default_output_stream (context, stream);
    }
    else {
        set_output_stream (this, stream);
    }
}

void DeviceSwitchWidget::drawImagColorFromTheme(UkuiButtonDrawSvg *button, QString iconStr)
{
    button->themeIcon.image = QIcon::fromTheme(iconStr).pixmap(24,24).toImage();
    button->themeIcon.color = QColor(0,0,0,216);

    if (mThemeName == UKUI_THEME_WHITE) {
        button->themeIcon.color = QColor(255,255,255,216);
    }
    else if (mThemeName == UKUI_THEME_BLACK) {
        button->themeIcon.color = QColor(255,255,255,216);
    }
}

/*
    深色主题时输出音量图标
*/
void DeviceSwitchWidget::outputVolumeDarkThemeImage(int value,bool status)
{
    QString iconStr;
    if (status) {
        iconStr = "audio-volume-muted-symbolic";
    }
    else if (value <= 0 && !shortcutMute) {
        iconStr = "audio-volume-muted-symbolic";
    }
    else if(value <= 0 && shortcutMute){
        iconStr = "audio-volume-low-symbolic";
    }
    else if (value > 0 && value <= 33) {
        iconStr = "audio-volume-low-symbolic";
    }
    else if (value >33 && value <= 66) {
        iconStr = "audio-volume-medium-symbolic";
    }
    else {
        iconStr = "audio-volume-high-symbolic";
    }

    QPixmap devicePixmap("/usr/share/ukui-media/img/device.svg");
    QPixmap applicationPixmap("/usr/share/ukui-media/img/application.svg");
    if ( mThemeName == "ukui-white" || mThemeName == "ukui-light") {
        miniWidget->muteBtn->themeIcon.color = QColor(0,0,0,216);
        devWidget->outputMuteBtn->themeIcon.color = QColor(0,0,0,216);
        appWidget->systemVolumeBtn->themeIcon.color = QColor(0,0,0,216);
        deviceBtn->setIcon(QIcon(drawDarkColoredPixmap(devicePixmap)));
        appVolumeBtn->setIcon(QIcon(drawDarkColoredPixmap(applicationPixmap)));
        miniWidget->muteBtn->themeIcon.image = QIcon::fromTheme(iconStr).pixmap(24,24).toImage();
        devWidget->outputMuteBtn->themeIcon.image = QIcon::fromTheme(iconStr).pixmap(24,24).toImage();
        appWidget->systemVolumeBtn->themeIcon.image = QIcon::fromTheme(iconStr).pixmap(32,32).toImage();
    }
    else if (mThemeName == UKUI_THEME_BLACK || mThemeName == "ukui-black" || mThemeName == "ukui-default") {
        miniWidget->muteBtn->themeIcon.color = QColor(255,255,255,216);
        devWidget->outputMuteBtn->themeIcon.color = QColor(255,255,255,216);
        appWidget->systemVolumeBtn->themeIcon.color = QColor(255,255,255,216);
        deviceBtn->setIcon(QIcon(drawLightColoredPixmap(devicePixmap)));
        appVolumeBtn->setIcon(QIcon(drawLightColoredPixmap(applicationPixmap)));
        miniWidget->muteBtn->themeIcon.image = QIcon::fromTheme(iconStr).pixmap(24,24).toImage();
        devWidget->outputMuteBtn->themeIcon.image = QIcon::fromTheme(iconStr).pixmap(24,24).toImage();
        appWidget->systemVolumeBtn->themeIcon.image = QIcon::fromTheme(iconStr).pixmap(32,32).toImage();
    }
    soundSystemTrayIcon->setIcon(QIcon::fromTheme(iconStr));
}

/*
    输入音量图标
*/
void DeviceSwitchWidget::inputVolumeDarkThemeImage(int value,bool status)
{
    QString inputIconStr;
    if (status) {
        inputIconStr = "microphone-sensitivity-muted-symbolic";
    }
    else if (value <= 0) {
        inputIconStr = "microphone-sensitivity-muted-symbolic";
    }
    else if (value > 0 && value <= 33) {
        inputIconStr = "microphone-sensitivity-low-symbolic";
    }
    else if (value >33 && value <= 66) {
        inputIconStr = "microphone-sensitivity-medium-symbolic";
    }
    else {
        inputIconStr = "microphone-sensitivity-high-symbolic";
    }

    if (mThemeName == "ukui-white" || mThemeName == "ukui-light") {
        devWidget->inputMuteButton->themeIcon.color = QColor(0,0,0,216);
        devWidget->inputMuteButton->themeIcon.image = QIcon::fromTheme(inputIconStr).pixmap(24,24).toImage();
    }
    else if (mThemeName == UKUI_THEME_BLACK || mThemeName == "ukui-black" || mThemeName == "ukui-default") {
        devWidget->inputMuteButton->themeIcon.color = QColor(255,255,255,216);
        devWidget->inputMuteButton->themeIcon.image = QIcon::fromTheme(inputIconStr).pixmap(24,24).toImage();
    }
}

/*
    获取输入音量值
*/
int DeviceSwitchWidget::getInputVolume()
{
    return devWidget->inputDeviceSlider->value();
}

/*
    获取输出音量值
*/
int DeviceSwitchWidget::getOutputVolume()
{
    return miniWidget->masterVolumeSlider->value();
}

/*
   获取输入状态
*/
bool DeviceSwitchWidget::getInputMuteStatus()
{
    MateMixerStream *pStream = mate_mixer_context_get_default_input_stream(context);
    MateMixerStreamControl *pControl = mate_mixer_stream_get_default_control(pStream);
    return mate_mixer_stream_control_get_mute(pControl);
}

/*
    获取输出状态
*/
bool DeviceSwitchWidget::getOutputMuteStatus()
{
    MateMixerStream *pStream = mate_mixer_context_get_default_output_stream(context);
    MateMixerStreamControl *pControl = mate_mixer_stream_get_default_control(pStream);
    return mate_mixer_stream_control_get_mute(pControl);
}

void DeviceSwitchWidget::setVolumeSettingValue(int value)
{
    if (QGSettings::isSchemaInstalled(UKUI_VOLUME_BRIGHTNESS_GSETTING_ID)) {
        if (m_pVolumeSetting->keys().contains("volumesize")) {
            m_pVolumeSetting->blockSignals(true);
            m_pVolumeSetting->set(UKUI_VOLUME_KEY,value);
            m_pVolumeSetting->blockSignals(false);
        }
    }
}

void DeviceSwitchWidget::themeChangeIcons()
{
    int nInputValue = getInputVolume();
    int nOutputValue = getOutputVolume();
    bool inputStatus = getInputMuteStatus();
    bool outputStatus = getOutputMuteStatus();
    
    inputVolumeDarkThemeImage(nInputValue,inputStatus);
    outputVolumeDarkThemeImage(nOutputValue,outputStatus);
    miniWidget->muteBtn->repaint();
    devWidget->inputMuteButton->repaint();
    devWidget->outputMuteBtn->repaint();
    appWidget->systemVolumeBtn->repaint();
}

void DeviceSwitchWidget::fontSizeChangedSlot(const QString &themeStr)
{
    QString fontType;
    if (m_pFontSetting->keys().contains("systemFont")) {
        fontType = m_pFontSetting->get("systemFont").toString();
    }
    if (m_pFontSetting->keys().contains("systemFontSize")) {
        int font = m_pFontSetting->get("system-font-size").toInt();
        QFont fontSize(fontType,font);
        devWidget->outputDeviceDisplayLabel->setFont(fontSize);
        appWidget->systemVolumeLabel->setFont(fontSize);
        devWidget->inputDeviceDisplayLabel->setFont(fontSize);
    }
    Q_EMIT font_change();
}
/*!
 * \brief
 * \details
 * 系统主题改变
 */
void DeviceSwitchWidget::ukuiThemeChangedSlot(const QString &themeStr)
{
    if (m_pThemeSetting->keys().contains("styleName")) {
        mThemeName = m_pThemeSetting->get(UKUI_THEME_NAME).toString();
    }
    themeChangeIcons();
    QPalette palette = dividerFrame->palette();
    QColor color = QColor(255,255,255);
    color.setAlphaF(0.08);
    palette.setColor(QPalette::WindowText, color);
    dividerFrame->setPalette(palette);
    Q_EMIT theme_change();
}


/*!
 * \brief
 * \details
 * 滚动输入滑动条控制输入音量
 */
void DeviceSwitchWidget::inputWidgetSliderChangedSlot(int value)
{
    input = mate_mixer_context_get_default_input_stream (context);
    MateMixerStreamControl *inputControl = mate_mixer_stream_get_default_control(input);
    QString percent;
    percent = QString::number(value);
    mate_mixer_stream_control_set_mute(inputControl,FALSE);
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(inputControl,volume);
    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 点击输入静音按钮，设置输入音量的静音状态
 */
void DeviceSwitchWidget::inputWidgetMuteButtonClicked()
{
    input = mate_mixer_context_get_default_input_stream (context);
    MateMixerStreamControl *inputControl = mate_mixer_stream_get_default_control(input);
    bool state = mate_mixer_stream_control_get_mute(inputControl);
    int volume = mate_mixer_stream_control_get_volume(inputControl);
    mate_mixer_stream_control_set_mute(inputControl,!state);
    themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 主屏更改时获取托盘的位置
 */
void DeviceSwitchWidget::primaryScreenChangedSlot(QScreen *screen)
{
    Q_UNUSED(screen);
    trayRect = soundSystemTrayIcon->geometry();
}

/*!
 * \brief
 * \details
 * 声音托盘的触发事件，包括鼠标左键点击，双击，滚轮，右击
 */
void DeviceSwitchWidget::activatedSystemTrayIconSlot(QSystemTrayIcon::ActivationReason reason)
{
    //获取透明度
    if (QGSettings::isSchemaInstalled(UKUI_TRANSPARENCY_SETTING)){
        if (m_pTransparencySetting->keys().contains("transparency")) {
            transparency = m_pTransparencySetting->get("transparency").toDouble();
        }
    }
    QString sheet = QString("QWidget{background-color:rgba(19,19,20,%1);"
                            "border-radius:6px;}").arg(transparency);
    switch(reason) {
    //鼠标中间键点击图标
    case QSystemTrayIcon::MiddleClick: {
        if (this->isHidden() || miniWidget->isHidden()) {
            Q_EMIT mouse_middle_clicked_signal();
        }
        else {
            hideWindow();
        }
        break;
    }
        //鼠标左键点击图标
    case QSystemTrayIcon::Trigger: {
        if (!isVisible() || windowState() & Qt::WindowMinimized) {
            switch (displayMode) {
            case MINI_MODE:
                miniWidgetShow();
                break;
            case ADVANCED_MODE:
                advancedWidgetShow();
                break;
            default:
                break;
            }
            break;
        }
        else {
            if (displayMode == ADVANCED_MODE) {
                this->hide();
            }
            else if (displayMode == MINI_MODE) {
                miniWidget->hide();
            }
            break;
        }
    }
        //鼠标左键双击图标
    case QSystemTrayIcon::DoubleClick: {
        hideWindow();
        break;
    }
    case QSystemTrayIcon::Context:{
    }
    default:
        break;
    }
}

/*!
 * \brief
 * \details
 * QWidgetAction初始化
 */
/*
void DeviceSwitchWidget::init_widget_action(QWidget *wid, QString iconstr, QString textstr)
{
    QHBoxLayout* layout=new QHBoxLayout(wid);
    wid->setLayout(layout);
    wid->setFixedSize(246,36);

    wid->setFocusPolicy(Qt::NoFocus);

    if(!iconstr.isEmpty()) {
        QLabel* labelicon=new QLabel(wid);
        QSvgRenderer* svg=new QSvgRenderer(wid);
        svg->load(iconstr);
        QPixmap* pixmap=new QPixmap(16,16);
        pixmap->fill(Qt::transparent);
        QPainter p(pixmap);
        svg->render(&p);
        labelicon->setPixmap(*pixmap);
        labelicon->setFixedSize(pixmap->size());
        labelicon->setAlignment(Qt::AlignCenter);
        labelicon->setStyleSheet("QLabel{background:transparent;border:0px;}");
        layout->addWidget(labelicon);
    }

    QLabel* labeltext=new QLabel(wid);
    labeltext->setStyleSheet("background:transparent;border:0px;color:#ffffff;font-size:14px;");
    QByteArray textbyte=textstr.toLocal8Bit();
    char* text=textbyte.data();
    labeltext->setText(tr(text));
    labeltext->adjustSize();
    layout->addWidget(labeltext);

    if(!iconstr.isEmpty()) {
        layout->setContentsMargins(10,0,wid->width()-16-labeltext->width()-20,0);
        layout->setSpacing(10);
    }
    else {
        layout->setContentsMargins(36,0,0,0);
    }
}
*/

/*!
 * \brief
 * \details
 * 初始化显示设备界面
 */
void DeviceSwitchWidget::deviceSwitchWidgetInit()
{
    const QSize iconSize(19,19);
    QWidget *deviceWidget = new QWidget(this);
    deviceWidget->setFixedSize(40,320);
    deviceBtn = new QPushButton(deviceWidget);
    appVolumeBtn = new QPushButton(deviceWidget);

    deviceBtn->setFlat(true);
    appVolumeBtn->setFlat(true);
    deviceBtn->setFocusPolicy(Qt::NoFocus);
    appVolumeBtn->setFocusPolicy(Qt::NoFocus);
    deviceBtn->setFixedSize(36,36);
    appVolumeBtn->setFixedSize(36,36);

    deviceBtn->setIconSize(iconSize);
    appVolumeBtn->setIconSize(iconSize);

    deviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/device.svg"));
    appVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/application.svg"));

    deviceBtn->move(2,10);
    appVolumeBtn->move(2,57);

    //切换按钮设置tooltip
    deviceBtn->setToolTip(tr("Device Volume"));
    appVolumeBtn->setToolTip(tr("Application Volume"));
    switch(btnType) {
        case DEVICE_VOLUME_BUTTON:
        appVolumeBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                    "padding-left:0px;}"
                                    "QPushButton::hover{background:rgba(255,255,255,0.12);"
                                    "border-radius:4px;}"
                                    "QPushButton::pressed{background:rgba(61,107,229,1);"
                                    "border-radius:4px;padding-left:0px;}");
        deviceBtn->setStyleSheet("QPushButton{background:rgba(61,107,229,1);"
                                 "border-radius:4px;padding-left:0px;}");
        break;
    case APP_VOLUME_BUTTON:
        deviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                 "padding-left:0px;}"
                                 "QPushButton::hover{background:rgba(255,255,255,0.12);"
                                 "border-radius:4px;}"
                                 "QPushButton::pressed{background:rgba(61,107,229,1);"
                                 "border-radius:4px;padding-left:0px;}");
        appVolumeBtn->setStyleSheet("QPushButton{background:rgba(61,107,229,1);"
                                    "border-radius:4px;padding-left:0px;}");
        break;
    }
}

int DeviceSwitchWidget::getScreenGeometry(QString methodName)
{
    int res = 0;
    QDBusMessage message = QDBusMessage::createMethodCall(DBUS_NAME,
                                                          DBUS_PATH,
                                                          DBUS_INTERFACE,
                                                          methodName);
    QDBusMessage response = QDBusConnection::sessionBus().call(message);
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        if(response.arguments().isEmpty() == false) {
            int value = response.arguments().takeFirst().toInt();
            res = value;
        }
    } else {
        qDebug()<<methodName<<"called failed";
    }
    return res;
}

/* get primary screen changed */
void DeviceSwitchWidget::priScreenChanged(int x, int y, int width, int height)
{
    qDebug() << "primary screen  changed, geometry is  x=%d, y=%d, windth=%d, height=%d"<<x <<y<<width<< height;
}


/*!
 * \brief
 * \details
 * 点击设备切换按钮切换到设备音量界面
 */
void DeviceSwitchWidget::deviceButtonClickedSlot()
{
    btnType = DEVICE_VOLUME_BUTTON;

    appVolumeBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                "padding-left:0px;}"
                                "QPushButton::hover{background:rgba(255,255,255,0.12);"
                                "border-radius:4px;}"
                                "QPushButton::pressed{background:rgba(61,107,229,1);"
                                "border-radius:4px;}");
    deviceBtn->setStyleSheet("QPushButton{background:rgba(61,107,229,1);"
                             "border-radius:4px;}");
    appWidget->hide();
    devWidget->show();
}

/*!
 * \brief
 * \details
 * 点击应用音量按钮切换到应用音量界面
 */
void DeviceSwitchWidget::appVolumeButtonClickedSlot()
{
    btnType = APP_VOLUME_BUTTON;
    deviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                             "padding-left:0px;}"
                             "QPushButton::hover{background:rgba(255,255,255,0.12);"
                             "border-radius:4px;}"
                             "QPushButton::pressed{background:rgba(61,107,229,1);"
                             "border-radius:4px;}");
    appVolumeBtn->setStyleSheet("QPushButton{background:rgba(61,107,229,1);"
                             "border-radius:4px;}");
    appWidget->show();
    devWidget->hide();
    //切换按钮样式
}

/*!
 * \brief
 * \details
 * 获取context状态
 */
void DeviceSwitchWidget::on_context_state_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    connect_to_pulse(w);
    Q_UNUSED(pspec);
    MateMixerState state = mate_mixer_context_get_state (context);
    MateMixerStream *pDefaultOutputStream = mate_mixer_context_get_default_output_stream(context);
    MateMixerStreamControl *pDefaultOutputControl = mate_mixer_stream_get_default_control(pDefaultOutputStream);
    gboolean bOutputMuteState = mate_mixer_stream_control_get_mute(pDefaultOutputControl);
    //初始化gsetting值
    if (QGSettings::isSchemaInstalled(UKUI_VOLUME_BRIGHTNESS_GSETTING_ID)) {
        if (w->m_pVolumeSetting->keys().contains("soundstate")) {
            w->m_pVolumeSetting->blockSignals(true);
            w->m_pVolumeSetting->set(UKUI_VOLUME_STATE,bOutputMuteState);
            w->m_pVolumeSetting->blockSignals(false);
        }
    }
    list_device(w,context);
    if (state == MATE_MIXER_STATE_READY) {
        MateMixerStream *stream = mate_mixer_context_get_default_input_stream (w->context);
        update_default_input_stream (w);
        update_icon_output(w,context);
        update_icon_input(w,stream);
    }
    else if (state == MATE_MIXER_STATE_FAILED) {
        qFatal("Failed to connect a sound system");
    }
    QString deviceStr = w->miniWidget->deviceLabel->text();
    QString inputDeviceStr = w->devWidget->inputDeviceDisplayLabel->text();

    //是否初始化音量
    if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){
        w->m_pInitSystemVolumeSetting = new QGSettings(UKUI_AUDIO_SCHEMA);
        if (w->m_pInitSystemVolumeSetting->keys().contains("firstRun")) {
            bool isRun = w->m_pInitSystemVolumeSetting->get(FIRST_RUN).toBool();
        }
    }

//    if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){
//        w->m_pInitSystemVolumeSetting = new QGSettings(UKUI_AUDIO_SCHEMA);
//        if(strstr(deviceStr.toLocal8Bit().data(),"模拟耳机")) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("headphoneOutputVolume")) {
//                int headphoneVolume = w->m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
//                if (strcmp(deviceStr.toLocal8Bit().data(),"模拟耳机") == 0) {
//                    if(!w->customSoundFile->isExist(deviceStr)){
//                        //if (w->m_pInitSystemVolumeSetting->get(HEADPHONES_1).toBool())
//                        {

//                            w->miniWidget->masterVolumeSlider->setValue(headphoneVolume);
//                            qDebug() << "set headphone volume --------"  << headphoneVolume << w->miniWidget->deviceLabel->text() << deviceStr.toLocal8Bit().data();
//                        }
//                        QString str = QString(deviceStr);
//                        w->customSoundFile->addXmlNode(str,false);  //将未初始化的端口，加入到xml文件中
//                    }
//                }
//                else if (strstr(deviceStr.toLocal8Bit().data(),"HDMI")) {
//                    if(!w->customSoundFile->isExist(deviceStr)){
//                        //if (w->m_pInitSystemVolumeSetting->get(HDMI).toBool())
//                        {
//                            int hdmiVolume = w->m_pInitSystemVolumeSetting->get(HDMI_VOLUME).toInt();
//                            w->miniWidget->masterVolumeSlider->setValue(hdmiVolume);
//                            w->m_pInitSystemVolumeSetting->set(HDMI,false);
//                            qDebug() << "set hdmi volume --------"  << hdmiVolume << w->miniWidget->deviceLabel->text() << deviceStr.toLocal8Bit().data();
//                        }
//                        QString str = QString(deviceStr);
//                        w->customSoundFile->addXmlNode(str,false);
//                    }
//                }
//                else {
//                    if(!w->customSoundFile->isExist(deviceStr)){
//                        //if (w->m_pInitSystemVolumeSetting->get(HEADPHONES_1).toBool() || w->m_pInitSystemVolumeSetting->get(HEADPHONES_2).toBool())
//                        {

//                            w->miniWidget->masterVolumeSlider->setValue(headphoneVolume);
//                            qDebug() << "set speaker volume --------"  << headphoneVolume << w->miniWidget->deviceLabel->text() << deviceStr.toLocal8Bit().data();
//                        }
//                        QString str = QString(deviceStr);
//                        w->customSoundFile->addXmlNode(str,false);
//                    }
//                }
//            }
//        }
//        else if(strstr(deviceStr.toLocal8Bit().data(),"HUAWEI Histen")){
//            w->initHuaweiAudio(w);
//        }

        //qDebug() << "设置麦克风音量值为"   <<w->devWidget->inputDeviceDisplayLabel->text() << inputDeviceStr.toLocal8Bit().data();
//        if (strstr(inputDeviceStr.toLocal8Bit().data(),"头挂麦克风")) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("headsetMic")) {
//                w->m_pInitSystemVolumeSetting->set(HEADSET_MIC,false);
//            }
//        }
//        else if (strstr(inputDeviceStr.toLocal8Bit().data(),"后麦克风")) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("rearMic")) {
//                w->m_pInitSystemVolumeSetting->set(REAR_MIC,false);
//            }
//        }
//        else if (strstr(inputDeviceStr.toLocal8Bit().data(),"输入插孔")) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("linein")) {
//                w->m_pInitSystemVolumeSetting->set(LINEIN,false);
//            }
//        }
//        if (strstr(inputDeviceStr.toLocal8Bit().data(),"内部话筒")) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("intelMic")) {
//                bool setIntelMic = w->m_pInitSystemVolumeSetting->get(INTEL_MIC).toBool();
//                if(!w->customSoundFile->isExist(inputDeviceStr)){
//                //if (setIntelMic)
//                    {
//                        int intelMicVolume = w->m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
//                        w->devWidget->inputDeviceSlider->setValue(intelMicVolume);
//                        w->m_pInitSystemVolumeSetting->set(INTEL_MIC,false);
//                    }
//                    QString str = QString(deviceStr);
//                    w->customSoundFile->addXmlNode(str,false);
//                }
//            }
//        }
//            else if (strcmp(deviceStr.toLocal8Bit().data(),"模拟耳机 2") == 0) {
//                if (w->m_pInitSystemVolumeSetting->keys().contains("headphones2")) {
//                    bool setHeadphone1 = w->m_pInitSystemVolumeSetting->get(HEADPHONES_2).toBool();
//                    if (setHeadphone1) {
//                        int headphoneVolume = w->m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
//                        w->miniWidget->masterVolumeSlider->setValue(headphoneVolume);
//                        w->m_pInitSystemVolumeSetting->set(HEADPHONES_2,false);
//                    }
//                }
//            }

//        w->m_pInitSystemVolumeSetting->set(PLUGIN_INIT_VOLUME,true);
//        w->m_pInitSystemVolumeSetting->set(FIRST_RUN,false);
//    }
}



void DeviceSwitchWidget::on_context_stored_control_added (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    MateMixerStreamControlMediaRole media_role;

    w->control = MATE_MIXER_STREAM_CONTROL (mate_mixer_context_get_stored_control (context, name));
    if (G_UNLIKELY (w->control == nullptr))
        return;

    media_role = mate_mixer_stream_control_get_media_role (w->control);

    if (media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT)
        bar_set_stream_control (w, MATE_MIXER_DIRECTION_UNKNOWN,w->control);
}


/*!
 * \brief
 * \details
 * 当切换声音设备时会触发此函数
 */
void DeviceSwitchWidget::on_context_stream_added (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    MateMixerStream *stream = mate_mixer_context_get_stream (context, name);
    if (G_UNLIKELY (stream == nullptr))
        return;

    /* If the newly added stream belongs to the currently selected device and
     * the test button is hidden, this stream may be the one to allow the
     * sound test and therefore we may need to enable the button */
    //为了防止监听监视流
//    if (strstr(name,".monitor")) {
//        return;
//    }
    qDebug() << "on_context_stream_added" <<name;
    add_stream (w, stream,context);
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);
    const gchar *streamName = mate_mixer_stream_get_name(stream);
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        //        if (!strstr(streamName,".monitor"))
        qDebug() << "add stream " << streamName;
        w->updateOutputDeviceLabel();
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT){
        if (!strstr(streamName,".monitor"))
        {
            w->updateInputDeviceLabel();
        }
    }
}

void DeviceSwitchWidget::list_device(DeviceSwitchWidget *w,MateMixerContext *context)
{
    const GList *m_pList;
    m_pList = mate_mixer_context_list_streams (context);
    while (m_pList != nullptr) {
        const gchar *pStreamName = mate_mixer_stream_get_name( MATE_MIXER_STREAM (m_pList->data));
        const gchar *pStreamLabel = mate_mixer_stream_get_label( MATE_MIXER_STREAM (m_pList->data));
        if (!strstr(pStreamName,"monitor") && !strstr(pStreamLabel,"Monitor")) {

            add_stream (w, MATE_MIXER_STREAM (m_pList->data),context);
            MateMixerDirection direction = mate_mixer_stream_get_direction(MATE_MIXER_STREAM (m_pList->data));
            if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
                w->output_stream_list->append(pStreamName);
                /*w->miniWidget->deviceCombox->addItem(label);*/
            }
            else if (direction == MATE_MIXER_DIRECTION_INPUT) {
                w->input_stream_list->append(pStreamName);
            }

        }
        m_pList = m_pList->next;
    }
    const GList *pDeviceList;
    const GList *switches;
    const gchar *profileName;
    const gchar *profileLabel;
    pDeviceList = mate_mixer_context_list_devices(context);
    while (pDeviceList) {
        add_device(w, MATE_MIXER_DEVICE (pDeviceList->data));
        switches = mate_mixer_device_list_switches (MATE_MIXER_DEVICE(pDeviceList->data));
        while (switches != nullptr) {
            MateMixerDeviceSwitch *swtch = MATE_MIXER_DEVICE_SWITCH (switches->data);
            const GList *options;
            options = mate_mixer_switch_list_options ( MATE_MIXER_SWITCH(swtch));
            while (options != NULL) {
                MateMixerSwitchOption *option = MATE_MIXER_SWITCH_OPTION (options->data);
                profileLabel = mate_mixer_switch_option_get_label (option);
                profileName = mate_mixer_switch_option_get_name(option);
                /* Select the currently active option of the switch */
                options = options->next;
            }
            switches = switches->next;
        }
        pDeviceList = pDeviceList->next;
    }

    MateMixerStream *pDefaultOutputStream = mate_mixer_context_get_default_output_stream(context);
    MateMixerStreamControl *pDefaultOutputControl = mate_mixer_stream_get_default_control(pDefaultOutputStream);
    gboolean bOutputMuteState = mate_mixer_stream_control_get_mute(pDefaultOutputControl);

    MateMixerStream *pDefaultInputStream = mate_mixer_context_get_default_input_stream(context);
    MateMixerStreamControl *pDefaultInputControl = mate_mixer_stream_get_default_control(pDefaultInputStream);
    gboolean bInputMuteState = mate_mixer_stream_control_get_mute(pDefaultInputControl);

    if (bOutputMuteState) {
        system("echo mute > /tmp/kylin_output_muted.log");
    }
    else {
        system("echo no > /tmp/kylin_output_muted.log");
    }
    qDebug() << "初始输出静音状态为 :" << bOutputMuteState;

    if (bInputMuteState) {
        system("echo mute > /tmp/kylin_input_muted.log");
    }
    else {
        system("echo no > /tmp/kylin_input_muted.log");
    }
}

void DeviceSwitchWidget::add_stream (DeviceSwitchWidget *w, MateMixerStream *stream,MateMixerContext *context)
{
    const GList *controls;
    //    gboolean is_default = FALSE;
    MateMixerDirection direction;
    const gchar *name = "";
    const gchar *label = "";
    direction = mate_mixer_stream_get_direction (stream);
    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        MateMixerStream *input;
        input = mate_mixer_context_get_default_input_stream (context);
        w->input = input;
        if (stream == input) {
            bar_set_stream (w, stream);
            MateMixerStreamControl *c = mate_mixer_stream_get_default_control(stream);
            update_input_settings (w,c);
        }
        else {
            name  = mate_mixer_stream_get_name (stream);
            label = mate_mixer_stream_get_label (stream);
            w->input_stream_list->append(name);
        }
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        MateMixerStream        *output;
        MateMixerStreamControl *control;
        output = mate_mixer_context_get_default_output_stream (context);
        control = mate_mixer_stream_get_default_control (stream);
        w->stream = stream;
        if (stream == output) {
            update_output_settings(w,control);
            bar_set_stream (w, stream);
        }
        name  = mate_mixer_stream_get_name (stream);
        label = mate_mixer_stream_get_label (stream);
        w->output_stream_list->append(name);

        const GList *switchList;
        MateMixerSwitch *swt;
        switchList = mate_mixer_stream_list_switches(stream);
        while (switchList != nullptr) {
            swt = MATE_MIXER_SWITCH(switchList->data);
            switchList = switchList->next;
        }
        /*w->device_name_list->append(name);
        w->miniWidget->deviceCombox->addItem(label);*/
    }
    controls = mate_mixer_stream_list_controls (stream);

    while (controls != nullptr) {
        w->control = MATE_MIXER_STREAM_CONTROL (controls->data);
        MateMixerStreamControlRole role;
        role = mate_mixer_stream_control_get_role (w->control);
        const gchar *m_pStreamControlName = mate_mixer_stream_control_get_name(w->control);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *m_pAppInfo = mate_mixer_stream_control_get_app_info(w->control);
            if (m_pAppInfo != nullptr) {
                const gchar *m_pAppName = mate_mixer_app_info_get_name(m_pAppInfo);
                const gchar *appIconName = mate_mixer_app_info_get_icon(m_pAppInfo);
                if (appIconName && strstr(appIconName,"recorder")) {
                    m_pAppName = "kylin-recorder";
                    appIconName = "kylin-recorder";
                }
                else if (!appIconName){
                    if (strstr(m_pAppName,"MPlayer")) {
                        appIconName = "kylin-video";
                    }
                }
                if (appIconName && strstr(appIconName,"kylin-camera")) {
                    m_pAppName = "kylin-camera";
                }
                if (strcmp(m_pAppName,"ukui-session") != 0 && strcmp(m_pAppName,"ukui-volume-control-applet-qt") != 0 && strcmp(m_pAppName,"Volume Control") && \
                        strcmp(m_pAppName,"ALSA plug-in [mate-screenshot]") && strcmp(m_pAppName,"ALSA plug-in [ukui-volume-control-applet-qt]") && \
                        strcmp(m_pAppName,"Ukui Volume Control App") && !strstr(m_pAppName,"QtPulseAudio") && strcmp(m_pAppName,"ukuimedia-volume-control") != 0 && \
                        !strstr(m_pAppName,"ukui-settings-daemon") && strcmp(m_pAppName,"multimedia-volume-control") != 0) {
                    if G_UNLIKELY (w->control == nullptr)
                            return;
                    add_application_control (w, w->control,m_pStreamControlName,direction);
                }
            }
            else
                return;
        }
        controls = controls->next;
    }

    // XXX find a way to disconnect when removed
    g_signal_connect (G_OBJECT (stream),
                      "control-added",
                      G_CALLBACK (on_stream_control_added),
                      w);
    g_signal_connect (G_OBJECT (stream),
                      "control-removed",
                      G_CALLBACK (on_stream_control_removed),
                      w);
}

/*!
 * \brief
 * \details
 * 当有应用播放或录制时会执行此函数
 */
void DeviceSwitchWidget::add_application_control (DeviceSwitchWidget *w, MateMixerStreamControl *control,const gchar *name,MateMixerDirection direction)
{
    MateMixerStreamControlMediaRole mediaRole;
    MateMixerAppInfo *info;
    guint app_count;

    const gchar *appId;
    const gchar *appName;
    const gchar *appIcon;
    app_count = appnum;

    mediaRole = mate_mixer_stream_control_get_media_role (control);

    /* Add stream to the applications page, but make sure the stream qualifies
     * for the inclusion */
    info = mate_mixer_stream_control_get_app_info (control);
    if (info == nullptr) {
        return;
    }

    /* Skip streams with roles we don't care about */
    if (mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT ||
            mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_TEST ||
            mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_ABSTRACT ||
            mediaRole == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_FILTER)
        return;

    appId = mate_mixer_app_info_get_id (info);

    /* These applications may have associated streams because they do peak
     * level monitoring, skip these too */
    if (!g_strcmp0 (appId, "org.mate.VolumeControl") ||
            !g_strcmp0 (appId, "org.gnome.VolumeControl") ||
            !g_strcmp0 (appId, "org.PulseAudio.pavucontrol"))
        return;

    const gchar *appIconName = mate_mixer_app_info_get_icon(info);
    appName = mate_mixer_app_info_get_name (info);
    if (appName == nullptr) {
        appName = mate_mixer_stream_control_get_label(control);
    }
    if (appName == nullptr) {
        appName = mate_mixer_stream_control_get_name(control);
    }
    if (appName == nullptr) {
        return;
    }
    if (appIconName) {
        //if (strstr(appIconName,"recording")) {
        if (strstr(appIconName,"record")) {
            appName = "kylin-recorder";
            appIconName = "kylin-recorder";
        }
    }
    else {
        if (strstr(appName,"MPlayer")||strstr(appName,"mpv")) {
            appIconName = "kylin-video";
        }
    }
    bool exist = false;
    for (int i = 0;  i< w->appControlVector.size(); ++i) {
        if(w->appControlVector.at(i).begin().key() == QString(appIconName)){
            exist = true;
            //保存该应用control名
            QMap<QString,QStringList> map = w->appControlVector.at(i);
            map.begin().value().append(QString(name));
            w->appControlVector.remove(i);
            w->appControlVector.insert(i,map);
            break;
        }
    }
    if(!exist){
        //保存应用名和该应用control名
        QMap<QString,QStringList> map;
        QStringList strList;
        strList.append(name);
        map.insert(QString(appName),strList);
        w->appControlVector.append(map);
        //添加应用添加到应用音量中
        add_app_to_appwidget(w,appName,appIconName,control,direction);
    }
    if (appName == nullptr)
        appName = mate_mixer_stream_control_get_label (control);
    if (appName == nullptr)
        appName = mate_mixer_stream_control_get_name (control);
    if (G_UNLIKELY (appName == nullptr))
        return;

    /* By default channel bars use speaker icons, use microphone icons
     * instead for recording applications */
    w->stream = mate_mixer_stream_control_get_stream (control);
    if (w->stream != nullptr) {
        direction = mate_mixer_stream_get_direction (w->stream);
    }

    appIcon = mate_mixer_app_info_get_icon (info);
    if (appIcon == nullptr) {
        if (direction == MATE_MIXER_DIRECTION_INPUT)
            appIcon = "audio-input-microphone";
        else
            appIcon = "applications-multimedia";
    }
    bar_set_stream_control (w,direction,control);
}

void DeviceSwitchWidget::on_stream_control_added (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w)
{
    MateMixerStreamControlRole role;
    //qDebug()<<"add control name:"<<QString(name);
    w->control = mate_mixer_stream_get_control (stream, name);
    MateMixerAppInfo *m_pAppInfo = mate_mixer_stream_control_get_app_info(w->control);

    if (m_pAppInfo != nullptr) {
        const gchar *m_pAppName = mate_mixer_app_info_get_name(m_pAppInfo);
        const gchar *appIconName = mate_mixer_app_info_get_icon(m_pAppInfo);
        if (appIconName && strstr(appIconName,"recorder")) {
            m_pAppName = "kylin-recorder";
            appIconName = "kylin-recorder";
        }
        else if (!appIconName){
            if (strstr(m_pAppName,"MPlayer")) {
                appIconName = "kylin-video";
            }
        }
        if (appIconName && strstr(appIconName,"kylin-camera")) {
            m_pAppName = "kylin-camera";
        }
        if (strcmp(m_pAppName,"ukui-session") != 0 && strcmp(m_pAppName,"ukui-volume-control-applet-qt") != 0 && strcmp(m_pAppName,"Volume Control") && \
                strcmp(m_pAppName,"ALSA plug-in [mate-screenshot]") && strcmp(m_pAppName,"ALSA plug-in [ukui-volume-control-applet-qt]") && \
                strcmp(m_pAppName,"Ukui Volume Control App") && !strstr(m_pAppName,"QtPulseAudio") && strcmp(m_pAppName,"ukuimedia-volume-control") != 0 && \
                !strstr(m_pAppName,"ukui-settings-daemon") && strcmp(m_pAppName,"multimedia-volume-control") != 0) {
            if G_UNLIKELY (w->control == nullptr)
                    return;

            role = mate_mixer_stream_control_get_role (w->control);
            if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
                MateMixerDirection direction = mate_mixer_stream_get_direction(stream);
                add_application_control (w, w->control,name,direction);
            }
        }
    }
    else {
        return;
    }
}

void DeviceSwitchWidget::on_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(stream);
    /* No way to be sure that it is an application control, but we don't have
     * any other than application bars that could match the name */

    QString appName;
    int index = -1;
    bool isRemove = false;
    for (int i = 0; i < w->appControlVector.size(); ++i) {
        QMap<QString,QStringList> map;
        map = w->appControlVector.at(i);
        QMap<QString,QStringList>::iterator iter;
        QStringList strList;
        for(iter=map.begin();iter!=map.end();iter++){
            strList = iter.value();
            for(int j=0;j<strList.size();++j){
                QString strName = strList.at(j);
                if(strName == QString(name)){
                    appName = iter.key();
                    strList.removeAt(j);
                    --j;
                }
                else{
                    std::string str =strName.toStdString();
                    const gchar *name = str.c_str();
                    MateMixerStreamControl *control = mate_mixer_stream_get_control (stream, name);
                    if(!MATE_MIXER_IS_STREAM_CONTROL(control)){
                        strList.removeAt(j);
                        --j;
                    }
                }
            }
            if(strList.isEmpty()){
                //移除App name
                index = i;
                isRemove = true;
                w->appControlVector.remove(i);
                w->appDisplayVector.remove(i);
                --i;
            }
            else{
                iter.value() = strList;

            }
            strList.clear();
        }
    }

    //if(w->appControlVector.size() > 0 && w->appDisplayVector.size() >0)
    {
        if(isRemove){
            QLayoutItem *item;
            if ((item = w->appWidget->m_pVlayout->takeAt(index)) != 0) {
                QWidget *wid = item->widget();
                w->appWidget->m_pVlayout->removeWidget(wid);
                wid->setParent(nullptr);
                delete wid;
                delete item;
            }

            if (appnum <= 0) {
                g_warn_if_reached ();
                appnum = 1;
            }
            appnum--;

            //设置布局的间距以及设置vlayout四周的间距
            w->appWidget->m_pVlayout->setSpacing(18);
            w->appWidget->displayAppVolumeWidget->resize(358,14+appnum*78);
            w->appWidget->m_pVlayout->setContentsMargins(18,14,34,18);
            w->appWidget->m_pVlayout->update();
            if (appnum <= 0) {
                w->appWidget->upWidget->hide();
            }
            else {
                w->appWidget->upWidget->show();
            }
        }
    }
}

/*!
 * \brief
 * \details
 * 当播放或录制应用退出时删除在应用音量界面上该应用的显示
 */
void DeviceSwitchWidget::remove_application_control (DeviceSwitchWidget *w,const gchar *m_pAppName)
{
    g_debug ("Removing application stream %s", m_pAppName);
    /* We could call bar_set_stream_control here, but that would pointlessly
     * invalidate the channel bar, so just remove it ourselves */
    int index = -1;
    bool exist = false;
    for(int i=0;i<w->appControlVector.size();i++){
        if(w->appControlVector.at(i).begin().key() == QString(m_pAppName)){
            index = i;
            exist = true;
            break;
        }
    }

    if(exist){
        QLayoutItem *item;
        if ((item = w->appWidget->m_pVlayout->takeAt(index)) != 0) {
            QWidget *wid = item->widget();
            w->appWidget->m_pVlayout->removeWidget(wid);
            wid->setParent(nullptr);
            delete wid;
            delete item;
        }

        if (appnum <= 0) {
            g_warn_if_reached ();
            appnum = 1;
        }
        appnum--;

        //设置布局的间距以及设置vlayout四周的间距
        w->appWidget->m_pVlayout->setSpacing(18);
        w->appWidget->displayAppVolumeWidget->resize(358,14+appnum*68);
        w->appWidget->m_pVlayout->setContentsMargins(18,14,34,0);
        w->appWidget->m_pVlayout->update();
        if (appnum <= 0) {
            w->appWidget->upWidget->hide();
        }
        else {
            w->appWidget->upWidget->show();
        }
    }

}

/*!
 * \brief
 * \details
 * 获取应用名称，从desktop全路径名下解析出应用名称
 */
QString DeviceSwitchWidget::getAppName(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);

    char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Name", nullptr, nullptr);
    QString namestr=QString::fromLocal8Bit(name);

    g_key_file_free(keyfile);
    return namestr;
}

/*!
 * \brief
 * \details
 * 获取应用图标，从desktop全路径名下解析出应用图标
 */
QString DeviceSwitchWidget::getAppIcon(QString desktopfp)
{
    GError** error=nullptr;
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte=desktopfp.toLocal8Bit();
    char* filepath=fpbyte.data();
    g_key_file_load_from_file(keyfile,filepath,flags,error);

    char* name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Icon", nullptr, nullptr);
    QString namestr=QString::fromLocal8Bit(name);

    g_key_file_free(keyfile);
    return namestr;
}


/*!
 * \brief
 * \details
 * 当有应用播放或录制音频时，将该应用添加到应用音量界面上
 */
void DeviceSwitchWidget::add_app_to_appwidget(DeviceSwitchWidget *w,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control,MateMixerDirection direction)
{
    appnum++;
    //获取应用静音状态及音量
    int volume = 0;
    gboolean is_mute = false;
    gdouble normal = 0.0;
    is_mute = mate_mixer_stream_control_get_mute(control);
    volume = int(mate_mixer_stream_control_get_volume(control));
    normal = mate_mixer_stream_control_get_normal_volume(control);
    int display_volume = int(100 * volume / normal);
    //设置应用的图标
    QString iconName = "/usr/share/applications/";

    /*!
     * \brief
     * \details
     * 由于获取到的麒麟影音软件获取的app icon不正确
     * 可能是软件没有发出正确的信号告知正确的icon，
     * 因此添加判断当匹配应用名为Kylinvideo时设置
     * 正确的icon名来保证找到正确的desktop,从而设置
     * 应用音量正确的应用名和图标
     */
    if (strcmp(app_name,"KylinVideo") == 0) {
        app_icon_name = "kylin-video";
    }
    /*!
     * \brief
     * \details
     * 由于腾讯视频是由谷歌浏览器网页版的腾讯视频封装的
     * 因此获取的appName和appIconName均和chromium
     * 相关，特定添加以下判断，来使得用户使用腾讯视频观看
     * 视频的时候不会出现应用名和图标显示不正确的问题
     */
    else if (strcmp(app_name,"Chromium") == 0 && strcmp(app_icon_name.toLatin1().data(),"chromium-browser") == 0) {
        app_icon_name = "qaxbrowser";
    }
    else if (strcmp(app_name,"ALSA plug-in [kylin-music]") == 0 ) {
        app_icon_name = "kylin-music";
    }
    else if (strcmp(app_name,"ALSA plug-in [ukui-sidebar]") == 0) {
        app_icon_name = "";
    }
    else if (strcmp(app_name,"Clock") == 0) {
        app_icon_name = "ukui-clock";
    }
    else if (strcmp(app_name,"wechat") == 0) {
        app_icon_name = "electronic-wechat";
    }
    else if (strcmp(app_name,"Firefox") == 0) {
        app_icon_name = "firefox";
    }

    iconName.append(app_icon_name);
    iconName.append(".desktop");
    if (strcmp(iconName.toLatin1().data(),"/usr/share/applications/firefox.desktop") == 0) {
        iconName = "/usr/share/applications/firefox-esr.desktop";
    }
    if (strcmp(iconName.toLatin1().data(),"/usr/share/applications/qaxbrowser.desktop") == 0) {
        iconName = "/usr/share/applications/qaxbrowser-safe.desktop";
    }

    QString pAppName = w->getAppName(iconName);
    QString pAppIcon = w->getAppIcon(iconName);

    w->appWidget->app_volume_list->append(app_icon_name);
    qDebug() << "add application to widget" << app_icon_name <<direction;

    //widget显示应用音量
    QWidget *app_widget = new QWidget(w->appWidget->displayAppVolumeWidget);
    app_widget->setFixedSize(306,68);//bjc将60改为70就可以

    QHBoxLayout *hlayout = new QHBoxLayout(app_widget);
    QVBoxLayout *vlayout = new QVBoxLayout();
    QSpacerItem *item1 = new QSpacerItem(18,20);
    QSpacerItem *item2 = new QSpacerItem(12,20);
    QWidget *wid = new QWidget(app_widget);
    wid->setAttribute(Qt::WA_TranslucentBackground);
    wid->setFixedSize(306,32);
//    wid->setStyleSheet("background-color:blue;");
    w->appWidget->appLabel = new QLabel(app_widget);
    w->appWidget->appLabel->setParent(app_widget);
    w->appWidget->appIconBtn = new QPushButton(wid);
    w->appWidget->appSlider = new UkmediaVolumeSlider(wid,true);

    w->appWidget->appMuteBtn = new QPushButton(wid);
    w->appWidget->appSlider->setOrientation(Qt::Horizontal);
    w->appWidget->appIconBtn->setFixedSize(32,32);
    w->appWidget->appMuteBtn->setFixedSize(24,24);
    //设置每项的固定大小
    w->appWidget->appLabel->setFixedSize(260,22);//bjc高度改为22即可

    QSize icon_size(32,32);
    w->appWidget->appIconBtn->setIconSize(icon_size);
    w->appWidget->appIconBtn->setIcon(QIcon::fromTheme(pAppIcon));
    w->appWidget->appIconBtn->setAttribute(Qt::WA_TranslucentBackground);
    w->appWidget->appIconBtn->setStyleSheet("QPushButton{background:raba(255,0,0,0);}");

    QPalette p;
    p.setColor(QPalette::Highlight,QColor(79,184,88));
    w->appWidget->appSlider->setPalette(p);
    QPalette paleteAppIcon =  w->appWidget->appIconBtn->palette();
    paleteAppIcon.setColor(QPalette::Highlight,Qt::transparent);
    paleteAppIcon.setBrush(QPalette::Button,QBrush(QColor(0,0,0,0)));
    w->appWidget->appIconBtn->setPalette(paleteAppIcon);

    QPalette paleteMuteIcon =  w->appWidget->appMuteBtn->palette();
    paleteMuteIcon.setColor(QPalette::Highlight,Qt::transparent);
    paleteMuteIcon.setBrush(QPalette::Button,QBrush(QColor(1,1,1,0)));
    w->appWidget->appMuteBtn->setPalette(paleteMuteIcon);

    w->appWidget->appSlider->setMaximum(100);
    w->appWidget->appSlider->setFixedSize(220,22);
    w->appWidget->appIconBtn->adjustSize();
    w->appWidget->appIconBtn->setProperty("useIconHighlightEffect",true);
    w->appWidget->appIconBtn->setProperty("iconHighlightEffectMode",true);
    w->appWidget->appMuteBtn->setProperty("useIconHighlightEffect",true);
    w->appWidget->appMuteBtn->setProperty("iconHighlightEffectMode",true);
    QSize appSize(24,24);
    w->appWidget->appMuteBtn->setIconSize(appSize);
    hlayout->addWidget(w->appWidget->appIconBtn);
    hlayout->addItem(item1);
    hlayout->addWidget(w->appWidget->appSlider);
    hlayout->addItem(item2);
    hlayout->addWidget(w->appWidget->appMuteBtn);
    hlayout->setSpacing(0);
    wid->setLayout(hlayout);
    wid->layout()->setContentsMargins(0,0,0,0);

    vlayout->addWidget(w->appWidget->appLabel);
    vlayout->addWidget(wid);
    vlayout->setSpacing(14);
    app_widget->setLayout(vlayout);
    app_widget->layout()->setContentsMargins(0,0,0,0);


    QString appSliderStr = app_name;
    QString appMuteBtnlStr = app_name;
    QString appVolumeLabelStr = app_name;

    appSliderStr.append("Slider");
    appMuteBtnlStr.append("button");
    appVolumeLabelStr.append("VolumeLabel");
    appSliderStr.append(QString::number(app_count));
    appMuteBtnlStr.append(QString::number(app_count));
    appVolumeLabelStr.append(QString::number(app_count));

    QStringList strList;
    strList<<appMuteBtnlStr<<appSliderStr;
    w->appDisplayVector.append(strList);
    w->appWidget->appMuteBtn->setObjectName(appMuteBtnlStr);
    w->appWidget->appSlider->setObjectName(appSliderStr);

    //设置label 和滑动条的值
    QSlider *slider = w->appWidget->findChild<QSlider*>(appSliderStr);
    if (slider == nullptr)
        return;
    slider->setValue(display_volume);

    QPushButton *btn = w->appWidget->findChild<QPushButton *>(appMuteBtnlStr);
    if (btn == nullptr)
        return;

    QString audioIconStr;
    QIcon audioIcon;

    if (is_mute) {
        audioIconStr = "audio-volume-muted-symbolic";
    }
    else if (display_volume <= 0 && !w->shortcutMute) {
        audioIconStr = "audio-volume-muted-symbolic";
    }
    else if(display_volume <= 0 && w->shortcutMute){
        audioIconStr = "audio-volume-low-symbolic";
    }
    else if (display_volume > 0 && display_volume <= 33) {
        audioIconStr = "audio-volume-low-symbolic";
    }
    else if(display_volume > 33 && display_volume <= 66) {
        audioIconStr = "audio-volume-medium-symbolic";
    }
    else if (display_volume > 66) {
        audioIconStr = "audio-volume-high-symbolic";
    }
    QSize iconSize(24,24);
    if ( w->mThemeName == "ukui-white" || w->mThemeName == "ukui-light") {
        btn->setIcon(QIcon(w->drawDarkColoredPixmap((QIcon::fromTheme(audioIconStr).pixmap(iconSize)))));
    }
    else if (w->mThemeName == UKUI_THEME_BLACK || w->mThemeName == "ukui-black" || w->mThemeName == "ukui-default") {
        btn->setIcon(QIcon(w->drawLightColoredPixmap((QIcon::fromTheme(audioIconStr).pixmap(iconSize)))));
    }
    audioIcon = QIcon::fromTheme(audioIconStr);
    if (pAppName == "") {
        w->appWidget->appLabel->setText(app_icon_name);
    }
    else {
        w->appWidget->appLabel->setText(pAppName);
    }

    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(w->context);
        if (MATE_MIXER_IS_STREAM(inputStream)) {
            w->notifySend(w->devWidget->inputDeviceDisplayLabel->text(),pAppName,pAppIcon,tr("is using"));
        }
    }
    //主题更改
    connect(w,&DeviceSwitchWidget::theme_change,[=](){
        QPushButton *btn = w->appWidget->findChild<QPushButton *>(appMuteBtnlStr);//由于btn会被释放掉,导致btn不存在,所以加上这一行
        if ( btn!= nullptr  ) {
            if ( w->mThemeName == "ukui-white" || w->mThemeName == "ukui-light") {
                btn->setIcon(QIcon(w->drawDarkColoredPixmap((QIcon::fromTheme(audioIconStr).pixmap(iconSize)))));
            }
            else if (w->mThemeName == UKUI_THEME_BLACK || w->mThemeName == "ukui-black" || w->mThemeName == "ukui-default") {
                btn->setIcon(QIcon(w->drawLightColoredPixmap((QIcon::fromTheme(audioIconStr).pixmap(iconSize)))));
            }
        }
    });
    connect(w,&DeviceSwitchWidget::font_change,[=](){
        QString fontType;
        if (w->m_pFontSetting->keys().contains("systemFont")) {
            fontType = w->m_pFontSetting->get("systemFont").toString();
        }
        if (w->m_pFontSetting->keys().contains("systemFontSize")) {
            int font = w->m_pFontSetting->get("system-font-size").toInt();
//            QFont fontSize(fontType,font);

//            w->appWidget->appLabel->setFont(fontSize);
        }
    });
    /*滑动条控制应用音量*/
    connect(w->appWidget->appSlider,&QSlider::valueChanged,[=](int value){
        QString appName = appSliderStr.split("Slider").at(0);
        int index = -1;
        bool isMute = false;
        for (int i=0;i<w->appControlVector.size();i++){
            if(w->appControlVector.at(i).begin().key() == appName){
               QStringList strList = w->appControlVector.at(i).begin().value();
               //qDebug()<<"strList :"<<strList;
                for (int j=0; j<strList.size(); j++) {
                    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(w->context);
                    QString str = strList.at(j);
                    std::string strr = str.toStdString();
                    const char *ctrlName = strr.c_str();
                    MateMixerStreamControl *control = mate_mixer_stream_get_control(stream,ctrlName);
                    //qDebug()<<"control name:"<<QString(mate_mixer_stream_control_get_name(control));
                    if(value>0){
                        if(mate_mixer_stream_control_get_mute(control) == true){
                            isMute = false;
                            mate_mixer_stream_control_set_mute(control,isMute);
                        }
                    }
                    else if(value<=0){
                        mate_mixer_stream_control_set_mute(control,true);
                        isMute = true;
                    }

                    int v = int(value*65536/100 + 0.5);
                    mate_mixer_stream_control_set_volume(control,guint(v));
                }
                index = i;
                break;
            }
        }

        application_name = appSliderStr;
        QSlider *slider = w->findChild<QSlider*>(appSliderStr);
        if (slider == nullptr)
            return;
        QPushButton *btn = w->findChild<QPushButton*>(appMuteBtnlStr);
        if (btn == nullptr)
            return;
        QString audioIconStr;
        if (value > 0) {
            //mate_mixer_stream_control_set_mute(control,false);
        }
        if (isMute) {
            audioIconStr = "audio-volume-muted-symbolic";
        }
        else if (value <= 0 && !w->shortcutMute) {
            audioIconStr = "audio-volume-muted-symbolic";
        }
        else if(volume <= 0 && w->shortcutMute){
            audioIconStr = "audio-volume-low-symbolic";
        }
        else if (value > 0 && value <= 33) {
            audioIconStr = "audio-volume-low-symbolic";
        }
        else if(value > 33 && value <= 66) {
            audioIconStr = "audio-volume-medium-symbolic";
        }
        else if (value > 66) {
            audioIconStr = "audio-volume-high-symbolic";
        }
        QSize iconSize(24,24);
        if ( w->mThemeName == "ukui-white" || w->mThemeName == "ukui-light") {
            btn->setIcon(QIcon(w->drawDarkColoredPixmap((QIcon::fromTheme(audioIconStr).pixmap(iconSize)))));
        }
        else if (w->mThemeName == UKUI_THEME_BLACK || w->mThemeName == "ukui-black" || w->mThemeName == "ukui-default") {
            btn->setIcon(QIcon(w->drawLightColoredPixmap((QIcon::fromTheme(audioIconStr).pixmap(iconSize)))));
        }
        Q_EMIT w->app_name_signal(appSliderStr);
    });
    /*应用音量同步*/
    g_signal_connect (G_OBJECT (control),
                      "notify::volume",
                      G_CALLBACK (update_app_volume),
                      w);
    g_signal_connect(G_OBJECT (control),
                     "notify::mute",
                     G_CALLBACK (app_volume_mute),
                     w);

    //应用静音按钮
    connect(w->appWidget->appMuteBtn,&QPushButton::clicked,[=](){
        QString appName = appMuteBtnlStr.split("button").at(0);
        int index = -1;
        bool isMute = false;
        int volume = -1;
        for (int i=0; i<w->appControlVector.size(); i++){
            if(w->appControlVector.at(i).begin().key() == appName){
               QStringList strList = w->appControlVector.at(i).begin().value();
                for (int j=0; j<strList.size(); j++) {  //获取音量和静音状态
                    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(w->context);
                    QString str = strList.at(j);
                    std::string strr = str.toStdString();
                    const char *ctrlName = strr.c_str();
                    MateMixerStreamControl *control = mate_mixer_stream_get_control(stream,ctrlName);
                    if(MATE_MIXER_IS_STREAM_CONTROL(control)){
                        if(mate_mixer_stream_control_get_mute(control) == false){
                            isMute = true;
                        }
                        volume = mate_mixer_stream_control_get_volume(control);
                        break;
                    }
                }


                for (int j=0; j<strList.size(); j++) {  //同一个应用的所有声音都要设置静音
                    MateMixerStream *stream = mate_mixer_context_get_default_output_stream(w->context);
                    QString str = strList.at(j);
                    std::string strr = str.toStdString();
                    const char *ctrlName = strr.c_str();
                    MateMixerStreamControl *control = mate_mixer_stream_get_control(stream,ctrlName);
                    mate_mixer_stream_control_set_mute(control,isMute);
                }

                index = i;
                break;
            }
        }

        volume = int(volume*100/65536 + 0.5);
        QPushButton *btn = w->appWidget->findChild<QPushButton *>(appMuteBtnlStr);
        if (btn == nullptr)
            return;
        QString muteButtonStr;
        QIcon muteButtonIcon;
        if (isMute) {
            muteButtonStr = "audio-volume-muted-symbolic";
        }
        else if (volume <= 0 && !w->shortcutMute) {
            muteButtonStr = "audio-volume-muted-symbolic";
        }
        else if(volume <= 0 && w->shortcutMute){
            muteButtonStr = "audio-volume-low-symbolic";
        }
        else if (volume > 0 && volume <= 33) {
            muteButtonStr = "audio-volume-low-symbolic";
        }
        else if(volume > 33 && volume <= 66) {
            muteButtonStr = "audio-volume-medium-symbolic";
        }
        else if (volume > 66) {
            muteButtonStr = "audio-volume-high-symbolic";
        }
        muteButtonIcon = QIcon::fromTheme(muteButtonStr);
        QSize iconSize(24,24);
        if ( w->mThemeName == "ukui-white" || w->mThemeName == "ukui-light") {
            btn->setIcon(QIcon(w->drawDarkColoredPixmap((QIcon::fromTheme(muteButtonStr).pixmap(iconSize)))));
        }
        else if (w->mThemeName == UKUI_THEME_BLACK || w->mThemeName == "ukui-black" || w->mThemeName == "ukui-default") {
            btn->setIcon(QIcon(w->drawLightColoredPixmap((QIcon::fromTheme(muteButtonStr).pixmap(iconSize)))));
        }
        QPalette paleteBtn = btn->palette();
        paleteBtn.setColor(QPalette::Highlight,Qt::transparent);
        paleteBtn.setBrush(QPalette::Button,QBrush(QColor(1,1,1,0)));
        btn->setPalette(paleteBtn);
    });

    if (appnum <= 0) {
        w->appWidget->upWidget->hide();
    }
    else {
        w->appWidget->upWidget->show();
    }
    app_count++;
    w->appWidget->m_pVlayout->addWidget(app_widget);
    //设置布局的垂直间距以及设置vlayout四周的间距
    w->appWidget->m_pVlayout->setSpacing(18);
    w->appWidget->displayAppVolumeWidget->resize(358,14+appnum*68);
    w->appWidget->m_pVlayout->setContentsMargins(18,0,34,0);
    w->appWidget->m_pVlayout->update();

    w->appWidget->appMuteBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                            "padding-left:0px;}");
}

/*!
 * \brief
 * \details
 * 更新应用音量的值
 */
void DeviceSwitchWidget::update_app_volume(MateMixerStreamControl *control, QString *str, DeviceSwitchWidget *w )
{
    Q_UNUSED(str);
    guint volume = mate_mixer_stream_control_get_volume(control);
    bool is_mute = mate_mixer_stream_control_get_mute(control);
    volume = guint(volume*100/65536.0+0.5);
    const gchar *controlName = mate_mixer_stream_control_get_name(control);

    int index = -1;
    for(int i=0; i<w->appControlVector.size(); ++i){
        QMap<QString,QStringList> map = w->appControlVector.at(i);
        if(map.begin().value().contains(QString(controlName))){
            index = i;
            break;
        }
    }
    QString appBtnName;
    QString appName;
    if(w->appDisplayVector.size()>index){
        appBtnName = w->appDisplayVector.at(index).at(0);
        appName = w->appDisplayVector.at(index).at(1);
    }
    else
        return;

    QString slider_str = appName;
    QString sliderMuteButtonStr;
    QIcon sliderMuteButtonIcon;
    QSlider *slider = w->findChild<QSlider*>(slider_str);
    if (slider == nullptr)
        return;
    slider->setValue(volume);
    QPushButton *btn = w->findChild<QPushButton *>(appBtnName);
    if (btn == nullptr)
        return;
    if (is_mute) {
        sliderMuteButtonStr = "audio-volume-muted-symbolic";
    }
    else if (volume <= 0 && !w->shortcutMute) {
        sliderMuteButtonStr = "audio-volume-muted-symbolic";
    }
    else if(volume <= 0 && w->shortcutMute){
        sliderMuteButtonStr = "audio-volume-low-symbolic";
    }
    else if (volume > 0 && volume <= 33) {
        sliderMuteButtonStr = "audio-volume-low-symbolic";
    }
    else if(volume > 33 && volume <= 66) {
        sliderMuteButtonStr = "audio-volume-medium-symbolic";
    }
    else if (volume > 66) {
        sliderMuteButtonStr = "audio-volume-high-symbolic";
    }
    QSize iconSize(24,24);
    if ( w->mThemeName == "ukui-white" || w->mThemeName == "ukui-light") {
        btn->setIcon(QIcon(w->drawDarkColoredPixmap((QIcon::fromTheme(sliderMuteButtonStr).pixmap(iconSize)))));
    }
    else if (w->mThemeName == UKUI_THEME_BLACK || w->mThemeName == "ukui-black" || w->mThemeName == "ukui-default") {
        btn->setIcon(QIcon(w->drawLightColoredPixmap((QIcon::fromTheme(sliderMuteButtonStr).pixmap(iconSize)))));
    }
    sliderMuteButtonIcon = QIcon::fromTheme(sliderMuteButtonStr);
}

/*!
 * \brief
 * \details
 * 应用音量静音
 */
void DeviceSwitchWidget::app_volume_mute (MateMixerStreamControl *control, QString *pspec ,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    Q_UNUSED(w);
    MateMixerAppInfo *app_info = mate_mixer_stream_control_get_app_info(control);
    if (app_info == nullptr)
        return;
    bool is_mute = mate_mixer_stream_control_get_mute(control);
    int volume = mate_mixer_stream_control_get_volume(control);
    volume = volume*100/65536.0+0.5;
//    qDebug() << "app mute notify" << is_mute << volume;
    /*if (is_mute) {
        w->appWidget->appMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (volume <= 0) {
        w->appWidget->appMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (volume > 0 && volume <= 33) {
       w->appWidget->appMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
    }
    else if(volume > 33 && volume <= 66) {
        w->appWidget->appMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
    }
    else if (volume > 66) {
        w->appWidget->appMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
    }
    //如果主音量处于静音模式，应用音量恢复时，主音量也因恢复
    Q_EMIT w->appvolume_mute_change_mastervolume_status();*/
}

void DeviceSwitchWidget::pulseDisconnectMseeageBox()
{
    //QMessageBox::critical(NULL, tr("Error"), tr("Unable to connect to the sound system, please check whether the pulseaudio service is running!"),  QMessageBox::Abort);
    //exit(-1);
}

/*!
 * \brief
 * \details
 * 处理context信号
 */
void DeviceSwitchWidget::set_context(DeviceSwitchWidget *w,MateMixerContext *context)
{
    MateMixerStream *pOutputStream = mate_mixer_context_get_default_output_stream(context);
    MateMixerState state =  mate_mixer_context_get_state(context);
    if (pOutputStream != nullptr) {
        w->pulseDisconnectMseeageBox();
    }
    qDebug() << "output stream" << mate_mixer_stream_get_name(pOutputStream) << state;
    g_signal_connect (G_OBJECT (context),
                      "stream-added",
                      G_CALLBACK (on_context_stream_added),
                      w);

    g_signal_connect (G_OBJECT (context),
                      "stream-removed",
                      G_CALLBACK (on_context_stream_removed),
                      w);

    g_signal_connect (G_OBJECT (context),
                      "device-added",
                      G_CALLBACK (on_context_device_added),
                      w);
    g_signal_connect (G_OBJECT (context),
                      "device-removed",
                      G_CALLBACK (on_context_device_removed),
                      w);

    g_signal_connect (G_OBJECT (context),
                      "notify::default-input-stream",
                      G_CALLBACK (on_context_default_input_stream_notify),
                      w);
    g_signal_connect (G_OBJECT (context),
                      "notify::default-output-stream",
                      G_CALLBACK (on_context_default_output_stream_notify),
                      w);
    g_signal_connect (G_OBJECT (context),
                      "stored-control-added",
                      G_CALLBACK (on_context_stored_control_added),
                      w);
    g_signal_connect (G_OBJECT (context),
                      "stored-control-removed",
                      G_CALLBACK (on_context_stored_control_removed),
                      w);
}

void DeviceSwitchWidget::on_context_stream_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(context);
    qDebug() << "remove stream" << name;
    remove_stream (w, name);
}

void DeviceSwitchWidget::remove_stream (DeviceSwitchWidget *w, const gchar *name)
{
    int index = w->output_stream_list->indexOf(name);
    if (index >= 0) {
        w->output_stream_list->removeAt(index);
        w->miniWidget->deviceCombox->removeItem(index);
    }
    index = w->input_stream_list->indexOf(name);
    if (index >= 0) {
        w->input_stream_list->removeAt(index);
    }
    /*w->miniWidget->deviceCombox->removeItem(index);*/
    if (w->appWidget->app_volume_list != nullptr) {
        bar_set_stream(w,nullptr);
    }
}

/*!
 * \brief
 * \details
 * 设备增加
 */
void DeviceSwitchWidget::on_context_device_added (MateMixerContext *context, const gchar *name, DeviceSwitchWidget *w)
{
    MateMixerDevice *device;
    if (strstr(name,"bluez_card")) {
        w->setBluezProfile = true;
        w->bluezDeviceName = name;
    }
    qDebug() << "on context device add" << name;
    device = mate_mixer_context_get_device (context, name);
    if (G_UNLIKELY (device == nullptr))
        return;
    add_device (w, device);
}

/*!
 * \brief
 * \details
 * 添加设备，存储设备列表 */
void DeviceSwitchWidget::add_device (DeviceSwitchWidget *w, MateMixerDevice *device)
{
    g_debug("add device");
    const gchar *pName;
    const gchar *pLabel;
    /*
         * const gchar *m_pLabel;
         * m_pLabel = mate_mixer_device_get_label (m_pDevice);
        */
    pName  = mate_mixer_device_get_name (device);
    pLabel = mate_mixer_device_get_label(device);
    if (w->device_name_list->contains(pName) == false) {
        w->device_name_list->append(pName);
    }
    qDebug() << "add_device" <<pName;
    MateMixerSwitch *profileSwitch;
    const gchar *profileLabel = "";
    const gchar *profileName = "";
    profileSwitch = w->findDeviceProfileSwitch(w,device);
    MateMixerSwitchOption *activeProfile;
    activeProfile = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH (profileSwitch));
    if (G_LIKELY (activeProfile != NULL)) {
        profileLabel = mate_mixer_switch_option_get_label(activeProfile);
        profileName = mate_mixer_switch_option_get_name(activeProfile);
    }

    if (strstr(pName,"bluez_card") && strcmp(profileName,"a2dp_sink") != 0) {
        QString deviceName = pName;
        QString cmd = "pactl set-card-profile " + deviceName + " a2dp_sink";
        qDebug() << "set card profile" << cmd;
        system(cmd.toLatin1().data());
    }
    qDebug() << "active profile is :" << profileLabel << profileName;

    if (profileSwitch != NULL) {

        activeProfile = mate_mixer_switch_get_active_option(profileSwitch);
        if (G_LIKELY (activeProfile != NULL))
            profileLabel = mate_mixer_switch_option_get_label(activeProfile);
        g_signal_connect (G_OBJECT (profileSwitch),
                          "notify::active-option",
                          G_CALLBACK (onDeviceProfileActiveOptionNotify),
                          w);
    }

    w->updateOutputDeviceLabel();
    w->updateInputDeviceLabel();
    /*const gchar *label;
    label = mate_mixer_device_get_label (device);
    w->miniWidget->deviceCombox->addItem(label);*/
}

//拔插耳机图标改变
void DeviceSwitchWidget::plug_IconChange(MateMixerSwitchOption *outputActivePort)
{
    const gchar *outputPortLabel = nullptr;
    outputPortLabel = mate_mixer_switch_option_get_label(outputActivePort);
    if(strstr(outputPortLabel,"耳机"))
    {
        qDebug()<<"current output device:"<<outputPortLabel;
        devWidget->outputDeviceBtn->setIcon(QIcon::fromTheme("audio-headphones",QIcon("/usr/share/ukui-media/img/audio-headphones.svg")));
        miniWidget->deviceBtn->setIcon(QIcon::fromTheme("audio-headphones",QIcon("/usr/share/ukui-media/img/audio-headphones.svg")));
    }
    else
    {
        qDebug()<<"current output device::"<<outputPortLabel;
        devWidget->outputDeviceBtn->setIcon(QIcon::fromTheme("audio-card",QIcon("/usr/share/ukui-media/img/audiocard.svg")));
        miniWidget->deviceBtn->setIcon(QIcon::fromTheme("audio-card",QIcon("/usr/share/ukui-media/img/audiocard.svg")));
    }
//    //发送DBus信号
//    QDBusMessage message =QDBusMessage::createSignal("/", "org.ukui.media", "DbusSignalRecorder");
//    message<<"拔插";
//    QDBusConnection::sessionBus().send(message);
}

void DeviceSwitchWidget::updateOutputDeviceLabel()
{
    MateMixerSwitch *outputPortSwitch;
    const GList  *options ;
    const gchar *outputPortLabel = nullptr;
    QString outputPortName;// = nullptr;

    MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(context);
    if (outputStream == nullptr) {
        return;
    }

    outputPortSwitch = findStreamPortSwitch(this,outputStream);
    options = mate_mixer_switch_list_options(outputPortSwitch);

    MateMixerSwitchOption *outputActivePort;
    outputActivePort = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH (outputPortSwitch));
    if (G_LIKELY (outputActivePort != NULL)) {
        outputPortLabel = mate_mixer_switch_option_get_label(outputActivePort);
        outputPortName = mate_mixer_switch_option_get_name(outputActivePort);
    }
    else {
        setOutputLabelDummyOutput();
    }

    if (outputPortSwitch != NULL) {
        if (G_LIKELY (outputActivePort != NULL))
            outputPortLabel = mate_mixer_switch_option_get_label(outputActivePort);
        if (!MATE_MIXER_IS_SWITCH_OPTION (outputActivePort)) {
//            setOutputLabelDummyOutput();
        }
        else {
            const gchar *outputStreamName = mate_mixer_stream_get_name(outputStream);
            if (strstr(outputStreamName,"bluez")) {
                miniWidget->deviceLabel->setText(tr("Bluetooth"));
                devWidget->outputDeviceDisplayLabel->setText(tr("Bluetooth"));
            }
            else {
                plug_IconChange(outputActivePort);
                miniWidget->deviceLabel->setText(outputPortLabel);
                devWidget->outputDeviceDisplayLabel->setText(outputPortLabel);
            }

            if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){

                qDebug() << "set output port name:" << outputPortName;
                if (m_pInitSystemVolumeSetting->keys().contains("pluginInitVolume")) {
                    bool needInit = m_pInitSystemVolumeSetting->get(PLUGIN_INIT_VOLUME).toBool();
                }

                outputPortName = outputPortName.trimmed();//去掉开头结尾空格
                if (outputPortName == "analog-output-headphones-huawei") {
                    if (m_pInitSystemVolumeSetting->keys().contains("headphones1")) {
                        if(!customSoundFile->isExist(outputPortName)){
                            int headphoneVolume = m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
                            miniWidget->masterVolumeSlider->setValue(headphoneVolume);
                            m_pInitSystemVolumeSetting->set(HEADPHONES_1,false);
                            customSoundFile->addXmlNode(outputPortName,false);
                        }
                    }
                }
                else if (outputPortName == "analog-output-headphones-huawei-2") {
                    if (m_pInitSystemVolumeSetting->keys().contains("headphones2")) {
                        if(!customSoundFile->isExist(outputPortName)){
                            int headphoneVolume = m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
                            miniWidget->masterVolumeSlider->setValue(headphoneVolume);
                            m_pInitSystemVolumeSetting->set(HEADPHONES_2,false);
                            customSoundFile->addXmlNode(outputPortName,false);
                        }
                    }
                }
                else if(outputPortName.contains("iec958-stereo-output")||outputPortName.contains("analog-output")){
                    MateMixerStream *stream = mate_mixer_context_get_default_output_stream (context);
                    QString streamName = mate_mixer_stream_get_name(stream);
                    if(streamName.contains("usb",Qt::CaseInsensitive)){
                        qDebug()<<"bbb usb youxian";
                        outputPortName = "usb_" + outputPortName;
                        if(!customSoundFile->isExist(outputPortName)){
                            if (m_pInitSystemVolumeSetting->keys().contains("headphoneOutputVolume")){
                                int outputVolume = m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
                                qDebug()<<"usb val:"<<outputVolume;
                                miniWidget->masterVolumeSlider->setValue(outputVolume);
                                customSoundFile->addXmlNode(outputPortName,false);
                                qDebug()<<"初始化USB耳机";
                            }
                        }
                    }
                }
                else if(outputPortName.contains("hdmi-output")) {
                    if (m_pInitSystemVolumeSetting->keys().contains("hdmi")) {
                        if(!customSoundFile->isExist(outputPortName)){
                            int hdmiVolume = m_pInitSystemVolumeSetting->get(HDMI_VOLUME).toInt();
                            printf("hdmi vl:%d\n",hdmiVolume);
                            qDebug()<<"hdmiVolume:::"<<QString::number(hdmiVolume);
                            miniWidget->masterVolumeSlider->setValue(hdmiVolume);
                            customSoundFile->addXmlNode(outputPortName,false);
                        }
                    }
                }
//                else if(outputPortName == "histen-algo"){
//                    initHuaweiAudio(this);
//                }
//                else {
//                    if (m_pInitSystemVolumeSetting->keys().contains("outputVolume")) {
//                        if(!customSoundFile->isExist(outputPortName)){
//                            int outputVolume = m_pInitSystemVolumeSetting->get(OUTPUT_VOLUME).toInt();
//                            miniWidget->masterVolumeSlider->setValue(outputVolume);
//                            m_pInitSystemVolumeSetting->set(SPEAKER,false);
//                            customSoundFile->addXmlNode(outputPortName,false);
//                        }
//                    }
//                }
            }

            //            osdWidgetShow(outputPortName);
        }
        g_signal_connect (G_OBJECT (outputPortSwitch),
                          "notify::active-option",
                          G_CALLBACK(onOutputSwitchActiveOptionNotify),
                          this);
        privOutputSwitch = outputPortSwitch;
    }
}

void DeviceSwitchWidget::updateInputDeviceLabel()
{
    MateMixerSwitch *inputPortSwitch;
    const GList  *inputOptions ;
    const gchar *inputPortLabel = nullptr;
    const gchar *inputPortName = nullptr;
    MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(context);
    if (inputStream == nullptr) {
        return;
    }
    inputPortSwitch = findStreamPortSwitch(this,inputStream);

    inputOptions = mate_mixer_switch_list_options(inputPortSwitch);
    MateMixerSwitchOption *inputActiveOption = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(inputPortSwitch));

    MateMixerSwitchOption *inputActivePort;
    inputActivePort = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH (inputPortSwitch));
    if (G_LIKELY (inputActiveOption != NULL)) {
        inputPortLabel = mate_mixer_switch_option_get_label(inputActivePort);
        inputPortName = mate_mixer_switch_option_get_name(inputActivePort);
    }

    if (inputPortSwitch != NULL) {
        if (G_LIKELY (inputActiveOption != NULL))
            inputPortLabel = mate_mixer_switch_option_get_label(inputActivePort);
        if (MATE_MIXER_IS_SWITCH_OPTION (inputActivePort)) {
            devWidget->inputDeviceDisplayLabel->setText(inputPortLabel);
        }
        if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){

            qDebug() << "set input port name:" << inputPortName;
            if (m_pInitSystemVolumeSetting->keys().contains("pluginInitVolume")) {
                bool needInit = m_pInitSystemVolumeSetting->get(PLUGIN_INIT_VOLUME).toBool();
            }
            if (strcmp(inputPortName,"analogInputHeadsetMicHuawei") == 0) {
                if (m_pInitSystemVolumeSetting->keys().contains("headsetMic")) {
                    bool setHeadset = m_pInitSystemVolumeSetting->get(HEADSET_MIC).toBool();
                    //if (setHeadset)
                    if(!customSoundFile->isExist(inputPortName)){
                        int headsetVolume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
                        devWidget->inputDeviceSlider->setValue(headsetVolume);
                        m_pInitSystemVolumeSetting->set(HEADSET_MIC,false);
                        QString str = QString(inputPortName);
                        customSoundFile->addXmlNode(str,false);
                    }
                }
            }
            else if (strcmp(inputPortName,"analogInputRearMicHuawei") == 0) {
                if (m_pInitSystemVolumeSetting->keys().contains("rearMic")) {
                    bool setRearMic = m_pInitSystemVolumeSetting->get(REAR_MIC).toBool();
                    //if (setRearMic)
                    if(!customSoundFile->isExist(inputPortName)){
                        int rearMicVolume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
                        devWidget->inputDeviceSlider->setValue(rearMicVolume);
                        m_pInitSystemVolumeSetting->set(REAR_MIC,false);
                        QString str = QString(inputPortName);
                        customSoundFile->addXmlNode(str,false);
                    }
                }
            }
            else if (strcmp(inputPortName,"analogInputLinein-huawei") == 0) {
                if (m_pInitSystemVolumeSetting->keys().contains("linein")) {
                    bool setLinein = m_pInitSystemVolumeSetting->get(LINEIN).toBool();
                    //if (setLinein)
                    if(!customSoundFile->isExist(inputPortName)){
                        int lineinVolume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
                        devWidget->inputDeviceSlider->setValue(lineinVolume);
                        m_pInitSystemVolumeSetting->set(LINEIN,false);
                        QString str = QString(inputPortName);
                        customSoundFile->addXmlNode(str,false);
                    }
                }
            }
            else{
                if (m_pInitSystemVolumeSetting->keys().contains("micVolume")) {
                    if(!customSoundFile->isExist(inputPortName)){
                        int intelMicVolume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
                        devWidget->inputDeviceSlider->setValue(intelMicVolume);
                        m_pInitSystemVolumeSetting->set(INTEL_MIC,false);
                        QString str = QString(inputPortName);
                        customSoundFile->addXmlNode(str,false);
                    }
                }
            }
        }
        g_signal_connect (G_OBJECT (inputPortSwitch),
                          "notify::active-option",
                          G_CALLBACK(onInputSwitchActiveOptionNotify),
                          this);
    }
}

void DeviceSwitchWidget::setOutputLabelDummyOutput()
{
    miniWidget->deviceLabel->setText(tr("Dummy output"));
    devWidget->outputDeviceDisplayLabel->setText(tr("Dummy output"));
    //伪输出还用输出图标
    devWidget->outputDeviceBtn->setIcon(QIcon::fromTheme("audio-card",QIcon("/usr/share/ukui-media/img/audiocard.svg")));
    miniWidget->deviceBtn->setIcon(QIcon::fromTheme("audio-card",QIcon("/usr/share/ukui-media/img/audiocard.svg")));
}

void DeviceSwitchWidget::onOutputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    MateMixerSwitchOption *action = mate_mixer_switch_get_active_option(swtch);
    mate_mixer_switch_option_get_label(action);
    const gchar *outputPortLabel = nullptr;
    QString outputPortName = nullptr;
    outputPortName = mate_mixer_switch_option_get_name(action); 
    outputPortLabel = mate_mixer_switch_option_get_label(action);
    if (!MATE_MIXER_IS_SWITCH_OPTION (action)) {
        w->setOutputLabelDummyOutput();
    }
    else {
        MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(w->context);
        const gchar *outputStreamName = mate_mixer_stream_get_name(outputStream);
        if (strstr(outputStreamName,"bluez")) {
            w->miniWidget->deviceLabel->setText(tr("Bluetooth"));
            w->devWidget->outputDeviceDisplayLabel->setText(tr("Bluetooth"));
        }
        /*else {
            QString str =(QString)outputPortLabel;
            if(str.contains("耳机"))
            {
                //w->devWidget->outputDeviceBtn->setIcon(QIcon::fromTheme("audio-headphones",QIcon("/usr/share/ukui-media/img/audio-headphones.svg")));
                //w->miniWidget->deviceBtn->setIcon(QIcon::fromTheme("audio-headphones",QIcon("/usr/share/ukui-media/img/audio-headphones.svg")));
            }
            else
            {
                //w->devWidget->outputDeviceBtn->setIcon(QIcon::fromTheme("audio-card",QIcon("/usr/share/ukui-media/img/audiocard.svg")));
                //w->miniWidget->deviceBtn->setIcon(QIcon::fromTheme("audio-card",QIcon("/usr/share/ukui-media/img/audiocard.svg")));
            }
            //w->miniWidget->deviceLabel->setText(outputPortLabel);
            //w->devWidget->outputDeviceDisplayLabel->setText(outputPortLabel);
        }*/
    }

//    if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){
//        if (w->m_pInitSystemVolumeSetting->keys().contains("pluginInitVolume")) {
//            //bool needInit = w->m_pInitSystemVolumeSetting->get(PLUGIN_INIT_VOLUME).toBool();
//            //if (needInit) {
//            if(outputPortName.contains("headphones-huawei")) {
//                if (outputPortName == "analog-output-headphones-huawei") {
//                    if (w->m_pInitSystemVolumeSetting->keys().contains("headphones1")) {
//                    //bool setHeadphoneVoulme = w->m_pInitSystemVolumeSetting->get(HEADPHONES_1).toBool();

//                        if (w->m_pInitSystemVolumeSetting->keys().contains("headphoneOutputVolume")) {
//                            if(!w->customSoundFile->isExist(outputPortName)){
//                                int headphoneVolume = w->m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
//                                w->miniWidget->masterVolumeSlider->setValue(headphoneVolume);
//                                w->m_pInitSystemVolumeSetting->set(HEADPHONES_1,false);
//                                QString str = QString(outputPortName);
//                                w->customSoundFile->addXmlNode(str,false);
//                            }

//                        }

//                    }
//                }
//            }
//                else if (outputPortName.contains("hdmi-output")) {
//                    if (w->m_pInitSystemVolumeSetting->keys().contains("hdmi")) {
//                        //bool setHdmiVolume = w->m_pInitSystemVolumeSetting->get(HDMI).toBool();

//                        if (w->m_pInitSystemVolumeSetting->keys().contains("hdmiVolume")) {
//                            if(!w->customSoundFile->isExist(outputPortName)){
//                                int hdmiVolume = w->m_pInitSystemVolumeSetting->get(HDMI_VOLUME).toInt();
//                                w->miniWidget->masterVolumeSlider->setValue(hdmiVolume);
//                                w->m_pInitSystemVolumeSetting->set(HDMI,false);
//                                w->customSoundFile->addXmlNode(outputPortName,false);
//                            }
//                        }

//                    }
//                }
//                else if(outputPortName == "histen-algo"){
//                    w->initHuaweiAudio(w);
//                }
//                else {
//                    if (w->m_pInitSystemVolumeSetting->keys().contains("outputVolume")) {
//                        if(!w->customSoundFile->isExist(outputPortName)){
//                            int outputVolume = w->m_pInitSystemVolumeSetting->get(OUTPUT_VOLUME).toInt();
//                            w->miniWidget->masterVolumeSlider->setValue(outputVolume);
//                            w->m_pInitSystemVolumeSetting->set(SPEAKER,false);
//                            w->customSoundFile->addXmlNode(outputPortName,false);
//                        }
//                    }
//                }
//            }
//        }

    //w->osdWidgetShow(outputPortName.toStdString().c_str());
}

void DeviceSwitchWidget::osdWidgetShow(const gchar *portName)
{
    //获取透明度
    if (QGSettings::isSchemaInstalled(UKUI_THEME_SETTING)){
        if (m_pTransparencySetting->keys().contains("transparency")) {
            transparency = m_pTransparencySetting->get("transparency").toDouble();
        }
    }

    if (!osdWidget->isHidden()) {
        osdWidget->hide();

        if (timer != nullptr) {
            timer->disconnect();
            timer = nullptr;
            delete timer;
        }
    }
    if (strstr(portName,"speaker")) {
        osdWidget->UkmediaOsdSetIcon("audio-card");
        osdWidget->show();
    }
    else if (strstr(portName,"headphone")) {
        osdWidget->UkmediaOsdSetIcon("audio-headphones");
        osdWidget->show();
    }
    osdWidget->setWindowOpacity(transparency);
    timer = new MyTimer(osdWidget); //this 为parent类, 表示当前窗口
    connect(timer, SIGNAL(timeOut()), this, SLOT(osdDisplayWidgetHide()));
}

void DeviceSwitchWidget::onInputSwitchActiveOptionNotify (MateMixerSwitch *swtch,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    MateMixerSwitchOption *action = mate_mixer_switch_get_active_option(swtch);
    mate_mixer_switch_option_get_label(action);
    const gchar *inputPortLabel = nullptr;
    MateMixerStream *defaultInputStream = mate_mixer_context_get_default_input_stream(w->context);
    MateMixerStream *stream = mate_mixer_stream_switch_get_stream(MATE_MIXER_STREAM_SWITCH(swtch));
    inputPortLabel = mate_mixer_switch_option_get_label(action);
    const gchar *inputPortName = mate_mixer_switch_option_get_name(action);
    qDebug() << "onInputSwitchActiveOptionNotify" << inputPortName <<mate_mixer_stream_get_name(stream);
    if(w->isInputPortSame != inputPortLabel && stream == defaultInputStream) {
        //发送DBus信号
        QDBusMessage message =QDBusMessage::createSignal("/", "org.ukui.media", "DbusSignalRecorder");
        message<<"拔插";
        QDBusConnection::sessionBus().send(message);

    }
    w->isInputPortSame = inputPortLabel;
//    if (QGSettings::isSchemaInstalled(UKUI_AUDIO_SCHEMA)){
//        if (strcmp(inputPortName,"analog-input-headset-mic-huawei") == 0) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("micVolume")) {
//                //bool setHeadset = w->m_pInitSystemVolumeSetting->get(HEADSET_MIC).toBool();
//                //if (setHeadset)
//                if(!w->customSoundFile->isExist(inputPortName)) {
//                    int headsetVolume = w->m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
//                    w->devWidget->inputDeviceSlider->setValue(headsetVolume);
//                    //w->m_pInitSystemVolumeSetting->set(HEADSET_MIC,false);
//                    QString str = QString(inputPortName);
//                    w->customSoundFile->addXmlNode(str,false);
//                }
//            }
//            w->initHuaweiAudio(w); //HW定制音量初始化
//        }
//        else if (strcmp(inputPortName,"analog-input-rear-mic-huawei") == 0) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("micVolume")) {
//                //bool setRearMic = w->m_pInitSystemVolumeSetting->get(REAR_MIC).toBool();
//                //if (setRearMic)
//                if(!w->customSoundFile->isExist(inputPortName)) {
//                    int rearMicVolume = w->m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
//                    w->devWidget->inputDeviceSlider->setValue(rearMicVolume);
//                    //w->m_pInitSystemVolumeSetting->set(REAR_MIC,false);
//                    QString str = QString(inputPortName);
//                    w->customSoundFile->addXmlNode(str,false);
//                }
//            }
//            w->initHuaweiAudio(w); //HW定制音量初始化
//        }
//        else if (strcmp(inputPortName,"analog-input-linein-huawei") == 0) {
//            if (w->m_pInitSystemVolumeSetting->keys().contains("micVolume")) {
//                //bool setLinein = w->m_pInitSystemVolumeSetting->get(LINEIN).toBool();
//                //if (setLinein)
//                if(!w->customSoundFile->isExist(inputPortName)){
//                    int lineinVolume = w->m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
//                    w->devWidget->inputDeviceSlider->setValue(lineinVolume);
//                    //w->m_pInitSystemVolumeSetting->set(LINEIN,false);
//                    QString str = QString(inputPortName);
//                    w->customSoundFile->addXmlNode(str,false);
//                }
//            }
//            w->initHuaweiAudio(w); //HW定制音量初始化
//        }
//        else if (strcmp(inputPortName,"analog-input-internal-mic-huawei") == 0) {

//            if (w->m_pInitSystemVolumeSetting->keys().contains("micVolume")) {
//                //bool setIntelMic = w->m_pInitSystemVolumeSetting->get(INTEL_MIC).toBool();
//                //if (setIntelMic)
//                if(!w->customSoundFile->isExist(inputPortName)){
//                    int intelMicVolume = w->m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
//                    w->devWidget->inputDeviceSlider->setValue(intelMicVolume);
//                    //w->m_pInitSystemVolumeSetting->set(INTEL_MIC,false);
//                    QString str = QString(inputPortName);
//                    w->customSoundFile->addXmlNode(str,false);
//                }
//            }
//            w->initHuaweiAudio(w); //HW定制音量初始化
//        }
//        else{
//            if (w->m_pInitSystemVolumeSetting->keys().contains("micVolume")) {
//                if(!w->customSoundFile->isExist(inputPortName)){
//                    int intelMicVolume = w->m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
//                    w->devWidget->inputDeviceSlider->setValue(intelMicVolume);
//                    //w->m_pInitSystemVolumeSetting->set(INTEL_MIC,false);
//                    QString str = QString(inputPortName);
//                    w->customSoundFile->addXmlNode(str,false);
//                }
//            }
//            w->initHuaweiAudio(w); //HW定制音量初始化
//        }
//    }

    if (MATE_MIXER_IS_SWITCH_OPTION (action)) {
//        w->devWidget->inputDeviceDisplayLabel->setText(inputPortLabel);
    }
}

void DeviceSwitchWidget::onDeviceProfileActiveOptionNotify (MateMixerDeviceSwitch *swtch,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    MateMixerDevice *device;
    device = mate_mixer_device_switch_get_device (swtch);
    //更新输出设备显示标签

    w->updateOutputDeviceLabel();
    w->updateInputDeviceLabel();
    //    updateDeviceInfo (w, device);
}

MateMixerSwitch * DeviceSwitchWidget::findDeviceProfileSwitch (DeviceSwitchWidget *w,MateMixerDevice *device)
{
    const GList *switches;
    const gchar *profileLabel = nullptr;
    const gchar *devName = nullptr;
    devName = mate_mixer_device_get_name(device);
    switches = mate_mixer_device_list_switches (device);
    while (switches != nullptr) {
        MateMixerDeviceSwitch *swtch = MATE_MIXER_DEVICE_SWITCH (switches->data);
        MateMixerSwitchOption *active;
        active = mate_mixer_switch_get_active_option (MATE_MIXER_SWITCH (swtch));
        if (G_LIKELY (active != NULL)) {
            profileLabel = mate_mixer_switch_option_get_label (active);
        }

        QString deviceStr = w->device_name_list->at(0);
        QByteArray bba = deviceStr.toLatin1();
        const gchar * deviceName = bba.data();
        if (strcmp(deviceName,devName) == 0) {
//            qDebug() << "设置当前配置*****************" << deviceName;
        }
        if (mate_mixer_device_switch_get_role (swtch) == MATE_MIXER_DEVICE_SWITCH_ROLE_PROFILE)
            return MATE_MIXER_SWITCH (swtch);

        switches =  switches->next;
    }
    return nullptr;
}

/*!
 * \brief
 * \details
 * 移除设备，将改设备名从列表移除
 */
void DeviceSwitchWidget::on_context_device_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(context);
    qDebug() <<"device remove:" << name;
    if (strstr(name,"bluez_card")) {
        w->setBluezProfile = false;
        w->bluezDeviceName = "";
    }
    int  index = w->device_name_list->indexOf(name);
    if (index == -1)
        return;
    if (index >= 0) {
        w->device_name_list->removeAt(index);
    }
}

/*!
 * \brief
 * \details
 * 默认的输入流更改时会触发此函数（拔插耳机，带输入的声卡设备）
 */
void DeviceSwitchWidget::on_context_default_input_stream_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    MateMixerStream *stream;

    stream = mate_mixer_context_get_default_input_stream (context);
    const gchar *streamName = mate_mixer_stream_get_name(stream);
    qDebug() << "on_context_default_input_stream_notify" << streamName;
    if (MATE_MIXER_IS_STREAM(stream)) {
        if (strstr(streamName,"bluez_source") || strstr(streamName,"bt_sco_source")) {
            if (w->appWidget->isRecording == false) {
                system("rm /tmp/test.raw");
                w->appWidget->fullushBlueRecordStream();
            }
        }
        else {
            if (w->appWidget->isRecording == true) {
                w->appWidget->deleteBlueRecordStream();
            }
        }
        //发送DBus信号
        QDBusMessage message =QDBusMessage::createSignal("/", "org.ukui.media", "DbusSignalRecorder");
        message << "拔插";
        QDBusConnection::sessionBus().send(message);
        qDebug() << "Default input stream change to" << mate_mixer_stream_get_name(stream) <<w->setBluezProfile;
        set_input_stream (w, stream);
        update_icon_input (w,stream);
        w->updateInputDeviceLabel();
    }
}

void DeviceSwitchWidget::set_input_stream (DeviceSwitchWidget *w, MateMixerStream *stream)
{
    if (stream == nullptr) {
        return;
    }
    bar_set_stream (w, stream);

    if (stream != nullptr) {
        const GList *controls;
        controls = mate_mixer_context_list_stored_controls (w->context);

        /* Move all stored controls to the newly selected default stream */
        while (controls != nullptr) {
            MateMixerStream *parent;
            MateMixerStreamControl *control = MATE_MIXER_STREAM_CONTROL (controls->data);
            parent  = mate_mixer_stream_control_get_stream (control);

            /* Prefer streamless controls to stay the way they are, forcing them to
             * a particular owning stream would be wrong for eg. event controls */
            if (parent != nullptr && parent != stream) {
                MateMixerDirection direction =mate_mixer_stream_get_direction (parent);

                if (direction == MATE_MIXER_DIRECTION_INPUT) {
                    mate_mixer_stream_control_set_stream (control, stream);
                }
            }
            controls = controls->next;
        }

        /* Enable/disable the peak level monitor according to mute state */
        g_signal_connect (G_OBJECT (stream),
                          "notify::mute",
                          G_CALLBACK (on_stream_control_mute_notify),
                          w);
    }
    MateMixerStreamControl *control = mate_mixer_stream_get_default_control(stream);
    update_input_settings (w,control);
}

/*!
 * \brief
 * \details
 * 静音通知功能
 */
void DeviceSwitchWidget::on_stream_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    Q_UNUSED(w);

    update_icon_output(w,w->context);
    /*Stop monitoring the input stream when it gets muted */
    if (mate_mixer_stream_control_get_mute (control) == TRUE) {
        mate_mixer_stream_control_set_monitor_enabled (control, FALSE);
    }
    else {
        mate_mixer_stream_control_set_monitor_enabled (control, TRUE);
    }
}

/*!
 * \brief
 * \details
 * 默认的输出流更改，切换输出设备（包括拔插耳机，声卡设备都会触发此函数）
 */
void DeviceSwitchWidget::on_context_default_output_stream_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    MateMixerStream *stream;
    stream = mate_mixer_context_get_default_output_stream (context);
    MateMixerStreamControl *control = mate_mixer_stream_get_default_control(stream);
    int value = mate_mixer_stream_control_get_volume(control);
    value = value * 100 / 65536.0 +0.5;
    if (stream == nullptr) {
        return;
    }
    qDebug() << "default output stream notify" << value << mate_mixer_stream_control_get_name(control);

    update_icon_output(w,context);
    //    set_output_stream (w, stream);
    w->updateOutputDeviceLabel();
}

/*!
 * \brief
 * \details
 * 移除存储的control
 */
void DeviceSwitchWidget::on_context_stored_control_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(context);
    Q_UNUSED(name);
    if (w->appWidget->app_volume_list != nullptr) {
        bar_set_stream (w, nullptr);
    }
}

/*!
 * \brief
 * \details
 * 设置context
 */
void DeviceSwitchWidget::context_set_property(DeviceSwitchWidget *w)
{
    set_context(w,w->context);
}

/*!
 * \brief
 * \details
 * 更新输入引来
 */
void DeviceSwitchWidget::update_icon_input (DeviceSwitchWidget *w,MateMixerStream *stream)
{
    qDebug()<<"update_icon_input";
    MateMixerStreamControl *control = nullptr;
    const gchar *app_id;
    const gchar *inputControlName;
    gboolean show = false;

    stream = mate_mixer_context_get_default_input_stream(w->context);
    if(!MATE_MIXER_IS_STREAM(stream))
    {
        qWarning()<<"current input stream is not a stream";
        return ;
    }
    const GList *inputs =mate_mixer_stream_list_controls(stream);
    control = mate_mixer_stream_get_default_control(stream);
    inputControlName = mate_mixer_stream_control_get_name(control);
    qDebug() << "update icon input" << inputControlName;
    if (inputControlName != nullptr && inputControlName != "auto_null.monitor") {
        if (strstr(inputControlName,"alsa_input") || strstr(inputControlName,"bluez_source") || strstr(inputControlName,"bt_sco_source") || strstr(inputControlName,"3a_source"))
            show = true;
    }

    //初始化滑动条的值
    int volume = int(mate_mixer_stream_control_get_volume(control));
    int value = int(volume *100 /65536.0+0.5);
    w->setInputVolume = true;
    w->devWidget->inputDeviceSlider->blockSignals(true);
    w->devWidget->inputDeviceSlider->setValue(value);
    w->devWidget->inputDeviceSlider->blockSignals(false);

    w->themeChangeIcons();
    while (inputs != nullptr) {
        MateMixerStreamControl *input = MATE_MIXER_STREAM_CONTROL (inputs->data);
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role (input);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *app_info = mate_mixer_stream_control_get_app_info (input);
            if (app_info == nullptr)
                return;
            app_id = mate_mixer_app_info_get_id (app_info);
            if (app_id == nullptr) {
                /* A recording application which has no
                 * identifier set */
                g_debug ("Found a recording application control %s",
                         mate_mixer_stream_control_get_label (input));

                if G_UNLIKELY (control == nullptr) {
                    /* In the unlikely case when there is no
                     * default input control, use the application
                     * control for the icon */
                    control = input;
                }
                /*show = true;*/
                break;
            }

            if (strcmp (app_id, "org.mate.VolumeControl") != 0 &&
                    strcmp (app_id, "org.gnome.VolumeControl") != 0 &&
                    strcmp (app_id, "org.PulseAudio.pavucontrol") != 0) {

                g_debug ("Found a recording application %s", app_id);
                if G_UNLIKELY (control == nullptr)
                        control = input;
                show = true;
                break;
            }
        }
        inputs = inputs->next;
    }

    if (show == true) {
        qDebug() << "Input icon enabled";
    }
    else {
        qDebug() << "There is no recording application, input icon disabled";
    }

    gvc_stream_status_icon_set_control (w, control);

    if(show) {
        w->devWidget->inputWidgetShow();
    }
    else {
        w->devWidget->inputWidgetHide();
    }
}

/*!
 * \brief
 * \details
 * 更新输出音量及托盘图标
 */
void DeviceSwitchWidget::update_icon_output (DeviceSwitchWidget *w,MateMixerContext *context)
{
    MateMixerStream *stream;
    MateMixerStreamControl *control = nullptr;
    stream = mate_mixer_context_get_default_output_stream (context);
    if (stream != nullptr){
        control = mate_mixer_stream_get_default_control (stream);
    w->outputControlName = mate_mixer_stream_control_get_name(control);
    w->miniWidget->masterVolumeSlider->setObjectName(w->outputControlName);
    gvc_stream_status_icon_set_control (w, control);
    //初始化滑动条的值
    bool state = mate_mixer_stream_control_get_mute(control);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    int value = int(volume *100 /65536.0+0.5);
    QSlider *slider1 = w->miniWidget->findChild<QSlider *>(w->outputControlName);
    if (slider1 == nullptr)
        return;
    w->setOutputVolume = true;
    w->devWidget->outputDeviceSlider->blockSignals(true);
    w->appWidget->systemVolumeSlider->blockSignals(true);
    w->miniWidget->masterVolumeSlider->blockSignals(true);
    w->devWidget->outputDeviceSlider->setValue(value);
    w->appWidget->systemVolumeSlider->setValue(value);
    w->miniWidget->masterVolumeSlider->setValue(value);
    QString percentStr = QString::number(value);
    w->miniWidget->displayVolumeLabel->setText(percentStr+"%");
    w->appWidget->systemVolumeDisplayLabel->setText(percentStr+"%");
    w->devWidget->outputDeviceSlider->blockSignals(false);
    w->appWidget->systemVolumeSlider->blockSignals(false);
    w->miniWidget->masterVolumeSlider->blockSignals(false);

    QString percent = QString::number(value);
    QString systemTrayIcon;
    QString audioIconStr;
    QString muteComboxStr;
    QIcon trayIcon;
    QIcon audioIcon;

    //设置为输出音量为的状态
    mate_mixer_stream_control_set_mute(control,state);
    qDebug() << "update icon output" << mate_mixer_stream_control_get_name(control);
    if (state) {
        systemTrayIcon = "audio-volume-muted-symbolic";
        audioIconStr = "audio-volume-muted-symbolic";
        muteComboxStr = "object-select-symbolic";
    }
    else if (value <= 0 && !w->shortcutMute) {
        systemTrayIcon = "audio-volume-muted-symbolic";
        audioIconStr = "audio-volume-muted-symbolic";
        muteComboxStr = "";
        //如果主主音量处于静音状态，应用音量取消静音则设置主音量取消静音
        /*if (state) {
            connect(w,&DeviceSwitchWidget::appvolume_mute_change_mastervolume_status,[=](){
                mate_mixer_stream_control_set_mute(control,!state);
            });
        }*/
    }
    else if(value <= 0 && w->shortcutMute){
        systemTrayIcon = "audio-volume-low-symbolic";
        audioIconStr = "audio-volume-low-symbolic";
        muteComboxStr = "";
    }
    else if (value > 0 && value <= 33) {
        systemTrayIcon = "audio-volume-low-symbolic";
        audioIconStr = "audio-volume-low-symbolic";
        muteComboxStr = "";
    }
    else if(value > 33 && value <= 66) {
        systemTrayIcon = "audio-volume-medium-symbolic";
        audioIconStr = "audio-volume-medium-symbolic";
        muteComboxStr = "";
    }
    else if (value > 66) {
        systemTrayIcon = "audio-volume-high-symbolic";
        audioIconStr = "audio-volume-high-symbolic";
        muteComboxStr = "";
    }

    w->themeChangeIcons();
    trayIcon = QIcon::fromTheme(systemTrayIcon);
    w->m_pMuteAction->setIcon(QIcon::fromTheme(muteComboxStr));
    w->soundSystemTrayIcon->setIcon(QIcon(trayIcon));
    w->appWidget->systemVolumeBtn->setIcon(QIcon(audioIcon));
    w->miniWidget->displayVolumeLabel->setText(percent+"%");
    w->appWidget->systemVolumeDisplayLabel->setText(percent+"%");
    }
}

//osd超时隐藏
void DeviceSwitchWidget::osdDisplayWidgetHide()
{
    this->osdWidget->hide();
}

void DeviceSwitchWidget::volumeSettingChangedSlot()
{
    MateMixerStream *stream;
    MateMixerStreamControl *control = nullptr;
    stream = mate_mixer_context_get_default_output_stream (context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control (stream);

    if (m_pVolumeSetting->keys().contains("volumesize")) {
        int valueSetting = m_pVolumeSetting->get(UKUI_VOLUME_KEY).toInt();
        miniWidget->masterVolumeSlider->setValue(valueSetting);
    }

    if (QGSettings::isSchemaInstalled(UKUI_VOLUME_BRIGHTNESS_GSETTING_ID)) {
        if (m_pVolumeSetting->keys().contains("soundstate")) {
            bool status = m_pVolumeSetting->get(UKUI_VOLUME_STATE).toBool();
            mate_mixer_stream_control_set_mute(control,status);
        }
    }
}

void DeviceSwitchWidget::gvc_stream_status_icon_set_control (DeviceSwitchWidget *w,MateMixerStreamControl *control)
{
    if (w->currentControl == control)
        return;

    if (control != NULL)
        g_object_ref (control);
    w->currentControl = control;

    if (w->currentControl != nullptr) {
        g_signal_connect ( G_OBJECT (w->currentControl),
                           "notify::volume",
                           G_CALLBACK (on_stream_control_volume_notify),
                           w);
        g_signal_connect (G_OBJECT (w->currentControl),
                          "notify::mute",
                          G_CALLBACK (on_stream_control_mute_notify),
                          w);
    }
    MateMixerStreamControlFlags flags = mate_mixer_stream_control_get_flags(control);
    if (flags & MATE_MIXER_STREAM_CONTROL_MUTE_READABLE) {
        g_signal_connect (G_OBJECT (control),
                          "notify::volume",
                          G_CALLBACK (on_stream_control_volume_notify),
                          w);

        g_signal_connect (G_OBJECT (control),
                          "notify::mute",
                          G_CALLBACK (on_control_mute_notify),
                          w);
    }
}

QPixmap DeviceSwitchWidget::drawDarkColoredPixmap(const QPixmap &source)
{
    QColor gray(255,255,255);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<20 && qAbs(color.green()-gray.green())<20 && qAbs(color.blue()-gray.blue())<20) {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
                    img.setPixelColor(x, y, color);
                }
                else {
                    color.setRed(0);
                    color.setGreen(0);
                    color.setBlue(0);
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

QPixmap DeviceSwitchWidget::drawLightColoredPixmap(const QPixmap &source)
{
    QColor gray(255,255,255);
    QColor standard (0,0,0);
    QImage img = source.toImage();
    for (int x = 0; x < img.width(); x++) {
        for (int y = 0; y < img.height(); y++) {
            auto color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                if (qAbs(color.red()-gray.red())<20 && qAbs(color.green()-gray.green())<20 && qAbs(color.blue()-gray.blue())<20) {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
                else {
                    color.setRed(255);
                    color.setGreen(255);
                    color.setBlue(255);
                    img.setPixelColor(x, y, color);
                }
            }
        }
    }
    return QPixmap::fromImage(img);
}

/*!
 * \brief
 * \details
 * 静音
 */
void DeviceSwitchWidget::on_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    gboolean mute = mate_mixer_stream_control_get_mute (control);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    MateMixerStream *stream = mate_mixer_stream_control_get_stream(control);
    if(!MATE_MIXER_IS_STREAM(stream)){
        qDebug() << "on_control_mute_notify  Exception handling --------------";
        stream = mate_mixer_context_get_stream(w->context,mate_mixer_stream_control_get_name(control));
        //使用命令重新设置音量
        bool isMuted = mate_mixer_stream_control_get_mute(control);
        QString cmd = "pactl set-sink-mute "+ QString(mate_mixer_stream_control_get_name(control)) +" "+ QString::number(isMuted,10);
        system(cmd.toLocal8Bit().data());
        return;
    }
    MateMixerDirection direction;
    if (stream != nullptr) {
        direction = mate_mixer_stream_get_direction(stream);
    }
    else {
        qDebug() << "stream is nullptr";
    }
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        if(!mute && volume<=0){
            w->shortcutMute = true;
        }
        else{
            w->shortcutMute = false;
        }
        w->updateSystemTrayIcon(volume,mute);

        if (mute) {
            system("echo mute > /tmp/kylin_output_muted.log");
        }
        else {
            system("echo no > /tmp/kylin_output_muted.log");
        }
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        if (mute) {
            system("echo mute > /tmp/kylin_input_muted.log");
        }
        else {
            system("echo no > /tmp/kylin_input_muted.log");
        }
    }
    if (QGSettings::isSchemaInstalled(UKUI_VOLUME_BRIGHTNESS_GSETTING_ID)) {
        if (w->m_pVolumeSetting->keys().contains("soundstate")) {
            w->m_pVolumeSetting->blockSignals(true);
            w->m_pVolumeSetting->set(UKUI_VOLUME_STATE,mute);
            w->m_pVolumeSetting->blockSignals(false);
        }
    }
    w->themeChangeIcons();
}

/*!
 * \brief
 * \details
 * 音量值更改时会触发此函数
 */
void DeviceSwitchWidget::on_stream_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    QString decscription;
    MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(w->context);
    MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(w->context);
    guint volume = 0;

    MateMixerStream *stream = mate_mixer_stream_control_get_stream(control);
    //异常处理：libmatemixer 的 mate_mixer_stream_control_get_stream() 存在bug，会导致stream failed，这里重新获取一下
    if(!MATE_MIXER_IS_STREAM(stream)){
        stream = mate_mixer_context_get_stream(w->context,mate_mixer_stream_control_get_name(control));
        //使用命令重新设置音量
        volume = mate_mixer_stream_control_get_volume(control);
        QString cmd = "pactl set-sink-volume "+ QString(mate_mixer_stream_control_get_name(control)) +" "+ QString::number(volume,10);
        system(cmd.toLocal8Bit().data());
    }
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);
//    qDebug()<<"control name:"<<mate_mixer_stream_control_get_name(control);
//    qDebug()<<"stream  name:"<<mate_mixer_stream_get_name(stream);
//    if (outputStream != stream && inputStream != stream) {
//        return;
//    }

    if (w->setOutputVolume == true && direction == MATE_MIXER_DIRECTION_OUTPUT) {
        w->setOutputVolume = false;
        return;
    }

    MateMixerStreamControlFlags flags;
    gboolean muted = FALSE;
    gdouble decibel = 0.0;

    if (control != nullptr)
        flags = mate_mixer_stream_control_get_flags(control);

    if(flags&MATE_MIXER_STREAM_CONTROL_MUTE_READABLE)
        muted = mate_mixer_stream_control_get_mute(control);

    if (flags&MATE_MIXER_STREAM_CONTROL_VOLUME_READABLE) {
        volume = mate_mixer_stream_control_get_volume(control);
    }

    if (flags&MATE_MIXER_STREAM_CONTROL_HAS_DECIBEL)
        decibel = mate_mixer_stream_control_get_decibel(control);
    decscription = mate_mixer_stream_control_get_label(control);

    if (MATE_MIXER_IS_STREAM(stream)) {
        //qDebug() << "***stream name is" << mate_mixer_stream_control_get_name(control) << mate_mixer_stream_get_label(stream);
    }
    else {
        stream = w->stream;
        direction = mate_mixer_stream_get_direction(MATE_MIXER_STREAM(stream));
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
            set_output_stream(w,stream);
            /*mate_mixer_context_set_default_output_stream(w->context,stream);*/
        }
        else if (direction == MATE_MIXER_DIRECTION_INPUT) {
            set_input_stream(w,stream);
            /*mate_mixer_context_set_default_input_stream(w->context,stream);*/
        }
    }

    direction = mate_mixer_stream_get_direction(MATE_MIXER_STREAM(stream));
    //设置输出滑动条的值

//    ca_context *context;
//    ca_context_create(&context);
    int value;
    value = int(volume*100/65536.0 + 0.5);

    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        w->setOutputVolume = true;
        w->devWidget->outputDeviceSlider->blockSignals(true);
        w->appWidget->systemVolumeSlider->blockSignals(true);
        w->miniWidget->masterVolumeSlider->blockSignals(true);
        w->devWidget->outputDeviceSlider->setValue(value);
        w->appWidget->systemVolumeSlider->setValue(value);
        w->miniWidget->masterVolumeSlider->setValue(value);
        QString percentStr = QString::number(value) ;
        w->miniWidget->displayVolumeLabel->setText(percentStr+"%");
        w->appWidget->systemVolumeDisplayLabel->setText(percentStr+"%");
        w->devWidget->outputDeviceSlider->blockSignals(false);
        w->appWidget->systemVolumeSlider->blockSignals(false);
        w->miniWidget->masterVolumeSlider->blockSignals(false);
        w->themeChangeIcons();
        qDebug() << "volume changed" << mate_mixer_stream_control_get_name(control) << volume;
        w->setVolumeSettingValue(value);
        w->updateSystemTrayIcon(value,muted);
        //设置调节输入音量的提示音
        const gchar *id = NULL;
        const gchar *desc = NULL;
        gint retval;

        QList<char *> existsPath = w->listExistsPath();
        QString filenameStr;
        for (char * path : existsPath) {

            char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
            char * allpath = strcat(prepath, path);

            const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
            const QByteArray bba(allpath);
            if(QGSettings::isSchemaInstalled(ba))
            {
                QGSettings * settings = new QGSettings(ba, bba);
                filenameStr = settings->get(FILENAME_KEY).toString();
                QString nameStr = settings->get(NAME_KEY).toString();
                if (nameStr == "volume-changed") {
                    break;
                }
            }
        }
        const QByteArray text = filenameStr.toLocal8Bit();
        id = text.data();
        desc = "Volume Changed";
        const gchar *eventId =id;
        bool status = g_settings_get_boolean(w->m_pSoundSettings, EVENT_SOUNDS_KEY);
        if (status) {
            w->m_pTimer->start(100);
//            retval = ca_context_play (w->caContext, 0,
//                                      CA_PROP_EVENT_ID, eventId,
//                                      CA_PROP_EVENT_DESCRIPTION, desc, NULL);
//            if (retval < 0) {
//                qWarning() << "fail to play " << eventId << ca_strerror(retval) << retval;
//            }

        }
    }
    //else
    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        w->setInputVolume = true;
        w->devWidget->inputDeviceSlider->blockSignals(true);
        w->devWidget->inputDeviceSlider->setValue(value);
        w->devWidget->inputDeviceSlider->blockSignals(false);
    }
}

//音量改变提示：策略是停止滑动时，播放提示声
void DeviceSwitchWidget::handleTimeout()
{
    if(mousePress){
        if(mouseReleaseState){
            gint retval;
            const gchar *desc = "Volume Changed";
            QString filenameStr;
            QList<char *> existsPath = this->listExistsPath();
            for (char * path : existsPath) {
                char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
                char * allpath = strcat(prepath, path);
                const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
                const QByteArray bba(allpath);
                if(QGSettings::isSchemaInstalled(ba))
                {
                    QGSettings * settings = new QGSettings(ba, bba);
                    filenameStr = settings->get(FILENAME_KEY).toString();
                    QString nameStr = settings->get(NAME_KEY).toString();
                    if (nameStr == "volume-changed") {
                        break;
                    }
                }
            }
            const QByteArray text = filenameStr.toLocal8Bit();
            const gchar *id = text.data();
            const gchar *eventId =id;
            if(desc){
                retval = ca_context_play (this->caContext, 0,
                                          CA_PROP_EVENT_ID, eventId,
                                          CA_PROP_EVENT_DESCRIPTION, desc, NULL);
            }
            m_pTimer->stop();
            mousePress = false;
            mouseReleaseState = false;
        }
        else {
            m_pTimer2->start(50);
        }
        m_pTimer->stop();
    }
    else
    {
        gint retval;
        const gchar *desc = "Volume Changed";
        QString filenameStr;
        QList<char *> existsPath = this->listExistsPath();
        for (char * path : existsPath) {
            char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
            char * allpath = strcat(prepath, path);
            const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
            const QByteArray bba(allpath);
            if(QGSettings::isSchemaInstalled(ba))
            {
                QGSettings * settings = new QGSettings(ba, bba);
                filenameStr = settings->get(FILENAME_KEY).toString();
                QString nameStr = settings->get(NAME_KEY).toString();
                if (nameStr == "volume-changed") {
                    break;
                }
            }
        }
        const QByteArray text = filenameStr.toLocal8Bit();
        const gchar *id = text.data();
        const gchar *eventId =id;
        if(desc){
            retval = ca_context_play (this->caContext, 0,
                                      CA_PROP_EVENT_ID, eventId,
                                      CA_PROP_EVENT_DESCRIPTION, desc, NULL);
        }
        m_pTimer->stop();
    }
}

void DeviceSwitchWidget::setHeadsetPort(QString str)
{
    MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(context);
    MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(context);
    if (!MATE_MIXER_IS_STREAM(outputStream) || !MATE_MIXER_IS_STREAM(inputStream)) {
        return;
    }

    MateMixerSwitch *outputPortSwitch = findStreamPortSwitch(this,outputStream);
    MateMixerSwitch *inputPortSwitch = findStreamPortSwitch(this,inputStream);

    const GList *options;
    const gchar *outputPortLabel;
    options = mate_mixer_switch_list_options(MATE_MIXER_SWITCH(inputPortSwitch));
    MateMixerSwitchOption *option = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(inputPortSwitch));
    outputPortLabel = mate_mixer_switch_option_get_label(option);
    while (options != nullptr) {
        MateMixerSwitchOption *opt = MATE_MIXER_SWITCH_OPTION(options->data);
        QString label = mate_mixer_switch_option_get_label(opt);
        QString name = mate_mixer_switch_option_get_name(opt);
        qDebug() <<"switch label name" <<label <<name;
        options = options->next;
    }
    MateMixerSwitchOption *outputActiveOption = mate_mixer_switch_get_active_option(outputPortSwitch);
    MateMixerSwitchOption *inputActiveOption = mate_mixer_switch_get_active_option(inputPortSwitch);
    MateMixerSwitchOption *outputOption;
    MateMixerSwitchOption *inputOption;

    const gchar *outputActiveOptionName = mate_mixer_switch_option_get_name(outputActiveOption);
    const gchar *inputActiveOptionName = mate_mixer_switch_option_get_name(inputActiveOption);
    /*
     *output: headphone
     *input: intel mic
    */
    if (strcmp(str.toLatin1().data(),"headphone") == 0) {
        if (strcmp(outputActiveOptionName,headphones_name)) {
            outputOption = mate_mixer_switch_get_option(outputPortSwitch,headphones_name);
            mate_mixer_switch_set_active_option(outputPortSwitch,outputOption);
        }
        if (strcmp(inputActiveOptionName,internalmic_name)) {
            inputOption = mate_mixer_switch_get_option(inputPortSwitch,internalmic_name);
            /*
             *当需要设置的输入端口不在同一个sink上时，需要设置默认的输入设备
            */
            if (!MATE_MIXER_IS_SWITCH_OPTION(inputOption)) {

                qDebug() << "internalmic_name" << internalmic_name;
//                QString portName = internalmic_name;
                 QString sourceName = findPortSource(internalmic_name);
                 MateMixerStream *stream = mate_mixer_context_get_stream(context,sourceName.toLatin1().data());
                 mate_mixer_context_set_default_input_stream(context,stream);
            }
            else
                mate_mixer_switch_set_active_option(inputPortSwitch,inputOption);

        }
        qDebug() << "setHeadsetPort" <<str <<outputActiveOptionName << inputActiveOptionName;
    } /*output: headphone     input: headset mic*/
    else if (strcmp(str.toLatin1().data(),"headset") == 0) {
        if (strcmp(outputActiveOptionName,headphones_name)) {
            outputOption = mate_mixer_switch_get_option(outputPortSwitch,headphones_name);
            mate_mixer_switch_set_active_option(outputPortSwitch,outputOption);
        }
        if (strcmp(inputActiveOptionName,headsetmic_name)) {
            inputOption = mate_mixer_switch_get_option(inputPortSwitch,headsetmic_name);
            /*
             *当需要设置的输入端口不在同一个sink上时，需要设置默认的输入设备
            */
            if (!MATE_MIXER_IS_SWITCH_OPTION(inputOption)) {

//                QString portName = internalmic_name;
                 QString sourceName = findPortSource(headsetmic_name);
                 MateMixerStream *stream = mate_mixer_context_get_stream(context,sourceName.toLatin1().data());
                 mate_mixer_context_set_default_input_stream(context,stream);
                 qDebug() << "headsetmic_name" << headsetmic_name << inputActiveOptionName;
            }
            else
                mate_mixer_switch_set_active_option(inputPortSwitch,inputOption);
        }
        qDebug() << "setHeadsetPort" <<str << outputActiveOptionName << inputActiveOptionName;
    }/*output: speaker     input: headphone mic*/
    else if (strcmp(str.toLatin1().data(),"headphone mic") == 0) {
        if (strcmp(outputActiveOptionName,internalspk_name)) {
            outputOption = mate_mixer_switch_get_option(outputPortSwitch,internalspk_name);
            mate_mixer_switch_set_active_option(outputPortSwitch,outputOption);
        }
        if (strcmp(inputActiveOptionName,headphonemic_name)) {
            inputOption = mate_mixer_switch_get_option(inputPortSwitch,headphonemic_name);

            /*
             *当需要设置的输入端口不在同一个sink上时，需要设置默认的输入设备
            */
            if (!MATE_MIXER_IS_SWITCH_OPTION(inputOption)) {

//                QString portName = internalmic_name;
                 QString sourceName = findPortSource(headphonemic_name);
                 MateMixerStream *stream = mate_mixer_context_get_stream(context,sourceName.toLatin1().data());
                 qDebug() << "headphonemic_name" << headphonemic_name << sourceName;
                 mate_mixer_context_set_default_input_stream(context,stream);
            }
            else
                mate_mixer_switch_set_active_option(inputPortSwitch,inputOption);
        }
        qDebug() << "setHeadsetPort" <<str << outputActiveOptionName << inputActiveOptionName;
    }
}

MateMixerSwitch* DeviceSwitchWidget::findStreamPortSwitch (DeviceSwitchWidget *widget,MateMixerStream *stream)
{
    const GList *switches;
    switches = mate_mixer_stream_list_switches (stream);
    while (switches != nullptr) {
        MateMixerStreamSwitch *swtch = MATE_MIXER_STREAM_SWITCH (switches->data);
        if (!MATE_MIXER_IS_STREAM_TOGGLE (swtch) &&
                mate_mixer_stream_switch_get_role (swtch) == MATE_MIXER_STREAM_SWITCH_ROLE_PORT) {
            return MATE_MIXER_SWITCH (swtch);
        }
        switches = switches->next;
    }
    return NULL;
}

/*!
 * \brief
 * \details
 * 更新输出设置
 */
void DeviceSwitchWidget::update_output_settings (DeviceSwitchWidget *w,MateMixerStreamControl *control)
{
    Q_UNUSED(w);
    if (control == nullptr) {
        return;
    }
    /*flags = mate_mixer_stream_control_get_flags(control);
    if (flags & MATE_MIXER_STREAM_CONTROL_CAN_BALANCE) {

    }*/
    /* Enable the port selector if the stream has one */
    w->m_pOutputPortList->clear();

    MateMixerStream *stream = mate_mixer_stream_control_get_stream(control);
    MateMixerSwitch *portSwitch = nullptr;
    //    = w->findStreamPortSwitch (w,stream);
    if (portSwitch != nullptr) {
        const GList *options;
        const gchar *outputPortLabel;
        options = mate_mixer_switch_list_options(MATE_MIXER_SWITCH(portSwitch));
        MateMixerSwitchOption *option = mate_mixer_switch_get_active_option(MATE_MIXER_SWITCH(portSwitch));
        outputPortLabel = mate_mixer_switch_option_get_label(option);
        while (options != nullptr) {
            MateMixerSwitchOption *opt = MATE_MIXER_SWITCH_OPTION(options->data);
            QString label = mate_mixer_switch_option_get_label(opt);
            QString name = mate_mixer_switch_option_get_name(opt);
            if (!w->m_pOutputPortList->contains(name)) {
                qDebug() << "设置组合框当前值为:" << label << outputPortLabel;
                //                w->m_pOutputPortList->append(name);
                //                w->m_pOutputWidget->m_pOutputPortCombobox->addItem(label);
            }
            options = options->next;
        }
    }


}

void DeviceSwitchWidget::set_output_stream (DeviceSwitchWidget *w, MateMixerStream *stream)
{
    if (!MATE_MIXER_IS_STREAM(stream)) {
        return;
    }

    bar_set_stream (w,stream);
    if (stream != nullptr) {
        const GList *controls;
        controls = mate_mixer_context_list_stored_controls (w->context);
        /* Move all stored controls to the newly selected default stream */
        while (controls != nullptr) {
            MateMixerStream        *parent;
            MateMixerStreamControl *control;

            control = MATE_MIXER_STREAM_CONTROL (controls->data);
            parent  = mate_mixer_stream_control_get_stream (control);
            /* Prefer streamless controls to stay the way they are, forcing them to
             * a particular owning stream would be wrong for eg. event controls */
            if (parent != nullptr && parent != stream) {
                MateMixerDirection direction = mate_mixer_stream_get_direction (parent);

                if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
                    qDebug() << "get control label:" << mate_mixer_stream_control_get_label(control) << "stream label:" << mate_mixer_stream_get_label(stream);
                    mate_mixer_stream_control_set_stream (control, stream);
                }
            }
            controls = controls->next;
        }
    }

    update_output_stream_list (w, stream);
    update_output_settings(w,w->m_pOutputBarStreamControl);
}

/*!
 * \brief
 * \details
 * 更新输出stream
 */
void DeviceSwitchWidget::update_output_stream_list(DeviceSwitchWidget *w,MateMixerStream *stream)
{
    Q_UNUSED(w);
    const gchar *name = nullptr;
    const gchar *label = nullptr;
    if (stream != nullptr) {
        name = mate_mixer_stream_get_name(stream);
        label = mate_mixer_stream_get_label(stream);
    }
}

/*!
 * \brief
 * \details
 * 设置stream
 */
void DeviceSwitchWidget::bar_set_stream (DeviceSwitchWidget  *w,MateMixerStream *stream)
{
    MateMixerStreamControl *control = nullptr;

    if (MATE_MIXER_IS_STREAM(stream)) {
        control = mate_mixer_stream_get_default_control (stream);
    }
    else {
        return;
    }
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);
    bar_set_stream_control (w,direction, control);
}

void DeviceSwitchWidget::bar_set_stream_control (DeviceSwitchWidget *w,MateMixerDirection direction,MateMixerStreamControl *control)
{
    Q_UNUSED(w);
    const gchar *name;

    if (control != nullptr) {
        if (direction == MATE_MIXER_DIRECTION_INPUT) {
            w->m_pInputBarStreamControl = control;
        }
        else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
            w->m_pOutputBarStreamControl = control;
        }
        name = mate_mixer_stream_control_get_name (control);
    }
}

bool DeviceSwitchWidget:: event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            if (displayMode == MINI_MODE) {
                miniWidget->hide();
            }
            else if (displayMode == ADVANCED_MODE) {
                this->hide();
            }
        }
    }
    return QWidget::event(event);
}

/*!
 * \brief
 * \details
 * 通过dbus获取任务栏位置
 */
int DeviceSwitchWidget::getPanelPosition(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelPosition", str);
    if (reply < 0)
        return 0;
    return reply;
}

/*!
 * \brief
 * \details
 * 通过dbus获取任务栏高度
 */
int DeviceSwitchWidget::getPanelHeight(QString str)
{
    QDBusInterface interface( "com.ukui.panel.desktop",
                              "/",
                              "com.ukui.panel.desktop",
                              QDBusConnection::sessionBus() );
    QDBusReply<int> reply = interface.call("GetPanelSize", str);
    if (reply <= 0)
        return 46;
    return reply;
}

/*!
 * \brief
 * \details
 * 在指定位置显示任务栏
 */
void DeviceSwitchWidget::miniWidgetShow()
{
    #define MARGIN 4
    QDBusInterface iface("org.ukui.panel",
                         "/panel/position",
                         "org.ukui.panel",
                         QDBusConnection::sessionBus());
    QDBusReply<QVariantList> reply=iface.call("GetPrimaryScreenGeometry");
    QVariantList position_list=reply.value();
    /*
     * 通过这个dbus接口获取到的6个参数分别为 ：可用屏幕大小的x坐标、y坐标、宽度、高度，任务栏位置
    */

    switch(reply.value().at(4).toInt()){
    case 1:
        miniWidget->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-miniWidget->width()-MARGIN,
                          position_list.at(1).toInt()+MARGIN,
                          miniWidget->width(),miniWidget->height());
        break;
    case 2:
        miniWidget->setGeometry(position_list.at(0).toInt()+MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-miniWidget->height()-MARGIN,
                          miniWidget->width(),miniWidget->height());
        break;
    case 3:
        miniWidget->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-miniWidget->width()-MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-miniWidget->height()-MARGIN,
                          miniWidget->width(),miniWidget->height());
        break;
    default:
        miniWidget->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-miniWidget->width()-MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-miniWidget->height()-MARGIN,
                          miniWidget->width(),miniWidget->height());
        break;
    }
    miniWidget->show();
}

/*!
 * \brief
 * \details
 * 完整模式界面的显示
 */
void DeviceSwitchWidget::advancedWidgetShow()
{
    #define MARGIN 4
    QDBusInterface iface("org.ukui.panel",
                         "/panel/position",
                         "org.ukui.panel",
                         QDBusConnection::sessionBus());
    QDBusReply<QVariantList> reply=iface.call("GetPrimaryScreenGeometry");
    QVariantList position_list=reply.value();
    /*
     * 通过这个dbus接口获取到的6个参数分别为 ：可用屏幕大小的x坐标、y坐标、宽度、高度，任务栏位置
    */

    switch(reply.value().at(4).toInt()){
    case 1:
        this->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-this->width()-MARGIN,
                          position_list.at(1).toInt()+MARGIN,
                          this->width(),this->height());
        break;
    case 2:
        this->setGeometry(position_list.at(0).toInt()+MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          this->width(),this->height());
        break;
    case 3:
        this->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-this->width()-MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          this->width(),this->height());
        break;
    default:
        this->setGeometry(position_list.at(0).toInt()+position_list.at(2).toInt()-this->width()-MARGIN,
                          position_list.at(1).toInt()+reply.value().at(3).toInt()-this->height()-MARGIN,
                          this->width(),this->height());
        break;
    }
    this->showNormal();
    this->raise();
    activateWindow();
    switchToMiniBtn->show();
}

/*!
 * \brief
 * \details
 * 更新托盘图标
 */
void DeviceSwitchWidget::updateSystemTrayIcon(int volume,bool isMute)
{
    QString systemTrayIcon;
    QString audioIconStr;
    QIcon audioIcon;
    QIcon icon;
    if (isMute) {
        systemTrayIcon = "audio-volume-muted-symbolic";
        audioIconStr = "audio-volume-muted-symbolic";
    }
    else if (volume <= 0 && !shortcutMute) {
        systemTrayIcon = "audio-volume-muted-symbolic";
        audioIconStr = "audio-volume-muted-symbolic";
    }
    else if(volume <= 0 && shortcutMute){
        systemTrayIcon = "audio-volume-low-symbolic";
        audioIconStr = "audio-volume-low-symbolic";
        m_pMuteAction->setIcon(QIcon(""));
    }
    else if (volume > 0 && volume <= 33) {
        systemTrayIcon = "audio-volume-low-symbolic";
        audioIconStr = "audio-volume-low-symbolic";
        m_pMuteAction->setIcon(QIcon(""));
    }
    else if (volume >33 && volume <= 66) {
        systemTrayIcon = "audio-volume-medium-symbolic";
        audioIconStr = "audio-volume-medium-symbolic";
        m_pMuteAction->setIcon(QIcon(""));
    }
    else {
        systemTrayIcon = "audio-volume-high-symbolic";
        audioIconStr = "audio-volume-high-symbolic";
        m_pMuteAction->setIcon(QIcon(""));
    }
    icon = QIcon::fromTheme(systemTrayIcon);
    audioIcon = QIcon::fromTheme(audioIconStr);
    soundSystemTrayIcon->setIcon(icon);
    miniWidget->muteBtn->setIcon(audioIcon);
    appWidget->systemVolumeBtn->setIcon(audioIcon);
    devWidget->outputMuteBtn->setIcon(audioIcon);

    //设置声音菜单栏静音选项的勾选状态
    if (isMute) {
        QString muteActionIconStr = "object-select-symbolic";
        QIcon muteActionIcon = QIcon::fromTheme(muteActionIconStr);
        m_pMuteAction->setIcon(muteActionIcon);
    }
    else {
        m_pMuteAction->setIcon(QIcon(""));
    }
}

/*!
 * \brief
 * \details
 * 更新默认的输入设备
 */
gboolean DeviceSwitchWidget::update_default_input_stream (DeviceSwitchWidget *w)
{
    MateMixerStream *stream;
    stream = mate_mixer_context_get_default_input_stream (w->context);
    if (w->input == nullptr) {
        qDebug() << "input stream is null ";
    }
    w->input = (stream == nullptr) ? nullptr : stream;
    if (w->input != nullptr) {
        g_signal_connect (G_OBJECT (w->input),
                          "control-added",
                          G_CALLBACK (on_input_stream_control_added),
                          w);
        g_signal_connect (G_OBJECT (w->input),
                          "control-removed",
                          G_CALLBACK (on_input_stream_control_removed),
                          w);
    }
    /* Return TRUE if the default input stream has changed */
    return TRUE;
}

void DeviceSwitchWidget::on_input_stream_control_added (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w)
{
    MateMixerStreamControl *control;
    control = mate_mixer_stream_get_control (stream, name);
    if G_LIKELY (control != nullptr) {
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role (control);

        /* Non-application input control doesn't affect the icon */
        if (role != MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION)
            return;
    }

    /* Either an application control has been added or we couldn't
     * read the control, this shouldn't happen but let's revalidate the
     * icon to be sure if it does */
    update_icon_input (w,stream);
}

void DeviceSwitchWidget::on_input_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w)
{
    //    Q_UNUSED(stream);
    Q_UNUSED(name);
    /* The removed stream could be an application input, which may cause
     * the input status icon to disappear */
    update_icon_input (w,stream);
}

/*!
 * \brief
 * \details
 * 更新输入设置
 */
void DeviceSwitchWidget::update_input_settings (DeviceSwitchWidget *w,MateMixerStreamControl *control)
{
    Q_UNUSED(w);
    MateMixerStream            *stream;
    MateMixerStreamControlFlags flags;
    g_debug ("Updating input settings");

    /* Get the control currently associated with the input slider */
    if (control == nullptr)
        return;

    flags = mate_mixer_stream_control_get_flags (control);
    /* Enable level bar only if supported by the control */
    if (flags & MATE_MIXER_STREAM_CONTROL_HAS_MONITOR) {
        g_debug("MATE_MIXER_STREAM_CONTROL_HAS_MONITOR");
    }

    /* Get owning stream of the control */
    stream = mate_mixer_stream_control_get_stream (control);
    if (G_UNLIKELY (stream == NULL))
        return;
}

QList<char *> DeviceSwitchWidget::listExistsPath()
{
    char ** childs;
    int len;

    DConfClient * client = dconf_client_new();
    childs = dconf_client_list (client, KEYBINDINGS_CUSTOM_DIR, &len);
    g_object_unref (client);

    QList<char *> vals;

    for (int i = 0; childs[i] != NULL; i++){
        if (dconf_is_rel_dir (childs[i], NULL)){
            char * val = g_strdup (childs[i]);

            vals.append(val);
        }
    }
    g_strfreev (childs);
    return vals;
}

QString DeviceSwitchWidget::findFreePath(){
    int i = 0;
    char * dir;
    bool found;
    QList<char *> existsdirs;

    existsdirs = listExistsPath();

    for (; i < MAX_CUSTOM_SHORTCUTS; i++){
        found = true;
        dir = QString("custom%1/").arg(i).toLatin1().data();
        for (int j = 0; j < existsdirs.count(); j++)
            if (!g_strcmp0(dir, existsdirs.at(j))){
                found = false;
                break;
            }
        if (found)
            break;
    }

    if (i == MAX_CUSTOM_SHORTCUTS){
        qDebug() << "Keyboard Shortcuts" << "Too many custom shortcuts";
        return "";
    }

    return QString("%1%2").arg(KEYBINDINGS_CUSTOM_DIR).arg(QString(dir));
}

void DeviceSwitchWidget::addValue(QString name,QString filename)
{
    //在创建setting表时，先判断是否存在该设置，存在时不创建
    QList<char *> existsPath = listExistsPath();

    for (char * path : existsPath) {

        char * prepath = QString(KEYBINDINGS_CUSTOM_DIR).toLatin1().data();
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);
        if(QGSettings::isSchemaInstalled(ba))
        {
            QGSettings * settings = new QGSettings(ba, bba);
            QString filenameStr = settings->get(FILENAME_KEY).toString();
            QString nameStr = settings->get(NAME_KEY).toString();

            g_warning("full path: %s", allpath);
            qDebug() << filenameStr << FILENAME_KEY <<NAME_KEY << nameStr;
            if (nameStr == name) {
                return;
            }
            delete settings;
        }
        else {
            continue;
        }

    }
    QString availablepath = findFreePath();

    const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
    const QByteArray idd(availablepath.toUtf8().data());
    if(QGSettings::isSchemaInstalled(id))
    {
        QGSettings * settings = new QGSettings(id, idd);
        settings->set(FILENAME_KEY, filename);
        settings->set(NAME_KEY, name);
    }
}

void DeviceSwitchWidget::setConnectingMessage(const char *string) {
    QByteArray markup = "<i>";
    if (!string)
        markup += tr("Establishing connection to PulseAudio. Please wait...").toUtf8().constData();
    else
        markup += string;
    markup += "</i>";
}

void DeviceSwitchWidget::context_state_callback(pa_context *c, void *userdata) {
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);
    g_assert(c);

    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_UNCONNECTED:
        case PA_CONTEXT_CONNECTING:
        case PA_CONTEXT_AUTHORIZING:
        case PA_CONTEXT_SETTING_NAME:
            break;

        case PA_CONTEXT_READY: {
            pa_operation *o;

            /* Create event widget immediately so it's first in the list */
            pa_context_set_subscribe_callback(c, subscribe_cb, w);
            if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)
                                           (PA_SUBSCRIPTION_MASK_SINK|
                                            PA_SUBSCRIPTION_MASK_SOURCE|
                                            PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                            PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT|
                                            PA_SUBSCRIPTION_MASK_CLIENT|
                                            PA_SUBSCRIPTION_MASK_SERVER|
                                            PA_SUBSCRIPTION_MASK_CARD), nullptr, nullptr))) {
                w->show_error(QObject::tr("pa_context_subscribe() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);

            if (!(o = pa_context_get_card_info_list(c, card_cb, w))) {
                w->show_error(QObject::tr("pa_context_get_card_info_list() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);

            if (!(o = pa_context_get_sink_info_list(c, sinkCb, w))) {
                w->show_error(QObject::tr("pa_context_get_sink_info_list() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);

            if (!(o = pa_context_get_source_info_list(c, sourceCb, w))) {
                w->show_error(QObject::tr("pa_context_get_source_info_list() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);

            /* These calls are not always supported */
            if ((o = pa_ext_stream_restore_read(c, ext_stream_restore_read_cb, w))) {
                pa_operation_unref(o);

                if ((o = pa_ext_stream_restore_subscribe(c, 1, nullptr, nullptr)))
                    pa_operation_unref(o);

            } else
                g_debug(QObject::tr("Failed to initialize stream_restore extension: %s").toUtf8().constData(), pa_strerror(pa_context_errno(w->m_paContext)));
            break;
        }
    case PA_CONTEXT_FAILED:
//        if (w->reconnectTime > 0) {
//            g_debug("%s", QObject::tr("Connection failed, attempting reconnect").toUtf8().constData());
//            qDebug() << "connect failed ,wait to reconnect";
//            g_timeout_add_seconds(w->reconnectTime, connectContext, w);
//        }
        return;
        case PA_CONTEXT_TERMINATED:
        default:
            return;
    }
}

void DeviceSwitchWidget::ext_stream_restore_subscribe_cb(pa_context *c, void *userdata)
{
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);
    pa_operation *o;
    if (!(o = pa_ext_stream_restore_read(c, w->ext_stream_restore_read_cb, w))) {
        w->show_error(QObject::tr("pa_ext_stream_restore_read() failed").toUtf8().constData());
        return;
    }

    pa_operation_unref(o);
}

void DeviceSwitchWidget::ext_stream_restore_read_cb(pa_context *,const pa_ext_stream_restore_info *i,int eol,void *userdata)
{
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);

    if (eol < 0) {
        return;
    }

    if (eol > 0) {
        qDebug() << "Failed to initialize stream_restore extension";
        return;
    }
}


void DeviceSwitchWidget::subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata)
{
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
    case PA_SUBSCRIPTION_EVENT_SINK:
        if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE){
        }
        else {
            pa_operation *o;
            if (!(o = pa_context_get_sink_info_by_index(c, index, sinkCb, w))) {
                w->show_error(QObject::tr("pa_context_get_sink_info_by_index() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);
        }
        break;

    case PA_SUBSCRIPTION_EVENT_SOURCE:
        if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE){
        }
        else {
            pa_operation *o;
            if (!(o = pa_context_get_source_info_by_index(c, index, sourceCb, w))) {
                w->show_error(QObject::tr("pa_context_get_source_info_by_index() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);
        }
        break;

    case PA_SUBSCRIPTION_EVENT_CARD:
        if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
            pa_operation *o;
            w->currentInputPortLabelMap.clear();
            w->currentOutputPortLabelMap.clear();
            //将移除的声卡信息在map删除
            QMap<int,QString>::iterator it;
            QMap<int,QList<QString>>::iterator temp;
            QMap<int, QMap<QString,int>>::iterator profilePriorityMap;
            for(it = w->cardMap.begin();it!=w->cardMap.end();)
            {
                if(it.key() == index)
                {
                    it = w->cardMap.erase(it);
                    continue;
                }
                ++it;
            }

            for(temp=w->cardProfileMap.begin();temp!=w->cardProfileMap.end();) {

                if (it.key() == index) {
                    temp= w->cardProfileMap.erase(temp);
                    continue;
                }
                ++temp;
            }

            for(profilePriorityMap=w->cardProfilePriorityMap.begin();profilePriorityMap!=w->cardProfilePriorityMap.end();) {

                if (profilePriorityMap.key() == index) {
                    profilePriorityMap= w->cardProfilePriorityMap.erase(profilePriorityMap);
                    continue;
                }
                ++profilePriorityMap;
            }

            //移除输入端口名
            for(it = w->inputPortNameMap.begin();it!=w->inputPortNameMap.end();)
            {
                if(it.key() == index)
                {
                    it = w->inputPortNameMap.erase(it);
                    continue;
                }
                ++it;
            }

            for(it = w->outputPortNameMap.begin();it!=w->outputPortNameMap.end();)
            {
                if(it.key() == index)
                {
                    it = w->outputPortNameMap.erase(it);
                    continue;
                }
                ++it;
            }
            for(it = w->inputPortLabelMap.begin();it!=w->inputPortLabelMap.end();)
            {
                if(it.key() == index)
                {
                    QString removePortLabel = it.value();
                    QMap<QString,QString>::iterator removeProfileMap;
                    for (removeProfileMap = w->inputPortProfileNameMap.begin();removeProfileMap!= w->inputPortProfileNameMap.end();) {
                        if (removeProfileMap.key() == removePortLabel) {
                            removeProfileMap = w->inputPortProfileNameMap.erase(removeProfileMap);
                            continue;
                        }
                        ++removeProfileMap;
                    }
                    it = w->inputPortLabelMap.erase(it);
                    continue;
                }
                ++it;
            }

            for(it = w->outputPortLabelMap.begin();it!=w->outputPortLabelMap.end();)
            {
                if(it.key() == index)
                {
                    QString removePortLabel = it.value();
                    QMap<QString,QString>::iterator removeProfileMap;
                    for (removeProfileMap = w->profileNameMap.begin();removeProfileMap!= w->profileNameMap.end();) {
                        if (removeProfileMap.key() == removePortLabel) {
                            removeProfileMap = w->profileNameMap.erase(removeProfileMap);
                            continue;
                        }
                        ++removeProfileMap;
                    }
                    it = w->outputPortLabelMap.erase(it);
                    continue;
                }
                ++it;
            }
        }
        else {
            pa_operation *o;
            if (!(o = pa_context_get_card_info_by_index(c, index, card_cb, w))) {
                w->show_error(QObject::tr("pa_context_get_card_info_by_index() failed").toUtf8().constData());
                return;
            }
            pa_operation_unref(o);
        }
        break;
    }
}

void DeviceSwitchWidget::card_cb(pa_context *, const pa_card_info *i, int eol, void *userdata) {
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);

    if (eol < 0) {
        if (pa_context_errno(w->m_paContext) == PA_ERR_NOENTITY)
            return;

        w->show_error(QObject::tr("Card callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
//        dec_outstanding(w);
        return;
    }
//    bool alreadyInclude = false;
//    QMap <int,QString>::iterator it;
//    for(it=w->cardMap.begin();it!=w->cardMap.end();) {
//        if (it.key() == i->index) {
//            alreadyInclude = true;
//            break;
//        }
//    }
//    if (alreadyInclude)
    w->cardMap.insert(i->index,i->name);
    //qDebug() << "update card" << i->name <<"--" << i->active_profile->name << i->index << i->ports << "card count: "<< w->cardMap.count()<<i->active_profile;
    w->updateCard(*i);

}

void DeviceSwitchWidget::updateCard(const pa_card_info &info) {
    const char *name;
    const char *description, *icon;
    std::set<pa_card_profile_info2 *, profile_prio_compare> profile_priorities;

    description = pa_proplist_gets(info.proplist, PA_PROP_DEVICE_DESCRIPTION);
    name = description ? description : info.name;

    icon = pa_proplist_gets(info.proplist, PA_PROP_DEVICE_ICON_NAME);

    this->hasSinks = false;
    this->hasSources = false;
    profile_priorities.clear();

    QList<QString> profileName;
    QMap<QString,int> profilePriorityMap;
    for (pa_card_profile_info2 ** p_profile = info.profiles2; *p_profile != nullptr; ++p_profile) {
        this->hasSinks = this->hasSinks || ((*p_profile)->n_sinks > 0);
        this->hasSources = this->hasSources || ((*p_profile)->n_sources > 0);
        profile_priorities.insert(*p_profile);

        profileName.append((*p_profile)->name);
        profilePriorityMap.insertMulti((*p_profile)->name,(*p_profile)->priority);
    }
    cardProfilePriorityMap.insertMulti(info.index,profilePriorityMap);

    //拔插耳机的时候删除端口
    QMap<int,QString>::iterator it;
    for(it = outputPortNameMap.begin();it!=outputPortNameMap.end();)
    {
        if(it.key() == info.index)
        {
            const char *portName = it.value().toLatin1().data();
            QString tempPortName = portName;//临时存储端口名
            const gchar *outputStreamName;
            const gchar *inputStreamName;
            MateMixerStream *outputStream = mate_mixer_context_get_default_output_stream(context);
            MateMixerStream *inputStream = mate_mixer_context_get_default_input_stream(context);
            if (!MATE_MIXER_IS_STREAM(outputStream))
                outputStreamName = "auto_null";
            else
                outputStreamName = mate_mixer_stream_get_name(outputStream);
            if (!MATE_MIXER_IS_STREAM(inputStream))
                inputStreamName = "auto_null";
            else
                inputStreamName = mate_mixer_stream_get_name(inputStream);
            if (tempPortName.contains("analog-output-headphones-huawei")) {
                qDebug()<<"portName+++++++++++++++"<<tempPortName << inputStreamName << outputStreamName;
                if (strstr(outputStreamName,"histen_sink") || strstr(outputStreamName,"auto_null")  ) {
                    if (!strstr(info.active_profile->name,"output:analog-stereo+input:analog-stereo") || info.n_ports < PORT_NUM) {
                        //发送DBus信号
                        QDBusMessage message =QDBusMessage::createSignal("/", "org.ukui.media", "DbusSingleTest");
                        message<<"pause";
                        QDBusConnection::sessionBus().send(message);
                    }
                }
            }
            if (tempPortName.contains("analog-output-headphones-huawei") || tempPortName.contains("analog-output-speaker-huawei")) {
                qDebug()<<"inputStreamName*************"<<inputStreamName;
                if (strstr(inputStreamName,"3a_source") ) {
                    //发送DBus信号
                    QDBusMessage message =QDBusMessage::createSignal("/", "org.ukui.media", "DbusSignalRecorder");
                    message<<"拔插";
                    QDBusConnection::sessionBus().send(message);
                }
            }
            qDebug() << "remove output port name map index" << info.index << outputPortNameMap.count() << it.value() << outputStreamName << portName;
            it = outputPortNameMap.erase(it);
            continue;
        }
        ++it;
    }
    for(it = outputPortLabelMap.begin();it!=outputPortLabelMap.end();)
    {
        if(it.key() == info.index)
        {
            QString removePortLabel = it.value();
            QMap<QString,QString>::iterator removeProfileMap;
            for (removeProfileMap = profileNameMap.begin();removeProfileMap!= profileNameMap.end();) {
                if (removeProfileMap.key() == removePortLabel) {
                    removeProfileMap = profileNameMap.erase(removeProfileMap);
                    continue;
                }
                ++removeProfileMap;
            }

            it = outputPortLabelMap.erase(it);

            continue;
        }
        ++it;
    }
    for(it = inputPortNameMap.begin();it!=inputPortNameMap.end();)
    {
        if(it.key() == info.index)
        {
            it = inputPortNameMap.erase(it);
            //qDebug() << "remove input port map index" << info.index << inputPortNameMap.count();
            continue;
        }
        ++it;
    }
    for(it = inputPortLabelMap.begin();it!=inputPortLabelMap.end();)
    {
        if(it.key() == info.index)
        {
            QString removePortLabel = it.value();
            QMap<QString,QString>::iterator removeProfileMap;
            for (removeProfileMap = inputPortProfileNameMap.begin();removeProfileMap!= inputPortProfileNameMap.end();) {
                if (removeProfileMap.key() == removePortLabel) {
                    removeProfileMap = inputPortProfileNameMap.erase(removeProfileMap);
                    continue;
                }
                ++removeProfileMap;
            }
            it = inputPortLabelMap.erase(it);
            //qDebug() << "remove input port Label map index" << info.index << inputPortLabelMap.count();
            continue;
        }
        ++it;
    }

    this->ports.clear();
    for (uint32_t i = 0; i < info.n_ports; ++i) {
        PortInfo p;

        p.name = info.ports[i]->name;
        p.description = info.ports[i]->description;
        p.priority = info.ports[i]->priority;
        p.available = info.ports[i]->available;
        p.direction = info.ports[i]->direction;
        p.latency_offset = info.ports[i]->latency_offset;

        if (info.ports[i]->profiles2) {
            for (pa_card_profile_info2 ** p_profile = info.ports[i]->profiles2; *p_profile != nullptr; ++p_profile) {
                p.profiles.push_back((*p_profile)->name);
            }
        }
        if (p.direction == 1 && p.available != PA_PORT_AVAILABLE_NO) {
            //-->
            //定制音量：初始化输出音量
            QString protName = QString(p.name);
            QString careName = QString(info.name);
            MateMixerStream *stream = mate_mixer_context_get_default_output_stream(context);
            QString streanName = mate_mixer_stream_get_name(stream);
            if(protName.contains("speaker-huawei") && !streanName.contains("blue")){
                if(!customSoundFile->isExist(protName)){
                    if (m_pInitSystemVolumeSetting->keys().contains("outputVolume")) {
                        int outputVolume = m_pInitSystemVolumeSetting->get(OUTPUT_VOLUME).toInt();
                        qDebug()<<"outputVolume:"<<outputVolume;
                        miniWidget->masterVolumeSlider->setValue(outputVolume);
                        customSoundFile->addXmlNode(protName,false);
                    }
                }
            }
            else if(protName.contains("headphones-huawei")){
                if(!careName.contains("blue") && !careName.contains("usb") && !streanName.contains("blue") && !streanName.contains("usb")){
                    if(!customSoundFile->isExist(protName)){
                        if (m_pInitSystemVolumeSetting->keys().contains("headphoneOutputVolume")){
                            int outputVolume = m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
                            miniWidget->masterVolumeSlider->setValue(outputVolume);
                            customSoundFile->addXmlNode(protName,false);
                        }
                    }
                }
            }
            else if(protName.contains("headphone") && careName.contains("blue")){
                protName = "blue_" + protName;
                if(!customSoundFile->isExist(protName)){
                    if (m_pInitSystemVolumeSetting->keys().contains("headphoneOutputVolume")){
                        int outputVolume = m_pInitSystemVolumeSetting->get(HEADPHONE_OUTPUT_VOLUME).toInt();
                        miniWidget->masterVolumeSlider->setValue(outputVolume);
                        customSoundFile->addXmlNode(protName,false);
                    }
                }
            }
            else if(protName.contains("hdmi",Qt::CaseInsensitive) && streanName.contains("hdmi",Qt::CaseInsensitive)){
                if(!customSoundFile->isExist(protName)){
                    if (m_pInitSystemVolumeSetting->keys().contains("hdmiVolume")){
                        int outputVolume = m_pInitSystemVolumeSetting->get(HDMI_VOLUME).toInt();
                        miniWidget->masterVolumeSlider->setValue(outputVolume);
                        customSoundFile->addXmlNode(protName,false);
                    }
                }
            }
            //-<
            outputPortNameMap.insertMulti(info.index,p.name);
            outputPortLabelMap.insertMulti(info.index,p.description.data());

            QList<QString> portProfileName;
            for (auto p_profile : p.profiles) {
                portProfileName.append(p_profile.data());
                profileNameMap.insertMulti(p.description.data(),p_profile.data());
            }
            cardProfileMap.insertMulti(info.index,portProfileName);
        }
        else if (p.direction == 2 && p.available != PA_PORT_AVAILABLE_NO){
            QString protName = QString(p.name);
            QString careName = QString(info.name);
            MateMixerStream *stream = mate_mixer_context_get_default_input_stream(context);
            QString streanName = mate_mixer_stream_get_name(stream);
            //-->
            //定制音量：初始化输入音量
            if(protName.contains("internal-mic")){ //板载输入
                if(!customSoundFile->isExist(protName)){
                    if (m_pInitSystemVolumeSetting->keys().contains("micVolume")){
                        int volume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
                        devWidget->inputDeviceSlider->setValue(volume);
                        customSoundFile->addXmlNode(protName,false);
                    }
                }
            }
            else if(protName.contains("headset-mic") && !careName.contains("bluez") && !careName.contains("usb")){
                if(!customSoundFile->isExist(protName)){
                    if (m_pInitSystemVolumeSetting->keys().contains("micVolume")){
                        int volume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
                        devWidget->inputDeviceSlider->setValue(volume);
                        customSoundFile->addXmlNode(protName,false);
                    }
                }
            }
            else if(protName.contains("headphone-input") && careName.contains("bluez")){
                protName = "blue_" + protName;
                if(!customSoundFile->isExist(protName)){
                    if (m_pInitSystemVolumeSetting->keys().contains("micVolume")){
                        int volume = m_pInitSystemVolumeSetting->get(MIC_VOLUME).toInt();
                        devWidget->inputDeviceSlider->setValue(volume);
                        customSoundFile->addXmlNode(protName,false);
                    }
                }
            }
            //-<
//            inputPortNameMap.insertMulti(info.index,p.name);
//            inputPortLabelMap.insertMulti(info.index,p.description.data());
//            for (auto p_profile : p.profiles) {
//                inputPortProfileNameMap.insertMulti(p.description.data(),p_profile.data());
//            }
        }
        this->ports[p.name] = p;
    }

    this->profiles.clear();

    for (auto p_profile : profile_priorities) {
        bool hasNo = false, hasOther = false;
        std::map<QByteArray, PortInfo>::iterator portIt;
        QByteArray desc = p_profile->description;
        for (portIt = this->ports.begin(); portIt != this->ports.end(); portIt++) {
            PortInfo port = portIt->second;

            if (std::find(port.profiles.begin(), port.profiles.end(), p_profile->name) == port.profiles.end())
                continue;

            if (port.available == PA_PORT_AVAILABLE_NO)
                hasNo = true;
            else {
                hasOther = true;
                break;
            }
        }
        if (hasNo && !hasOther)
            desc += tr(" (unplugged)").toUtf8().constData();

        if (!p_profile->available)
            desc += tr(" (unavailable)").toUtf8().constData();

        this->profiles.push_back(std::pair<QByteArray,QByteArray>(p_profile->name, desc));
        if (p_profile->n_sinks == 0 && p_profile->n_sources == 0)
            this->noInOutProfile = p_profile->name;
    }
    this->activeProfile = info.active_profile ? info.active_profile->name : "";
    check_audio_device_selection_needed(this,control,&info);
    qDebug() << "this->active profile -----------------" << info.name <<info.active_profile->name << info.active_profile2->name;

    if (strstr(info.name,"bluez_card") && strcmp(info.active_profile->name,"a2dp_sink") != 0) {
        QTimer *time = new QTimer;
        time->start(300);
        qDebug() << "bluez card profile is off,need to set profile...";
        connect(time,&QTimer::timeout,[=](){
            QString deviceName = info.name;
            QString cmd = "pactl set-card-profile " + deviceName + " a2dp_sink";
            qDebug() << "set card profile" << cmd;
            system(cmd.toLatin1().data());
        });

    }
    /* Because the port info for sinks and sources is discontinued we need
     * to update the port info for them here. */
    if (this->hasSinks) {
        updatePorts(this, info, this->ports);
    }
}

void DeviceSwitchWidget::show_error(const char *txt) {
    char buf[256];

    snprintf(buf, sizeof(buf), "%s: %s", txt, pa_strerror(pa_context_errno(m_paContext)));
    qDebug() << "show error:" << QString::fromUtf8(buf);
}


pa_context* DeviceSwitchWidget::get_context()
{
    return m_paContext;
}

void DeviceSwitchWidget::updatePorts(DeviceSwitchWidget *w, const pa_card_info &info, std::map<QByteArray, PortInfo> &ports) {
    std::map<QByteArray, PortInfo>::iterator it;
    PortInfo p;
    for (auto & port : w->ports) {
        QByteArray desc;
        it = ports.find(port.first);

        if (it == ports.end())
            continue;

        p = it->second;
        desc = p.description;

        if (p.available == PA_PORT_AVAILABLE_YES) {
            desc +=  DeviceSwitchWidget::tr(" (plugged in)").toUtf8().constData();

        }
        else if (p.available == PA_PORT_AVAILABLE_NO) {
            if (p.name == "analog-output-speaker" ||
                p.name == "analog-input-microphone-internal")
                desc += DeviceSwitchWidget::tr(" (unavailable)").toUtf8().constData();
            else
                desc += DeviceSwitchWidget::tr(" (unplugged)").toUtf8().constData();
        }
    }

}

/*
    应用使用麦克风通知
*/
void DeviceSwitchWidget::notifySend(QString arg,QString appName , QString appIconName ,QString deviceName)
{
    QDBusInterface iface("org.freedesktop.Notifications",
                         "/org/freedesktop/Notifications",
                         "org.freedesktop.Notifications",
                         QDBusConnection::sessionBus());
    QList<QVariant> args;
    QStringList argg;

    QMap<QString, QVariant> pearMap;
    pearMap.insert("cmd1","ukui-control-center");
    pearMap.insert("cmd2","qtcreater");

    args<<appName
        <<((unsigned int) 0)
        <<QString(appIconName)
        <<deviceName //显示的是什么类型的信息//系统升级
        <<arg //显示的具体信息
        <<argg
        <<pearMap
        <<(int)-1;
    iface.callWithArgumentList(QDBus::AutoDetect,"Notify",args);
}

/*
   In PulseAudio without ucm, ports will show up with the following names:
   Headphones - analog-output-headphones
   Headset mic - analog-input-headset-mic (was: analog-input-microphone-headset)
   Jack in mic-in mode - analog-input-headphone-mic (was: analog-input-microphone)

   However, since regular mics also show up as analog-input-microphone,
   we need to check for certain controls on alsa mixer level too, to know
   if we deal with a separate mic jack, or a multi-function jack with a
   mic-in mode (also called "headphone mic").
   We check for the following names:

   Headphone Mic Jack - indicates headphone and mic-in mode share the same jack,
     i e, not two separate jacks. Hardware cannot distinguish between a
     headphone and a mic.
   Headset Mic Phantom Jack - indicates headset jack where hardware can not
     distinguish between headphones and headsets
   Headset Mic Jack - indicates headset jack where hardware can distinguish
     between headphones and headsets. There is no use popping up a dialog in
     this case, unless we already need to do this for the mic-in mode.

   From the PA_PROCOTOL_VERSION=34, The device_port structure adds 2 members
   availability_group and type, with the help of these 2 members, we could
   consolidate the port checking and port setting for non-ucm and with-ucm
   cases.
*/

#define HEADSET_PORT_SET(dst, src) \
        do { \
                if (!(dst) || (dst)->priority < (src)->priority) \
                        dst = src; \
        } while (0)

#define GET_PORT_NAME(x) (x ? g_strdup (x->name) : NULL)

headset_ports *
DeviceSwitchWidget::get_headset_ports (MateMixerStreamControl    *control,
                                       const pa_card_info *c)
{
    headset_ports *h;
    guint i;

    h = g_new0 (headset_ports, 1);

    for (i = 0; i < c->n_ports; i++) {
        pa_card_port_info *p = c->ports[i];
        //                if (control->priv->server_protocol_version < 34) {
        if (g_str_equal (p->name, "analog-output-headphones") || g_str_equal (p->name, "[Out] Headphones1"))
            h->headphones = p;
        else if (g_str_equal (p->name, "analog-input-headset-mic") || g_str_equal (p->name, "[In] Headset"))
            h->headsetmic = p;
        else if (g_str_equal (p->name, "analog-input-headphone-mic") || g_str_equal (p->name, "[In] Headphones2"))
            h->headphonemic = p;
        else if (g_str_equal (p->name, "analog-input-internal-mic") || g_str_equal (p->name, "[In] Mic"))
            h->internalmic = p;
        else if (g_str_equal (p->name, "analog-output-speaker") || g_str_equal (p->name, "[Out] Speaker"))
            h->internalspk = p;
        //                } else {
#if (PA_PROTOCOL_VERSION >= 34)
        /* in the first loop, set only headphones */
        /* the microphone ports are assigned in the second loop */
        if (p->type == PA_DEVICE_PORT_TYPE_HEADPHONES) {
            if (p->availability_group)
                HEADSET_PORT_SET (h->headphones, p);
        } else if (p->type == PA_DEVICE_PORT_TYPE_SPEAKER) {
            HEADSET_PORT_SET (h->internalspk, p);
        } else if (p->type == PA_DEVICE_PORT_TYPE_MIC) {
            if (!p->availability_group)
                HEADSET_PORT_SET (h->internalmic, p);
        }
#else
        //                        g_warning_once ("libgnome-volume-control running against PulseAudio %u, "
        //                                        "but compiled against older %d, report a bug to your distribution",
        //                                        control->priv->server_protocol_version,
        //                                        PA_PROTOCOL_VERSION);
#endif
        //                }
    }

#if (PA_PROTOCOL_VERSION >= 35)
    if (h->headphones && (control->priv->server_protocol_version >= 34)) {
        for (i = 0; i < c->n_ports; i++) {
            pa_card_port_info *p = c->ports[i];
            if (g_strcmp0(h->headphones->availability_group, p->availability_group))
                continue;
            if (p->direction != PA_DIRECTION_INPUT)
                continue;
            if (p->type == PA_DEVICE_PORT_TYPE_HEADSET)
                HEADSET_PORT_SET (h->headsetmic, p);
            else if (p->type == PA_DEVICE_PORT_TYPE_MIC)
                HEADSET_PORT_SET (h->headphonemic, p);
        }
    }
#endif

    return h;
}

gboolean
DeviceSwitchWidget::verify_alsa_card (int cardindex,
                                      gboolean *headsetmic,
                                      gboolean *headphonemic)
{
    char *ctlstr;
    snd_hctl_t *hctl;
    snd_ctl_elem_id_t *id;
    int err;

    *headsetmic = FALSE;
    *headphonemic = FALSE;

    ctlstr = g_strdup_printf ("hw:%i", cardindex);
    if ((err = snd_hctl_open (&hctl, ctlstr, 0)) < 0) {
        g_warning ("snd_hctl_open failed: %s", snd_strerror(err));
        g_free (ctlstr);
        return FALSE;
    }
    g_free (ctlstr);

    if ((err = snd_hctl_load (hctl)) < 0) {
//        if (hasNo && !hasOther)
        g_warning ("snd_hctl_load failed: %s", snd_strerror(err));
        snd_hctl_close (hctl);
        return FALSE;
    }

    snd_ctl_elem_id_alloca (&id);

    snd_ctl_elem_id_clear (id);
    snd_ctl_elem_id_set_interface (id, SND_CTL_ELEM_IFACE_CARD);
    snd_ctl_elem_id_set_name (id, "Headphone Mic Jack");
    if (snd_hctl_find_elem (hctl, id))
        *headphonemic = TRUE;

    snd_ctl_elem_id_clear (id);
    snd_ctl_elem_id_set_interface (id, SND_CTL_ELEM_IFACE_CARD);
    snd_ctl_elem_id_set_name (id, "Headset Mic Phantom Jack");
    if (snd_hctl_find_elem (hctl, id))
        *headsetmic = TRUE;

    if (*headphonemic) {
        snd_ctl_elem_id_clear (id);
        snd_ctl_elem_id_set_interface (id, SND_CTL_ELEM_IFACE_CARD);
        snd_ctl_elem_id_set_name (id, "Headset Mic Jack");
        if (snd_hctl_find_elem (hctl, id))
            *headsetmic = TRUE;
    }

    snd_hctl_close (hctl);
    return *headsetmic || *headphonemic;
}

/*
 * Four-segment 3.5mm earphone jack events
 */
void
DeviceSwitchWidget::check_audio_device_selection_needed (DeviceSwitchWidget *widget,
                                                         MateMixerStreamControl    *control,
                                                         const pa_card_info *info)
{
    headset_ports *h;
    gboolean start_dialog, stop_dialog;
    qDebug() << "check_audio_device_selection_needed" <<info->name;

    start_dialog = FALSE;
    stop_dialog = FALSE;
    h = get_headset_ports (control, info);

    if (!h->headphones ||
            (!h->headsetmic && !h->headphonemic)) {
        qDebug() << "no headset jack" ;
        /* Not a headset jack */
        goto out;
    }
    else {
        qDebug() << "headset jack" << h->headphonemic << h->headphones;
    }
    if (widget->headset_card != (int) info->index) {
        int cardindex;
        gboolean hsmic = TRUE;
        gboolean hpmic = TRUE;
        const char *s;

        s = pa_proplist_gets (info->proplist, "alsa.card");
        if (!s)
            goto out;

        cardindex = strtol (s, NULL, 10);
        if (cardindex == 0 && strcmp(s, "0") != 0)
            goto out;
        if (!verify_alsa_card(cardindex, &hsmic, &hpmic))
            goto out;

        widget->headset_card = info->index;
        widget->has_headsetmic = hsmic && h->headsetmic;
        widget->has_headphonemic = hpmic && h->headphonemic;
    } else {
        start_dialog = (h->headphones->available != PA_PORT_AVAILABLE_NO) && !widget->headset_plugged_in;
        stop_dialog = (h->headphones->available == PA_PORT_AVAILABLE_NO) && widget->headset_plugged_in;
    }

    widget->headset_plugged_in = h->headphones->available != PA_PORT_AVAILABLE_NO;
//    widget->free_priv_port_names (control);
    widget->headphones_name = GET_PORT_NAME(h->headphones);
    widget->headsetmic_name = GET_PORT_NAME(h->headsetmic);
    widget->headphonemic_name = GET_PORT_NAME(h->headphonemic);
    widget->internalspk_name = GET_PORT_NAME(h->internalspk);
    widget->internalmic_name = GET_PORT_NAME(h->internalmic);

    if (widget->firstLoad)
        widget->firstLoad = false;
    else {
        if (widget->headset_plugged_in) {
            widget->headsetWidget->showWindow();
        }
        else {
            if (widget->headsetWidget->isShow) {
                widget->headsetWidget->hide();
                widget->headsetWidget->isShow = false;
            }
        }
    }
    qDebug() << "check_audio_device_selection_needed" << widget->headphones_name <<  widget->headsetmic_name << widget->headphonemic_name << widget->internalspk_name << widget->internalmic_name << widget->headset_plugged_in;
    if (!start_dialog &&
            !stop_dialog)
        goto out;

out:
    g_free (h);
}

void DeviceSwitchWidget::free_priv_port_names (MateMixerStreamControl    *control)
{
    g_clear_pointer (&headphones_name, g_free);
    g_clear_pointer (&headsetmic_name, g_free);
    g_clear_pointer (&headphonemic_name, g_free);
    g_clear_pointer (&internalspk_name, g_free);
    g_clear_pointer (&internalmic_name, g_free);
}

/*
 * sink callback
 */
void DeviceSwitchWidget::sinkCb(pa_context *c, const pa_sink_info *i, int eol, void *userdata)
{
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);

    if (eol < 0) {
        if (pa_context_errno(w->m_paContext) == PA_ERR_NOENTITY)
            return;

        w->show_error(QObject::tr("Sink callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
//        dec_outstanding(w);
        return;
    }
#if HAVE_EXT_DEVICE_RESTORE_API
    if (w->updateSink(*i)) {
//        ext_device_restore_subscribe_cb(c, PA_DEVICE_TYPE_SINK, i->index, w);
    }
#else
    w->updateSink(*i);
#endif
}

void DeviceSwitchWidget::sourceCb(pa_context *, const pa_source_info *i, int eol, void *userdata)
{
    DeviceSwitchWidget *w = static_cast<DeviceSwitchWidget*>(userdata);

    if (eol < 0) {
        if (pa_context_errno(w->m_paContext) == PA_ERR_NOENTITY)
            return;

        w->show_error(QObject::tr("Source callback failure").toUtf8().constData());
        return;
    }

    if (eol > 0) {
//        dec_outstanding(w);
        return;
    }

    w->updateSource(*i);
}

/*
  * Port update and new or reduced sink
*/
bool DeviceSwitchWidget::updateSink(const pa_sink_info &info)
{
    bool isNew = false;
    QMap<QString,QString>temp;

    for (pa_sink_port_info ** sinkPort = info.ports; *sinkPort != nullptr; ++sinkPort) {
        temp.insertMulti(info.name,(*sinkPort)->name);
    }
    sinkPortMap.insert(info.index,temp);

    return isNew;
}

/*
  * Port update and new or reduced source
*/
void DeviceSwitchWidget::updateSource(const pa_source_info &info)
{
    QMap<QString,QString>temp;

    if(info.ports) {
        for (pa_source_port_info ** sourcePort = info.ports; *sourcePort != nullptr; ++sourcePort) {
            temp.insertMulti(info.name,(*sourcePort)->name);
        }
        sourcePortMap.insert(info.index,temp);
    }
}

/*
 * Find the corresponding sink according to the port name
 */
QString DeviceSwitchWidget::findPortSink(QString portName)
{
    QMap<int, QMap<QString,QString>>::iterator it;
    QMap<QString,QString> portNameMap;
    QMap<QString,QString>::iterator tempMap;

    for (it=sinkPortMap.begin();it!=sinkPortMap.end();) {
        portNameMap = it.value();
        for (tempMap=portNameMap.begin();tempMap!=portNameMap.end();) {
            qDebug() <<"find port sink" << tempMap.value() << portName<< tempMap.key();
            if ( tempMap.value() == portName) {
                break;
            }
            ++tempMap;
        }

        ++it;
    }
    return tempMap.key();
}

/*
 * Find the corresponding source according to the port name
 */
QString DeviceSwitchWidget::findPortSource(QString portName)
{
    QMap<int, QMap<QString,QString>>::iterator it;
    QMap<QString,QString> portNameMap;
    QMap<QString,QString>::iterator tempMap;
    QString sourceStr = "";
    for (it=sourcePortMap.begin();it!=sourcePortMap.end();) {
        portNameMap = it.value();
        for (tempMap=portNameMap.begin();tempMap!=portNameMap.end();) {
            if ( tempMap.value() == portName) {
                sourceStr = tempMap.key();
                break;
            }
            ++tempMap;
        }
        ++it;
    }
    return sourceStr;
}

MyTimer::MyTimer(QObject *parent)
    :QObject(parent)
{
    m_nTimerID = this->startTimer(TIMER_TIMEOUT);
}

MyTimer::~MyTimer()
{

}

void MyTimer::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_nTimerID){
        handleTimeout();
        Q_EMIT timeOut();
    }
}

void MyTimer::handleTimeout()
{
    killTimer(m_nTimerID);
}


DeviceSwitchWidget::~DeviceSwitchWidget()
{

}
