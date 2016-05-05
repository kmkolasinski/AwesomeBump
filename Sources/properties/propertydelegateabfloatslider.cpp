#include "propertydelegateabfloatslider.h"

#include <QtnProperty/PropertyWidget/Delegates/PropertyDelegateFactory.h>
#include <QtnProperty/PropertyWidget/Delegates/Utils/PropertyDelegateSliderBox.h>
#include <QtnProperty/Core/Core/PropertyFloat.h>
#include <QtnProperty/Core/Core/PropertyInt.h>

void regABSliderDelegates()
{


    QtnPropertyDelegateFactory::staticInstance()
        .registerDelegateDefault(&QtnPropertyFloatBase::staticMetaObject
                                , &qtnCreateDelegate<QtnPropertyDelegateSlideBoxTyped<QtnPropertyFloatBase>, QtnPropertyFloatBase>
                                , "SliderBox");

    QtnPropertyDelegateFactory::staticInstance()
        .registerDelegateDefault(&QtnPropertyIntBase::staticMetaObject
                                , &qtnCreateDelegate<QtnPropertyDelegateSlideBoxTyped<QtnPropertyIntBase>, QtnPropertyIntBase>
                                , "SliderBox");
}


