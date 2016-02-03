#include "propertydelegateabfloatslider.h"

#include <QtnProperty/PropertyWidget/Delegates/PropertyDelegateFactory.h>
#include <QtnProperty/PropertyWidget/Delegates/Utils/PropertyDelegateSliderBox.h>
#include <QtnProperty/Core/Core/PropertyFloat.h>
#include <QtnProperty/Core/Core/PropertyInt.h>
//#include <QtnProperty/PropertyWidget/Delegates/Utils/PropertyEditorHandler.h>

void regABSliderDelegates()
{
/*
    QtnPropertyDelegateFactory::staticInstance()
        .registerDelegate(&QtnPropertyFloatBase::staticMetaObject
                                , &qtnCreateDelegate<QtnPropertyDelegateABFloatSlider, QtnPropertyFloatBase>
                                , "ABFloatSlider");


    QtnPropertyDelegateFactory::staticInstance()
        .registerDelegate(&QtnPropertyIntBase::staticMetaObject
                                , &qtnCreateDelegate<QtnPropertyDelegateABIntSlider, QtnPropertyIntBase>
                                , "ABIntSlider");
*/

    QtnPropertyDelegateFactory::staticInstance()
            .registerDelegate(&QtnPropertyFloatBase::staticMetaObject
                                    , &qtnCreateDelegate<QtnPropertyDelegateSlideBoxTyped<QtnPropertyFloatBase>, QtnPropertyFloatBase>
                                    , "ABFloatSlider");

    QtnPropertyDelegateFactory::staticInstance()
            .registerDelegate(&QtnPropertyIntBase::staticMetaObject
                                    , &qtnCreateDelegate<QtnPropertyDelegateSlideBoxTyped<QtnPropertyIntBase>, QtnPropertyIntBase>
                                    , "ABIntSlider");

}


