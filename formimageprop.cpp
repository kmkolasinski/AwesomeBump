#include "formimageprop.h"
#include "ui_formimageprop.h"
QDir* FormImageProp::recentDir;

FormImageProp::FormImageProp(QMainWindow *parent, QGLWidget* qlW_ptr) :
    QWidget(parent),
    ui(new Ui::FormImageProp)
{
    ui->setupUi(this);

    imageProp.glWidget_ptr = qlW_ptr;
    
    connect(ui->pushButtonOpenImage,SIGNAL(released()),this,SLOT(open()));
    connect(ui->pushButtonSaveImage,SIGNAL(released()),this,SLOT(save()));
    connect(ui->pushButtonRestoreSettings,SIGNAL(released()),this,SLOT(reloadImageSettings()));

    connect(ui->checkBoxRemoveShading,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxGrayScale,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    // gray scale properties
    connect(ui->comboBoxGrayScalePresets,SIGNAL(activated(int)),this,SLOT(updateGrayScalePreset(int)));
    connect(ui->horizontalSliderGrayScaleR,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrayScaleG,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrayScaleB,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));


    connect(ui->checkBoxInvertB,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxInvertR,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxInvertG,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    // standard enchancement
    connect(ui->horizontalSliderRemoveShadingGaussIter,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderBlurNoPasses          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSmallDetails          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderMediumDetails         ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderDetailDepth           ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    // specular manipulation tool
    connect(ui->checkBoxSpecularControl             ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderSpeculatW1          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularW2          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularRadius      ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularContrast    ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularAmplifier   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularBrightness  ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->horizontalSliderSpeculatW1           ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularW2           ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularRadius       ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularRadius       ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularContrast     ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularAmplifier    ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularBrightness   ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));

    connect(ui->horizontalSliderSharpenBlur,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalsStep,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    // height conversion buttons
    connect(ui->horizontalSliderConversionHNDepth   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->checkBoxEnableHeightToNormal        ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxConversionHNAttachToNormal  ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxEnableHeightToNormal        ,SIGNAL(clicked()),this,SLOT(toggleHNPreviewMode()));
    connect(ui->pushButtonConverToNormal            ,SIGNAL(released()),this,SLOT(applyHeightToNormalConversion()));
    connect(ui->pushButtonShowDepthCalculator       ,SIGNAL(released()),this,SLOT(showHeightCalculatorDialog()));


    // normal convertion buttons and sliders
    connect(ui->checkBoxEnableNormalToHeight                ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderNormalToHeightItersHuge     ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersVeryLarge,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersLarge    ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersMedium   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersSmall    ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersVerySmall,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->pushButtonConvertToHeight,SIGNAL(released()),this,SLOT(applyNormalToHeightConversion()));

    // base map convertion
    connect(ui->checkBoxEnableBaseMapToOthers               ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderBaseMapAmplidute            ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBaseMapFlatness   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderBMNoIters                   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBMFilterRadius    ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBaseMapMixing     ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBMPreSmoothRadius ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBaseMapBlending   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->pushButtonConvertToNormalAndHeight,SIGNAL(released()),this,SLOT(applyBaseConversionConversion()));

    // ssao calculation
    connect(ui->horizontalSliderSSAOBias        ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAODepth       ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAOIntensity   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAONoIters     ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->horizontalSliderSSAOBias        ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSSAODepth       ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSSAOIntensity   ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSSAONoIters     ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));


    // levels properties
    connect(ui->horizontalSliderHeightProcMinValue,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));    
    connect(ui->horizontalSliderHeightProcMaxValue,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderHeightAveRadius   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->horizontalSliderHeightProcMinValue,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderHeightProcMaxValue,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderHeightAveRadius   ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));

    setAcceptDrops(true);

    ui->groupBoxGrayScale->hide();
    heightCalculator = new DialogHeightCalculator;

}

FormImageProp::~FormImageProp()
{
    delete heightCalculator;
    delete ui;
}



void FormImageProp::open()
{

    QStringList picturesLocations;// = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    if(recentDir == NULL ) picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    else  picturesLocations << recentDir->absolutePath();


    QFileDialog dialog(this,
                       tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first(),
                       tr("All Images (*.png *.jpg  *.tga *.jpeg *.bmp);;"
                          "Images (*.png);;"
                          "Images (*.jpg);;"
                          "Images (*.tga);;"
                          "Images (*.jpeg);;"
                          "Images (*.bmp);;"
                          "All files (*.*)"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

bool FormImageProp::loadFile(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QImage _image;

    // Targa support added
    if(fileInfo.completeSuffix().compare("tga") == 0){
        TargaImage tgaImage;
        _image = tgaImage.read(fileName);
    }else{
        QImageReader loadedImage(fileName);
        _image = loadedImage.read();
    }

    if (_image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    qDebug() << "<FormImageProp> Open image:" << fileName;

    imageName = fileInfo.baseName();
    (*recentDir).setPath(fileName);
    image    = _image;
    imageProp.init(image);

    emit imageChanged();
    return true;
}

void FormImageProp::save(){

    QStringList picturesLocations;
    if(recentDir == NULL ) picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    else{
         QFileInfo fileInfo(recentDir->absolutePath());
         QString fullFileName = fileInfo.absolutePath()+ "/" +
                                imageName + PostfixNames::getPostfix(imageProp.imageType)
                                + PostfixNames::outputFormat;
         picturesLocations << fullFileName;
         qDebug() << "<FormImageProp>:: Saving to file:" << fullFileName;
    }


    QFileDialog dialog(this,
                       tr("Save current image to file"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first(),
                       tr("All images (*.png *.jpg  *.tga *.jpeg *.bmp);;All files (*.*)"));
    dialog.setDirectory(recentDir->absolutePath());
    dialog.setAcceptMode(QFileDialog::AcceptSave);


    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

bool FormImageProp::saveFile(const QString &fileName){
    qDebug() << Q_FUNC_INFO << "image:" << fileName;

    QFileInfo fileInfo(fileName);
    (*recentDir).setPath(fileInfo.absolutePath());
    image = imageProp.getImage();

    if( PostfixNames::outputFormat.compare(".tga") == 0 || fileInfo.completeSuffix().compare("tga") == 0 ){
        TargaImage tgaImage;
        tgaImage.write(image,fileName);
    }else{
        image.save(fileName);
    }
    return true;
}

void FormImageProp::saveFileToDir(const QString &dir){

    QString fullFileName = dir + "/" +
                           imageName + PostfixNames::getPostfix(imageProp.imageType)
                           + PostfixNames::outputFormat;
    saveFile(fullFileName);
}

void FormImageProp::saveImageToDir(const QString &dir,QImage& image){

    QString fullFileName = dir + "/" +
                           imageName + PostfixNames::getPostfix(imageProp.imageType)
                           + PostfixNames::outputFormat;

    qDebug() << "<FormImageProp> save image:" << fullFileName;
    QFileInfo fileInfo(fullFileName);
    (*recentDir).setPath(fileInfo.absolutePath());

    if( PostfixNames::outputFormat.compare(".tga") == 0){
        TargaImage tgaImage;
        tgaImage.write(image,fullFileName);
    }else
        image.save(fullFileName);
}

void FormImageProp::setImage(QImage _image){
    image    = _image;
    if (imageProp.glWidget_ptr->isValid())
      imageProp.init(image);
    else
        qDebug() << Q_FUNC_INFO << "Invalid context.";
}
void FormImageProp::setImageName(QString name){
    imageName = name;
}
QString FormImageProp::getImageName(){
    return imageName;
}

void FormImageProp::setSpecularControlChecked(){
    ui->checkBoxSpecularControl->setChecked(true);
}

void FormImageProp::updateGrayScalePreset(int index){
    switch(index){
        case(0):
        imageProp.grayScalePreset.mode1();
        break;
        case(1):
        imageProp.grayScalePreset.mode2();
        break;
        default:break;
    };
    bLoading = true;
    ui->horizontalSliderGrayScaleR->setValue(imageProp.grayScalePreset.R*255);
    ui->horizontalSliderGrayScaleG->setValue(imageProp.grayScalePreset.G*255);
    ui->horizontalSliderGrayScaleB->setValue(imageProp.grayScalePreset.B*255);
    bLoading = false;

    emit imageChanged();
}

void FormImageProp::updateGuiSpinBoxesAndLabes(int){
    if(bLoading == true) return;

    ui->doubleSpinBoxSpecularW1         ->setValue(ui->horizontalSliderSpeculatW1           ->value()/100.0);
    ui->doubleSpinBoxSpecularW2         ->setValue(ui->horizontalSliderSpecularW2           ->value()/100.0);
    ui->doubleSpinBoxSpecularRadius     ->setValue(ui->horizontalSliderSpecularRadius       ->value()*1.0);
    ui->doubleSpinBoxSpecularContrast   ->setValue(ui->horizontalSliderSpecularContrast     ->value()/100.0-1.0);
    ui->doubleSpinBoxSpecularAmplifier  ->setValue(ui->horizontalSliderSpecularAmplifier    ->value()/10.0);
    ui->doubleSpinBoxSpecularBrightness ->setValue(ui->horizontalSliderSpecularBrightness   ->value()/100.0);
    ui->doubleSpinBoxConversionHNDepth  ->setValue(ui->horizontalSliderConversionHNDepth    ->value()/5.0);
    // gray scale preset
    imageProp.grayScalePreset.R = ui->horizontalSliderGrayScaleR->value()/255.0;
    imageProp.grayScalePreset.G = ui->horizontalSliderGrayScaleG->value()/255.0;
    imageProp.grayScalePreset.B = ui->horizontalSliderGrayScaleB->value()/255.0;

    imageProp.specularW1        = ui->doubleSpinBoxSpecularW1       ->value();
    imageProp.specularW2        = ui->doubleSpinBoxSpecularW2       ->value();
    imageProp.specularRadius    = ui->doubleSpinBoxSpecularRadius   ->value();
    imageProp.specularContrast  = ui->doubleSpinBoxSpecularContrast ->value();
    imageProp.specularAmplifier = ui->doubleSpinBoxSpecularAmplifier->value();
    imageProp.specularBrightness= ui->doubleSpinBoxSpecularBrightness->value();

    imageProp.noRemoveShadingGaussIter = ui->horizontalSliderRemoveShadingGaussIter->value();

    imageProp.noBlurPasses      = ui->horizontalSliderBlurNoPasses ->value();
    imageProp.smallDetails      = ui->horizontalSliderSmallDetails ->value()/100.0;
    imageProp.mediumDetails     = ui->horizontalSliderMediumDetails->value()/100.0;

    imageProp.detailDepth       = ui->horizontalSliderDetailDepth->value()/20.0;
    imageProp.sharpenBlurAmount = ui->horizontalSliderSharpenBlur->value();
    imageProp.normalsStep       = ui->horizontalSliderNormalsStep->value()/100.0;

    imageProp.conversionHNDepth         = ui->doubleSpinBoxConversionHNDepth->value();

    imageProp.conversionNHItersHuge     = ui->horizontalSliderNormalToHeightItersHuge->value();
    imageProp.conversionNHItersVeryLarge= ui->horizontalSliderNormalToHeightItersVeryLarge->value();
    imageProp.conversionNHItersLarge    = ui->horizontalSliderNormalToHeightItersLarge->value();
    imageProp.conversionNHItersMedium   = ui->horizontalSliderNormalToHeightItersMedium->value();
    imageProp.conversionNHItersSmall    = ui->horizontalSliderNormalToHeightItersSmall->value();
    imageProp.conversionNHItersVerySmall= ui->horizontalSliderNormalToHeightItersVerySmall->value();

    imageProp.conversionBaseMapAmplitude    = ui->horizontalSliderBaseMapAmplidute->value()/100.0;
    imageProp.conversionBaseMapFlatness     = ui->horizontalSliderConversionBaseMapFlatness->value()/100.0;
    imageProp.conversionBaseMapNoIters      = ui->horizontalSliderBMNoIters->value();
    imageProp.conversionBaseMapFilterRadius = ui->horizontalSliderConversionBMFilterRadius->value();
    imageProp.conversionBaseMapMixNormals   = ui->horizontalSliderConversionBaseMapMixing->value()/200.0;
    imageProp.conversionBaseMapPreSmoothRadius= ui->horizontalSliderConversionBMPreSmoothRadius->value()/10.0;
    imageProp.conversionBaseMapBlending     = ui->horizontalSliderConversionBaseMapBlending->value()/100.0;

    imageProp.ssaoNoIters   = ui->horizontalSliderSSAONoIters->value();
    imageProp.ssaoDepth     = ui->horizontalSliderSSAODepth->value()/100.0;
    imageProp.ssaoIntensity = ui->horizontalSliderSSAOIntensity->value()/100.0;
    imageProp.ssaoBias      = ui->horizontalSliderSSAOBias->value()/100.0;

    ui->doubleSpinBoxSSAONoIters->setValue(imageProp.ssaoNoIters);
    ui->doubleSpinBoxSSAODepth  ->setValue(imageProp.ssaoDepth);
    ui->doubleSpinBoxSSAOBias   ->setValue(imageProp.ssaoBias);
    ui->doubleSpinBoxSSAOIntensity->setValue(imageProp.ssaoIntensity);

    imageProp.heightMinValue        = ui->horizontalSliderHeightProcMinValue->value()/200.0;
    imageProp.heightMaxValue        = ui->horizontalSliderHeightProcMaxValue->value()/200.0;
    imageProp.heightAveragingRadius = ui->horizontalSliderHeightAveRadius->value();

    ui->labelHeightProcMinValue->setText(QString::number(imageProp.heightMinValue));
    ui->labelHeightProcMaxValue->setText(QString::number(imageProp.heightMaxValue));

}

void FormImageProp::updateSlidersOnRelease(){
    if(bLoading == true) return;
    updateGuiSpinBoxesAndLabes(0);
    emit imageChanged();
}

void FormImageProp::updateGuiCheckBoxes(){
    if(bLoading == true) return;

    imageProp.bRemoveShading  = ui->checkBoxRemoveShading->isChecked();
    imageProp.bSpeclarControl = ui->checkBoxSpecularControl->isChecked();

    imageProp.bGrayScale            = ui->checkBoxGrayScale->isChecked();
    imageProp.bInvertB              = ui->checkBoxInvertB->isChecked();
    imageProp.bInvertR              = ui->checkBoxInvertR->isChecked();
    imageProp.bInvertG              = ui->checkBoxInvertG->isChecked();

    imageProp.bConversionHN         = ui->checkBoxEnableHeightToNormal->isChecked();
    imageProp.bConversionNH         = ui->checkBoxEnableNormalToHeight->isChecked();
    imageProp.bConversionBaseMap    = ui->checkBoxEnableBaseMapToOthers->isChecked();

    // one must treat height separately
    if(imageProp.imageType == HEIGHT_TEXTURE) FBOImageProporties::bAttachNormalToHeightMap = ui->checkBoxConversionHNAttachToNormal->isChecked();

    emit imageChanged();
}

void FormImageProp::applyHeightToNormalConversion(){
    conversionHeightToNormalApplied();
}
void FormImageProp::applyNormalToHeightConversion(){
   conversionNormalToHeightApplied();
}

void FormImageProp::applyBaseConversionConversion(){
    conversionBaseConversionApplied();
}

void FormImageProp::toggleHNPreviewMode(){
    repaintNormalTexture();
}
void FormImageProp::toggleAttachToNormal(bool toggle){
 ui->checkBoxConversionHNAttachToNormal->setChecked(toggle);
}


void FormImageProp::showHeightCalculatorDialog(){

     heightCalculator->setImageSize(imageProp.ref_fbo->width(),imageProp.ref_fbo->height());
     unsigned int result = heightCalculator->exec();
     if(result == QDialog::Accepted){
        ui->horizontalSliderConversionHNDepth->setValue(heightCalculator->getDepthInPixels()*5);
        updateSlidersOnRelease();
        qDebug() << "Height map::Depth calculated:" << heightCalculator->getDepthInPixels();
     }
}


void FormImageProp::hideSpecularGroupBox(){
    ui->groupBoxSpecular->hide();
}

void FormImageProp::hideHNGroupBox(){
    ui->checkBoxConversionHNAttachToNormal->setChecked(false);
    ui->groupBoxHN->hide();
}
void FormImageProp::hideNHGroupBox(){
    ui->groupBoxNH->hide();
}
void FormImageProp::hideBMGroupBox(){
    ui->groupBoxBM->hide();
}

void FormImageProp::hideNormalStepBar(){
    ui->labelNormalStep->hide();
    ui->labelNormalStepValue->hide();
    ui->horizontalSliderNormalsStep->hide();
}

void FormImageProp::hideSSAOBar(){
    ui->groupBoxSSAO->hide();
}

void FormImageProp::hideHeightProcessingBox(){
    ui->groupBoxHeightProcessing->hide();
}

void FormImageProp::hideGrayScaleControl(){
    ui->checkBoxGrayScale->hide();
}

void FormImageProp::dropEvent(QDropEvent *event)
{

    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int i = 0;
    QString localPath = droppedUrls[i].toLocalFile();
    QFileInfo fileInfo(localPath);

    loadFile(fileInfo.absoluteFilePath());

    event->acceptProposedAction();

}

void FormImageProp::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasText() || event->mimeData()->hasImage()) {
        event->acceptProposedAction();
    }
}

void FormImageProp::reloadSettings(){
    bLoading = true;
    ui->horizontalSliderGrayScaleR->setValue(imageProp.grayScalePreset.R*255);
    ui->horizontalSliderGrayScaleG->setValue(imageProp.grayScalePreset.G*255);
    ui->horizontalSliderGrayScaleB->setValue(imageProp.grayScalePreset.B*255);

    ui->checkBoxSpecularControl->setChecked(imageProp.bSpeclarControl);
    ui->checkBoxRemoveShading  ->setChecked(imageProp.bRemoveShading);
    ui->checkBoxGrayScale      ->setChecked(imageProp.bGrayScale);

    ui->checkBoxInvertB->setChecked(imageProp.bInvertB);
    ui->checkBoxInvertR->setChecked(imageProp.bInvertR);
    ui->checkBoxInvertG->setChecked(imageProp.bInvertG);

    ui->checkBoxEnableHeightToNormal    ->setChecked(imageProp.bConversionHN);
    ui->checkBoxEnableNormalToHeight    ->setChecked(imageProp.bConversionNH);
    ui->checkBoxEnableBaseMapToOthers   ->setChecked(imageProp.bConversionBaseMap);

    ui->horizontalSliderRemoveShadingGaussIter->setValue(imageProp.noRemoveShadingGaussIter);


    ui->horizontalSliderSpeculatW1        ->setValue(imageProp.specularW1*100);
    ui->horizontalSliderSpecularW2        ->setValue(imageProp.specularW2*100);
    ui->horizontalSliderSpecularRadius    ->setValue(imageProp.specularRadius);
    ui->horizontalSliderSpecularContrast  ->setValue(imageProp.specularContrast*100+100);
    ui->horizontalSliderSpecularAmplifier ->setValue(imageProp.specularAmplifier*10);
    ui->horizontalSliderSpecularBrightness->setValue(imageProp.specularBrightness*100);


    ui->horizontalSliderBlurNoPasses    ->setValue(imageProp.noBlurPasses);
    ui->horizontalSliderSmallDetails    ->setValue(imageProp.smallDetails*100);
    ui->horizontalSliderMediumDetails   ->setValue(imageProp.mediumDetails*100);

    ui->horizontalSliderDetailDepth->setValue(imageProp.detailDepth*20);
    ui->horizontalSliderSharpenBlur->setValue(imageProp.sharpenBlurAmount);
    ui->horizontalSliderNormalsStep->setValue(imageProp.normalsStep*100);

    ui->horizontalSliderHeightAveRadius     ->setValue(imageProp.heightAveragingRadius);
    ui->horizontalSliderHeightProcMaxValue  ->setValue(imageProp.heightMaxValue*200);
    ui->horizontalSliderHeightProcMinValue  ->setValue(imageProp.heightMinValue*200);
    ui->labelHeightProcMinValue             ->setText(QString::number(imageProp.heightMinValue));
    ui->labelHeightProcMaxValue             ->setText(QString::number(imageProp.heightMaxValue));

    ui->horizontalSliderConversionHNDepth           ->setValue(imageProp.conversionHNDepth*5);
    ui->doubleSpinBoxConversionHNDepth              ->setValue(imageProp.conversionHNDepth);

    ui->horizontalSliderNormalToHeightItersHuge     ->setValue(imageProp.conversionNHItersHuge);
    ui->horizontalSliderNormalToHeightItersVeryLarge->setValue(imageProp.conversionNHItersVeryLarge);
    ui->horizontalSliderNormalToHeightItersLarge    ->setValue(imageProp.conversionNHItersLarge);
    ui->horizontalSliderNormalToHeightItersMedium   ->setValue(imageProp.conversionNHItersMedium);
    ui->horizontalSliderNormalToHeightItersVerySmall->setValue(imageProp.conversionNHItersVerySmall);
    ui->horizontalSliderNormalToHeightItersSmall    ->setValue(imageProp.conversionNHItersSmall);

    ui->horizontalSliderBaseMapAmplidute            ->setValue(imageProp.conversionBaseMapAmplitude*100);
    ui->horizontalSliderConversionBaseMapFlatness   ->setValue(imageProp.conversionBaseMapFlatness*100);
    ui->horizontalSliderBMNoIters                   ->setValue(imageProp.conversionBaseMapNoIters);

    ui->horizontalSliderConversionBMFilterRadius    ->setValue(imageProp.conversionBaseMapFilterRadius);
    ui->horizontalSliderConversionBaseMapMixing     ->setValue(imageProp.conversionBaseMapMixNormals*200);
    ui->horizontalSliderConversionBaseMapBlending   ->setValue(imageProp.conversionBaseMapBlending*100);


    ui->horizontalSliderSSAONoIters     ->setValue(imageProp.ssaoNoIters);
    ui->horizontalSliderSSAOBias        ->setValue(imageProp.ssaoBias*100);
    ui->horizontalSliderSSAOIntensity   ->setValue(imageProp.ssaoIntensity*100);
    ui->horizontalSliderSSAODepth       ->setValue(imageProp.ssaoDepth*100);

    ui->doubleSpinBoxSSAONoIters        ->setValue(imageProp.ssaoNoIters);
    ui->doubleSpinBoxSSAODepth          ->setValue(imageProp.ssaoDepth);
    ui->doubleSpinBoxSSAOBias           ->setValue(imageProp.ssaoBias);
    ui->doubleSpinBoxSSAOIntensity      ->setValue(imageProp.ssaoIntensity);


    bLoading = false;
    ui->horizontalSliderConversionBMPreSmoothRadius->setValue(imageProp.conversionBaseMapPreSmoothRadius*10);
    
    // forcing gray scale for specular image
    if(imageProp.imageType == SPECULAR_TEXTURE){
        ui->checkBoxGrayScale->setChecked(true);
        imageProp.bGrayScale = true;
    }

}

void FormImageProp::reloadImageSettings(){
    emit reloadSettingsFromConfigFile(imageProp.imageType);
}
