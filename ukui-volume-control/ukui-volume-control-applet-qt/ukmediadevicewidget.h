#ifndef UKMEDIADEVICEWIDGET_H
#define UKMEDIADEVICEWIDGET_H
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QEvent>
#include <QMouseEvent>
#include <QCoreApplication>

class UkmediaDeviceSlider : public QSlider
{
    Q_OBJECT
public:
    UkmediaDeviceSlider(QWidget *parent = nullptr);
    ~UkmediaDeviceSlider();

protected:
    void mousePressEvent(QMouseEvent *ev)
    {
        //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
        QSlider::mousePressEvent(ev);
        //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
        double pos = ev->pos().x() / (double)width();
        setValue(pos *(maximum() - minimum()) + minimum());
        //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
        QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
        QCoreApplication::sendEvent(parentWidget(), &evEvent);
    }
};

class UkmediaDeviceWidget:public QWidget
{
    Q_OBJECT
public:
    UkmediaDeviceWidget(QWidget *parent = nullptr);
    ~UkmediaDeviceWidget();
    void deviceWidgetInit();

    friend class  DeviceSwitchWidget;
private:
    QLabel *outputDeviceLabel;
    QLabel *outputVolumeLabel;
    QLabel *outputDeviceDisplayLabel;
    QPushButton *outputDeviceBtn;
    UkmediaDeviceSlider *outputDeviceSlider;

    QLabel *inputDeviceLabel;
    QLabel *inputVolumeLabel;
    QLabel *inputDeviceDisplayLabel;
    QPushButton *inputDeviceBtn;
    UkmediaDeviceSlider *inputDeviceSlider;
};

#endif // UKMEDIADEVICEWIDGET_H
