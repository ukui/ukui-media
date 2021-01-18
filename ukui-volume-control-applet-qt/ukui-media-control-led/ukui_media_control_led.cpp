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
