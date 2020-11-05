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
#ifndef UKMEDIAVOLUMESLIDER_H
#define UKMEDIAVOLUMESLIDER_H
#include <QSlider>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QFrame>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QEvent>
#include <QHideEvent>
#include <QScrollArea>

typedef struct UkuiThemeIcon
{
    QImage image;
    QColor color;
}UkuiThemeIcon;
enum DisplayerMode{
    MINI_MODE,
    ADVANCED_MODE
};
enum SwitchButtonState{
    SWITCH_BUTTON_NORMAL,
    SWITCH_BUTTON_HOVER,
    SWITCH_BUTTON_PRESS
};
static QColor symbolic_color = Qt::gray;

class UkuiMediaSliderTipLabel:public QLabel
{
  public:
    UkuiMediaSliderTipLabel();
    ~UkuiMediaSliderTipLabel();
protected:
    void paintEvent(QPaintEvent*);
};

class UkuiButtonDrawSvg:public QPushButton
{
    Q_OBJECT
public:
    UkuiButtonDrawSvg(QWidget *parent = nullptr);
    ~UkuiButtonDrawSvg();
    QPixmap filledSymbolicColoredPixmap(QImage &source, QColor &baseColor);
    QRect IconGeometry();
    void draw(QPaintEvent* e);
    void init(QImage image ,QColor color);
    friend class DeviceSwitchWidget;
    friend class UkmediaOsdDisplayWidget;

protected:
    void paintEvent(QPaintEvent *event);
    bool event(QEvent *e);
private:
    UkuiThemeIcon themeIcon;
};

class UkuiApplicationWidget:public QWidget
{
    Q_OBJECT
public:
    UkuiApplicationWidget(QWidget *parent = nullptr);
    ~UkuiApplicationWidget();
protected:
    void paintEvent(QPaintEvent*);
};

class UkuiMediaButton:public QPushButton
{
    Q_OBJECT
public:
    UkuiMediaButton(QWidget *parent = nullptr);
    ~UkuiMediaButton();
    friend class UkmediaMiniMasterVolumeWidget;

Q_SIGNALS:
    void advanceToMiniSignal();
    void miniToAdvanceSignal();
    void moveMiniSwitchBtnSignale();
    void moveAdvanceSwitchBtnSignal();
protected:
    void mousePressEvent(QMouseEvent *e)override;
    void mouseMoveEvent(QMouseEvent *e)override;
    void mouseReleaseEvent(QMouseEvent *e)override;
//    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
private:
};

class UkmediaVolumeSlider : public QSlider
{
    Q_OBJECT
public:
//    UkmediaVolumeSlider(QWidget *parent = nullptr);
    UkmediaVolumeSlider(QWidget *parent = nullptr,bool needTip = false);
    void initStyleOption(QStyleOptionSlider *option);
    ~UkmediaVolumeSlider();
private:
    UkuiMediaSliderTipLabel *m_pTiplabel;
    bool state = false;
Q_SIGNALS:
    void silderPressedSignal(int);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *e)
    {
        setCursor(QCursor(Qt::OpenHandCursor));
//        m_displayLabel->move((this->width()-m_displayLabel->width())*this->value()/(this->maximum()-this->minimum()),3);
        QSlider::mouseMoveEvent(e);
    }
    void leaveEvent(QEvent *e);

    void enterEvent(QEvent *e);
    void paintEvent(QPaintEvent *e);
};

class UkuiScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    UkuiScrollArea(QWidget *parent = nullptr);
    ~UkuiScrollArea();

protected:
    void paintEvent(QPaintEvent *e);

};

class UkuiQMenu:public QMenu
{
    Q_OBJECT
public:
    UkuiQMenu(QMenu *parent = nullptr);
    ~UkuiQMenu();
protected:
    void hideEvent(QHideEvent *e);
    virtual bool event(QEvent *e) override;


};

#endif // UKMEDIAVOLUMESLIDER_H
