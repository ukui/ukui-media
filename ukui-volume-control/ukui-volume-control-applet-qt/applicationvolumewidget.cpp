#include "applicationvolumewidget.h"
#include <QDebug>

ApplicationVolumeWidget::ApplicationVolumeWidget(QWidget *parent) : QWidget (parent)
{
    this->setFixedSize(360,260);
    applicationLabel = new QLabel(tr("Application Volume"),this);
    noAppLabel = new QLabel(tr("No application is currently playing or recording audio"),this);
//    qDebug() << 10;
    app_volume_list = new QStringList;
    applicationLabel->move(20,23);
    noAppLabel->move(60,123);
}

ApplicationVolumeWidget::~ApplicationVolumeWidget()
{
    delete  app_volume_list;
}
