/*
   Copyright (c) 2012-1015 Alex Zhondin <qtinuum.team@gmail.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef PROPERTY_AB_COLOR_H
#define PROPERTY_AB_COLOR_H

#include "GUI/PropertyQColor.h"

class QtnPropertyABColor: public QtnPropertyQColor
{
    Q_OBJECT
    QtnPropertyABColor(const QtnPropertyABColor& other) Q_DECL_EQ_DELETE;

public:
    explicit QtnPropertyABColor(QObject *parent)
        : QtnPropertyQColor(parent)
    {
    }

    P_PROPERTY_DECL_MEMBER_OPERATORS2(QtnPropertyABColor, QtnPropertyQColor)

    void invokeClick();
    void setClickHandler(const std::function<void( QtnPropertyABColor*)> &clickHandler);

Q_SIGNALS:
    void click(QtnPropertyABColor* property);
};

#endif // PROPERTY_AB_COLOR_H
