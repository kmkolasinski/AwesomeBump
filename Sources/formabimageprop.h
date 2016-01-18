#ifndef FORMABIMAGEPROP_H
#define FORMABIMAGEPROP_H

#include <QWidget>
#include "CommonObjects.h"
#include "properties/ImageProperties.peg.h"
#include <PropertyWidget.h>
namespace Ui {
class FormABImageProp;
}

class FormABImageProp : public QWidget
{
    Q_OBJECT

public:
    explicit FormABImageProp(QWidget *parent = 0);
    ~FormABImageProp();
    QtnPropertySetFormImageProp* imageProp;

public slots:
    void propertyChanged(const QtnPropertyBase* changedProperty,
                         const QtnPropertyBase* firedProperty,
                         QtnPropertyChangeReason reason);
    void propertyFinishedEditing();
signals:
    void imageChanged();
private:
    Ui::FormABImageProp *ui;

};

#endif // FORMABIMAGEPROP_H
