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
extern "C" {
#include <glib-object.h>
#include <glib.h>
#include <gio/gio.h>
#include <gobject/gparamspecs.h>
#include <glib/gi18n.h>
}
#include <XdgIcon>
#include <XdgDesktopFile>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStringList>
#include <QSpacerItem>
#include <QListView>
#include <QPainter>
#include <QScreen>
#include <QProcess>
#include <QApplication>
#include <QSvgRenderer>
#include <QScrollBar>
#include <QtCore/qmath.h>
#include <QDebug>
#include <QList>


typedef enum {
    DEVICE_VOLUME_BUTTON,
    APP_VOLUME_BUTTON
}ButtonType;

ButtonType btnType = DEVICE_VOLUME_BUTTON;
guint appnum = 0;
int app_count = 0;
bool isShow = true;
extern DisplayerMode displayMode ;
QString application_name;

UkmediaTrayIcon::UkmediaTrayIcon(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaTrayIcon::~UkmediaTrayIcon()
{

}

/*
    获取托盘图标的滚动事件
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

void DeviceSwitchWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x13,0x14,0xB2)));
    p.setPen(Qt::NoPen);
    QPainterPath path;
    opt.rect.adjust(0,0,0,0);
    path.addRoundedRect(opt.rect,6,6);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*
    显示window
*/
void DeviceSwitchWidget::showWindow()
{
    this->show();
    isShow = false;
}

/*
    隐藏window
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
    isShow = true;
}

/*
    右键菜单
*/
void DeviceSwitchWidget::showMenu()
{
    int panelHeight = getPanelHeight("panelheight");
    int panelPosition = getPanelPosition("panelposition");

    if (!panelHeight && !panelPosition) {
        //给定任务栏高度和初始值
         panelHeight = 46;
         panelPosition = 0;
    }

    int totalHeight = qApp->primaryScreen()->size().height() + qApp->primaryScreen()->geometry().y();
    int totalWidth = qApp->primaryScreen()->size().width() + qApp->primaryScreen()->geometry().x();
    int localX = 0;
    int localY = 0;
    if (panelPosition == 0) { //任务栏在下
        if (totalWidth -  cursor().pos().x() < menu->width()) {//靠边显示
            localX = totalWidth - menu->width();
            localY = totalHeight - panelHeight - menu->height() - 2;
        }
        else {
            localX = cursor().pos().x();
            localY = totalHeight - panelHeight - menu->height() - 2;
        }
    }
    else if (panelPosition == 1) { //任务栏在上
        if (totalWidth - cursor().pos().x() < menu->width()) {
            localX = totalWidth - menu->width();
            localY = qApp->primaryScreen()->geometry().y() + panelHeight + 2;
        }
        else {
            localX = cursor().pos().x();
            localY = qApp->primaryScreen()->geometry().y() + panelHeight + 2;
        }
    }
    else if (panelPosition == 2) { //任务栏在左
        localX = qApp->primaryScreen()->geometry().x() + panelHeight + 2;
        localY = cursor().pos().y();
    }
    else if (panelPosition == 3) {//任务栏在右
        localX = totalWidth - menu->width() - panelHeight - 2;
        localY = cursor().pos().y();
    }
    menu->setGeometry(localX,localY,menu->width(),menu->height());
}

DeviceSwitchWidget::DeviceSwitchWidget(QWidget *parent) : QWidget (parent)
{
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    devWidget = new UkmediaDeviceWidget(this);
    appWidget = new ApplicationVolumeWidget(this);//appScrollWidget->area);
    miniWidget = new UkmediaMiniMasterVolumeWidget();

    appWidget->appArea = new QScrollArea(appWidget);
    appWidget->displayAppVolumeWidget = new UkuiApplicationWidget(appWidget->appArea);
    appWidget->appArea->setWidget(appWidget->displayAppVolumeWidget);
    appWidget->m_pVlayout = new QVBoxLayout(appWidget->displayAppVolumeWidget);

    appWidget->appArea->setFixedSize(358,177);
    appWidget->appArea->move(0,143);
    appWidget->displayAppVolumeWidget->setFixedWidth(358);
    appWidget->displayAppVolumeWidget->move(0,143);

    switchToMiniBtn = new UkuiMediaButton(this);
    switchToMiniBtn->setParent(this);
    switchToMiniBtn->setFocusPolicy(Qt::NoFocus);

    switchToMiniBtn->setToolTip(tr("Go Into Mini Mode"));
    QSize switchSize(16,16);
    switchToMiniBtn->setIconSize(switchSize);
    switchToMiniBtn->setFixedSize(36,36);
    switchToMiniBtn->move(361,6);
    switchToMiniBtn->setStyle(new CustomStyle());
    switchToMiniBtn->setIcon(QIcon("/usr/share/ukui-media/img/mini-module.svg"));

    output_stream_list = new QStringList;
    input_stream_list = new QStringList;
    device_name_list = new QStringList;
    device_display_name_list = new QStringList;
    stream_control_list = new QStringList;
    app_name_list = new QStringList;
    appBtnNameList = new QStringList;

    //初始化matemixer
    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }
    //创建context
    context = mate_mixer_context_new();
    mate_mixer_context_set_app_name (context,_("Ukui Volume Control App"));//设置app名
    mate_mixer_context_set_app_id(context, GVC_APPLET_DBUS_NAME);
    mate_mixer_context_set_app_version(context,VERSION);
    mate_mixer_context_set_app_icon(context,"ukuimedia-volume-control");

    //打开context
    if G_UNLIKELY (mate_mixer_context_open(context) == FALSE) {
        g_warning ("Failed to connect to a sound system**********************");
    }
    appWidget->setFixedSize(358,320);
    devWidget->setFixedSize(358,320);

    devWidget->move(42,0);
    appWidget->move(42,0);
    this->setFixedSize(400,320);

    devWidget->show();
    appWidget->hide();
    //添加托盘及菜单
    systemTrayMenuInit();
    //初始化设备界面
    deviceSwitchWidgetInit();

    connect(switchToMiniBtn,SIGNAL(moveAdvanceSwitchBtnSignal()),this,SLOT(moveAdvanceSwitchBtnSlot()));
    //mini模式下切换设备，由于设计需求不需要此功能，删除此功能
    /*connect(miniWidget->deviceCombox,SIGNAL(currentIndexChanged(QString)),this,SLOT(deviceComboxIndexChanged(QString)));*/
    context_set_property(this);
    g_signal_connect (G_OBJECT (context),
                     "notify::state",
                     G_CALLBACK (on_context_state_notify),
                     this);
    //输出音量控制
    //mini模式下系统音量的控制
    connect(miniWidget->masterVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(miniMastrerSliderChangedSlot(int)));
    //应用音量下系统音量控制
    connect(appWidget->systemVolumeSlider,SIGNAL(valueChanged(int)),this,SLOT(advancedSystemSliderChangedSlot(int)));
    //高级模式下系统音量的控制
    connect(devWidget->outputDeviceSlider,SIGNAL(valueChanged(int)),this,SLOT(outputDeviceSliderChangedSlot(int)));
    //mini切换高级模式
    connect(miniWidget->switchBtn,SIGNAL(miniToAdvanceSignal()),this,SLOT(miniToAdvancedWidget()));
    //高级切换mini模式
    connect(switchToMiniBtn,SIGNAL(advanceToMiniSignal()),this,SLOT(advancedToMiniWidget()));
    //高级模式下设备音量和应用音量的切换
    connect(deviceBtn,SIGNAL(clicked()),this,SLOT(deviceButtonClickedSlot()));
    connect(appVolumeBtn,SIGNAL(clicked()),this,SLOT(appVolumeButtonClickedSlot()));
    //高级输出设备模式下静音控制
    connect(devWidget->outputMuteBtn,SIGNAL(clicked()),this,SLOT(devWidgetMuteButtonClickedSlot()));
    //mini模式下的一键静音
    connect(miniWidget->muteBtn,SIGNAL(clicked()),this,SLOT(miniWidgetMuteButtonClickedSlot()));
    //应用音量下一键静音
    connect(appWidget->systemVolumeBtn,SIGNAL(clicked()),this,SLOT(appWidgetMuteButtonCLickedSlot()));
    //静音action点击
//    connect(actionMute,SIGNAL(triggered()),this,SLOT(actionMuteTriggeredSLot()));
    connect(m_pMuteAction,SIGNAL(triggered()),this,SLOT(actionMuteTriggeredSLot()));
    //鼠标中间健点击托盘图标
    connect(this,SIGNAL(mouse_middle_clicked_signal()),this,SLOT(mouseMeddleClickedTraySlot()));
    //鼠标滚轮滚动托盘图标
    connect(soundSystemTrayIcon,SIGNAL(wheelRollEventSignal(bool)),this,SLOT(trayWheelRollEventSlot(bool)));
    //当mini模式界面显示时鼠标滚轮控制声音
    connect(miniWidget,SIGNAL(mouse_wheel_signal(bool)),this,SLOT(miniWidgetWheelSlot(bool)));
    //mini模式上下左右控制声音
    connect(miniWidget,SIGNAL(keyboard_pressed_signal(int)),this,SLOT(miniWidgetKeyboardPressedSlot(int)));
    //静音输入
    connect(devWidget->inputMuteButton,SIGNAL(clicked()),this,SLOT(inputWidgetMuteButtonClicked()));
    //当有录音时，输入滑块控制输入声音
    connect(devWidget->inputDeviceSlider,SIGNAL(valueChanged(int)),this,SLOT(inputWidgetSliderChangedSlot(int)));

    //主屏改变
    connect(qApp,SIGNAL(primaryScreenChanged(QScreen *)),this,SLOT(primaryScreenChangedSlot(QScreen *)));

    trayRect = soundSystemTrayIcon->geometry();
    appWidget->displayAppVolumeWidget->setLayout(appWidget->m_pVlayout);

    this->setObjectName("mainWidget");
    appWidget->setObjectName("appWidget");

    appWidget->appArea->setStyleSheet("QScrollArea{border:none;}");
    appWidget->appArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    appWidget->appArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    appWidget->appArea->viewport()->setStyleSheet("background-color:transparent;");
    appWidget->appArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{margin:0px 0px 0px 0px;background:transparent;border:0px;width:2px;height:100px;}"
                                                    "QScrollBar::up-arrow:vertical{height:0px;}"
                                                    "QScrollBar::sub-line:vertical{border:0px solid;height:0px}"
                                                    "QScrollBar::sub-page:vertical{background:transparent;height:20px;}"
                                                    "QScrollBar::handle:vertical{background-color:rgba(255,255,255,0.25);"
                                                    "opacity:0.25;border-radius:1px;width:2px;}"
                                                    "QScrollBar::handle:vertical:hover{background-color:#3593b5;}"
                                                    "QScrollBar::handle:vertical:pressed{background-color:#3593b5;}"
                                                    "QScrollBar::add-page:vertical{background:transparent;height:20px;}"
                                                    "QScrollBar::add-line:vertical{border:0px solid;height:0px}"
                                                    "QScrollBar::down-arrow:vertical{height:0px;}");
    if (appnum <= 0) {
        appWidget->upWidget->hide();
    }
    else {
        appWidget->upWidget->show();
    }
}

/*
    托盘菜单初始化
*/
void DeviceSwitchWidget::systemTrayMenuInit()
{
    menu = new QMenu(this);
    soundSystemTrayIcon = new UkmediaTrayIcon(this);

    //为系统托盘图标添加菜单静音和声音首选项
    soundSystemTrayIcon->setToolTip(tr("Output volume control"));
    m_pMuteAction = new QAction(QIcon(""),tr("Mute"));
    m_pSoundPreferenceAction = new QAction(QIcon("/usr/share/ukui-media/img/setting.svg"),tr("Sound preference(S)"));

    //设置右键菜单
    menu->addAction(m_pMuteAction);
    menu->addSeparator();
    menu->addAction(m_pSoundPreferenceAction);

    menu->setObjectName("outputSoundMenu");
    soundSystemTrayIcon->setContextMenu(menu);

    soundSystemTrayIcon->setVisible(true);

    connect(soundSystemTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),\
            this,SLOT(activatedSystemTrayIconSlot(QSystemTrayIcon::ActivationReason)));
    connect(m_pSoundPreferenceAction,SIGNAL(triggered()),this,SLOT(jumpControlPanelSlot()));

    menu->setWindowFlag(Qt::FramelessWindowHint);        //重要
    menu->setAttribute(Qt::WA_TranslucentBackground);    //重要
}

/*
    mini模式下系统音量的控制
*/
void DeviceSwitchWidget::miniMastrerSliderChangedSlot(int value)
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    QString percent;
    percent = QString::number(value);
    mate_mixer_stream_control_set_mute(control,FALSE);
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(control,guint(volume));
    miniWidget->displayVolumeLabel->setText(percent);
}

/*
    应用音量下系统音量控制
*/
void DeviceSwitchWidget::advancedSystemSliderChangedSlot(int value)
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    QString percent;
    percent = QString::number(value);
    mate_mixer_stream_control_set_mute(control,FALSE);
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(control,guint(volume));
    appWidget->systemVolumeDisplayLabel->setText(percent);
}

/*
    高级模式下系统音量的控制
*/
void DeviceSwitchWidget::outputDeviceSliderChangedSlot(int value)
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    QString percent;

    percent = QString::number(value);
    mate_mixer_stream_control_set_mute(control,FALSE);
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(control,guint(volume));
}

/*
    高级模式中设备音量的静音按钮点击
*/
void DeviceSwitchWidget::devWidgetMuteButtonClickedSlot()
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    else {
        status =true;
        m_pMuteAction->setIcon(QIcon("/usr/share/ukui-media/img/tick.svg"));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    Q_EMIT system_muted_signal(status);
    menu->hide();
}

/*
    mini模式下静音按钮点击
*/
void DeviceSwitchWidget::miniWidgetMuteButtonClickedSlot()
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    else {
        status =true;
        m_pMuteAction->setIcon(QIcon("/usr/share/ukui-media/img/tick.svg"));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    Q_EMIT system_muted_signal(status);
    menu->hide();
}

/*
    应用音量下静音按钮点击
*/
void DeviceSwitchWidget::appWidgetMuteButtonCLickedSlot()
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    else {
        status =true;
        m_pMuteAction->setIcon(QIcon("/usr/share/ukui-media/img/tick.svg"));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    Q_EMIT system_muted_signal(status);
    menu->hide();
}

/*
    菜单静音勾选框点击
*/
void DeviceSwitchWidget::muteCheckBoxReleasedSlot()
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0 + 0.5);
    bool status = mate_mixer_stream_control_get_mute(control);
    if (status) {
        status = false;
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    else {
        status =true;
        m_pMuteAction->setIcon(QIcon("/usr/share/ukui-media/img/tick.svg"));
        mate_mixer_stream_control_set_mute(control,status);
        updateSystemTrayIcon(volume,status);
    }
    Q_EMIT system_muted_signal(status);
    menu->hide();
}

/*
    菜单静音选项点击
*/
void DeviceSwitchWidget::actionMuteTriggeredSLot()
{
    bool isMute = false;
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    isMute = mate_mixer_stream_control_get_mute(control);
    int opVolume = int(mate_mixer_stream_control_get_volume(control));
    opVolume = int(opVolume*100/65536.0 + 0.5);
    if (isMute) {
        m_pMuteAction->setIcon(QIcon(""));
        mate_mixer_stream_control_set_mute(control,FALSE);
    }
    else {
        m_pMuteAction->setIcon(QIcon("/usr/share/ukui-media/img/tick.svg"));
        mate_mixer_stream_control_set_mute(control,TRUE);
    }
    isMute = mate_mixer_stream_control_get_mute(control);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    updateSystemTrayIcon(volume,isMute);
    Q_EMIT system_muted_signal(isMute);
}

/*
    鼠标滚轮键点击托盘
*/
void DeviceSwitchWidget::mouseMeddleClickedTraySlot()
{
    stream = mate_mixer_context_get_default_output_stream(context);
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
    updateSystemTrayIcon(volume,isMute);

    //发送系统静音信号给应用音量
    Q_EMIT system_muted_signal(isMute);
}

/*
    鼠标滚轮在托盘上滚动
*/
void DeviceSwitchWidget::trayWheelRollEventSlot(bool step)
{
    stream = mate_mixer_context_get_default_output_stream(context);
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

/*
    mini界面显示时，鼠标在界面外滚动控制音量
*/
void DeviceSwitchWidget::miniWidgetWheelSlot(bool step)
{
    stream = mate_mixer_context_get_default_output_stream(context);
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

/*
    mini界面显示时按上下左右控制音量
*/
void DeviceSwitchWidget::miniWidgetKeyboardPressedSlot(int volumeGain)
{
    stream = mate_mixer_context_get_default_output_stream(context);
    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control(stream);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    miniWidget->masterVolumeSlider->setValue(volume+volumeGain);
}


/*
    跳转到声音首选项
*/
void DeviceSwitchWidget::jumpControlPanelSlot()
{
    m_process = new QProcess(this);
    m_process->start("ukui-control-center -s");
}

/*
    按下高级模式的切换界面按钮移动1px
*/
void DeviceSwitchWidget::moveAdvanceSwitchBtnSlot()
{
    switchToMiniBtn->move(362,7);
}

/*
    mini 跳转到 advance
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

/*Right-click menu to jump to sound settings
    advanced 跳转到 mini
*/
void DeviceSwitchWidget::advancedToMiniWidget()
{
    miniWidgetShow();
    this->hide();
    displayMode = MINI_MODE;
}

/*
    mini模式下切换输出设备
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

/*
    滚动输入音量条
*/
void DeviceSwitchWidget::inputWidgetSliderChangedSlot(int value)
{
    stream = mate_mixer_context_get_default_input_stream (context);
    control = mate_mixer_stream_get_default_control(stream);
    QString percent;
    percent = QString::number(value);
    mate_mixer_stream_control_set_mute(control,FALSE);
    bool status = mate_mixer_stream_control_get_mute(control);
    updateMicrophoneIcon(value,status);
    int volume = value*65536/100;
    mate_mixer_stream_control_set_volume(control,volume);
}

/*
    输入静音按钮点击
*/
void DeviceSwitchWidget::inputWidgetMuteButtonClicked()
{
    stream = mate_mixer_context_get_default_input_stream (context);
    control = mate_mixer_stream_get_default_control(stream);
    bool state = mate_mixer_stream_control_get_mute(control);
    int volume = mate_mixer_stream_control_get_volume(control);
    mate_mixer_stream_control_set_mute(control,!state);
    updateMicrophoneIcon(volume,!state);
}

/*
    主屏改变获取托盘所在位置
*/
void DeviceSwitchWidget::primaryScreenChangedSlot(QScreen *screen)
{
    Q_UNUSED(screen);
    trayRect = soundSystemTrayIcon->geometry();
}

/*
    激活声音托盘图标
*/
void DeviceSwitchWidget::activatedSystemTrayIconSlot(QSystemTrayIcon::ActivationReason reason)
{
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
        if (isShow) {
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
            isShow = true;
            break;
        }
    }
    //鼠标左键双击图标
    case QSystemTrayIcon::DoubleClick: {
        hideWindow();
        break;
    }
    case QSystemTrayIcon::Context: {
        showMenu();
        break;
    }
    default:
        break;
    }
}

/*
    QWidgetAction 初始化
*/
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

/*
    初始化主界面
*/
void DeviceSwitchWidget::deviceSwitchWidgetInit()
{
    const QSize iconSize(19,19);
    QWidget *deviceWidget = new QWidget(this);
    deviceWidget->setFixedSize(42,320);

    deviceBtn = new QPushButton(deviceWidget);
    appVolumeBtn = new QPushButton(deviceWidget);

    deviceBtn->setFlat(true);
    appVolumeBtn->setFlat(true);
    deviceBtn->setFocusPolicy(Qt::NoFocus);
    appVolumeBtn->setFocusPolicy(Qt::NoFocus);
    deviceBtn->setFixedSize(37,37);
    appVolumeBtn->setFixedSize(37,37);

    deviceBtn->setIconSize(iconSize);
    appVolumeBtn->setIconSize(iconSize);

    deviceBtn->setIcon(QIcon("/usr/share/ukui-media/img/device.svg"));
    appVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/application.svg"));

    deviceBtn->move(2,10);
    appVolumeBtn->move(2,57);

    //切换按钮设置tooltip
    deviceBtn->setToolTip(tr("Device Volume"));
    appVolumeBtn->setToolTip(tr("Application Volume"));

    deviceBtn->toolTip().resize(98,38);
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
    deviceWidget->setStyleSheet("QWidget{ border-right: 1px solid rgba(255,255,255,0.08); }");
}

/*点击切换设备按钮对应的槽函数*/
void DeviceSwitchWidget::deviceButtonClickedSlot()
{
    btnType = DEVICE_VOLUME_BUTTON;
    appWidget->hide();
    devWidget->show();

    appVolumeBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                "padding-left:0px;}"
                                "QPushButton::hover{background:rgba(255,255,255,0.12);"
                                "border-radius:4px;}"
                                "QPushButton::pressed{background:rgba(61,107,229,1);"
                                "border-radius:4px;}");
    deviceBtn->setStyleSheet("QPushButton{background:rgba(61,107,229,1);"
                             "border-radius:4px;}");
}

/*点击切换应用音量按钮对应的槽函数*/
void DeviceSwitchWidget::appVolumeButtonClickedSlot()
{
    btnType = APP_VOLUME_BUTTON;
    appWidget->show();
    devWidget->hide();
    //切换按钮样式
    deviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                             "padding-left:0px;}"
                             "QPushButton::hover{background:rgba(255,255,255,0.12);"
                             "border-radius:4px;}"
                             "QPushButton::pressed{background:rgba(61,107,229,1);"
                             "border-radius:4px;}");
    appVolumeBtn->setStyleSheet("QPushButton{background:rgba(61,107,229,1);"
                             "border-radius:4px;}");
}

/*
 * context状态通知
*/
void DeviceSwitchWidget::on_context_state_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    MateMixerState state = mate_mixer_context_get_state (context);
    list_device(w,context);
    if (state == MATE_MIXER_STATE_READY) {
        update_default_input_stream (w);
        update_icon_output(w,context);
        update_icon_input(w,context);
    }
    else if (state == MATE_MIXER_STATE_FAILED) {
        g_warning("Failed to connect a sound system");
    }
}

/*
    context 存储control增加
*/
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


/*
    当其他设备插入时添加这个stream
*/
void DeviceSwitchWidget::on_context_stream_added (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    w->stream = mate_mixer_context_get_stream (context, name);
    if (G_UNLIKELY (w->stream == nullptr))
        return;

    /* If the newly added stream belongs to the currently selected device and
     * the test button is hidden, this stream may be the one to allow the
     * sound test and therefore we may need to enable the button */
    add_stream (w, w->stream,context);
}

/*
    列出设备
*/
void DeviceSwitchWidget::list_device(DeviceSwitchWidget *w,MateMixerContext *context)
{
    const GList *list;
    list = mate_mixer_context_list_streams (context);

    while (list != nullptr) {
        add_stream (w, MATE_MIXER_STREAM (list->data),context);
        w->stream = MATE_MIXER_STREAM(list->data);
        const gchar *stream_name = mate_mixer_stream_get_name(w->stream);
        MateMixerDirection direction = mate_mixer_stream_get_direction(w->stream);
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
            w->output_stream_list->append(stream_name);
            /*w->miniWidget->deviceCombox->addItem(label);*/
        }
        else if (direction == MATE_MIXER_DIRECTION_INPUT) {
            w->input_stream_list->append(stream_name);
        }
        list = list->next;
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
        if (stream == input) {
            bar_set_stream (w, stream);
            MateMixerStreamControl *c = mate_mixer_stream_get_default_control(stream);
            update_input_settings (w,c);
        }
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        MateMixerStream        *output;
        MateMixerStreamControl *control;
        output = mate_mixer_context_get_default_output_stream (context);
        control = mate_mixer_stream_get_default_control (stream);

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
            MateMixerSwitchOption *opt = mate_mixer_switch_get_active_option(swt);
            const char *name = mate_mixer_switch_option_get_name(opt);
            const char *label = mate_mixer_switch_option_get_label(opt);
            qDebug() << "opt name:" << name << "opt label:" << label;

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
                if (strcmp(m_pAppName,"ukui-session") != 0) {
                    w->stream_control_list->append(m_pStreamControlName);
                    if G_UNLIKELY (w->control == nullptr)
                        return;
                    add_application_control (w, w->control);
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

/*
    添加应用音量控制
*/
void DeviceSwitchWidget::add_application_control (DeviceSwitchWidget *w, MateMixerStreamControl *control)
{
    MateMixerStreamControlMediaRole media_role;
    MateMixerAppInfo *info;
    guint app_count;
    MateMixerDirection direction = MATE_MIXER_DIRECTION_UNKNOWN;
    const gchar *app_id;
    const gchar *app_name;
    const gchar *app_icon;
    app_count = appnum;

    media_role = mate_mixer_stream_control_get_media_role (control);

    /* Add stream to the applications page, but make sure the stream qualifies
     * for the inclusion */
    info = mate_mixer_stream_control_get_app_info (control);
    if (info == nullptr)
        return;

    /* Skip streams with roles we don't care about */
    if (media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT ||
        media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_TEST ||
        media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_ABSTRACT ||
        media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_FILTER)
        return;

    app_id = mate_mixer_app_info_get_id (info);

    /* These applications may have associated streams because they do peak
     * level monitoring, skip these too */
    if (!g_strcmp0 (app_id, "org.mate.VolumeControl") ||
        !g_strcmp0 (app_id, "org.gnome.VolumeControl") ||
        !g_strcmp0 (app_id, "org.PulseAudio.pavucontrol"))
        return;

    QString app_icon_name = mate_mixer_app_info_get_icon(info);
    app_name = mate_mixer_app_info_get_name (info);
    //添加应用添加到应用音量中
    add_app_to_appwidget(w,app_name,app_icon_name,control);

    if (app_name == nullptr)
        app_name = mate_mixer_stream_control_get_label (control);
    if (app_name == nullptr)
        app_name = mate_mixer_stream_control_get_name (control);
    if (G_UNLIKELY (app_name == nullptr))
        return;

    /* By default channel bars use speaker icons, use microphone icons
     * instead for recording applications */
    w->stream = mate_mixer_stream_control_get_stream (control);
    if (w->stream != nullptr)
        direction = mate_mixer_stream_get_direction (w->stream);

    if (direction == MATE_MIXER_DIRECTION_INPUT) {
    }
    app_icon = mate_mixer_app_info_get_icon (info);
    if (app_icon == nullptr) {
        if (direction == MATE_MIXER_DIRECTION_INPUT)
            app_icon = "audio-input-microphone";
        else
            app_icon = "applications-multimedia";
    }
    bar_set_stream_control (w,direction,control);
}

void DeviceSwitchWidget::on_stream_control_added (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w)
{
    MateMixerStreamControlRole role;
    w->control = mate_mixer_stream_get_control (stream, name);
    MateMixerAppInfo *m_pAppInfo = mate_mixer_stream_control_get_app_info(w->control);
    if (m_pAppInfo != nullptr) {
        const gchar *m_pAppName = mate_mixer_app_info_get_name(m_pAppInfo);
        if (strcmp(m_pAppName,"ukui-session") != 0) {
            if G_UNLIKELY (w->control == nullptr)
                return;

            role = mate_mixer_stream_control_get_role (w->control);
            if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
                w->stream_control_list->append(name);
                add_application_control (w, w->control);
            }
        }
    }
    else {
        return;
    }
}

/*
    移除control
*/
void DeviceSwitchWidget::on_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(stream);
    /* No way to be sure that it is an application control, but we don't have
     * any other than application bars that could match the name */

    if (!w->stream_control_list->contains(name)) {
        return;
    }

    if (w->stream_control_list->count() > 0 && w->app_name_list->count() > 0) {
        remove_application_control (w, name);
    }
    else {
        w->stream_control_list->clear();
        w->app_name_list->clear();
        w->appBtnNameList->clear();
    }
}

void DeviceSwitchWidget::remove_application_control (DeviceSwitchWidget *w,const gchar *m_pAppName)
{
    g_debug ("Removing application stream %s", m_pAppName);
    /* We could call bar_set_stream_control here, but that would pointlessly
     * invalidate the channel bar, so just remove it ourselves */
    int index = -1;
    if (w->stream_control_list->contains(m_pAppName)) {
        index = w->stream_control_list->indexOf(m_pAppName);
    }
    if (index == -1)
        return;
//    if (w->stream_control_list->count() > w->app_name_list->count()) {
//        w->stream_control_list->removeAt(index);
//        return;
//    }
//    if (index > w->app_name_list->count() || index > w->appWidget->m_pVlayout->count()) {
//        return;
//    }
    w->stream_control_list->removeAt(index);
    w->app_name_list->removeAt(index);
    w->appBtnNameList->removeAt(index);
    QLayoutItem *item ;

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

void DeviceSwitchWidget::add_app_to_appwidget(DeviceSwitchWidget *w,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control)
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
    iconName.append(app_icon_name);
    iconName.append(".desktop");
    XdgDesktopFile xdg;
    xdg.load(iconName);
    QString title(xdg.localizedValue("Name").toString());
    QString xdgicon(xdg.localizedValue("Icon").toString());

//    GError **error = nullptr;
//    GKeyFileFlags flags = G_KEY_FILE_NONE;
//    GKeyFile *keyflie = g_key_file_new();

//    g_key_file_load_from_file(keyflie,iconName.toLocal8Bit(),flags,error);
//    char *icon_str = g_key_file_get_locale_string(keyflie,"Desktop Entry","Icon",nullptr,nullptr);
//    QIcon icon = QIcon::fromTheme(QString::fromLocal8Bit(icon_str));
    w->appWidget->app_volume_list->append(app_icon_name);

    //widget显示应用音量
    QWidget *app_widget = new QWidget(w->appWidget->displayAppVolumeWidget);
    app_widget->setFixedSize(306,60);
    QHBoxLayout *hlayout = new QHBoxLayout(app_widget);
    QVBoxLayout *vlayout = new QVBoxLayout();
    QSpacerItem *item1 = new QSpacerItem(18,20);
    QSpacerItem *item2 = new QSpacerItem(12,20);
    QWidget *wid = new QWidget(app_widget);

    wid->setFixedSize(306,38);
    w->appWidget->appLabel = new QLabel(app_widget);
    w->appWidget->appIconBtn = new QPushButton(wid);
    w->appWidget->appSlider = new UkmediaVolumeSlider(wid);
    w->appWidget->appMuteBtn = new QPushButton(wid);
    w->appWidget->appSlider->setOrientation(Qt::Horizontal);
    w->appWidget->appIconBtn->setFixedSize(38,38);
    w->appWidget->appMuteBtn->setFixedSize(24,24);

    w->appWidget->appSlider->setStyle(new CustomStyle());
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
    vlayout->setSpacing(8);
    app_widget->setLayout(vlayout);
    app_widget->layout()->setContentsMargins(0,0,0,0);

    //设置每项的固定大小
    w->appWidget->appLabel->setFixedSize(220,18);

    QSize icon_size(32,32);
    w->appWidget->appIconBtn->setIconSize(icon_size);
//    w->appWidget->appIconBtn->setStyleSheet("QPushButton{background:rgba(255,0,0,0.4);}");
    w->appWidget->appIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");
    w->appWidget->appIconBtn->setIcon(QIcon::fromTheme(xdgicon));
    w->appWidget->appIconBtn->setFocusPolicy(Qt::NoFocus);

    w->appWidget->appSlider->setMaximum(100);
    w->appWidget->appSlider->setFixedSize(220,22);

    QString appSliderStr = app_name;
    QString appMuteBtnlStr = app_name;
    QString appVolumeLabelStr = app_name;

    appSliderStr.append("Slider");
    appMuteBtnlStr.append("button");
    appVolumeLabelStr.append("VolumeLabel");
    appSliderStr.append(QString::number(app_count));
    appMuteBtnlStr.append(QString::number(app_count));
    appVolumeLabelStr.append(QString::number(app_count));

    w->app_name_list->append(appSliderStr);
    w->appBtnNameList->append(appMuteBtnlStr);
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

    if (is_mute) {
        btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (display_volume <= 0) {
        btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (display_volume > 0 && display_volume <= 33) {
       btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
    }
    else if(display_volume > 33 && display_volume <= 66) {
        btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
    }
    else if (display_volume > 66) {
        btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
    }

    if (title == "") {
        w->appWidget->appLabel->setText(app_icon_name);
    }
    else {
        w->appWidget->appLabel->setText(title);
    }

    /*滑动条控制应用音量*/
    connect(w->appWidget->appSlider,&QSlider::valueChanged,[=](int value){
        application_name = appSliderStr;
        QSlider *slider = w->findChild<QSlider*>(appSliderStr);
        if (slider == nullptr)
            return;
        slider->setValue(value);
        QPushButton *btn = w->findChild<QPushButton*>(appMuteBtnlStr);
        if (btn == nullptr)
            return;

        bool status = mate_mixer_stream_control_get_mute(control);
        int v = int(value*65536/100 + 0.5);
        mate_mixer_stream_control_set_volume(control,guint(v));
        if (value > 0) {
            mate_mixer_stream_control_set_mute(control,false);
        }
        if (status) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (value <= 0) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (value > 0 && value <= 33) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        }
        else if(value > 33 && value <= 66) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        }
        else if (value > 66) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
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
        bool isMute = mate_mixer_stream_control_get_mute(control);
        int volume = mate_mixer_stream_control_get_volume(control);
        mate_mixer_stream_control_set_mute(control,!isMute);
        mate_mixer_stream_control_set_volume(control,guint(volume));
        volume = int(volume*100/65536 + 0.5);
        QPushButton *btn = w->appWidget->findChild<QPushButton *>(appMuteBtnlStr);
        if (btn == nullptr)
            return;
        isMute = mate_mixer_stream_control_get_mute(control);
        if (isMute) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (volume <= 0) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (volume > 0 && volume <= 33) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        }
        else if(volume > 33 && volume <= 66) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        }
        else if (volume > 66) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        }
    });

    connect(w,&DeviceSwitchWidget::app_volume_changed,[=](bool is_mute,int volume,QString app_name,QString appBtnName){
        Q_UNUSED(is_mute);
        QString slider_str = app_name;

        QSlider *slider = w->findChild<QSlider*>(slider_str);
        if (slider == nullptr)
            return;
        slider->setValue(volume);
        QPushButton *btn = w->findChild<QPushButton *>(appBtnName);
        if (btn == nullptr)
            return;
        if (is_mute) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (volume <= 0) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (volume > 0 && volume <= 33) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        }
        else if(volume > 33 && volume <= 66) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        }
        else if (volume > 66) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        }
    });

    connect(w,&DeviceSwitchWidget::system_muted_signal,[=](bool isMute){
        mate_mixer_stream_control_set_mute(control,isMute);

        int volume = mate_mixer_stream_control_get_volume(control);
        volume = volume*100/65536.0;
        QPushButton *btn = w->findChild<QPushButton *>(appMuteBtnlStr);
        if (btn == nullptr)
            return;
        if (isMute) {
            btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (volume <= 0) {
             btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        }
        else if (volume > 0 && volume <= 33) {
             btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        }
        else if(volume > 33 && volume <= 66) {
             btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        }
        else if (volume > 66) {
             btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        }
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
    w->appWidget->displayAppVolumeWidget->resize(358,14+appnum*78);
    w->appWidget->m_pVlayout->setContentsMargins(18,14,34,18);
    w->appWidget->m_pVlayout->update();
    
    app_widget->setStyleSheet("QWidget{ background:transparent;}");
    w->appWidget->displayAppVolumeWidget->setStyleSheet("QWidget{ background:transparent;}");
    w->appWidget->appMuteBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                                            "padding-left:0px;}");
    w->appWidget->appLabel->setStyleSheet("QLabel{width:210px;"
                                          "height:14px;"
                                          "font-family:Noto Sans CJK SC;"
                                          "font-size:14px;"
                                          "color:rgba(255,255,255,0.91);"
                                          "line-height:28px;}");
    w->appWidget->appSlider->setStyleSheet("QSlider::groove:horizontal {border: 0px solid #bbb;}"
                                           "QSlider::sub-page:horizontal {"
                                           "background: rgb(133,219,138);"
                                           "border-radius: 2px;"
                                           "margin-top:9px;"
                                           "margin-bottom:9px;}"
                                           "QSlider::add-page:horizontal {"
                                           "background:  rgba(255,255,255,0.1);"
                                           "border: 0px solid #777;"
                                           "border-radius: 2px;"
                                           "margin-top:9px;"
                                           "margin-bottom:9px;}"
                                           "QSlider::handle:horizontal {"
                                           "width: 20px;height: 20px;"
                                           "background: rgb(79,184,88);"
                                           "border-radius:10px;}"
                                           "QSlider::handle:horizontal:hover {"
                                           "width: 20px;height: 20px;"
                                           "background: rgb(133,219,138);"
                                           "border-radius:10px;}"
                                           "QSlider::handle:horizontal:selected {"
                                           "width: 18px;height: 18px;"
                                           "background: rgb(64,158,74)"
                                           "border-radius:9px;}");
}

/*
    同步应用音量
*/
void DeviceSwitchWidget::update_app_volume(MateMixerStreamControl *control, QString *str, DeviceSwitchWidget *w )
{
    Q_UNUSED(str);
    guint volume = mate_mixer_stream_control_get_volume(control);
    bool is_mute = mate_mixer_stream_control_get_mute(control);
    volume = guint(volume*100/65536.0+0.5);
    const gchar *controlName = mate_mixer_stream_control_get_name(control);
    int index = w->stream_control_list->indexOf(controlName);
    if (index == -1)
        return;
    /*MateMixerAppInfo *info = mate_mixer_stream_control_get_app_info(control);
    const gchar *app_name = mate_mixer_app_info_get_name(info);*/
    QString appName = w->app_name_list->at(index);
    QString appBtnName = w->appBtnNameList->at(index);
    Q_EMIT w->app_volume_changed(is_mute,int(volume),appName,appBtnName);
}

/*应用音量静音*/
void DeviceSwitchWidget::app_volume_mute (MateMixerStreamControl *control, QString *pspec ,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    Q_UNUSED(w);
    MateMixerAppInfo *app_info = mate_mixer_stream_control_get_app_info(control);
    if (app_info == nullptr)
        return;
    /*bool is_mute = mate_mixer_stream_control_get_mute(control);*/
    int volume = mate_mixer_stream_control_get_volume(control);
    volume = volume*100/65536.0+0.5;
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

/*
    连接context，处理不同信号
*/
void DeviceSwitchWidget::set_context(DeviceSwitchWidget *w,MateMixerContext *context)
{
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

/*
    remove stream
*/
void DeviceSwitchWidget::on_context_stream_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(context);
    remove_stream (w, name);
}

/*
    移除stream
*/
void DeviceSwitchWidget::remove_stream (DeviceSwitchWidget *w, const gchar *name)
{
    int index = w->output_stream_list->indexOf(name);
    if (index == -1) {
        return;
    }
    else if (index >= 0) {
        w->output_stream_list->removeAt(index);
        w->miniWidget->deviceCombox->removeItem(index);
    }
    /*w->miniWidget->deviceCombox->removeItem(index);*/
    if (w->appWidget->app_volume_list != nullptr) {
        bar_set_stream(w,nullptr);
    }
}

/*
    context 添加设备并设置到单选框
*/
void DeviceSwitchWidget::on_context_device_added (MateMixerContext *context, const gchar *name, DeviceSwitchWidget *w)
{
    MateMixerDevice *device;
    device = mate_mixer_context_get_device (context, name);
    if (G_UNLIKELY (device == nullptr))
        return;
    add_device (w, device);
}

/*
    添加设备
*/
void DeviceSwitchWidget::add_device (DeviceSwitchWidget *w, MateMixerDevice *device)
{
    const gchar *name;

    name  = mate_mixer_device_get_name (device);
    w->device_name_list->append(name);
    /*const gchar *label;
    label = mate_mixer_device_get_label (device);
    w->miniWidget->deviceCombox->addItem(label);*/
}

/*
    移除设备
*/
void DeviceSwitchWidget::on_context_device_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(context);
    int  index = w->device_name_list->indexOf(name);
    if (index == -1)
        return;
    if (index >= 0) {
        w->device_name_list->removeAt(index);
    }

    /*MateMixerDevice *dev = mate_mixer_context_get_device(context,name);
    QString str = mate_mixer_device_get_label(dev);
    do {
        if (name == w->device_name_list->at(count)) {
            w->device_name_list->removeAt(count);
            w->miniWidget->deviceCombox->removeItem(count);
            break;
        }
        count++;
        if (count > w->device_name_list->size()) {
            break;
        }
    }while(1);
    */
}

/*
    默认输入流通知
*/
void DeviceSwitchWidget::on_context_default_input_stream_notify (MateMixerContext *context,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    MateMixerStream *stream;

    g_debug ("Default input stream has changed");
    stream = mate_mixer_context_get_default_input_stream (context);
    update_icon_input (w,w->context);
    set_input_stream (w, stream);
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

                if (direction == MATE_MIXER_DIRECTION_INPUT)
                    mate_mixer_stream_control_set_stream (control, stream);
            }
            controls = controls->next;
        }

        /* Enable/disable the peak level monitor according to mute state */
        g_signal_connect (G_OBJECT (stream),
                          "notify::mute",
                          G_CALLBACK (on_stream_control_mute_notify),
                          w);
    }
    update_input_settings (w,w->m_pInputBarStreamControl);
}

/*
    control 静音通知
*/
void DeviceSwitchWidget::on_stream_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    Q_UNUSED(w);
//    update_icon_output(w,w->context);
    /* Stop monitoring the input stream when it gets muted */
//    if (mate_mixer_stream_control_get_mute (control) == TRUE)
//        mate_mixer_stream_control_set_monitor_enabled (control, FALSE);
//    else
//        mate_mixer_stream_control_set_monitor_enabled (control, TRUE);
}

/*
    默认输出流通知
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
    update_icon_output(w,context);
//    set_output_stream (w, stream);
}

/*
    移除存储control
*/
void DeviceSwitchWidget::on_context_stored_control_removed (MateMixerContext *context,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(context);
    Q_UNUSED(name);
    if (w->appWidget->app_volume_list != nullptr) {
        bar_set_stream (w, nullptr);
    }
}

/*
 * context设置属性
*/
void DeviceSwitchWidget::context_set_property(DeviceSwitchWidget *w)
{
    set_context(w,w->context);
}

/*
    更新输入音量及图标
*/
void DeviceSwitchWidget::update_icon_input (DeviceSwitchWidget *w,MateMixerContext *context)
{
    MateMixerStream *stream;
    MateMixerStreamControl *control = nullptr;
    const gchar *app_id;
    gboolean show = FALSE;

    stream = mate_mixer_context_get_default_input_stream (context);

    const GList *inputs =mate_mixer_stream_list_controls(stream);
    control = mate_mixer_stream_get_default_control(stream);

    //初始化滑动条的值
    bool status = mate_mixer_stream_control_get_mute(control);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    int value = int(volume *100 /65536.0+0.5);
    w->devWidget->inputDeviceSlider->setValue(value);
    QString percent = QString::number(value);
    w->updateMicrophoneIcon(value,status);
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
                show = TRUE;
                break;
            }

            if (strcmp (app_id, "org.mate.VolumeControl") != 0 &&
                strcmp (app_id, "org.gnome.VolumeControl") != 0 &&
                strcmp (app_id, "org.PulseAudio.pavucontrol") != 0) {

                g_debug ("Found a recording application %s", app_id);
                if G_UNLIKELY (control == nullptr)
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

    gvc_stream_status_icon_set_control (w, control);

    if (control != nullptr) {
        g_debug ("Output icon enabled");
    }
    else {
        g_debug ("There is no output stream/control, output icon disabled");
    }

    if(show) {
        w->devWidget->inputWidgetShow();
    }
    else {
        w->devWidget->inputWidgetHide();
    }
}

/*
    更新输出音量及图标
*/
void DeviceSwitchWidget::update_icon_output (DeviceSwitchWidget *w,MateMixerContext *context)
{
    MateMixerStream *stream;
    MateMixerStreamControl *control = nullptr;

    stream = mate_mixer_context_get_default_output_stream (context);
    if (stream != nullptr)
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
    w->devWidget->outputDeviceSlider->setValue(value);
//    w->miniWidget->masterVolumeSlider->setValue(value);
    w->appWidget->systemVolumeSlider->setValue(value);
    slider1->setValue(value);
    QString percent = QString::number(value);
    QString systemTrayIcon;
    QIcon icon;
    if (state) {
        systemTrayIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(systemTrayIcon);
        w->soundSystemTrayIcon->setIcon(QIcon(icon));
        w->devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        w->appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        w->miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (value <= 0) {
        systemTrayIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(systemTrayIcon);
        w->miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        w->devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        w->appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        w->soundSystemTrayIcon->setIcon(QIcon(icon));
        w->m_pMuteAction->setIcon(QIcon(""));
        //如果主主音量处于静音状态，应用音量取消静音则设置主音量取消静音
        /*if (state) {
            connect(w,&DeviceSwitchWidget::appvolume_mute_change_mastervolume_status,[=](){
                mate_mixer_stream_control_set_mute(control,!state);
            });
        }*/
    }
    else if (value > 0 && value <= 33) {
        systemTrayIcon = "audio-volume-low";
        icon = QIcon::fromTheme(systemTrayIcon);
        w->soundSystemTrayIcon->setIcon(QIcon(icon));
        w->miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        w->devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        w->appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
    }
    else if(value > 33 && value <= 66) {
        systemTrayIcon = "audio-volume-medium";
        icon = QIcon::fromTheme(systemTrayIcon);
        w->soundSystemTrayIcon->setIcon(QIcon(icon));
        w->appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        w->devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        w->miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        w->m_pMuteAction->setIcon(QIcon(""));
    }
    else if (value > 66) {
        systemTrayIcon = "audio-volume-high";
        icon = QIcon::fromTheme(systemTrayIcon);
        w->soundSystemTrayIcon->setIcon(QIcon(icon));
        w->miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        w->devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        w->appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        w->m_pMuteAction->setIcon(QIcon(""));
    }
    w->miniWidget->displayVolumeLabel->setText(percent);
    w->appWidget->systemVolumeDisplayLabel->setText(percent);

    if (control != nullptr) {
            g_debug ("Output icon enabled");
    }
    else {
            g_debug ("There is no output stream/control, output icon disabled");
    }
}

void DeviceSwitchWidget::gvc_stream_status_icon_set_control (DeviceSwitchWidget *w,MateMixerStreamControl *control)
{
    g_signal_connect ( G_OBJECT (control),
                      "notify::volume",
                      G_CALLBACK (on_stream_control_volume_notify),
                      w);
    g_signal_connect (G_OBJECT (control),
                      "notify::mute",
                      G_CALLBACK (on_stream_control_mute_notify),
                      w);
    MateMixerStreamControlFlags flags = mate_mixer_stream_control_get_flags(control);
    if (flags & MATE_MIXER_STREAM_CONTROL_MUTE_READABLE) {
        g_signal_connect (G_OBJECT (control),
                          "notify::mute",
                          G_CALLBACK (on_control_mute_notify),
                          w);
    }
}

/*
    静音通知
*/
void DeviceSwitchWidget::on_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    gboolean mute = mate_mixer_stream_control_get_mute (control);
    int volume = int(mate_mixer_stream_control_get_volume(control));
    volume = int(volume*100/65536.0+0.5);
    MateMixerStream *stream = mate_mixer_stream_control_get_stream(control);
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        w->updateSystemTrayIcon(volume,mute);
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        w->updateMicrophoneIcon(volume,mute);
    }
}

/*
    stream control 声音通知
*/
void DeviceSwitchWidget::on_stream_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,DeviceSwitchWidget *w)
{
    Q_UNUSED(pspec);
    MateMixerStreamControlFlags flags;
    gboolean muted = FALSE;
    gdouble decibel = 0.0;
    guint volume = 0;

    QString decscription;

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

    MateMixerStream *stream = mate_mixer_stream_control_get_stream(control);
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);

    //设置输出滑动条的值
    int value = int(volume*100/65536.0 + 0.5);
//    QSlider *s = w->miniWidget->findChild<QSlider *>(w->outputControlName);
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        w->devWidget->outputDeviceSlider->setValue(value);
        w->appWidget->systemVolumeSlider->setValue(value);
        w->miniWidget->masterVolumeSlider->setValue(value);
        w->updateSystemTrayIcon(value,muted);
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        w->devWidget->inputDeviceSlider->setValue(value);
        w->updateMicrophoneIcon(value,muted);
    }
}

/*
    更新输出设置
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

}

void DeviceSwitchWidget::set_output_stream (DeviceSwitchWidget *w, MateMixerStream *stream)
{
    if (stream == nullptr) {
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

                if (direction == MATE_MIXER_DIRECTION_OUTPUT)
                    mate_mixer_stream_control_set_stream (control, stream);
            }
            controls = controls->next;
        }
    }

    update_output_stream_list (w, stream);
    update_output_settings(w,w->m_pOutputBarStreamControl);
}

/*
    更新输出stream 列表
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

/*
    bar设置stream
*/
void DeviceSwitchWidget::bar_set_stream (DeviceSwitchWidget  *w,MateMixerStream *stream)
{
    MateMixerStreamControl *control = nullptr;

    if (stream != nullptr)
        control = mate_mixer_stream_get_default_control (stream);
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

/*
    点击窗口之外的部分隐藏
*/
bool DeviceSwitchWidget:: event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            if (displayMode == MINI_MODE) {
                miniWidget->hide();
                isShow = true;
            }
            else if (displayMode == ADVANCED_MODE) {
                this->hide();
                isShow = true;
            }
        }
    }
    return QWidget::event(event);
}

/*
    更新高级模式下麦克风图标
*/
void DeviceSwitchWidget::updateMicrophoneIcon(int value, bool status)
{
    if (status) {
        devWidget->inputMuteButton->setIcon(QIcon("/usr/share/ukui-media/img/microphone-muted.svg"));
    }
    else if (value <= 0) {
        devWidget->inputMuteButton->setIcon(QIcon("/usr/share/ukui-media/img/microphone-muted.svg"));
    }
    else if (value > 0 && value <= 33) {
       devWidget->inputMuteButton->setIcon(QIcon("/usr/share/ukui-media/img/microphone-low.svg"));
    }
    else if(value > 33 && value <= 66) {
        devWidget->inputMuteButton->setIcon(QIcon("/usr/share/ukui-media/img/microphone-medium.svg"));
    }
    else if (value > 66) {
        devWidget->inputMuteButton->setIcon(QIcon("/usr/share/ukui-media/img/microphone-high.svg"));
    }

}

/*
    dbus获取任务栏的位置
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

/*
    在指定位置显示mini界面
*/
void DeviceSwitchWidget::miniWidgetShow()
{
    //给定任务栏高度和初始值
    int panelHeight = 46;
    int panelPosition = 0;

    panelHeight = getPanelHeight("panelheight");
    panelPosition = getPanelPosition("panelposition");

    int totalHeight = qApp->primaryScreen()->size().height() + qApp->primaryScreen()->geometry().y();
    int totalWidth = qApp->primaryScreen()->size().width() + qApp->primaryScreen()->geometry().x();
    if (panelPosition == 0) { //任务栏在下
        miniWidget->setGeometry(totalWidth-miniWidget->width(),totalHeight-panelHeight-miniWidget->height()-2,miniWidget->width(),miniWidget->height());
    }
    else if (panelPosition == 1) { //任务栏在上
        miniWidget->setGeometry(totalWidth-miniWidget->width(),qApp->primaryScreen()->geometry().y()+panelHeight+2,miniWidget->width(),miniWidget->height());
    }
    else if (panelPosition == 2) {//任务栏在左
        miniWidget->setGeometry(qApp->primaryScreen()->geometry().x()+panelHeight+2,totalHeight-miniWidget->height(),miniWidget->width(),miniWidget->height());
    }
    else if (panelPosition == 3) { //任务栏在右
        miniWidget->setGeometry(totalWidth-panelHeight-miniWidget->width()-2,totalHeight-miniWidget->height(),miniWidget->width(),miniWidget->height());
    }
    miniWidget->show();
}

/*
    高级模式在指定位置显示
*/
void DeviceSwitchWidget::advancedWidgetShow()
{
    //给定任务栏高度和位置初始值
    int panelHeight = 46;
    int panelPosition = 0;

    panelHeight = getPanelHeight("panelheight");
    panelPosition = getPanelPosition("panelposition");

    int totalHeight = qApp->primaryScreen()->size().height() + qApp->primaryScreen()->geometry().y();
    int totalWidth = qApp->primaryScreen()->size().width() + qApp->primaryScreen()->geometry().x();
    if (panelPosition == 0) { //任务栏在下
        this->setGeometry(totalWidth-this->width(),totalHeight-panelHeight-this->height()-2,this->width(),this->height());
    }
    else if (panelPosition == 1) { //任务栏在上
        this->setGeometry(totalWidth-this->width(),qApp->primaryScreen()->geometry().y()+panelHeight+2,this->width(),this->height());
    }
    else if (panelPosition == 2) {//任务栏在左
        this->setGeometry(qApp->primaryScreen()->geometry().x()+panelHeight+2,totalHeight-this->height(),this->width(),this->height());
    }
    else if (panelPosition == 3) { //任务栏在右
        this->setGeometry(totalWidth-panelHeight-this->width()-2,totalHeight-this->height(),this->width(),this->height());
    }
    this->show();
}

/*
    获取任务栏高度
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

/*
    更新声音托盘图标
*/
void DeviceSwitchWidget::updateSystemTrayIcon(int volume,bool isMute)
{
    QString systemTrayIcon;
    QIcon icon;
    if (isMute) {
        systemTrayIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(systemTrayIcon);
        m_pMuteAction->setIcon(QIcon("/usr/share/ukui-media/img/tick.svg"));
        soundSystemTrayIcon->setIcon(icon);
        miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (volume <= 0) {
        systemTrayIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(systemTrayIcon);
        m_pMuteAction->setIcon(QIcon(""));
        soundSystemTrayIcon->setIcon(icon);
        miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
        devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-muted.svg"));
    }
    else if (volume > 0 && volume <= 33) {
        systemTrayIcon = "audio-volume-low";
        m_pMuteAction->setIcon(QIcon(""));
        icon = QIcon::fromTheme(systemTrayIcon);
        soundSystemTrayIcon->setIcon(icon);
        miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
        devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-low.svg"));
    }
    else if (volume >33 && volume <= 66) {
        systemTrayIcon = "audio-volume-medium";
        m_pMuteAction->setIcon(QIcon(""));
        icon = QIcon::fromTheme(systemTrayIcon);
        soundSystemTrayIcon->setIcon(icon);
        miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
        devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-medium.svg"));
    }
    else {
        systemTrayIcon = "audio-volume-high";
        m_pMuteAction->setIcon(QIcon(""));
        icon = QIcon::fromTheme(systemTrayIcon);
        soundSystemTrayIcon->setIcon(icon);
        miniWidget->muteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        appWidget->systemVolumeBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
        devWidget->outputMuteBtn->setIcon(QIcon("/usr/share/ukui-media/img/audio-volume-high.svg"));
    }

    //设置声音菜单栏静音选项的勾选状态
    if (isMute) {
        m_pMuteAction->setIcon(QIcon("/usr/share/ukui-media/img/tick.svg"));
    }
    else {
        m_pMuteAction->setIcon(QIcon(""));
    }
}

/*
    更新默认的输入stream
*/
gboolean DeviceSwitchWidget::update_default_input_stream (DeviceSwitchWidget *w)
{
    MateMixerStream *stream;
    stream = mate_mixer_context_get_default_input_stream (w->context);

    if (w->input != nullptr) {
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

/*
    输入stream control add
*/
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
    update_icon_input (w,w->context);
}

/*
    输入stream control removed
*/
void DeviceSwitchWidget::on_input_stream_control_removed (MateMixerStream *stream,const gchar *name,DeviceSwitchWidget *w)
{
    Q_UNUSED(stream);
    Q_UNUSED(name);
    /* The removed stream could be an application input, which may cause
     * the input status icon to disappear */
    update_icon_input (w,w->context);
}

/*
    更新输入设置w
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

DeviceSwitchWidget::~DeviceSwitchWidget()
{

}
