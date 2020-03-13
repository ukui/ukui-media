#include "ukmedia_volume_slider.h"
DisplayerMode displayMode = MINI_MODE;
#include <QPalette>

#include <QPainter>
#include <QWidget>
#include <QStyleOptionComplex>
#include <QDebug>




MyCustomSlider::MyCustomSlider(QWidget *parent):QSlider(parent)
{
   m_displayLabel=new QLabel(this);
   m_displayLabel->setFixedSize(QSize(20,20));
   //设置游标背景为白色
   m_displayLabel->setAutoFillBackground(true);
   QPalette palette;
   palette.setColor(QPalette::Background, Qt::white);
   m_displayLabel->setPalette(palette);

   m_displayLabel->setAlignment(Qt::AlignCenter);

   m_displayLabel->setVisible(false);
   m_displayLabel->move(0,3);
}

MyCustomSlider::~MyCustomSlider()
{

}

void MyCustomSlider::mousePressEvent(QMouseEvent *event)
{
    if(!m_displayLabel->isVisible())
    {
        m_displayLabel->setVisible(true);
        m_displayLabel->setText(QString::number(this->value()));
    }
    QSlider::mousePressEvent(event);
}

void MyCustomSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if(m_displayLabel->isVisible())
    {
        m_displayLabel->setVisible(false);
    }
    QSlider::mouseReleaseEvent(event);
}

void MyCustomSlider::mouseMoveEvent(QMouseEvent *event)
{
    m_displayLabel->setText(QString::number(this->value()));
    m_displayLabel->move((this->width()-m_displayLabel->width())*this->value()/(this->maximum()-this->minimum()),3);
    QSlider::mouseMoveEvent(event);
}

UkuiMediaButton::UkuiMediaButton(QWidget *parent)
{
    Q_UNUSED(parent);
    this->setFixedSize(36,36);
}
UkuiMediaButton::~UkuiMediaButton()
{

}

void UkuiMediaButton::mousePressEvent(QMouseEvent *e)
{
    if (displayMode == MINI_MODE) {
        Q_EMIT moveMiniSwitchBtnSignale();
        this->setFixedSize(34,34);
        QSize iconSize(14,14);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/complete-module.svg"));
    }
    else {
        this->setFixedSize(34,34);
        QSize iconSize(14,14);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/mini-module.svg"));
        Q_EMIT moveAdvanceSwitchBtnSignal();
    }
}

void UkuiMediaButton::mouseReleaseEvent(QMouseEvent *e)
{
    if (displayMode == MINI_MODE) {
        Q_EMIT miniToAdvanceSignal();
        this->setFixedSize(36,36);
        QSize iconSize(16,16);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/complete-module.svg"));
    }
    else {
        Q_EMIT advanceToMiniSignal();
        this->setFixedSize(36,36);
        QSize iconSize(16,16);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/mini-module.svg"));
    }

}

UkmediaVolumeSlider::UkmediaVolumeSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

UkmediaVolumeSlider::~UkmediaVolumeSlider()
{

}
