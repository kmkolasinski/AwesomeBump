#ifndef DIALOGLOGGER_H
#define DIALOGLOGGER_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "CommonObjects.h"

namespace Ui {
class DialogLogger;
}

class DialogLogger : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLogger(QWidget *parent = 0);
    ~DialogLogger();
public slots:
    void showLog();

private:
    Ui::DialogLogger *ui;
};

#endif // DIALOGLOGGER_H
