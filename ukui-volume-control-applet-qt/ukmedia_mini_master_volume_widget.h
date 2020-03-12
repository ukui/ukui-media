#ifndef UKMEDIAMINIMASTERVOLUMEWIDGET_H
#define UKMEDIAMINIMASTERVOLUMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include "ukmedia_volume_slider.h"
#include "customstyle.h"

class UkmediaMiniMasterVolumeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaMiniMasterVolumeWidget(QWidget *parent = nullptr);
    ~UkmediaMiniMasterVolumeWidget();
    friend class DeviceSwitchWidget;
Q_SIGNALS:
    void mouse_wheel_signal(bool step);

private Q_SLOTS:
    void moveMiniSwitchBtnSlot();
private:
    QWidget *masterWidget;
    UkmediaVolumeSlider *masterVolumeSlider;
    QLabel *displayVolumeLabel;
//    QLabel *deviceLabel;
    QPushButton *muteBtn;
    UkuiMediaButton *switchBtn;

    QPushButton *deviceBtn;
    QLabel *deviceLabel;
    QComboBox *deviceCombox;
protected:
    void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // UKMEDIAMINIMASTERVOLUMEWIDGET_H
