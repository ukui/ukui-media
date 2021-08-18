/*
 * Copyright (C) 2021 KylinSoft Co., Ltd.
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
#include "ukui_media_control_led.h"
#include <QFileInfo>
#include <QDebug>

UkuiMediaControlLed::UkuiMediaControlLed()
{
    m_outputFileWatcher = new QFileSystemWatcher();
    m_inputFileWatcher = new QFileSystemWatcher();
    QFileInfo inputInfo("/tmp/kylin_input_muted.log");
    QFileInfo outputInfo("/tmp/kylin_output_muted.log");
    if (outputInfo.exists() == false) {
        return;
    }
    if (inputInfo.exists() == false) {
        return;
    }
    m_inputFileWatcher->addPath("/tmp/kylin_input_muted.log");
    m_outputFileWatcher->addPath("/tmp/kylin_output_muted.log");

    connect(m_inputFileWatcher, &QFileSystemWatcher::fileChanged, this, &UkuiMediaControlLed::on_inputFileChanged);
    connect(m_outputFileWatcher, &QFileSystemWatcher::fileChanged, this, &UkuiMediaControlLed::on_outputFileChanged);
}

void UkuiMediaControlLed::on_outputFileChanged(const QString &path)
{
    QFile file("/tmp/kylin_output_muted.log");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray t = file.readAll();
    const char *mute = t.data();

    if (strstr(mute,"mute")) {
	qDebug() << "输出音量静音";
        system("echo 1 > /sys/class/leds/platform::volmute/brightness");
    }
    else if (strstr(mute,"no")) {
	qDebug() << "输出音量取消静音";
        system("echo 0 > /sys/class/leds/platform::volmute/brightness");
    }

    file.close();

}

void UkuiMediaControlLed::on_inputFileChanged(const QString &path)
{
    QFile file("/tmp/kylin_input_muted.log");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray t = file.readAll();
    const char *mute = t.data();

    if (strstr(mute,"mute")) {
	qDebug() << "输入音量静音";
        system("echo 1 > /sys/class/leds/platform::micmute/brightness");
    }
    else if (strstr(mute,"no")) {
	qDebug() << "输入音量取消静音";
        system("echo 0 > /sys/class/leds/platform::micmute/brightness");
    }

    file.close();

}

UkuiMediaControlLed::~UkuiMediaControlLed()
{

}
