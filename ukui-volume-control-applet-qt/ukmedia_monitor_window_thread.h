#ifndef UKMEDIAMONITORWINDOWTHREAD_H
#define UKMEDIAMONITORWINDOWTHREAD_H

#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <X11/Xlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <X11/extensions/shape.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <X11/Xatom.h>

class UkmediaMonitorWindowThread : public QThread
{
public:
    UkmediaMonitorWindowThread();
    void get_window_nameAndid();
    //结束线程标志位
    bool bStopThread;

    void run();
};

#endif // UKMEDIAMONITORWINDOWTHREAD_H
