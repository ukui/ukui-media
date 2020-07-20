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
#include <QApplication>
#include <QDebug>
#include <QShortcut>
#include <QTranslator>
#include <QtSingleApplication>
#include <QFile>
#include "ukmedia_device_switch_widget.h"
#include <QObject>
#include <QDateTime>
#include <QMutex>
#include <QFile>
#include <QDesktopWidget>
#include <QDir>

/*! The ukui-media is the media of UKUI.

  Usage: ukui-media [CONFIG_ID]

    CONFIG_ID      Section qInstallMessageHandlername in config file ~/.config/ukui/ukui-media.conf

                   (default main)

 */
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    //调试信息提示
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
        //一般的warning提示
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
        //严重错误提示
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
        //致命错误提示
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    QFile outFile(qgetenv("HOME") +"/.config/ukui/ukui-media.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);  //
    ts << txt << endl;

}

int main(int argc, char *argv[])
{
//    QApplication app(argc,argv);
    QtSingleApplication app("ukui-volume-control-applet",argc,argv);
    if (app.isRunning()) {
       app.sendMessage("raise_window_noop");
       return EXIT_SUCCESS;
    }
    if (QApplication::desktop()->width() >= 2560) {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
            QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
            QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif
    }
    //加载qm翻译文件o
    QString locale = QLocale::system().name();
    QTranslator translator;

    if (locale == "zh_CN") {
        if (translator.load("/usr/share/ukui-media/translations/ukui-volume-control-applet-qt-zh_CN.qm")) {
            app.installTranslator(&translator);
        }
        else {
            qDebug() << "Load translations file" << locale << "failed!";
        }
    }
    //4k屏自动放大字体
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //注册MessageHandler
//    qInstallMessageHandler(outputMessage);
    //加载qss文件
    QFile qss(":/data/qss/ukuimedia.qss");
    qss.open(QFile::ReadOnly);

    qApp->setStyleSheet(qss.readAll());
    qss.close();
    DeviceSwitchWidget w;
//    w.show();

    return app.exec();
}

