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
#include <KWindowEffects>
#include <QMessageBox>
#include <QLibraryInfo>
#include <X11/Xlib.h>

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
    Display *display = XOpenDisplay(NULL);
    Screen *scrn = DefaultScreenOfDisplay(display);
    if(scrn == nullptr) {
        return 0;
    }
    int width = scrn->width;
//    QApplication app(argc,argv);
    if (width >= 2560) {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif
    }
    QtSingleApplication app("ukui-volume-control-applet",argc,argv);
    if (app.isRunning()) {
       app.sendMessage("raise_window_noop");
       return EXIT_SUCCESS;
    }

#ifndef QT_NO_TRANSLATION
    QString translatorFileName = QLatin1String("qt_");
    translatorFileName += QLocale::system().name();
    QTranslator *pTranslator = new QTranslator();
    if (pTranslator->load(translatorFileName, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(pTranslator);
#endif

    //加载qm文件
    QTranslator translator;
    translator.load("/usr/share/ukui-media/translations/" + QLocale::system().name());
    app.installTranslator(&translator);

    //注册MessageHandler
//    qInstallMessageHandler(outputMessage);
    //加载qss文件
    QFile qss(":/data/qss/ukuimedia.qss");
    qss.open(QFile::ReadOnly);

    qss.close();
    DeviceSwitchWidget w;
    KWindowEffects::enableBlurBehind(w.winId(),true);
    w.raise();
//    w.show();
    XCloseDisplay(display);
    return app.exec();
}

