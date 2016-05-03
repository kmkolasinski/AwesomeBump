#ifndef PROPERTYCONSTRUCTOR
#define PROPERTYCONSTRUCTOR

#include <QStringList>
#include <QDebug>
#include <QString>

QStringList init_grunge_maps(){
    // ------------------------------------------------------- //
    //               Loading grunge maps folders
    // ------------------------------------------------------- //
    qDebug() << "Loading Grunge maps:";
    QDir currentDir(QString(RESOURCE_BASE) + "Core/2D/grunge");
    currentDir.setFilter(QDir::Files);
    QStringList entries = currentDir.entryList();
    QStringList grungeMaps;
    for( QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry ){
        QString dirname=*entry;
        if(dirname != QString(".") && dirname != QString("..")){
            qDebug() << "Loading grunge map:" << dirname;
            grungeMaps << dirname;
        }
    }// end of for
    return grungeMaps;
}

QString first_grunge_map(){
    QDir currentDir(QString(RESOURCE_BASE) + "Core/2D/grunge");
    currentDir.setFilter(QDir::Files);
    QStringList entries = currentDir.entryList();
    QStringList grungeMaps;
    for( QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry ){
        QString dirname=*entry;
        if(dirname != QString(".") && dirname != QString("..")){
            grungeMaps << dirname;
        }
    }// end of for
    qDebug() << "First grunge map:" << grungeMaps[0];
    return grungeMaps[0];
}


#endif // PROPERTYCONSTRUCTOR

