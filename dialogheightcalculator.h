/**
 * Simple class which allows to calculate the depth of the nomral texture
 * based on the height texture and its given physical parameters.
 *
*/
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
    void setImageSize(int width, int height); // set current image size
    // calculate the depth of the normal map based on given parameters
    // this parameter is then used during normal map evaluation
    float getDepthInPixels();

private slots:
    void calculateDepthInPixels(double p_depth);

private:

    Ui::DialogHeightCalculator *ui;
};

#endif // DIALOGHEIGHTCALCULATOR_H
