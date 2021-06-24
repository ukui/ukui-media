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
#include <ukui-log4qt.h>

int main(int argc, char *argv[])
{
    initUkuiLog4qt("ukui-media");
    Display *display = XOpenDisplay(NULL);
    Screen *scrn = DefaultScreenOfDisplay(display);
    if(scrn == nullptr) {
        return 0;
    }
    int width = scrn->width;
    if (width >= 2560) {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    #endif
    }
    QString id = QString("QtSingleApplication-Name"+QLatin1String(getenv("DISPLAY")));
    QtSingleApplication app(id,argc,argv);
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

    //加载qss文件
    QFile qss(":/data/qss/ukuimedia.qss");
    qss.open(QFile::ReadOnly);
    qss.close();
    DeviceSwitchWidget w;
    KWindowEffects::enableBlurBehind(w.winId(),true);
    w.raise();
    XCloseDisplay(display);
    return app.exec();
}

