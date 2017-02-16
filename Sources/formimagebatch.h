#ifndef FORMIMAGEBATCH_H
#define FORMIMAGEBATCH_H

#include <QWidget>

namespace Ui {
class FormImageBatch;
}

class FormImageBatch : public QWidget
{
    Q_OBJECT

public:
    explicit FormImageBatch(QWidget *parent = 0);
    ~FormImageBatch();

private:
    Ui::FormImageBatch *ui;
};

#endif // FORMIMAGEBATCH_H
