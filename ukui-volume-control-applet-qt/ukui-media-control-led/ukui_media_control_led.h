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
#ifndef MATE_MEDIA_CONTROL_LED_H
#define MATE_MEDIA_CONTROL_LED_H

#include <QFileSystemWatcher>
#include <QFile>
#include <QDebug>

class UkuiMediaControlLed : public QObject
{
    Q_OBJECT
public:
    UkuiMediaControlLed();
    ~UkuiMediaControlLed();

//    void SaveCameraConfig(QStringList strliConfig);

private slots:
    void on_outputFileChanged(const QString &path);
    void on_inputFileChanged(const QString &path);
//    void on_directoryChanged(const QString &path);

//    void on_pushButton_clicked();

private:
//    Ui::MainWindow *ui;

    QFileSystemWatcher *m_outputFileWatcher;
    QFileSystemWatcher *m_inputFileWatcher;
};

#endif // MATE_MEDIA_CONTROL_LED_H
