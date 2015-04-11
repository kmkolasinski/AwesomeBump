#include "formbasemapconversionlevels.h"
#include "ui_formbasemapconversionlevels.h"

FormBaseMapConversionLevels::FormBaseMapConversionLevels(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBaseMapConversionLevels)
{
    ui->setupUi(this);

    connect(ui->horizontalSliderBaseMapAmplidute,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderConversionBaseMapFlatness,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderBMNoIters,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderConversionBMFilterRadius,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderConversionBaseMapMixing,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderConversionBaseMapBlending,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderConversionBMPreSmoothRadius,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderBaseMapWeight,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
}

FormBaseMapConversionLevels::~FormBaseMapConversionLevels()
{
    delete ui;
}


void FormBaseMapConversionLevels::updateSliders(BaseMapConvLevelProperties& prop){

    bLoadindData = true;
    ui->horizontalSliderBaseMapAmplidute            ->setValue(prop.conversionBaseMapAmplitude*100);
    ui->horizontalSliderConversionBaseMapFlatness   ->setValue(prop.conversionBaseMapFlatness*100);
    ui->horizontalSliderBMNoIters                   ->setValue(prop.conversionBaseMapNoIters);

    ui->horizontalSliderConversionBMFilterRadius    ->setValue(prop.conversionBaseMapFilterRadius);
    ui->horizontalSliderConversionBaseMapMixing     ->setValue(prop.conversionBaseMapMixNormals*200);
    ui->horizontalSliderConversionBaseMapBlending   ->setValue(prop.conversionBaseMapBlending*100);
    ui->horizontalSliderConversionBMPreSmoothRadius ->setValue(prop.conversionBaseMapPreSmoothRadius*10);
    ui->horizontalSliderBaseMapWeight->setValue(prop.conversionBaseMapWeight);

    bLoadindData = false;
}

void FormBaseMapConversionLevels::updateSliders(){
    if(bLoadindData) return;
    emit slidersChanged();
}

void FormBaseMapConversionLevels::getSlidersValues(BaseMapConvLevelProperties &imageProp){

    imageProp.conversionBaseMapAmplitude    = ui->horizontalSliderBaseMapAmplidute->value()/100.0;
    imageProp.conversionBaseMapFlatness     = ui->horizontalSliderConversionBaseMapFlatness->value()/100.0;
    imageProp.conversionBaseMapNoIters      = ui->horizontalSliderBMNoIters->value();
    imageProp.conversionBaseMapFilterRadius = ui->horizontalSliderConversionBMFilterRadius->value();
    imageProp.conversionBaseMapMixNormals   = ui->horizontalSliderConversionBaseMapMixing->value()/200.0;
    imageProp.conversionBaseMapPreSmoothRadius= ui->horizontalSliderConversionBMPreSmoothRadius->value()/10.0;
    imageProp.conversionBaseMapBlending     = ui->horizontalSliderConversionBaseMapBlending->value()/100.0;
    imageProp.conversionBaseMapWeight       = ui->horizontalSliderBaseMapWeight->value();
}
