#include "ukui_media_control_led.h"
//#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    system("touch /tmp/kylin_output_muted.log /tmp/kylin_input_muted.log");
    system("chmod 777 /tmp/kylin_output_muted.log /tmp/kylin_input_muted.log");
    UkuiMediaControlLed control;

    return a.exec();
}
