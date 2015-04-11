#ifndef FORMBASEMAPCONVERSIONLEVELS_H
#define FORMBASEMAPCONVERSIONLEVELS_H

#include <QWidget>
#include "CommonObjects.h"


namespace Ui {
class FormBaseMapConversionLevels;
}

class FormBaseMapConversionLevels : public QWidget
{
    Q_OBJECT

public:
    explicit FormBaseMapConversionLevels(QWidget *parent = 0);
    ~FormBaseMapConversionLevels();
    void updateSliders(BaseMapConvLevelProperties &prop);
    void getSlidersValues(BaseMapConvLevelProperties &prop);
private slots:
    void updateSliders();
signals:
    void slidersChanged();
private:
    bool bLoadindData;
    Ui::FormBaseMapConversionLevels *ui;
};

#endif // FORMBASEMAPCONVERSIONLEVELS_H
