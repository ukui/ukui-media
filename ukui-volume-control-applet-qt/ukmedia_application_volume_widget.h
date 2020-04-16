
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
#ifndef APPLICATIONVOLUMEWIDGET_H
#define APPLICATIONVOLUMEWIDGET_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include "ukmedia_volume_slider.h"

class ApplicationVolumeWidget : public QWidget
{
    Q_OBJECT
public:
    ApplicationVolumeWidget(QWidget *parent = nullptr);
    ~ApplicationVolumeWidget();
    friend class DeviceSwitchWidget;
private:
    QLabel *applicationLabel;
    QLabel *systemVolumeLabel;
    QPushButton *systemVolumeBtn;
    UkmediaVolumeSlider *systemVolumeSlider;
    QLabel *systemVolumeDisplayLabel;

    QWidget *upWidget;
    QWidget *systemVolumeWidget;
    QWidget *systemVolumeSliderWidget;
    UkuiApplicationWidget *displayAppVolumeWidget;
    QStringList *app_volume_list;
    QLabel *appLabel;
    QPushButton *appMuteBtn;
    QPushButton *appIconBtn;
    QSlider *appSlider;
    QScrollArea *appArea;
    QVBoxLayout *m_pVlayout;
protected:


};

#endif // APPLICATIONVOLUMEWIDGET_H
