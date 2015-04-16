#include "dialogshortcuts.h"
#include "ui_dialogshortcuts.h"

DialogShortcuts::DialogShortcuts(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogShortcuts)
{
    ui->setupUi(this);
}

DialogShortcuts::~DialogShortcuts()
{
    delete ui;
}
