#ifndef PROPERTYDELEGATEABFLOATSLIDER_H
#define PROPERTYDELEGATEABFLOATSLIDER_H
/*
#include <QtnProperty/PropertyWidget/Delegates/Utils/PropertyDelegateSliderBox.h>
#include <QtnProperty/PropertyWidget/Delegates/PropertyDelegate.h>


class QTN_PW_EXPORT QtnPropertyDelegateABSlider: public QtnPropertyDelegateSlideBox
{
    Q_DISABLE_COPY(QtnPropertyDelegateABSlider)

};



template <typename PropertyClass>
class QtnPropertyDelegateABSliderTyped: public QtnPropertyDelegateTyped<PropertyClass, QtnPropertyDelegateABSlider>
{
    Q_DISABLE_COPY(QtnPropertyDelegateABSliderTyped)

public:
    QtnPropertyDelegateABSliderTyped(PropertyClass& owner)
        : QtnPropertyDelegateTyped<PropertyClass, QtnPropertyDelegateABSlider>(owner)
    {}

protected:
    typedef typename PropertyClass::ValueType ValueType;

    float propertyValuePart() const override
    {
        ValueType valueInterval = owner().maxValue() - owner().minValue();
        if (valueInterval <= 0)
            return -1.f;

        return float(owner().value() - owner().minValue())/valueInterval;
    }
    QString valuePartToStr(float valuePart) const
    {
        return QString::number(partToValue(valuePart));
    }
    void incrementPropertyValue(int steps)
    {
        owner().incrementValue(steps);
    }

    void setPropertyValuePart(float valuePart)
    {
        owner().setValue(partToValue(valuePart));
    }

    ValueType partToValue(float valuePart) const
    {
        return owner().minValue() + ValueType(valuePart * (owner().maxValue() - owner().minValue()));
    }
};

/*
#include <QKeyEvent>
#include <QtnProperty/PropertyWidget/PropertyView.h>
#include <QtnProperty/PropertyWidget/Delegates/PropertyEditorAux.h>
#include <QtnProperty/PropertyWidget/Delegates/PropertyDelegateSlideBox.h>
#include <QtnProperty/Core/Core/PropertyFloat.h>
#include <QtnProperty/Core/Core/PropertyInt.h>
#include <QtnProperty/PropertyWidget/Delegates/PropertyEditorAux.h>
#include <QtnProperty/PropertyWidget/Delegates/PropertyDelegate.h>


template <typename PropertyClass, typename DelegateClass = QtnPropertyDelegateWithValueEditor>
class QTN_PW_EXPORT QtnPropertyDelegateABSlider: public QtnPropertyDelegateTyped<PropertyClass, DelegateClass>
{
    Q_DISABLE_COPY(QtnPropertyDelegateABSlider)

public:
    QtnPropertyDelegateABSlider(PropertyClass& owner)
        : QtnPropertyDelegateTyped<PropertyClass, DelegateClass>(owner)
    {
        m_boxFillColor = QColor::fromRgb(200, 200, 255);
        m_dragValue = 0;
        bMouseDragged = false;
    }

protected:
    void applyAttributesImpl(const QtnPropertyDelegateAttributes& attributes) override
    {
        qtnGetAttribute(attributes, "fillColor", m_boxFillColor);
    }
    bool createSubItemValueImpl(QtnDrawContext& context, QtnSubItem& subItemValue) override
    {
        subItemValue.trackState();
        subItemValue.drawHandler = qtnMemFn(this, &QtnPropertyDelegateABSlider::draw);
        subItemValue.eventHandler = qtnMemFn(this, &QtnPropertyDelegateABSlider::event);
        return true;
    }

protected:
    void draw(QtnDrawContext& context, const QtnSubItem& item)
    {
        float valueInterval = this->owner().maxValue() - this->owner().minValue();
        if (valueInterval <= 0)
            return;

        float value = (item.state() == QtnSubItemStatePushed) ? m_dragValue : this->owner().value();
        float valuePart = (value - this->owner().minValue())/valueInterval;

        auto boxRect = item.rect;
        boxRect.adjust(-2, 1, 0, -1);
        auto valueRect = boxRect;
        valueRect.setRight(valueRect.left() + int(valuePart * valueRect.width()));
        valueRect.setTop(valueRect.top()+0);

        auto& painter = *context.painter;

        painter.save();

        painter.setPen(m_boxFillColor.lighter(110));
        //painter.drawLine(boxRect.left(),boxRect.bottom(),boxRect.left(),boxRect.bottom()-4);
        //painter.drawLine(boxRect.left()+boxRect.width()/2,boxRect.bottom(),boxRect.left()+boxRect.width()/	2,boxRect.bottom()-4);
        //painter.drawLine(boxRect.right(),boxRect.bottom(),boxRect.right(),boxRect.bottom()-4);
        //painter.drawLine(boxRect.left()+boxRect.width()*0.25,boxRect.bottom(),boxRect.left()+	boxRect.width()*0.25,boxRect.bottom()-3);
        //painter.drawLine(boxRect.left()+boxRect.width()*0.75,boxRect.bottom(),boxRect.left()+	boxRect.width()*0.75,boxRect.bottom()-3);

        QLinearGradient gradient(boxRect.topLeft(), boxRect.bottomRight()); // diagonal gradient from top-	left to bottom-right
        gradient.setColorAt(0, m_boxFillColor.lighter(125));
        gradient.setColorAt(1, m_boxFillColor);
        painter.fillRect(valueRect, context.widget->palette().color(QPalette::Highlight));

        painter.restore();

        boxRect.adjust(context.widget->valueLeftMargin(), 0, 0, 0);
        auto strValue = QString::number(this->owner().value());
        if(bMouseDragged)
            strValue = QString::number(m_dragValue);

        this->drawValueText(strValue, painter, boxRect, this->state(context.isActive, item.state()), nullptr);
    }
    bool event(QtnEventContext& context, const QtnSubItem& item)
    {
        switch (context.eventType())
        {
        case QEvent::KeyPress:
        {

            int key =  context.eventAs<QKeyEvent>()->key();

            if ((key == Qt::Key_Plus) || (key == Qt::Key_Equal))
                this->owner().incrementValue(1);
            else if ((key == Qt::Key_Minus) || (key == Qt::Key_Underscore))
                this->owner().incrementValue(-1);
            else
                return false;

            return true;
        } break;

        case QEvent::Wheel:
        {
            int steps = context.eventAs<QWheelEvent>()->angleDelta().y()/120;
            this->owner().incrementValue(steps);
            return true;
        } break;


        case QEvent::MouseMove:
        {
            if (item.state() == QtnSubItemStatePushed)
            {
                updateDragValue(context.eventAs<QMouseEvent>()->x(), item.rect);
                context.widget->viewport()->update();
            }
            return true;
        } break;

        case QtnSubItem::SubItemReleaseMouse:
        {
            //updateDragValue(context.eventAs<QMouseEvent>()->x(), item.rect);
            bMouseDragged = false;
            this->owner().setValue(m_dragValue);
            return true;
        } break;

        default:
            return false;
        }
    }
    void updateDragValue(int x, const QRect& rect)
    {
        float valuePart = float(x - rect.left()) / rect.width();
        if (valuePart < 0)
            valuePart = 0;
        else if (valuePart > 1.f)
            valuePart = 1.f;

        m_dragValue = this->owner().minValue() + valuePart * (this->owner().maxValue() - this->owner().minValue());
        float step = this->owner().stepValue();
        m_dragValue = int(m_dragValue/step)*step;
        bMouseDragged = true;
    }

    QColor m_boxFillColor;
    float m_dragValue;
    bool bMouseDragged;
};


class QTN_PW_EXPORT QtnPropertyDelegateABFloatSlider: public QtnPropertyDelegateABSlider<QtnPropertyFloatBase, QtnPropertyDelegateWithValue>
{
    Q_DISABLE_COPY(QtnPropertyDelegateABFloatSlider)

public:
    QtnPropertyDelegateABFloatSlider(QtnPropertyFloatBase& owner)
        : QtnPropertyDelegateABSlider<QtnPropertyFloatBase, QtnPropertyDelegateWithValue>(owner)
    {
    }

};

class QTN_PW_EXPORT QtnPropertyDelegateABIntSlider: public QtnPropertyDelegateABSlider<QtnPropertyIntBase, QtnPropertyDelegateWithValue>
{
    Q_DISABLE_COPY(QtnPropertyDelegateABIntSlider)

public:
    QtnPropertyDelegateABIntSlider(QtnPropertyIntBase& owner)
        : QtnPropertyDelegateABSlider<QtnPropertyIntBase, QtnPropertyDelegateWithValue>(owner)
    {
    }

};
*/

#endif // PROPERTYDELEGATEABFLOATSLIDER_H
