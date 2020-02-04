#ifndef APPLICATIONVOLUMEWIDGET_H
#define APPLICATIONVOLUMEWIDGET_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
class ApplicationVolumeWidget : public QWidget
{
    Q_OBJECT
public:
    ApplicationVolumeWidget(QWidget *parent = nullptr);
    ~ApplicationVolumeWidget();
    friend class DeviceSwitchWidget;
private:
    QLabel *applicationLabel;
    QLabel *noAppLabel;

    QStringList *app_volume_list;
    QLabel *appLabel;
    QLabel *appIconLabel;
    QPushButton *appIconBtn;
    QLabel *appVolumeLabel;
    QSlider *appSlider;
};

#endif // APPLICATIONVOLUMEWIDGET_H
