#ifndef CUSTOMSOUND_H
#define CUSTOMSOUND_H
//#include <QtXml>
#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QDebug>

class CustomSound
{
public:
    CustomSound();
    ~CustomSound();
    bool createAudioFile();
    bool isExist(QString nodeName);
    int addXmlNode(QString nodeNane, bool initState);
    bool isFirstRun();


public:
    QFile *file;
private:
    QDomDocument *doc;
    QString audioPath;
};

#endif // CUSTOMSOUND_H
