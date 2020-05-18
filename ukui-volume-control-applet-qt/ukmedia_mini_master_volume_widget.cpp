/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#include "ukmedia_mini_master_volume_widget.h"
#include <QLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QSpacerItem>
#include <QApplication>
UkmediaMiniMasterVolumeWidget::UkmediaMiniMasterVolumeWidget(QWidget *parent) : QWidget(parent)
{
    masterWidget = new QWidget(this);
    muteBtn = new UkuiButtonDrawSvg(masterWidget);
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

    QIcon icon;
    QSize deviceIconSize(16,16);
    QString deviceBtnIcon = "audio-card";
    icon = QIcon::fromTheme(deviceBtnIcon);
    deviceBtn->setIconSize(deviceIconSize);
    deviceBtn->setIcon(QIcon(icon));

    masterVolumeSlider->setOrientation(Qt::Horizontal);
    masterVolumeSlider->setRange(0,100);
    deviceBtn->setFixedSize(16,16);
    deviceLabel->setFixedSize(154,14);
    masterWidget->setFixedSize(345,31);
    switchBtn->resize(36,36);
    deviceCombox->setFixedSize(300,36);
    displayVolumeLabel->setFixedSize(32,20);

    deviceBtn->move(16,16);
    deviceLabel->move(36,18);
    deviceCombox->move(2,6);
    switchBtn->move(305,6);
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

    this->setWindowOpacity(0.8);
    setObjectName("miniWidget");
    deviceCombox->setVisible(false);
    switchBtn->setToolTip(tr("Go Into Full Mode"));
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
                                      "background:  rgba(255,255,255,0.1);"
                                      "border: 0px solid #777;"
                                      "border-radius: 2px;"
                                      "margin-top:9px;"
                                      "margin-bottom:9px;}"
                                      "QSlider::handle:horizontal {"
                                      "width: 20px;"
                                      "height: 20px;"
                                      "background: rgb(61,107,229);"
                                      "border-radius:10px;}");
//    this->setStyleSheet("QWidget#miniWidget{border:1px solid rgba(255, 255, 255, 0.05);"
//                        "background:rgba(19,19,20,0.8);"
//                        "border-radius:6px 6px 6px 6px;}");

}

void UkmediaMiniMasterVolumeWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setBrush(QBrush(QColor(0x13,0x13,0x14,0xB2)));
    p.setPen(Qt::NoPen);
    QPainterPath path;
    opt.rect.adjust(0,0,0,0);
    path.addRoundedRect(opt.rect,6,6);
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.drawRoundedRect(opt.rect,6,6);
    setProperty("blurRegion",QRegion(path.toFillPolygon().toPolygon()));
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//    QWidget::paintEvent(event);
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
    按键事件,控制系统音量
*/
void UkmediaMiniMasterVolumeWidget::keyPressEvent(QKeyEvent *event)
{
    int volumeGain ;
    if (event->key() == Qt::Key_Escape) {
        this->hide();
    }
    else if (event->key() == Qt::Key_Up) {
        volumeGain = 1;
        Q_EMIT keyboard_pressed_signal(volumeGain);
    }
    else if (event->key() == Qt::Key_Down) {
        volumeGain = -1;
        Q_EMIT keyboard_pressed_signal(volumeGain);
    }
    else if (event->key() == Qt::Key_Left) {
        volumeGain = -1;
        Q_EMIT keyboard_pressed_signal(volumeGain);
    }
    else if (event->key() == Qt::Key_Right) {
        volumeGain = 1;
        Q_EMIT keyboard_pressed_signal(volumeGain);
    }
}

/*
    点击窗口之外的部分隐藏
*/
bool UkmediaMiniMasterVolumeWidget:: event(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (QApplication::activeWindow() != this) {
            this->hide();
        }
    }
    return QWidget::event(event);
}

/*
    点击按钮是移动1px
*/
void UkmediaMiniMasterVolumeWidget::moveMiniSwitchBtnSlot()
{
    switchBtn->move(306,7);
}

UkmediaMiniMasterVolumeWidget::~UkmediaMiniMasterVolumeWidget()
{

}
