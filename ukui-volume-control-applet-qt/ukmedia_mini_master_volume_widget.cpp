#include "ukmedia_mini_master_volume_widget.h"
#include <QLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QSpacerItem>

UkmediaMiniMasterVolumeWidget::UkmediaMiniMasterVolumeWidget(QWidget *parent) : QWidget(parent)
{
    masterWidget = new QWidget(this);
    muteBtn = new QPushButton(masterWidget);
    displayVolumeLabel = new QLabel(masterWidget);
    masterVolumeSlider = new UkmediaVolumeSlider(masterWidget);

    deviceCombox = new QComboBox(this);
    switchBtn = new UkuiMediaButton(this);
    switchBtn->setParent(this);
    switchBtn->setStyle(new CustomStyle());
    deviceBtn = new QPushButton(this);
    deviceLabel = new QLabel(tr("Speaker (Realtek Audio)"),this);
    QSpacerItem *item1 = new QSpacerItem(16,20);
    QSpacerItem *item2 = new QSpacerItem(13,20);
    QSpacerItem *item3 = new QSpacerItem(15,20);
    QSpacerItem *item4 = new QSpacerItem(19,20);

    switchBtn->show();
    masterVolumeSlider->setOrientation(Qt::Horizontal);
    masterVolumeSlider->setRange(0,100);
    deviceBtn->setFixedSize(16,16);
    deviceLabel->setFixedSize(154,14);
    masterWidget->setFixedSize(345,31);
    deviceCombox->setFixedSize(300,36);
    displayVolumeLabel->setFixedSize(32,20);
    switchBtn->resize(36,36);

    deviceBtn->move(16,16);
    deviceLabel->move(36,18);
    deviceCombox->move(2,6);
    switchBtn->move(307,6);
    masterWidget->move(0,59);
    QSize switchSize(16,16);
    QSize deviceSize(16,16);
    QSize iconSize(32,32);
    muteBtn->setFixedSize(32,32);
    masterVolumeSlider->setFixedSize(220,22);
    muteBtn->setIconSize(iconSize);
    switchBtn->setIconSize(switchSize);
    deviceBtn->setIconSize(deviceSize);

    switchBtn->setIcon(QIcon("/usr/share/ukui-media/img/complete-module.svg"));
    this->setFixedSize(345,100);
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground);
    QHBoxLayout *hlayout  = new QHBoxLayout(masterWidget);

    hlayout->addItem(item1);
    hlayout->addWidget(muteBtn);
    hlayout->addItem(item2);
    hlayout->addWidget(masterVolumeSlider);
    hlayout->addItem(item3);
    hlayout->addWidget(displayVolumeLabel);
    hlayout->addItem(item4);
    masterWidget->setLayout(hlayout);
    hlayout->setSpacing(0);
    masterWidget->layout()->setContentsMargins(0,0,0,0);

    this->setWindowOpacity(0.9);
    setObjectName("miniWidget");
    deviceCombox->setVisible(false);

    connect(switchBtn,SIGNAL(moveMiniSwitchBtnSignale()),this,SLOT(moveMiniSwitchBtnSlot()));
    deviceBtn->setStyleSheet("QPushButton{background:transparent;border:0px;"
                             "padding-left:0px;}");
    deviceLabel->setStyleSheet("QLabel{font-size:14px;font-family:Noto Sans CJK SC;"
                               "font-weight:400;color:rgba(255,255,255,0.97);"
                               "line-height:34px;}");
    displayVolumeLabel->setStyleSheet("QLabel{font-size:20px;font-family:Noto Sans CJK SC;"
                                      "font-weight:400;color:rgba(255,255,255,0.91);"
                                      "line-height:24px;opacity:0.91;}");
    muteBtn->setStyleSheet("QPushButton{background:transparent;border:0px;}");
    masterVolumeSlider->setStyleSheet("QSlider::groove:horizontal {border: 0px solid #bbb;}"
                                      "QSlider::sub-page:horizontal {"
                                      "background: rgb(107,142,235);"
                                      "border-radius: 2px;"
                                      "margin-top:9px;"
                                      "margin-bottom:9px;}"
                                      "QSlider::add-page:horizontal {"
                                      "background:  rgba(52,70,80,90%);"
                                      "border: 0px solid #777;"
                                      "border-radius: 2px;"
                                      "margin-top:9px;"
                                      "margin-bottom:9px;}"
                                      "QSlider::handle:horizontal {"
                                      "width: 20px;"
                                      "height: 20px;"
                                      "background: rgb(61,107,229);"
                                      "border-radius:10px;}");
    this->setStyleSheet("QWidget#miniWidget{"
                        "background:rgba(19,19,20,0.9);"
                        "border-radius:6px 6px 6px 6px;}");


}

void UkmediaMiniMasterVolumeWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
//    p.setBrush(QBrush(QColor(0x00,0xFF,0xFF,0x59)));
    p.setPen(Qt::NoPen);
    QPainterPath path;
//    opt.rect.adjust(0,0,0,0);
    path.addRoundedRect(opt.rect,6,6);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    p.drawRoundedRect(opt.rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

/*
    滚轮滚动事件
*/
void UkmediaMiniMasterVolumeWidget::wheelEvent(QWheelEvent *event)
{
    bool step;
    if (event->delta() >0 ) {
        step = true;
    }
    else if (event->delta() < 0 ) {
        step = false;
    }
    Q_EMIT mouse_wheel_signal(step);
    event->accept();
}

/*
    点击按钮是移动1px
*/
void UkmediaMiniMasterVolumeWidget::moveMiniSwitchBtnSlot()
{
    switchBtn->move(308,7);
}

UkmediaMiniMasterVolumeWidget::~UkmediaMiniMasterVolumeWidget()
{

}
