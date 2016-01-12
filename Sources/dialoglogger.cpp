#include "dialoglogger.h"
#include "ui_dialoglogger.h"

DialogLogger::DialogLogger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLogger)
{
    ui->setupUi(this);

}

DialogLogger::~DialogLogger()
{
    delete ui;
}

void DialogLogger::showLog(){
    qDebug() << "Show logger.";
    ui->textEdit->clear();
    QFile logFile(AB_LOG);
    if(logFile.exists()){
        logFile.open(QFile::ReadOnly | QFile::Text);
        QTextStream in(&logFile);
        ui->textEdit->setText(in.readAll());
        ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
    }else{
        ui->textEdit->setText(AB_LOG+QString(" does not exist."));
    }
    show();
}
