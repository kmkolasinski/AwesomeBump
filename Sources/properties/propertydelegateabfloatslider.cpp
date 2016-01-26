#include "propertydelegateabfloatslider.h"
/*
#include <QtnProperty/PropertyWidget/Delegates/PropertyDelegateFactory.h>
#include <QtnProperty/PropertyWidget/Delegates/PropertyEditorHandler.h>
#include <QtnProperty/PropertyWidget/Delegates/PropertyEditorAux.h>
#include <QtnProperty/PropertyWidget/PropertyView.h>

#include <QtnProperty/Core/Core/PropertyFloat.h>
#include <QtnProperty/PropertyWidget/Delegates/Core/PropertyDelegateFloat.h>


#include <QDoubleSpinBox>
#include <QKeyEvent>

void regABSliderFloatDelegate()
{

    QtnPropertyDelegateFactory::staticInstance()
        .registerDelegate(&QtnPropertyFloatBase::staticMetaObject
                                , &qtnCreateDelegate<QtnPropertyDelegateABFloatSlider, QtnPropertyFloatBase>
                                , "ABFloatSlider");
}

void QtnPropertyDelegateABFloatSlider::applyAttributesImpl(const QtnPropertyDelegateAttributes& attributes)
{
    qtnGetAttribute(attributes, "fillColor", m_boxFillColor);
}

bool QtnPropertyDelegateABFloatSlider::createSubItemValueImpl(QtnPropertyDelegateDrawContext&, QtnPropertyDelegateSubItem& subItemValue)
{
    subItemValue.drawHandler = qtnMemFn(this, &QtnPropertyDelegateABFloatSlider::draw);
    subItemValue.eventHandler = qtnMemFn(this, &QtnPropertyDelegateABFloatSlider::event);
    return true;
}

void QtnPropertyDelegateABFloatSlider::draw(QtnPropertyDelegateDrawContext& context, const QtnPropertyDelegateSubItem& item)
{
    float valueInterval = owner().maxValue() - owner().minValue();
    if (valueInterval <= 0)
        return;

    float valuePart = (owner().value() - owner().minValue())/valueInterval;

    auto boxRect = item.rect;
    boxRect.adjust(-1, 1, 0, -1);
    auto valueRect = boxRect;
    valueRect.setRight(valueRect.left() + int(valuePart * valueRect.width()));
    valueRect.setTop(valueRect.top()+1);

    auto& painter = *context.painter;

    painter.save();

    painter.setPen(m_boxFillColor.lighter(110));

    QLinearGradient gradient(boxRect.topLeft(), boxRect.bottomRight()); // diagonal gradient from top-left to bottom-right
    gradient.setColorAt(0, m_boxFillColor.lighter(125));
    gradient.setColorAt(1, m_boxFillColor);
//    painter.fillRect(valueRect, gradient);
    painter.fillRect(valueRect, context.widget->palette().color(QPalette::Highlight));


    painter.restore();

    boxRect.adjust(context.widget->valueLeftMargin(), 0, 0, 0);
    auto strValue = QString::number(owner().value());
    drawValueText(strValue, painter, boxRect, state(context.isActive), nullptr);
}

bool QtnPropertyDelegateABFloatSlider::event(QtnPropertyDelegateEventContext& context, const QtnPropertyDelegateSubItem& item)
{
    static bool bMousePressed = false;
    qDebug() << context.eventType() ;
    switch (context.eventType())
    {
    case QEvent::KeyPress:
    {
        int key = context.eventAs<QKeyEvent>()->key();

        if ((key == Qt::Key_Plus) || (key == Qt::Key_Equal))
            owner().incrementValue(1);
        else if ((key == Qt::Key_Minus) || (key == Qt::Key_Underscore))
            owner().incrementValue(-1);
        else
            return false;

        return true;
    }

    case QEvent::Wheel:
    {
        int steps = context.eventAs<QWheelEvent>()->angleDelta().y()/120;
        owner().incrementValue(steps);
        return true;
    }

//    case QEvent::MouseButtonPress:
////    case QEvent::MouseButtonDblClick:
//    {
//        int x = context.eventAs<QMouseEvent>()->x();
//        if (item.rect.left() <= x && x <= item.rect.right())
//        {
//            float valuePart = float(x - item.rect.left()) / item.rect.width();
//            float value = owner().minValue() + valuePart * (owner().maxValue() - owner().minValue());
//            owner().setValue(value);
//            return true;
//        }
//    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    {
        int x = context.eventAs<QMouseEvent>()->x();
        if (item.rect.left() <= x && x <= item.rect.right())
        {
            float valuePart = float(x - item.rect.left()) / item.rect.width();
            float value = owner().minValue() + valuePart * (owner().maxValue() - owner().minValue());
            owner().setValue(value);
            return true;
        }
    }
    default:
        return false;
    }

}


*/
