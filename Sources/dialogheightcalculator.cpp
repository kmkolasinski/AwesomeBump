#include "dialogheightcalculator.h"
#include "ui_dialogheightcalculator.h"

DialogHeightCalculator::DialogHeightCalculator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHeightCalculator)
{
    ui->setupUi(this);
    connect(ui->doubleSpinBoxPhysicalDepth,SIGNAL(valueChanged(double)),this,SLOT(calculateDepthInPixels(double)));
    connect(ui->doubleSpinBoxPhysicalHeight,SIGNAL(valueChanged(double)),this,SLOT(calculateDepthInPixels(double)));
    connect(ui->doubleSpinBoxPhysicalWidth,SIGNAL(valueChanged(double)),this,SLOT(calculateDepthInPixels(double)));
    ui->doubleSpinBoxPhysicalWidth ->setValue(1.0);
    ui->doubleSpinBoxPhysicalHeight->setValue(1.0);
    ui->doubleSpinBoxPhysicalDepth ->setValue(0.01);
    ui->doubleSpinBoxPhysicalDepth->setValue(10.4);

}

DialogHeightCalculator::~DialogHeightCalculator()
{
    delete ui;
}


void DialogHeightCalculator::setImageSize(int width, int height){
    ui->spinBoxImageWidth ->setValue(width);
    ui->spinBoxImageHeight->setValue(height);
}

float DialogHeightCalculator::getDepthInPixels(){
    return ui->doubleSpinBoxDepthInPixels->value();
}


void DialogHeightCalculator::calculateDepthInPixels(double p_depth){
    double wp = ui->spinBoxImageWidth ->value();
    double hp = ui->spinBoxImageHeight->value();
    double ws = ui->doubleSpinBoxPhysicalWidth ->value(); // physical width
    double hs = ui->doubleSpinBoxPhysicalHeight->value(); // physical height
    double zs = ui->doubleSpinBoxPhysicalDepth->value();  // physical depth
    double z_width  = 0; // depth in pixels calculated from width ratio
    double z_height = 0; // depth in pixels from height ratio

    if(ws > 0.000001){
        z_width = wp * zs / ws;
    }
    if(hs > 0.000001){
        z_height= hp * zs / hs;
    }
    double depth = (z_width + z_height)/2.0;
    ui->doubleSpinBoxDepthInPixels->setValue(depth);
}



