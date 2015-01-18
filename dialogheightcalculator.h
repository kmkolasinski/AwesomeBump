#ifndef DIALOGHEIGHTCALCULATOR_H
#define DIALOGHEIGHTCALCULATOR_H

#include <QDialog>

namespace Ui {
class DialogHeightCalculator;
}

class DialogHeightCalculator : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHeightCalculator(QWidget *parent = 0);
    ~DialogHeightCalculator();
    void setImageSize(int width, int height);
    float getDepthInPixels();

private slots:
    void calculateDepthInPixels(double p_depth);

private:

    Ui::DialogHeightCalculator *ui;
};

#endif // DIALOGHEIGHTCALCULATOR_H
