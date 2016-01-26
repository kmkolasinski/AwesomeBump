#ifndef PROPERTYDELEGATEABFLOATSLIDER_H
#define PROPERTYDELEGATEABFLOATSLIDER_H
/*
#include <QtnProperty/PropertyWidget/Delegates/PropertyDelegate.h>
#include <QtnProperty/Core/Core/PropertyFloat.h>
#include <QDebug>
class QtnPropertyFloatBase;

class QTN_PW_EXPORT QtnPropertyDelegateABFloatSlider: public QtnPropertyDelegateTyped<QtnPropertyFloatBase, QtnPropertyDelegateWithValue>
{
    Q_DISABLE_COPY(QtnPropertyDelegateABFloatSlider)

public:
    QtnPropertyDelegateABFloatSlider(QtnPropertyFloatBase& owner)
        : QtnPropertyDelegateTyped<QtnPropertyFloatBase, QtnPropertyDelegateWithValue>(owner)
    {
        m_boxFillColor = QColor::fromRgb(200, 200, 255);
    }

protected:
    void applyAttributesImpl(const QtnPropertyDelegateAttributes& attributes) override;
    bool createSubItemValueImpl(QtnPropertyDelegateDrawContext& context, QtnPropertyDelegateSubItem& subItemValue) override;

private:
    void draw(QtnPropertyDelegateDrawContext& context, const QtnPropertyDelegateSubItem& item);
    bool event(QtnPropertyDelegateEventContext& context, const QtnPropertyDelegateSubItem& item);

    QColor m_boxFillColor;
};
*/
#endif // PROPERTYDELEGATEABFLOATSLIDER_H
