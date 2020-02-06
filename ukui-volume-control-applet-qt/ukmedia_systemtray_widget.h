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
#ifndef UKMEDIA_SYSTEMTRAY_WIDGET_H
#define UKMEDIA_SYSTEMTRAY_WIDGET_H

#include <QMenu>
#include <QWidget>
#include <QAction>
#include <QMainWindow>
#include <QPushButton>
#include <QWheelEvent>
#include <QSystemTrayIcon>
#include <QWidgetAction>
#include <QCheckBox>
#include "ukmedia_control_widget.h"
//#include "ukmediakeybordcontrolwidget.h"

#define TRANSLATIONS_DIR "/home/fzx/fzx/ukui-media/ukui-media/po"

class UkmediaSystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    UkmediaSystemTrayIcon(QWidget *parent = nullptr);
    ~UkmediaSystemTrayIcon();

Q_SIGNALS:
    void wheelRollEventSignal(bool);

protected:
    bool event(QEvent *e) ;
};

class UkmediaSystemTrayWidget : public QMainWindow
{
    Q_OBJECT
public:
     bool voiceOnOrOff = 1;
    UkmediaSystemTrayWidget(QWidget *parent = nullptr);
    ~UkmediaSystemTrayWidget();
    void showWindow();
    void hideWindow();
    void outputSystemTrayIconInit(int volume,bool status);
    void inputSystemTrayIconInit(int volume,bool status);
    void outputSystemTrayMenuInit();
    void inputSystemTrayMenuInit();

    void ipSystemTrayControl();
    void opSystemTrayControl();

    void updateOutputSystemTrayIcon(int volume,bool status);
    void updateInputSystemTrayIcon(int volume,bool status);

    void init_widget_action(QWidget* wid, QString iconstr, QString textstr);
    friend class UkmediaSystemTrayIcon;

Q_SIGNALS:
    void customContextMenuRequested(const QPoint&);
    void triggeredSystemIcon(SystemTrayIconType type);

    void systemTrayRoll(SystemTrayIconType type);

public Q_SLOTS:
    void activatedOutputSystemTrayIcon(QSystemTrayIcon::ActivationReason);
    void activatedinputSystemTrayIcon(QSystemTrayIcon::ActivationReason);
    void jumpControlPanel();
    void sliderChangeSystemTrayIcon(int volume);
    void buttonChangeSystemTrayIcon();

    void outputActionMuteTriggered(bool);
    void inputActionMuteTriggered(bool);
    void acceptOpWheelRollEvent(bool);
    void acceptIpWheelRollEvent(bool);

    void iconThemeChanged();
    void acceptSliderSystemTrayIcon(SystemTrayIconType type);
    void soundPreferenceChangeSystemTrayIcon(int volume,SystemTrayIconType type,bool status);
    void change();

    void keyControlVolume();
    void opMuteCheckBoxReleasedSlot();
    void ipMuteCheckBoxReleasedSlot();
private:
    QMenu *inputMenu;
    QMenu *outputMenu ;
    QPushButton *btnVoice;
    QWidgetAction *inputActionMute;
    QWidgetAction *outputActionMute;
    QWidgetAction *inputActionSoundPreference;
    QWidgetAction *outputActionSoundPreference;

    UkmediaControlWidget *widget;
    QGSettings *iconThemeSettings;
    UkmediaSystemTrayIcon *inputSystemTray;
    UkmediaSystemTrayIcon *outputSystemTray;

    QWidget *inputActionSoundPreferenceWid;
    QWidget *inputActionMuteWid;
    QWidget *outputActionSoundPreferenceWid;
    QWidget *outputActionMuteWid;
    QCheckBox *ipMuteCheckBox;
    QLabel *ipMuteLabel;
    QCheckBox *opMuteCheckBox;
    QLabel *opMuteLabel;

protected:
    //将窗口设置为随着窗口变化而变化
    bool event(QEvent *event);//重写窗口事件
    void mousePressEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // UKMEDIA_SYSTEMTRAY_WIDGET_H
