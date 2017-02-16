#include "formimagebatch.h"
#include "ui_formimagebatch.h"

FormImageBatch::FormImageBatch(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormImageBatch)
{
    ui->setupUi(this);
}

FormImageBatch::~FormImageBatch()
{
    delete ui;
}
