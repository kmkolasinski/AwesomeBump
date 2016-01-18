#include "formabimageprop.h"
#include "ui_formabimageprop.h"

FormABImageProp::FormABImageProp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormABImageProp)
{
    ui->setupUi(this);

    imageProp = new QtnPropertySetFormImageProp(this);

    QtnPropertyWidget* propertyWidget = new QtnPropertyWidget(this);

    propertyWidget->setParts(QtnPropertyWidgetPartsDescriptionPanel);
    propertyWidget->setPropertySet(imageProp);

    ui->verticalLayout->setMargin(0);
    ui->verticalLayout->addWidget(propertyWidget);

    connect(imageProp,SIGNAL(propertyDidChange(const QtnPropertyBase*,const QtnPropertyBase*,QtnPropertyChangeReason)),
                 this,SLOT  (propertyChanged  (const QtnPropertyBase*,const QtnPropertyBase*,QtnPropertyChangeReason)));

    connect(imageProp,SIGNAL(propertyDidFinishEditing()),this,SLOT(propertyFinishedEditing()));
}

void FormABImageProp::propertyChanged(const QtnPropertyBase* changedProperty,
                                      const QtnPropertyBase* firedProperty,
                                            QtnPropertyChangeReason reason){
    if (reason & QtnPropertyChangeReasonValue){
        //
        if(dynamic_cast<const QtnPropertyBool*>(changedProperty))
        {
            emit imageChanged();
        }
    }
}
void FormABImageProp::propertyFinishedEditing(){
    //qDebug() << "asd";
    emit imageChanged();
}


FormABImageProp::~FormABImageProp()
{
    delete ui;
}
