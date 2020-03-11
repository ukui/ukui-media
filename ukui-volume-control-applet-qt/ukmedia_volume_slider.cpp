#include "ukmedia_volume_slider.h"
DisplayerMode displayMode = MINI_MODE;
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
        this->setIcon(QIcon("/usr/share/ukui-media/img/complete-module-s.svg"));
    }
    else {
        this->setFixedSize(34,34);
        QSize iconSize(14,14);
        this->setIconSize(iconSize);
        this->setIcon(QIcon("/usr/share/ukui-media/img/mini-module-s.svg"));
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
