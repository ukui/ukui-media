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
#include <QSize>
#include <QDebug>
#include <QEvent>
#include <QTimer>
#include <QWindow>
#include <QLayout>
#include <QScreen>
#include <QPalette>
#include <QPainter>
#include <QProcess>
#include <QDateTime>
#include <QGridLayout>
#include <QMetaObject>
#include <QMouseEvent>
#include <QMainWindow>
#include <QDesktopWidget>
#include <qsvgrenderer.h>
#include "ukmedia_systemtray_widget.h"

#include <QShortcut>
//监控系统主题
#define KEY_THEME_SCHEMA   "org.mate.interface"
#define THEME_ICON_KEY "icon-theme"

int count = 0;

UkmediaSystemTrayIcon::UkmediaSystemTrayIcon(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaSystemTrayIcon::~UkmediaSystemTrayIcon()
{

}

UkmediaSystemTrayWidget::UkmediaSystemTrayWidget(QWidget *parent)
    : QMainWindow (parent)
{
    iconThemeSettings = new QGSettings(KEY_THEME_SCHEMA);
    QVariant var = iconThemeSettings->get(THEME_ICON_KEY);
    widget = new UkmediaControlWidget();
    widget->dockWidgetInit();

//    kWidget = new UkmediaKeybordControlWidget();
    //初始化麦克风和声音托盘
    inputSystemTrayMenuInit();
    outputSystemTrayMenuInit();

    //麦克风、声音托盘获取control
    ipSystemTrayControl();
    opSystemTrayControl();

    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup);
    this->setObjectName("ukmediaSystemWidget");

    //在构造中设置窗体的MouseTracking属性
    this->setMouseTracking(true);
    //设置获取焦点事件
    setFocusPolicy(Qt::ClickFocus);

    //系统图标主题改变
    connect(iconThemeSettings,SIGNAL(changed(const QString &)),this,SLOT(iconThemeChanged()));
    //点击托盘栏声音图标显示音量条
    connect(outputSystemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),\
            this,SLOT(activatedOutputSystemTrayIcon(QSystemTrayIcon::ActivationReason)));
    connect(inputSystemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),\
            this,SLOT(activatedinputSystemTrayIcon(QSystemTrayIcon::ActivationReason)));

    //更改系统托盘图
    connect(widget->m_volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(sliderChangeSystemTrayIcon(int)));
    connect(widget->m_muteButton,SIGNAL(clicked()),this,SLOT(buttonChangeSystemTrayIcon()));

    //声音首选项中改变托盘图标
    connect(widget,SIGNAL(updateSystemTrayIconSignal(int,SystemTrayIconType,bool)),
            this,SLOT(soundPreferenceChangeSystemTrayIcon(int,SystemTrayIconType,bool)));

    QShortcut *shortCut = new QShortcut(this);
    shortCut->setKey(tr("F10"));
    shortCut->setAutoRepeat(false);

    connect(shortCut,SIGNAL(activated()),this,SLOT(keyControlVolume()));
    //设置中心窗口
    this->setCentralWidget(widget);
    resize(300,56);
    setWindowOpacity(0.95);
}

void UkmediaSystemTrayWidget::iconThemeChanged()
{
    int ipVolume,opVolume;
    bool ipStatus,opStatus;
    QVariant var = iconThemeSettings->get(THEME_ICON_KEY);
    QString value = var.value<QString>();
//    set->set(THEME_ICON_KEY,var);
    opVolume = mate_mixer_stream_control_get_volume(widget->outputControl);
    opStatus = mate_mixer_stream_control_get_mute(widget->outputControl);
    ipVolume = mate_mixer_stream_control_get_volume(widget->inputControl);
    ipStatus = mate_mixer_stream_control_get_mute(widget->inputControl);
    opVolume = opVolume*100/65536.0+0.5;
    ipVolume = ipVolume*100/65536.0+0.5;
//    QString inputSystemTrayIcon;
//    QIcon icon;
//    inputSystemTrayIcon = "audio-input-microphone-medium";
//    icon = QIcon::fromTheme(inputSystemTrayIcon);
//    inputSystemTray->setIcon(icon);
    updateInputSystemTrayIcon(ipVolume,ipStatus);
    updateOutputSystemTrayIcon(opVolume,opStatus);
    qDebug() << "设置的主题为" << value;
}

void UkmediaSystemTrayWidget::ipSystemTrayControl()
{
    int ipVolume;
    bool ipStatus;
    //获取输入流
    widget->getDefaultInputStream();
    ipVolume = widget->getIpVolume();
    ipStatus = widget->getIpMuteStatus();

    //设置麦克风托盘图标的初始值
    inputSystemTrayIconInit(ipVolume,ipStatus);

    //点击、滚动麦克风托盘图标
    connect(this,SIGNAL(triggeredSystemIcon(SystemTrayIconType)),widget,
            SLOT(acceptIpSystemTrayIconTriggered(SystemTrayIconType)));
    connect(this,SIGNAL(systemTrayRoll(SystemTrayIconType)),widget,
            SLOT(acceptIpSystemTrayIconRoll(SystemTrayIconType)));
    connect(inputSystemTray,SIGNAL(wheelRollEventSignal(bool)),this,
            SLOT(acceptIpWheelRollEvent(bool)));

    //静音和声音首选项的设置
    connect(inputActionMute,SIGNAL(triggered(bool)),this,SLOT(inputActionMuteTriggered(bool)));
    connect(inputActionSoundPreference,SIGNAL(triggered()),this,SLOT(jumpControlPanel()));
}

/*
    更新麦克风托盘图标
*/
void UkmediaSystemTrayWidget::updateInputSystemTrayIcon(int volume,bool isMute)
{
    bool show = false;
    QString inputSystemTrayIcon;
    QIcon icon;
    if (isMute) {
        mate_mixer_stream_control_set_mute(widget->inputControl,isMute);
        inputSystemTrayIcon = "audio-input-microphone-muted";
        icon = QIcon::fromTheme(inputSystemTrayIcon);
        ipMuteCheckBox->setChecked(true);
        inputSystemTray->setIcon(icon);
    }
    else if (volume <= 0) {
        inputSystemTrayIcon = "audio-input-microphone-muted";
        icon = QIcon::fromTheme(inputSystemTrayIcon);
        ipMuteCheckBox->setChecked(true);
    }
    else if (volume > 0 && volume <= 33) {
        inputSystemTrayIcon = "audio-input-microphone-low";
        icon = QIcon::fromTheme(inputSystemTrayIcon);
        inputSystemTray->setIcon(icon);
        ipMuteCheckBox->setChecked(false);
    }
    else if (volume >33 && volume <= 66) {
        inputSystemTrayIcon = "audio-input-microphone-medium";
        icon = QIcon::fromTheme(inputSystemTrayIcon);
        inputSystemTray->setIcon(icon);
        ipMuteCheckBox->setChecked(false);
    }
    else {
        inputSystemTrayIcon = "audio-input-microphone-high";
        icon = QIcon::fromTheme(inputSystemTrayIcon);
        inputSystemTray->setIcon(icon);
        ipMuteCheckBox->setChecked(false);
    }
    //设置麦克风右键菜单的静音标签的勾选状态
    if (mate_mixer_stream_control_get_mute(widget->inputControl)) {
        inputActionMute->setChecked(true);
    }
    else {
        inputActionMute->setChecked(false);
    }

    //设置麦克风托盘图标是否可见
    MateMixerStreamControl *control;
    const gchar *app_id;
    const GList *inputs = mate_mixer_stream_list_controls(widget->inputStream);
    control = mate_mixer_stream_get_default_control(widget->inputStream);

    while (inputs != NULL) {
        MateMixerStreamControl *input = MATE_MIXER_STREAM_CONTROL (inputs->data);
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role(input);

        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *app_info = mate_mixer_stream_control_get_app_info (input);
            app_id = mate_mixer_app_info_get_id (app_info);
            if (app_id == NULL) {
               /* A recording application which has no
                * identifier set */
                g_debug ("Found a recording application control %s",
                         mate_mixer_stream_control_get_label (input));
                if G_UNLIKELY (control == NULL) {
                       /* In the unlikely case when there is no
                        * default input control, use the application
                        * control for the icon */
                    control = input;
                }
                show = TRUE;
                break;
            }
            if (strcmp (app_id, "org.mate.VolumeControl") != 0 &&
                strcmp (app_id, "org.gnome.VolumeControl") != 0 &&
                strcmp (app_id, "org.PulseAudio.pavucontrol") != 0) {
                g_debug ("Found a recording application %s", app_id);
                if G_UNLIKELY (control == NULL)
                    control = input;
                show = TRUE;
                break;
            }
        }
        inputs = inputs->next;
    }
    if (show == TRUE)
        g_debug ("Input icon enabled");
    else
        g_debug ("There is no recording application, input icon disabled");
    inputSystemTray->setVisible(show);
}

/*
    更新声音托盘图标
*/
void UkmediaSystemTrayWidget::updateOutputSystemTrayIcon(int volume,bool isMute)
{
    QString outputSystemTrayIcon;
    QIcon icon;
    if (isMute) {
        mate_mixer_stream_control_set_mute(widget->outputControl,isMute);
        outputSystemTrayIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(outputSystemTrayIcon);
        opMuteCheckBox->setChecked(true);
        outputSystemTray->setIcon(icon);
    }
    else if (volume <= 0) {
        outputSystemTrayIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(outputSystemTrayIcon);
        opMuteCheckBox->setChecked(true);
        outputSystemTray->setIcon(icon);
    }
    else if (volume > 0 && volume <= 33) {
        outputSystemTrayIcon = "audio-volume-low";
        opMuteCheckBox->setChecked(false);
        icon = QIcon::fromTheme(outputSystemTrayIcon);
        outputSystemTray->setIcon(icon);
    }
    else if (volume >33 && volume <= 66) {
        outputSystemTrayIcon = "audio-volume-medium";
        opMuteCheckBox->setChecked(false);
        icon = QIcon::fromTheme(outputSystemTrayIcon);
        outputSystemTray->setIcon(icon);
    }
    else {
        outputSystemTrayIcon = "audio-volume-high";
        opMuteCheckBox->setChecked(false);
        icon = QIcon::fromTheme(outputSystemTrayIcon);
        outputSystemTray->setIcon(icon);
    }

    //设置声音菜单栏静音选项的勾选状态
    if (mate_mixer_stream_control_get_mute(widget->outputControl)) {
        outputActionMute->setChecked(true);
    }
    else {
        outputActionMute->setChecked(false);
    }
}

void UkmediaSystemTrayWidget::opSystemTrayControl()
{
    int opVolume;
    bool opStatus;

    //获取输出流、输出声音以及声音静音状态
    widget->getDefaultOutputStream();
    opVolume = widget->getOpVolume();
    opStatus = widget->getOpMuteStatus();

    //声音托盘图标初始化以及更新声音托盘图标
    outputSystemTrayIconInit(opVolume,opStatus);
    opVolume = int(opVolume*100/65536.0+0.5);
    updateOutputSystemTrayIcon(opVolume,opStatus);

    //点击、滚动声音托盘实现音量的控制
    connect(this,SIGNAL(triggeredSystemIcon(SystemTrayIconType)),widget,SLOT(acceptOpSystemTrayIconTriggered(SystemTrayIconType)));
    connect(this,SIGNAL(systemTrayRoll(SystemTrayIconType)),widget,SLOT(acceptOpSystemTrayIconRoll(SystemTrayIconType)));
    connect(outputSystemTray,SIGNAL(wheelRollEventSignal(bool)),this,SLOT(acceptOpWheelRollEvent(bool)));

    //菜单栏静音和声音首选项
    connect(outputActionMute,SIGNAL(triggered(bool)),this,SLOT(outputActionMuteTriggered(bool)));
    connect(outputActionSoundPreference,SIGNAL(triggered()),this,SLOT(jumpControlPanel()));
}

/*
    麦克风托盘图标初始化
*/
void UkmediaSystemTrayWidget::inputSystemTrayMenuInit()
{
    inputMenu = new QMenu(this);
    inputSystemTray = new UkmediaSystemTrayIcon(this);
    QIcon icon;
    QString soundPreference;
    widget->inputVolumeNotify();
    //麦克风添加菜单静音和声音首选项

    inputActionMute = new QWidgetAction(inputMenu);
    inputActionMute->setCheckable(true);
    inputActionMute->setText(tr("Mute(M)"));
    inputSystemTray->setToolTip((tr("Input volume control")));

    inputActionMute->setObjectName("inputActionMute");
    inputActionSoundPreference = new QWidgetAction(inputMenu);
    inputActionMute->setCheckable(true);

    inputSystemTray->setToolTip(tr("Output volume control"));
    inputActionMute->setObjectName("outputActionMute");
    inputActionSoundPreference = new QWidgetAction(inputMenu);
    inputActionSoundPreferenceWid = new QWidget();
    inputActionMuteWid = new QWidget();

    ipMuteCheckBox = new QCheckBox(inputActionMuteWid);
    ipMuteCheckBox->setFixedSize(16,16);
    ipMuteCheckBox->setFocusPolicy(Qt::NoFocus);

    ipMuteLabel = new QLabel(tr("Mute(M)"),inputActionMuteWid);
    QHBoxLayout *hLayout;
    hLayout = new QHBoxLayout();
    hLayout->addWidget(ipMuteCheckBox);
    hLayout->addWidget(ipMuteLabel);
    hLayout->setSpacing(10);

    ipMuteCheckBox->setStyleSheet("QCheckBox{background:transparent;border:0px;}");
    ipMuteLabel->setStyleSheet("QLabel{background:transparent;border:0px;}");
    inputActionMuteWid->setLayout(hLayout);

//    inputActionMuteWid->setObjectName("muteWid");

    inputActionSoundPreference->setDefaultWidget(inputActionSoundPreferenceWid);
    inputActionMute->setDefaultWidget(inputActionMuteWid);

    soundPreference = "application-audio";
    icon = QIcon::fromTheme(soundPreference);
    inputActionSoundPreference->setIcon(icon);
    inputActionSoundPreference->setText(tr("Sound preference(S)"));

    inputMenu->addAction(inputActionMute);
    inputMenu->addAction(inputActionSoundPreference);
    inputMenu->setFixedWidth(250);

    inputMenu->setObjectName("microphoneMenu");
    inputSystemTray->setContextMenu(inputMenu);

    init_widget_action(inputActionSoundPreferenceWid,"/usr/share/ukui-media/img/setting.svg",tr("Sound preference(S)"));
    init_widget_action(inputActionMuteWid,"","");
    inputMenu->setWindowOpacity(0.95);
    widget->inputVolumeChanged();

    //点击静音复选框设置静音模式
    connect(ipMuteCheckBox,SIGNAL(released()),this,SLOT(ipMuteCheckBoxReleasedSlot()));
}

void UkmediaSystemTrayWidget::outputSystemTrayMenuInit()
{
    QIcon icon;
    QString soundPreference;
    outputMenu = new QMenu(this);
    outputSystemTray = new UkmediaSystemTrayIcon(this);
    widget->outputVolumeNotify();
    //为系统托盘图标添加菜单静音和声音首选项
    outputActionMute = new QWidgetAction(outputMenu);
//    outputActionMute = new QAction(tr("Mute(M)"));
    outputActionMute->setCheckable(true);
//    outputActionMute->setText();
    outputSystemTray->setToolTip(tr("Output volume control"));
    outputActionMute->setObjectName("outputActionMute");
    outputActionSoundPreference = new QWidgetAction(outputMenu);
    outputActionSoundPreferenceWid = new QWidget();
    outputActionMuteWid = new QWidget();

    QHBoxLayout *hLayout;
    hLayout = new QHBoxLayout();

    opMuteCheckBox = new QCheckBox(outputActionMuteWid);
    opMuteCheckBox->setFixedSize(16,16);
    opMuteCheckBox->setFocusPolicy(Qt::NoFocus);
    opMuteLabel = new QLabel(tr("Mute(M)"),outputActionMuteWid);

    hLayout->addWidget(opMuteCheckBox);
    hLayout->addWidget(opMuteLabel);
    hLayout->setSpacing(10);

    opMuteCheckBox->setStyleSheet("QCheckBox{background:transparent;border:0px;}");
    opMuteLabel->setStyleSheet("QLabel{background:transparent;border:0px;}");
    outputActionMuteWid->setLayout(hLayout);

    outputActionMuteWid->setObjectName("muteWid");

    outputActionSoundPreference->setDefaultWidget(outputActionSoundPreferenceWid);
    outputActionMute->setDefaultWidget(outputActionMuteWid);
    //设置右键菜单
    outputMenu->addAction(outputActionMute);

    outputMenu->addAction(outputActionSoundPreference);
    outputMenu->setFixedWidth(250);

    init_widget_action(outputActionSoundPreferenceWid,"/usr/share/ukui-media/img/setting.svg",tr("Sound preference(S)"));
    init_widget_action(outputActionMuteWid,"","");
    outputMenu->setObjectName("outputSoundMenu");
    outputSystemTray->setContextMenu(outputMenu);

    outputMenu->setWindowOpacity(0.95);
    widget->outputVolumeChanged();

    //点击静音复选框设置静音模式
    connect(opMuteCheckBox,SIGNAL(released()),this,SLOT(opMuteCheckBoxReleasedSlot()));
}

/*
    激活麦克风托盘图标
*/
void UkmediaSystemTrayWidget::activatedinputSystemTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    QRect rect;
    int localWidth ,availableWidth,totalWidth;
    int localHeight,availableHeight,totalHeight;

    widget->inputVolumeChanged();
    trayIconType = SYSTEMTRAYICON_MICROPHONE;

    //发送点击托盘的信号
    Q_EMIT triggeredSystemIcon(trayIconType);
    switch(reason) {
    //鼠标中间键点击图标
    case QSystemTrayIcon::MiddleClick: {
        if (this->isHidden()) {
            if (!inputActionMute->isChecked()) {
                inputActionMute->setChecked(true);
            }
            else {
                inputActionMute->setChecked(false);
            }
            widget->mute();
            bool isMute = mate_mixer_stream_control_get_mute(widget->inputControl);
            int volume = mate_mixer_stream_control_get_volume(widget->inputControl);
            volume = int(volume*100/65536.0+0.5);
            updateInputSystemTrayIcon(volume,isMute);
        }
        else {
            hideWindow();
        }
        break;
    }
    //鼠标左键点击图标
    case QSystemTrayIcon::Trigger: {
        int volume = widget->getIpVolume();
        widget->setIpSystemTrayIconVolume();
        bool isMute = mate_mixer_stream_control_get_mute(widget->inputControl);
        volume = int(volume*100/65536.0+0.5);
        updateInputSystemTrayIcon(volume,isMute);
        rect = inputSystemTray->geometry();

        //屏幕可用宽高
        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();
        //总共宽高
        totalWidth =  QGuiApplication::screens().at(0)->size().width();
        totalHeight = QGuiApplication::screens().at(0)->size().height();
        localWidth = availableWidth - this->width();
        localHeight = availableHeight - this->height();

        if (voiceOnOrOff) {
            if (rect.x() > availableWidth/2 && rect.x()< availableWidth  && rect.y() > availableHeight) {

                this->setGeometry(localWidth,availableHeight-this->height(),300,56);
            }
            else if (rect.x() > availableWidth/2 && rect.x()< availableWidth && rect.y() < 40 ) {

                this->setGeometry(localWidth,totalHeight-availableHeight,300,56);
            }
            else if (rect.x() < 40 && rect.y() > availableHeight/2 && rect.y()< availableHeight) {

                this->setGeometry(totalWidth-availableWidth,localHeight,300,56);//左
            }
            else if (rect.x() > availableWidth && rect.y() > availableHeight/2 && rect.y() < availableHeight) {

                this->setGeometry(localWidth,localHeight,300,56);
            }
            this->show();
            break;
        }
        else {
            this->hide();
            break;
        }
    }
    //鼠标左键双击图标
    case QSystemTrayIcon::DoubleClick: {
        this->hide();
        break;
    }
    case QSystemTrayIcon::Context: {
        if (!this->isHidden()) {
            this->hide();
            inputMenu->setVisible(false);
        }
        else {
            inputMenu->setVisible(true);
        }
        break;
    }
    default:
        break;
    }
}

void UkmediaSystemTrayWidget::change()
{
    qDebug() << "主窗口改变";
}

/*
    激活声音托盘图标
*/
void UkmediaSystemTrayWidget::activatedOutputSystemTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    trayIconType = SYSTEMTRAYICON_VOLUME;

    //输出声音改变通知
    widget->outputVolumeChanged();
    //发送点击托盘的信号
    Q_EMIT triggeredSystemIcon(trayIconType);
    QRect rect;
    int localWidth ,availableWidth,totalWidth;
    int localHeight,availableHeight,totalHeight;
    switch(reason) {
    //鼠标中间键点击图标
    case QSystemTrayIcon::MiddleClick:
        if (this->isHidden()) {
            if (!outputActionMute->isChecked()) {
                outputActionMute->setChecked(true);
            }
            else {
                outputActionMute->setChecked(false);
            }
            widget->mute();
            bool isMute = mate_mixer_stream_control_get_mute(widget->outputControl);
            int volume = mate_mixer_stream_control_get_volume(widget->outputControl);
            volume = int(volume*100/65536.0+0.5);
            updateOutputSystemTrayIcon(volume,isMute);
        }
        else {
            hideWindow();
        }
        break;
    //鼠标左键点击图标
    case QSystemTrayIcon::Trigger: {

        widget->setOpSystemTrayIconVolume();
        bool isMute = mate_mixer_stream_control_get_mute(widget->outputControl);
        int volume = widget->getOpVolume();
        volume = int(volume*100/65536.0+0.5);
        updateOutputSystemTrayIcon(volume,isMute);
        rect = outputSystemTray->geometry();

        availableWidth = QGuiApplication::screens().at(0)->availableGeometry().width();
        availableHeight = QGuiApplication::screens().at(0)->availableGeometry().height();

        QDesktopWidget *desktopWidget = QApplication::desktop();
        int screenNum = desktopWidget->screenCount();
        int primaryScreen = desktopWidget->primaryScreen();

        connect(desktopWidget,SIGNAL(primaryScreenChanged()),this,SLOT(change()));

        totalWidth = desktopWidget->width();
        totalHeight = desktopWidget->height();
        //totalWidth =  QGuiApplication::screens().at(0)->size().width();
        //totalHeight = QGuiApplication::screens().at(0)->size().height();
        localWidth = availableWidth - this->width();
        localHeight = availableHeight - this->height();
        if (voiceOnOrOff) {
            if (rect.x() > availableWidth/2 && rect.x()< availableWidth  && rect.y() > availableHeight) {

                this->setGeometry(localWidth,availableHeight-this->height(),300,56);//下
            }
            else if (rect.x() > availableWidth/2 && rect.x()< availableWidth && rect.y() < 40 ) {

                this->setGeometry(localWidth,totalHeight-availableHeight,300,56);
            }
            else if (rect.x() < 40 && rect.y() > availableHeight/2 && rect.y()< availableHeight) {

                this->setGeometry(totalWidth-availableWidth,localHeight,300,56);//左
            }
            else if (rect.x() > availableWidth && rect.y() > availableHeight/2 && rect.y() < availableHeight) {

                this->setGeometry(localWidth,localHeight,300,56);
            }
            showWindow();
            break;
        }
        else {
            hideWindow();
            break;
        }
    }
    //鼠标中间键点击图标
    case QSystemTrayIcon::DoubleClick:
        hideWindow();
        break;
    case QSystemTrayIcon::Context: {
        if (!this->isHidden()) {
            hideWindow();
            outputMenu->setVisible(false);
        }
        else {
            outputMenu->setVisible(true);
        }
        break;
    }
    default:
        break;
    }
}

void UkmediaSystemTrayWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    hideWindow();
}

/*
    显示window
*/
void UkmediaSystemTrayWidget::showWindow()
{
    this->show();
    voiceOnOrOff = 0;
}

/*
    隐藏window
*/
void UkmediaSystemTrayWidget::hideWindow()
{
    this->hide();
    voiceOnOrOff = 1;
}

void UkmediaSystemTrayWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    hideWindow();
}

/*
    点击窗口之外的部分隐藏
*/
bool UkmediaSystemTrayWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            hideWindow();
        }
    }
    return QWidget::event(event);
}

/*
    获取托盘图标的滚动事件
*/
bool UkmediaSystemTrayIcon::event(QEvent *event)
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

/*
    window的滚动事件
*/
void UkmediaSystemTrayWidget::wheelEvent(QWheelEvent *event)
{
    int volume;
    int wheelStep = 65536/20;//滚动大小为5
    switch (trayIconType) {
    case SYSTEMTRAYICON_VOLUME:
        volume = widget->getOpVolume();
        if (event->delta() >0 ) {
            volume = volume + wheelStep;
        }
        else if (event->delta() < 0 ) {
           volume = volume - wheelStep;
        }
        if (volume < 0) {
            widget->setOpVolume(0);
        }
        else if (volume >= 0 && volume <= 65535) {

            widget->setOpVolume(volume);
        }
        else if (volume > 65535) {
            widget->setOpVolume(65535);
        }
        break;
    case SYSTEMTRAYICON_MICROPHONE:
        volume = widget->getIpVolume();
        if (event->delta() >0 ) {
            volume = volume + wheelStep;
        }
        else if (event->delta() < 0 ) {
           volume = volume - wheelStep;
        }
        if (volume < 0) {
            widget->setIpVolume(0);
        }
        else if (volume >= 0 && volume <= 65535) {

            widget->setIpVolume(volume);
        }
        else if (volume > 65535) {
            widget->setIpVolume(65535);
        }
        break;
    default:
        break;
    }
    event->accept();
}

/*
    跳转到声音首选项
*/
void UkmediaSystemTrayWidget::jumpControlPanel()
{
    QProcess *m_process = new QProcess;
    m_process->start("ukui-volume-control");
}

/*
    麦克风托盘图标初始化
*/
void UkmediaSystemTrayWidget::inputSystemTrayIconInit(int volume, bool status)
{
    volume = volume*100/65536.0 + 0.5;
    if (status) {
        if (!inputActionMute->isChecked()) {
            inputActionMute->setChecked(true);
        }
    }
    updateInputSystemTrayIcon(volume,status);
}

/*
    声音托盘图标初始化
*/
void UkmediaSystemTrayWidget::outputSystemTrayIconInit(int volume,bool status)
{
    volume = volume*100/65536.0 + 0.5;

    if (status) {
        if (!outputActionMute->isChecked()) {
            outputActionMute->setChecked(true);
        }
    }
    updateOutputSystemTrayIcon(volume,status);
    outputSystemTray->setVisible(TRUE);
}

/*
    静音按钮改变托盘图标
*/
void UkmediaSystemTrayWidget::buttonChangeSystemTrayIcon()
{
    int volume;
    switch (trayIconType) {
    case SYSTEMTRAYICON_UNKNOW:
        break;
    case SYSTEMTRAYICON_MICROPHONE: {
        volume = widget->getIpVolume();
        volume = int(volume*100/65536.0+0.5);
        bool isMute = mate_mixer_stream_control_get_mute(widget->inputControl);
        updateInputSystemTrayIcon(volume,isMute);
        break;
    }
    case SYSTEMTRAYICON_VOLUME:
        volume = widget->getOpVolume();
        volume = int(volume*100/65536.0+0.5);
        bool isMute = mate_mixer_stream_control_get_mute(widget->outputControl);
        updateOutputSystemTrayIcon(volume,isMute);
        break;
    }
}

/*
    拖动滑动条改变托盘图标
*/
void UkmediaSystemTrayWidget::sliderChangeSystemTrayIcon(int volume)
{
    //接收滑动条的托盘类型
    connect(widget,SIGNAL(sliderSystemTrayIcon(SystemTrayIconType)),this,SLOT(acceptSliderSystemTrayIcon(SystemTrayIconType)));
    switch (trayIconType) {
    case SYSTEMTRAYICON_VOLUME: {
        bool isMute = mate_mixer_stream_control_get_mute(widget->outputControl);
        updateOutputSystemTrayIcon(volume,isMute);
        break;
    }
    case SYSTEMTRAYICON_MICROPHONE: {
        bool isMute = mate_mixer_stream_control_get_mute(widget->inputControl);
        updateInputSystemTrayIcon(volume,isMute);
        break;
    }
    default:
        break;
    }
}

/*
    声音首选项中改变托盘图标
*/
void UkmediaSystemTrayWidget::soundPreferenceChangeSystemTrayIcon(int volume,SystemTrayIconType type,bool status)
{
    switch (type) {
    case SYSTEMTRAYICON_VOLUME:
        updateOutputSystemTrayIcon(volume,status);
        break;
    case SYSTEMTRAYICON_MICROPHONE:
        updateInputSystemTrayIcon(volume,status);
        break;
    default:
        break;
    }
}

/*
    麦克风菜单静音点击
*/
void UkmediaSystemTrayWidget::inputActionMuteTriggered(bool status)
{
    Q_UNUSED(status);
    widget->mute();
    bool isMute = mate_mixer_stream_control_get_mute(widget->inputControl);
    int volume = mate_mixer_stream_control_get_volume(widget->inputControl);
    volume = int(volume*100/65536.0+0.5);
    updateInputSystemTrayIcon(volume,isMute);
}

/*
    声音菜单静音点击
*/
void UkmediaSystemTrayWidget::outputActionMuteTriggered(bool status)
{
    Q_UNUSED(status);
    widget->mute();
    bool isMute = mate_mixer_stream_control_get_mute(widget->outputControl);
    int volume = mate_mixer_stream_control_get_volume(widget->outputControl);
    volume = int(volume*100/65536.0+0.5);
    updateOutputSystemTrayIcon(volume,isMute);
}

/*
    按键事件,控制系统音量
*/
void UkmediaSystemTrayWidget::keyPressEvent(QKeyEvent *event)
{
    int volume;
    switch (trayIconType) {
    case SYSTEMTRAYICON_MICROPHONE : {
        if (event->key() == Qt::Key_Home) {
            widget->m_volumeSlider->setValue(0);
        }
        else if (event->key() == Qt::Key_End) {
            widget->m_volumeSlider->setValue(100);
        }
        else if (event->key() == Qt::Key_Left) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume-1);
        }
        else if (event->key() == Qt::Key_Right) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume+1);
        }
        else if(event->key() == Qt::Key_Up) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume+1);
        }
        else if (event->key() == Qt::Key_Down) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume-1);
        }
        else if (event->key() == Qt::Key_Escape) {
            hideWindow();
        }
        break;
    }
    case SYSTEMTRAYICON_VOLUME : {
        if (event->key() == Qt::Key_Home) {
            widget->m_volumeSlider->setValue(0);
        }
        else if (event->key() == Qt::Key_End) {
            widget->m_volumeSlider->setValue(100);
        }
        else if (event->key() == Qt::Key_Left) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume-1);
        }
        else if (event->key() == Qt::Key_Right) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume+1);
        }
        else if(event->key() == Qt::Key_Up) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume+1);
        }
        else if (event->key() == Qt::Key_Down) {
            volume = widget->m_volumeSlider->value();
            widget->m_volumeSlider->setValue(volume-1);
        }
        else if (event->key() == Qt::Key_Escape) {
            hideWindow();
        }
        break;
    }
    default:
        break;
    }
}

/*
    接收麦克风滚动事件
*/
void UkmediaSystemTrayWidget::acceptIpWheelRollEvent(bool value)
{
    trayIconType = SYSTEMTRAYICON_MICROPHONE;
    int volume = widget->getIpVolume();
    volume = int(volume*100/65536.0+0.5);
    Q_EMIT systemTrayRoll(trayIconType);
    if (value) {
        widget->m_volumeSlider->setValue(volume+5);
    }
    else {
        widget->m_volumeSlider->setValue(volume-5);
    }
}

/*
    接收声音滚动事件
*/
void UkmediaSystemTrayWidget::acceptOpWheelRollEvent(bool value)
{
    trayIconType = SYSTEMTRAYICON_VOLUME;
    Q_EMIT systemTrayRoll(trayIconType);
    int volume = widget->getOpVolume();
    volume = int(volume*100/65536.0+0.5);
    if (value) {
        widget->m_volumeSlider->setValue(volume+5);
    }
    else {
        widget->m_volumeSlider->setValue(volume-5);
    }
}

/*
    拖动不同的滑动条更改托盘图标的类型
*/
void UkmediaSystemTrayWidget::acceptSliderSystemTrayIcon(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    键盘按键控制声音
*/
void UkmediaSystemTrayWidget::keyControlVolume()
{
    qDebug() << "按下F10";

}

/*
    QWidgetAction 初始化
*/
void UkmediaSystemTrayWidget::init_widget_action(QWidget *wid, QString iconstr, QString textstr)
{
    QString style="QWidget{background:transparent;border:0px;}\
            QWidget:hover{background-color:#34bed8ef;}\
            QWidget:pressed{background-color:#3a123456;}";

    QHBoxLayout* layout=new QHBoxLayout(wid);
    wid->setLayout(layout);
    wid->setFixedSize(244,34);
    wid->setStyleSheet(style);
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

/*
    按下静音复选框，设置静音或取消静音
*/
void UkmediaSystemTrayWidget::opMuteCheckBoxReleasedSlot()
{
    int volume = mate_mixer_stream_control_get_volume(widget->outputControl);
    volume = volume*100/65536.0 + 0.5;
    bool status = mate_mixer_stream_control_get_mute(widget->outputControl);
    if (status) {
//        widget->outputControl
        status = false;
        opMuteCheckBox->setChecked(status);
        mate_mixer_stream_control_set_mute(widget->outputControl,status);
        updateOutputSystemTrayIcon(volume,status);
    }
    else {
        status =true;
        opMuteCheckBox->setChecked(status);
        mate_mixer_stream_control_set_mute(widget->outputControl,status);
        updateOutputSystemTrayIcon(volume,status);
    }
    outputMenu->hide();
}

void UkmediaSystemTrayWidget::ipMuteCheckBoxReleasedSlot()
{
    int volume = mate_mixer_stream_control_get_volume(widget->inputControl);
    bool status = mate_mixer_stream_control_get_mute(widget->inputControl);
    if (status) {
//        widget->outputControl
        status = false;
        ipMuteCheckBox->setChecked(status);
        mate_mixer_stream_control_set_mute(widget->inputControl,status);
        updateInputSystemTrayIcon(volume,status);
    }
    else {
        status =true;
        ipMuteCheckBox->setChecked(status);
        mate_mixer_stream_control_set_mute(widget->inputControl,status);
        updateInputSystemTrayIcon(volume,status);
    }
    inputMenu->hide();
}

UkmediaSystemTrayWidget::~UkmediaSystemTrayWidget()
{

}
