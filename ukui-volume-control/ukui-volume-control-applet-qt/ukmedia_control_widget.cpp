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
#include <QBoxLayout>
#include <QMainWindow>
#include <QWindow>
#include <QSlider>
#include <QMouseEvent>
#include <QDebug>
#include <QProcess>
#include <QString>
#include <QSpacerItem>
#include <string>
extern "C" {
#include <gdk/gdk.h>
#include <glib-object.h>
#include <glib/gi18n.h>
}
#include "ukmedia_control_widget.h"

gboolean isMute;
gboolean ipIsMute;
SystemTrayIconType trayIconType = SYSTEMTRAYICON_UNKNOW;

UkmediaControlWidget::UkmediaControlWidget(QWidget *parent) : QWidget (parent)
{
    outputStream = new MateMixerStream;
    inputStream = new MateMixerStream;

    mateMixerInit();
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);//|Qt::Popup);
    this->setFixedSize(300,56);
    setWindowOpacity(0.95);
}

/*
    mate初始化设置
*/
void UkmediaControlWidget::mateMixerInit()
{
//    GSettings *settings;
//    settings = g_settings_new(KEY_SOUNDS_SCHEMA);
//    Q_UNUSED(settings);

    //主题图标

    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }
    ukuiContext = mate_mixer_context_new();
    mate_mixer_context_set_backend_type(ukuiContext,MATE_MIXER_BACKEND_ALSA);
    mate_mixer_context_set_app_name (ukuiContext,_("Ukui Volume Control App"));//设置app名
    mate_mixer_context_set_app_id(ukuiContext, GVC_APPLET_DBUS_NAME);
    mate_mixer_context_set_app_version(ukuiContext,VERSION);
    mate_mixer_context_set_app_icon(ukuiContext,"ukuimedia-volume-control");

    if G_UNLIKELY (mate_mixer_context_open(ukuiContext) == FALSE) {
        g_warning ("Failed to connect to a sound system**********************");
    }

    g_signal_connect (G_OBJECT (ukuiContext),
                           "stream-added",
                           G_CALLBACK (on_context_stream_added),
                           nullptr);

    g_signal_connect (G_OBJECT (ukuiContext),
                              "device-added",
                              G_CALLBACK (on_context_device_added),
                              nullptr);
//    mate_mixer_switch_get_active_option(sw);
}

/*
    设置麦克风值及托盘图标
*/
void UkmediaControlWidget::setIpSystemTrayIconVolume()
{
    ipIsMute = getIpMuteStatus();
    int volume = getIpVolume();

    volume = int(volume*100/65536.0+0.5);
    if(!ipIsMute) {
        m_volumeSlider->setValue(volume);
    }
    else {
        muteWidget(volume,ipIsMute);
    }
}

/*
    设置声音值及托盘图标
*/
void UkmediaControlWidget::setOpSystemTrayIconVolume()
{
    isMute = getOpMuteStatus();
    int volume = getOpVolume();
    volume = int(volume*100/65536.0+0.5);
    if (!isMute) {
        m_volumeSlider->setValue(volume);
    }
    else {
        muteWidget(volume,isMute);
    }
}

/*
    弹出窗口静音设置
*/
void UkmediaControlWidget::muteWidget(int volume,bool status)
{
    QString muteButtonIcon;
    QIcon icon;
    switch (trayIconType) {
    case SYSTEMTRAYICON_MICROPHONE: {
        m_volumeSlider->setValue(volume);
        m_displayVolumeValue->setNum(volume);
        mate_mixer_stream_control_set_mute(inputControl,status);
        muteButtonIcon = "audio-input-microphone-muted";
        icon = QIcon::fromTheme(muteButtonIcon);
        m_muteButton->setIcon(icon);
        break;
    }
    case SYSTEMTRAYICON_VOLUME: {
        m_volumeSlider->setValue(volume);
        m_displayVolumeValue->setNum(volume);
        mate_mixer_stream_control_set_mute(outputControl,status);
        muteButtonIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(muteButtonIcon);
        m_muteButton->setIcon(icon);
        break;
    }
    default:
        break;
    }
}

/*
    获取默认输入流及设置图标
*/
void UkmediaControlWidget::getDefaultInputStream()
{
    int volumeRead;
    int voiceState;
    int volume;

    volumeRead = mate_mixer_stream_control_get_volume(inputControl);//获取音量值
    ipIsMute = mate_mixer_stream_control_get_mute(inputControl);
    voiceState = mate_mixer_stream_control_get_mute(inputControl);//获取声音的状态
    //初始化label的值

    if (!voiceState) {
        volume = volumeRead * 100.0 / 65536.0 + 0.5;
        m_displayVolumeValue->setNum(volume);
        m_volumeSlider->setValue(volume);
    }
    else {
        if (int value = mate_mixer_stream_control_get_volume(inputControl)) {
            value = value*100/65536.0+0.5;
            m_displayVolumeValue->setNum(value);
        }
        else {
            m_displayVolumeValue->setNum(0);
        }

    }
    volume = m_volumeSlider->value();
    setIpMuteButtonIcon(volume);
}

/*
    麦克风声音通知更改麦克风的值
*/
void UkmediaControlWidget::inputVolumeNotify()
{
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(inputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
        g_signal_connect (inputControl,"notify::volume",G_CALLBACK (inputControlVolumeNotify),this);
    connect(this,SIGNAL(micMixerMuteSignal(bool,guint)),this,SLOT(acceptIpMixerMute(bool,guint)));
}

/*
    麦克风值改变
*/
void UkmediaControlWidget::inputVolumeChanged()
{
    connect(this,&UkmediaControlWidget::emitVolume,this,&UkmediaControlWidget::acceptIpVolume);
}

/*
    获取默认输出流
*/
void UkmediaControlWidget::getDefaultOutputStream()
{
    int volumeRead;
    int volume;
    int voiceState;

    volumeRead = mate_mixer_stream_control_get_volume(outputControl);//获取音量值
    isMute = mate_mixer_stream_control_get_mute(outputControl);
    voiceState = mate_mixer_stream_control_get_mute(outputControl);//获取声音的状态
    //初始化label的值
    if (!voiceState) {
        volume = volumeRead * 100.0 / 65536.0 + 0.5;
        m_displayVolumeValue->setNum(volume);
        m_volumeSlider->setValue(volume);
    }
    else {
        if (int value = mate_mixer_stream_control_get_volume(outputControl)) {
            value = value*100/65536.0+0.5;
            m_displayVolumeValue->setNum(value);
        }
        else {
            m_displayVolumeValue->setNum(0);
        }
    }
    volume = m_volumeSlider->value();
    setOpMuteButtonIcon(volume);
}

/*
    声音通知
*/
void UkmediaControlWidget::outputVolumeNotify()
{
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(outputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE)
        g_signal_connect(outputControl,"notify::volume",G_CALLBACK (outputControlVolumeNotify),this);
    connect(this,SIGNAL(soundMixerMuteSignal(bool,guint)),this,SLOT(acceptOpMixerMute(bool,guint)));
}

/*
    输出声音值改变
*/
void UkmediaControlWidget::outputVolumeChanged()
{
    connect(this,&UkmediaControlWidget::emitVolume,this,&UkmediaControlWidget::acceptOpVolume);
}

/*
    弹出窗口初始化
*/
void UkmediaControlWidget::dockWidgetInit()
{
    const QSize iconSize = QSize(24,24);
    m_displayVolumeValue = new QLabel(this);
    m_muteButton = new QPushButton(this);
    m_volumeSlider = new UkmediaSlider(this);
    m_displayVolumeValue->setFrameStyle(QFrame::NoFrame);
    m_muteButton->setFixedSize(24,24);
//    m_muteButton->setFlat(true);
    m_muteButton->setFocusPolicy(Qt::NoFocus);
    m_muteButton->setIconSize(iconSize);
    m_muteButton->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");
    m_displayVolumeValue->setObjectName("displayVolumeLabel");


    m_volumeSlider->setMaximum(100);
    m_volumeSlider->setOrientation(Qt::Horizontal);
    m_volumeSlider->setFixedSize(178,20);
    m_volumeSlider->setSingleStep(10);

    //弹出框的控件布局
    QHBoxLayout *hLayout;
    hLayout = new QHBoxLayout();

    this->setFixedWidth(300);
    hLayout->addWidget(m_muteButton);
    hLayout->addWidget(m_volumeSlider);
    hLayout->setSpacing(10);
    hLayout->addWidget(m_displayVolumeValue);
//    this->setContentsMargins(15,10,10,15);
    outputStream = mate_mixer_context_get_default_output_stream(this->ukuiContext);
    outputControl = mate_mixer_stream_get_default_control(outputStream);

    inputStream = mate_mixer_context_get_default_input_stream(ukuiContext);
    inputControl = mate_mixer_stream_get_default_control(inputStream);

    this->setLayout(hLayout);
    hLayout->setAlignment(this,Qt::AlignCenter);

    hLayout->setContentsMargins(20,12,20,12);

    //当滑动条条发生改变时改变音量
    connect(m_volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(volumeSliderChanged(int)));
    //静音按钮设置静音
    connect(m_muteButton,SIGNAL(clicked()),this,SLOT(muteButtonClicked()));
}

/*
    ukui-media静音功能
*/
void UkmediaControlWidget:: mute()
{
    QString muteButtonIcon;
    QIcon icon;
    switch (trayIconType) {
    case SYSTEMTRAYICON_MICROPHONE: {
        ipIsMute = mate_mixer_stream_control_get_mute(inputControl);
        int ipVolume = mate_mixer_stream_control_get_volume(inputControl);
        ipVolume = ipVolume*100/65536.0 + 0.5;
        if (ipIsMute) {
            m_displayVolumeValue->setNum(ipVolume);
            mate_mixer_stream_control_set_mute(inputControl,FALSE);
            setIpMuteButtonIcon(ipVolume);
        }
        else {
            mate_mixer_stream_control_set_mute(inputControl,TRUE);
            muteButtonIcon = "audio-input-microphone-muted";
            icon = QIcon::fromTheme(muteButtonIcon);
            m_muteButton->setIcon(icon);
        }
        break;
    }
    case SYSTEMTRAYICON_VOLUME: {
        isMute = mate_mixer_stream_control_get_mute(outputControl);
        int opVolume = mate_mixer_stream_control_get_volume(outputControl);
        opVolume = opVolume*100/65536.0 + 0.5;
        if (isMute) {
            m_displayVolumeValue->setNum(opVolume);
            mate_mixer_stream_control_set_mute(outputControl,FALSE);
            setOpMuteButtonIcon(opVolume);
        }
        else {
            mate_mixer_stream_control_set_mute(outputControl,TRUE);
            muteButtonIcon = "audio-volume-muted";
            icon = QIcon::fromTheme(muteButtonIcon);
            m_muteButton->setIcon(icon);
        }
        break;
    }
    default:
        break;
    }
}

/*
    麦克风声音通知
*/
void UkmediaControlWidget::inputControlVolumeNotify(MateMixerStreamControl *inputControl, GParamSpec *pspec, UkmediaControlWidget *p)
{
    Q_UNUSED(pspec);
    trayIconType = SYSTEMTRAYICON_MICROPHONE;
    guint value = mate_mixer_stream_control_get_volume(inputControl);
    guint volume = guint(value*100/65536.0+0.5);
    bool isMute = mate_mixer_stream_control_get_mute(inputControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(inputControl);
    if (control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE ) {
        Q_EMIT p->micMixerMuteSignal(isMute,volume);
    }
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
     Q_EMIT p->emitVolume(volume);
    }
}

/*
    输出声音通知
*/
void UkmediaControlWidget::outputControlVolumeNotify(MateMixerStreamControl *outputControl, GParamSpec *pspec, UkmediaControlWidget *p)
{
    Q_UNUSED(pspec);
    trayIconType = SYSTEMTRAYICON_VOLUME;
    guint value = mate_mixer_stream_control_get_volume(outputControl);
    guint volume ;
    volume = guint(value*100/65536.0+0.5);
    bool isMute = mate_mixer_stream_control_get_mute(outputControl);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(outputControl);
    //静音可读并且处于静音
    if ((control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE) ) {
        Q_EMIT p->soundMixerMuteSignal(isMute,volume);
    }
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
        //设置滑动条的值
        Q_EMIT p->emitVolume(volume);
    }
}

/*
    接收麦克风静音
*/
void UkmediaControlWidget::acceptIpMixerMute(bool status,guint volume)
{
    if (status) {
        trayIconType = SYSTEMTRAYICON_MICROPHONE;
        mate_mixer_stream_control_set_mute(inputControl,status);
        muteWidget(int(volume),status);
    }
    else {
        mate_mixer_stream_control_set_mute(inputControl,status);
        setIpMuteButtonIcon(volume);
    }
    Q_EMIT updateSystemTrayIconSignal(volume,trayIconType,status);
}

/*
    接收输出静音
*/
void UkmediaControlWidget::acceptOpMixerMute(bool status,guint volume)
{
    if (status) {
        trayIconType = SYSTEMTRAYICON_VOLUME;
        mate_mixer_stream_control_set_mute(outputControl,status);
        muteWidget(int(volume),status);
    }
    else {
        mate_mixer_stream_control_set_mute(outputControl,status);
        setOpMuteButtonIcon(volume);
    }
    Q_EMIT updateSystemTrayIconSignal(volume,trayIconType,status);
}

/*
    设置输入滑动条的值
*/
void UkmediaControlWidget::acceptIpVolume(int volume)
{
    m_volumeSlider->setValue(volume);
}

/*
    设置输出滑动条的值
*/
void UkmediaControlWidget::acceptOpVolume(int volume)
{
    m_volumeSlider->setValue(volume);
}

/*
    获取麦克风声音的值
*/
int UkmediaControlWidget::getIpVolume()
{
    return mate_mixer_stream_control_get_volume(inputControl);
}

/*
    获取输出声音的值
*/
int UkmediaControlWidget::getOpVolume()
{
    return mate_mixer_stream_control_get_volume(outputControl);
}

/*
    设置麦克风声音
*/
void UkmediaControlWidget::setIpVolume(int volume)
{
    if (volume > 0)
        mate_mixer_stream_control_set_mute(inputControl,false);
    mate_mixer_stream_control_set_volume(inputControl,volume);
}

/*
    设置输出声音
*/
void UkmediaControlWidget::setOpVolume(int volume)
{
    if (volume > 0)
        mate_mixer_stream_control_set_mute(outputControl,false);
    mate_mixer_stream_control_set_volume(outputControl,volume);
}

/*
    获取麦克风静音状态
*/
bool UkmediaControlWidget::getIpMuteStatus()
{
    return mate_mixer_stream_control_get_mute(inputControl);
}

/*
    获取输出声音静音状态
*/
bool UkmediaControlWidget::getOpMuteStatus()
{
    return mate_mixer_stream_control_get_mute(outputControl);
}

/*
    移动滑动条改变输入输出声音的值
*/
void UkmediaControlWidget::volumeSliderChanged(int volume)
{
    switch (trayIconType) {
    //麦克风托盘图标
    case SYSTEMTRAYICON_MICROPHONE: {
        if (volume > 0) {
            mate_mixer_stream_control_set_mute(inputControl,FALSE);
            m_displayVolumeValue->setNum(volume);
            mate_mixer_stream_control_set_volume(inputControl,volume*65536/100);
        }
        else if (volume <= 0) {
            mate_mixer_stream_control_set_mute(inputControl,TRUE);
            mate_mixer_stream_control_set_volume(inputControl,0);
            m_displayVolumeValue->setNum(0);
        }
        Q_EMIT sliderSystemTrayIcon(trayIconType);
        setIpMuteButtonIcon(volume);
        break;
    }
    //输出声音托盘图标
    case SYSTEMTRAYICON_VOLUME: {
        if (volume > 0) {
            mate_mixer_stream_control_set_mute(outputControl,FALSE);
            m_displayVolumeValue->setNum(volume);
            mate_mixer_stream_control_set_volume(outputControl,volume*65536/100);
        }
        else if (volume <= 0) {
            mate_mixer_stream_control_set_mute(outputControl,TRUE);
            mate_mixer_stream_control_set_volume(outputControl,0);
            m_displayVolumeValue->setNum(0);
        }
        Q_EMIT sliderSystemTrayIcon(trayIconType);
        setOpMuteButtonIcon(volume);
        break;
    }
    default:
        break;
    }
}

/*
    点击窗口以外的部分隐藏窗口
*/
//bool UkmediaControlWidget::event(QEvent *event)
//{
//    if (event->type() == QEvent::ActivationChange) {
//        if(QApplication::activeWindow() != this) {
//            hide();
//        }
//    }
//    return QWidget::event(event);
//}

/*
    点击时设置托盘图标类型为麦克风
*/
void UkmediaControlWidget::acceptIpSystemTrayIconTriggered(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    滚动时设置托盘图标类型为麦克风
*/
void UkmediaControlWidget::acceptIpSystemTrayIconRoll(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    滚动时设置托盘图标类型为声音
*/
void UkmediaControlWidget::acceptOpSystemTrayIconRoll(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    点击时设置托盘图标类型为声音
*/
void UkmediaControlWidget::acceptOpSystemTrayIconTriggered(SystemTrayIconType type)
{
    trayIconType = type;
}

/*
    点击静音按钮
*/
void UkmediaControlWidget::muteButtonClicked()
{
    switch (trayIconType) {
    case SYSTEMTRAYICON_UNKNOW:
        break;
    case SYSTEMTRAYICON_MICROPHONE : {
        mute();
    break;
    }
    case SYSTEMTRAYICON_VOLUME:
        mute();
        break;
    }
}

/*
    设置麦克风静音按钮图标
*/
void UkmediaControlWidget::setIpMuteButtonIcon(int volume)
{
    QString inputMuteButtonIcon;
    QIcon icon;
    if (volume <= 0) {
        inputMuteButtonIcon = "audio-input-microphone-muted";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 0 && volume <= 33) {
        inputMuteButtonIcon = "audio-input-microphone-low";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 33 && volume <= 66) {
        inputMuteButtonIcon = "audio-input-microphone-medium";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else {
        inputMuteButtonIcon = "audio-input-microphone-high";
        icon = QIcon::fromTheme(inputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
}

/*
    设置输出声音静音按钮图标
*/
void UkmediaControlWidget::setOpMuteButtonIcon(int volume)
{
    QString outputMuteButtonIcon;
    QIcon icon;
    if (volume <= 0) {
        outputMuteButtonIcon = "audio-volume-muted";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 0 && volume <= 33) {
        outputMuteButtonIcon = "audio-volume-low";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else if (volume > 33 && volume <= 66) {
        outputMuteButtonIcon = "audio-volume-medium";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
    else {
        outputMuteButtonIcon = "audio-volume-high";
        icon = QIcon::fromTheme(outputMuteButtonIcon);
        m_muteButton->setIcon(icon);
    }
}

void UkmediaControlWidget::on_context_stream_added(MateMixerContext *context,
                                    const gchar      *name,
                                    UkmediaControlWidget *w)
{
    MateMixerStream   *stream;
    MateMixerDirection direction;

    stream = mate_mixer_context_get_stream(context,"hw:1");
    qDebug() << "获取stream的名为:" << mate_mixer_stream_get_name(stream);
    mate_mixer_context_set_default_input_stream(context,stream);

    stream = mate_mixer_context_get_stream (context, name); //获取流
    if (G_UNLIKELY (stream == NULL))
            return;

    direction = mate_mixer_stream_get_direction (stream);

    /* If the newly added stream belongs to the currently selected device and               如果新添加的流属于当前选定的设备并且隐藏了测试按钮，
     * the test button is hidden, this stream may be the one to allow the                           则此流可能是允许进行声音测试的流，因此我们可能需要启用该按钮
     * sound test and therefore we may need to enable the button */
//    if (dialog->priv->hw_profile_combo != NULL && direction == MATE_MIXER_DIRECTION_OUTPUT) {
//            MateMixerDevice *device1;
//            MateMixerDevice *device2;

//            device1 = mate_mixer_stream_get_device (stream);
//            device2 = g_object_get_data (G_OBJECT (dialog->priv->hw_profile_combo),
//                                         "device");// 从关联的对象表中获取命名字段

//            if (device1 == device2) {
//                    gboolean show_button;

//                    g_object_get (G_OBJECT (dialog->priv->hw_profile_combo),
//                                  "show-button", &show_button,
//                                  NULL);    //获取dialog->priv->hw_profile_combo属性

//                    if (show_button == FALSE)
//                            update_device_test_visibility (dialog);
//            }
//    }

//    bar = g_hash_table_lookup (dialog->priv->bars, name);//在dialog->priv->bars查找name
//    if (G_UNLIKELY (bar != NULL))
//            return;

//    add_stream (dialog, stream);
}

void UkmediaControlWidget::on_context_device_added (MateMixerContext *context, const gchar *name, UkmediaControlWidget *dialog)
{
        MateMixerDevice *device;

        device = mate_mixer_context_get_device (context, name);
        if (G_UNLIKELY (device == NULL))
                return;

        add_device (dialog, device);
}

void UkmediaControlWidget::add_device(UkmediaControlWidget *dialog, MateMixerDevice *device)
{
        GtkTreeModel    *model;
        GtkTreeIter      iter;
        GIcon           *icon;
        const gchar     *name;
        const gchar     *label;
        gchar           *status;
        const gchar     *profile_label = NULL;
        MateMixerSwitch *profile_switch;

//        model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->hw_treeview));

        name  = mate_mixer_device_get_name (device);
        label = mate_mixer_device_get_label (device);

//        if (find_tree_item_by_name (GTK_TREE_MODEL (model),
//                                    name,
//                                    HW_NAME_COLUMN,
//                                    &iter) == FALSE) {
//                                             gtk_list_store_append (GTK_LIST_STORE (model), &iter);
//                                    }


        icon = g_themed_icon_new_with_default_fallbacks (mate_mixer_device_get_icon (device));

        profile_switch = find_device_profile_switch (device);
        if (profile_switch != NULL) {
                MateMixerSwitchOption *active;

                active = mate_mixer_switch_get_active_option (profile_switch);
                if (G_LIKELY (active != NULL))
                        profile_label = mate_mixer_switch_option_get_label (active);

                g_signal_connect (G_OBJECT (profile_switch),
                                  "notify::active-option",
                                  G_CALLBACK (on_device_profile_active_option_notify),
                                  nullptr);
        }

//        status = device_status (device);

//        gtk_list_store_set (GTK_LIST_STORE (model),
//                            &iter,
//                            HW_NAME_COLUMN, name,
//                            HW_LABEL_COLUMN, label,
//                            HW_ICON_COLUMN, icon,
//                            HW_PROFILE_COLUMN, profile_label,
//                            HW_STATUS_COLUMN, status,
//                            -1);
        g_free (status);

}

void UkmediaControlWidget::on_device_profile_active_option_notify (MateMixerDeviceSwitch *swtch,GParamSpec *pspec,UkmediaControlWidget *dialog)
{
        MateMixerDevice *device;
        device = mate_mixer_device_switch_get_device (swtch);

        qDebug() << "switch设备名为:" << mate_mixer_device_get_name(device);
//        update_device_info (dialog, device);
}

MateMixerSwitch *UkmediaControlWidget::find_device_profile_switch (MateMixerDevice *device)
{
        const GList *switches;

        switches = mate_mixer_device_list_switches (device);
        while (switches != NULL) {
                MateMixerDeviceSwitch *swtch = MATE_MIXER_DEVICE_SWITCH (switches->data);

                if (mate_mixer_device_switch_get_role (swtch) == MATE_MIXER_DEVICE_SWITCH_ROLE_PROFILE)
                        return MATE_MIXER_SWITCH (swtch);

                switches = switches->next;
        }
        return NULL;
}

UkmediaSlider::UkmediaSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaSlider::~UkmediaSlider()
{

}

UkmediaControlWidget::~UkmediaControlWidget()
{

}
