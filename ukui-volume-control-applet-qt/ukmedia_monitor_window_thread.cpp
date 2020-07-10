#include "ukmedia_monitor_window_thread.h"
//#include <QDebug>
UkmediaMonitorWindowThread::UkmediaMonitorWindowThread()
{
    bStopThread = false;
}

void UkmediaMonitorWindowThread::get_window_nameAndid()
{
    Display *display;
    Window rootwin;
    display = XOpenDisplay( NULL );
    rootwin = DefaultRootWindow( display );
    XSelectInput( display, rootwin, SubstructureNotifyMask);/*事件可以参考x.h*/
    XEvent event;
//    while ( 1 )
//    {
    printf("get event\n");
    fflush(stdout);
        XNextEvent(display, &event );
        printf("event type is %d\n",event.type);
        fflush(stdout);
        if ( event.type == /*ReparentNotify*/CreateNotify )
        {
            Window x11window;
            Display *x11display;
            char **srname = (char **)malloc(sizeof(char *));
            XReparentEvent *reparentevent = (XReparentEvent *)&event;

            printf( "new window: %ld \n", (unsigned long)(reparentevent->window));
            printf( "new parent: %ld \n", (unsigned long)(reparentevent->parent));
            fflush(stdout);
            /*获取到新建窗口的window ID*/
            x11window = (unsigned long)(reparentevent->window);
            x11display =(reparentevent->display);
            XFetchName(x11display, x11window, srname);
            x11display = XOpenDisplay( NULL );/*！！！注意这里以后如果你想对获取到的window ID进行操作必须重新链接*/

            if(*srname != NULL)
                printf("new window name: %s\n" ,*srname);

            free(*srname);
        }
        else if (event.type == /*DestroyNotify*/UnmapNotify) {
            printf("close window\n\n");
            fflush(stdout);

        }
//    }
}

//线程执行函数
void UkmediaMonitorWindowThread::run()
{
//    qDebug()<<"thread run!";
    bStopThread = false;
    while(!bStopThread)
    {
        printf("thread run! \n\n");
        fflush(stdout);
        //to do what you want
        get_window_nameAndid();
        //延时等待一会
        QEventLoop eventloop;
        QTimer::singleShot(1000, &eventloop, SLOT(quit()));
        eventloop.exec();
    }
}
