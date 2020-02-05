#include "applicationvolumewidget.h"
#include <QDebug>

ApplicationVolumeWidget::ApplicationVolumeWidget(QWidget *parent) : QWidget (parent)
{
    this->setFixedSize(360,260);
    applicationLabel = new QLabel(tr("Application Volume"),this);
    noAppLabel = new QLabel(tr("No application is currently playing or recording audio"),this);
//    qDebug() << 10;
    gridlayout = new QGridLayout(this);
    app_volume_list = new QStringList;
    noAppLabel->setFixedSize(210,14);
    applicationLabel->move(20,23);
    noAppLabel->move(60,123);
}

ApplicationVolumeWidget::~ApplicationVolumeWidget()
{
    delete  app_volume_list;
}
