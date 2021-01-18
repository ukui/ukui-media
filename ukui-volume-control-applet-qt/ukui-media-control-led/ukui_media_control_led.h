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
