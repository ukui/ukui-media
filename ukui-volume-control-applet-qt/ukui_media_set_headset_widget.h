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
