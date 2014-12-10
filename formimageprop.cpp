#include "formimageprop.h"
#include "ui_formimageprop.h"
QDir* FormImageProp::recentDir;

FormImageProp::FormImageProp( QWidget *parent,QGLWidget* qlW_ptr) :
    QWidget(parent),
    ui(new Ui::FormImageProp)
{
    ui->setupUi(this);
    imageProp.glWidget_ptr = qlW_ptr;
    connect(ui->pushButtonOpenImage,SIGNAL(released()),this,SLOT(open()));
    connect(ui->pushButtonSaveImage,SIGNAL(released()),this,SLOT(save()));

    connect(ui->checkBoxRemoveShading,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxGrayScale,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxInvertB,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxInvertR,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxInvertG,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));

    connect(ui->horizontalSliderRemoveShadingGaussIter,SIGNAL(valueChanged(int)),this,SLOT(setRemoveShadingGaussIter(int)));
    connect(ui->horizontalSliderBlurNoPasses,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderSmallDetails,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderMediumDetails,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));

    connect(ui->horizontalSliderDetailDepth,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));


    connect(ui->checkBoxSpecularControl,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderSpeculatW1,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderSpecularW2,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderSpecularRadius,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderSpecularContrast,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderSpecularAmplifier,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderSharpenBlur,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->horizontalSliderNormalsStep,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));


    connect(ui->horizontalSliderConversionHNDepth,SIGNAL(valueChanged(int)),this,SLOT(updateGuiSpinBoxes(int)));
    connect(ui->checkBoxEnableHeightToNormal,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->pushButtonConverToNormal,SIGNAL(released()),this,SLOT(applyHeightToNormalConversion()));


    connect(ui->checkBoxEnableNormalToHeight,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderNormalToHeightItersHuge,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersVeryLarge,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersLarge,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersMedium,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersSmall,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersVerySmall,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->pushButtonConvertToHeight,SIGNAL(released()),this,SLOT(applyNormalToHeightConversion()));

    connect(ui->checkBoxEnableBaseMapToOthers,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderBaseMapAmplidute,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBaseMapFlatness,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderBMNoIters,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBMFilterRadius,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBaseMapMixing,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBMPreSmoothRadius,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderConversionBaseMapBlending,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->pushButtonConvertToNormalAndHeight,SIGNAL(released()),this,SLOT(applyBaseConversionConversion()));


    connect(ui->horizontalSliderSSAOBias,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAODepth,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAOIntensity,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAONoIters,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->pushButtonOcclusionRecalculate,SIGNAL(released()),this,SLOT(applyRecalculateOcclusion()));

    setAcceptDrops(true);


}

FormImageProp::~FormImageProp()
{
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
                       tr("Images (*.png *.jpg *.JPG *.PNG);;All files (*.*)"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

bool FormImageProp::loadFile(const QString &fileName)
{
    QImage _image(fileName);
    if (_image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));

        return false;
    }

    qDebug() << "<FormImageProp> Open image:" << fileName;
    QFileInfo fileInfo(fileName);
    imageName = fileInfo.baseName();
    (*recentDir).setPath(fileInfo.absolutePath());

    image    = _image;
    imageProp.init(image);

    imageChanged();
    return true;
}

void FormImageProp::save(){

    QStringList picturesLocations;
    if(recentDir == NULL ) picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    else{

         QFileInfo fileInfo(recentDir->absolutePath());
         QString fullFileName = fileInfo.absolutePath()+ "/" +
                                imageName + PostfixNames::getPostfix(imageProp.imageType)
                                + ".png";
         picturesLocations << fullFileName;
         qDebug() << "<FormImageProp>:: Saving to file:" << fullFileName;
    }


    QFileDialog dialog(this,
                       tr("Save to file"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first(),
                       tr("Images (*.png *.jpg *.JPG *.PNG);;All files (*.*)"));
    dialog.setDirectory(recentDir->absolutePath());
    dialog.setAcceptMode(QFileDialog::AcceptSave);


    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

bool FormImageProp::saveFile(const QString &fileName)
{
    qDebug() << "<FormImageProp> save image:" << fileName;
    QFileInfo fileInfo(fileName);
    (*recentDir).setPath(fileInfo.absolutePath());
    image = imageProp.getImage();
    image.save(fileName);

    return true;
}

void FormImageProp::saveFileToDir(const QString &dir){

    QString fullFileName = dir + "/" +
                           imageName + PostfixNames::getPostfix(imageProp.imageType)
                           + ".png";

    saveFile(fullFileName);
}

void FormImageProp::saveImageToDir(const QString &dir,QImage& image){

    QString fullFileName = dir + "/" +
                           imageName + PostfixNames::getPostfix(imageProp.imageType)
                           + ".png";

    qDebug() << "<FormImageProp> save image:" << fullFileName;
    QFileInfo fileInfo(fullFileName);
    (*recentDir).setPath(fileInfo.absolutePath());

    image.save(fullFileName);
}

void FormImageProp::setImage(QImage _image){
    image    = _image;
    imageProp.init(image);
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




void FormImageProp::updateGuiSpinBoxes(int){
    if(bLoading == true) return;
    ui->doubleSpinBoxSpecularW1->setValue(ui->horizontalSliderSpeculatW1->value()/100.0);
    ui->doubleSpinBoxSpecularW2->setValue(ui->horizontalSliderSpecularW2->value()/100.0);
    ui->doubleSpinBoxSpecularRadius->setValue(ui->horizontalSliderSpecularRadius->value()*1.0);
    ui->doubleSpinBoxSpecularContrast->setValue(ui->horizontalSliderSpecularContrast->value()/100.0-1.0);
    ui->doubleSpinBoxSpecularAmplifier->setValue(ui->horizontalSliderSpecularAmplifier->value()/10.0);

    imageProp.specularW1        = ui->doubleSpinBoxSpecularW1->value();
    imageProp.specularW2        = ui->doubleSpinBoxSpecularW2->value();
    imageProp.specularRadius    = ui->doubleSpinBoxSpecularRadius->value();
    imageProp.specularContrast  = ui->doubleSpinBoxSpecularContrast->value();
    imageProp.specularAmplifier = ui->doubleSpinBoxSpecularAmplifier->value();

    imageProp.noBlurPasses      = ui->horizontalSliderBlurNoPasses->value();
    imageProp.smallDetails      = ui->horizontalSliderSmallDetails->value()/100.0;
    imageProp.mediumDetails     = ui->horizontalSliderMediumDetails->value()/100.0;

    imageProp.detailDepth       = ui->horizontalSliderDetailDepth->value()/20.0;
    imageProp.sharpenBlurAmount = ui->horizontalSliderSharpenBlur->value();
    imageProp.normalsStep       = ui->horizontalSliderNormalsStep->value()/100.0;

    imageProp.conversionHNDepth         = ui->horizontalSliderConversionHNDepth->value()/30.0;
    imageProp.conversionNHItersHuge     = ui->horizontalSliderNormalToHeightItersHuge->value();
    imageProp.conversionNHItersVeryLarge= ui->horizontalSliderNormalToHeightItersVeryLarge->value();
    imageProp.conversionNHItersLarge    = ui->horizontalSliderNormalToHeightItersLarge->value();
    imageProp.conversionNHItersMedium   = ui->horizontalSliderNormalToHeightItersMedium->value();
    imageProp.conversionNHItersSmall    = ui->horizontalSliderNormalToHeightItersSmall->value();
    imageProp.conversionNHItersVerySmall= ui->horizontalSliderNormalToHeightItersVerySmall->value();

    imageProp.conversionBaseMapAmplitude = ui->horizontalSliderBaseMapAmplidute->value()/100.0;
    imageProp.conversionBaseMapFlatness  = ui->horizontalSliderConversionBaseMapFlatness->value()/100.0;
    imageProp.conversionBaseMapNoIters   = ui->horizontalSliderBMNoIters->value();
    imageProp.conversionBaseMapFilterRadius= ui->horizontalSliderConversionBMFilterRadius->value();
    imageProp.conversionBaseMapMixNormals= ui->horizontalSliderConversionBaseMapMixing->value()/200.0;
    imageProp.conversionBaseMapPreSmoothRadius= ui->horizontalSliderConversionBMPreSmoothRadius->value()/10.0;
    imageProp.conversionBaseMapBlending  = ui->horizontalSliderConversionBaseMapBlending->value()/100.0;




    imageProp.ssaoNoIters = ui->horizontalSliderSSAONoIters->value();
    imageProp.ssaoDepth   = ui->horizontalSliderSSAODepth->value()/100.0;
    imageProp.ssaoIntensity= ui->horizontalSliderSSAOIntensity->value()/100.0;
    imageProp.ssaoBias    = ui->horizontalSliderSSAOBias->value()/100.0;


    ui->doubleSpinBoxSSAONoIters->setValue(imageProp.ssaoNoIters);
    ui->doubleSpinBoxSSAODepth->setValue(imageProp.ssaoDepth);
    ui->doubleSpinBoxSSAOBias->setValue(imageProp.ssaoBias);
    ui->doubleSpinBoxSSAOIntensity->setValue(imageProp.ssaoIntensity);

    imageChanged();
}

void FormImageProp::updateSlidersOnRelease(){
    updateGuiSpinBoxes(0);
}

void FormImageProp::updateGuiCheckBoxes(){
    if(bLoading == true) return;
    imageProp.bSpeclarControl = ui->checkBoxSpecularControl->isChecked();
    imageProp.bRemoveShading  = ui->checkBoxRemoveShading->isChecked();
    imageProp.bGrayScale      = ui->checkBoxGrayScale->isChecked();
    imageProp.bInvertB        = ui->checkBoxInvertB->isChecked();
    imageProp.bInvertR        = ui->checkBoxInvertR->isChecked();
    imageProp.bInvertG        = ui->checkBoxInvertG->isChecked();



    imageProp.bConversionHN         = ui->checkBoxEnableHeightToNormal->isChecked();
    imageProp.bConversionNH         = ui->checkBoxEnableNormalToHeight->isChecked();
    imageProp.bConversionBaseMap    = ui->checkBoxEnableBaseMapToOthers->isChecked();


    imageChanged();
}

void FormImageProp::setRemoveShading(bool bVal){
    if(bLoading == true) return;
    imageProp.bRemoveShading = bVal;
    imageChanged();
}
void FormImageProp::setRemoveShadingGaussIter(int iVal){
    if(bLoading == true) return;
    imageProp.noRemoveShadingGaussIter = iVal;
    imageChanged();
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
void FormImageProp::applyRecalculateOcclusion(){
    recalculateOcclusion();
}

void FormImageProp::hideSpecularGroupBox(){
    ui->groupBoxSpecular->hide();
}

void FormImageProp::hideHNGroupBox(){
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
    ui->checkBoxSpecularControl->setChecked(imageProp.bSpeclarControl);
    ui->checkBoxRemoveShading->setChecked(imageProp.bRemoveShading);
    ui->checkBoxGrayScale->setChecked(imageProp.bGrayScale);
    ui->checkBoxInvertB->setChecked(imageProp.bInvertB);
    ui->checkBoxInvertR->setChecked(imageProp.bInvertR);
    ui->checkBoxInvertG->setChecked(imageProp.bInvertG);
    ui->checkBoxEnableHeightToNormal->setChecked(imageProp.bConversionHN);
    ui->checkBoxEnableNormalToHeight->setChecked(imageProp.bConversionNH);
    ui->checkBoxEnableBaseMapToOthers->setChecked(imageProp.bConversionBaseMap);

    ui->horizontalSliderRemoveShadingGaussIter->setValue(imageProp.noRemoveShadingGaussIter);


    ui->horizontalSliderSpeculatW1->setValue(imageProp.specularW1*100);
    ui->horizontalSliderSpecularW2->setValue(imageProp.specularW2*100);
    ui->horizontalSliderSpecularRadius->setValue(imageProp.specularRadius);
    ui->horizontalSliderSpecularContrast->setValue(imageProp.specularContrast*100+100);
    ui->horizontalSliderSpecularAmplifier->setValue(imageProp.specularAmplifier*10);


    ui->horizontalSliderBlurNoPasses->setValue(imageProp.noBlurPasses);
    ui->horizontalSliderSmallDetails->setValue(imageProp.smallDetails*100);
    ui->horizontalSliderMediumDetails->setValue(imageProp.mediumDetails*100);

    ui->horizontalSliderDetailDepth->setValue(imageProp.detailDepth*20);
    ui->horizontalSliderSharpenBlur->setValue(imageProp.sharpenBlurAmount);
    ui->horizontalSliderNormalsStep->setValue(imageProp.normalsStep*100);

    ui->horizontalSliderConversionHNDepth->setValue(imageProp.conversionHNDepth*30);

    ui->horizontalSliderNormalToHeightItersHuge->setValue(imageProp.conversionNHItersHuge);
    ui->horizontalSliderNormalToHeightItersVeryLarge->setValue(imageProp.conversionNHItersVeryLarge);
    ui->horizontalSliderNormalToHeightItersLarge->setValue(imageProp.conversionNHItersLarge);
    ui->horizontalSliderNormalToHeightItersMedium->setValue(imageProp.conversionNHItersMedium);
    ui->horizontalSliderNormalToHeightItersVerySmall->setValue(imageProp.conversionNHItersVerySmall);
    ui->horizontalSliderNormalToHeightItersSmall->setValue(imageProp.conversionNHItersSmall);

    ui->horizontalSliderBaseMapAmplidute->setValue(imageProp.conversionBaseMapAmplitude*100);
    ui->horizontalSliderConversionBaseMapFlatness->setValue(imageProp.conversionBaseMapFlatness*100);
    ui->horizontalSliderBMNoIters->setValue(imageProp.conversionBaseMapNoIters);

    ui->horizontalSliderConversionBMFilterRadius->setValue(imageProp.conversionBaseMapFilterRadius);
    ui->horizontalSliderConversionBaseMapMixing->setValue(imageProp.conversionBaseMapMixNormals*200);
    ui->horizontalSliderConversionBaseMapBlending->setValue(imageProp.conversionBaseMapBlending*100);

    ui->horizontalSliderSSAONoIters->setValue(imageProp.ssaoNoIters);
    ui->horizontalSliderSSAOBias->setValue(imageProp.ssaoBias*100);
    ui->horizontalSliderSSAOIntensity->setValue(imageProp.ssaoIntensity*100);
    ui->horizontalSliderSSAODepth->setValue(imageProp.ssaoDepth*100);

    ui->doubleSpinBoxSSAONoIters->setValue(imageProp.ssaoNoIters);
    ui->doubleSpinBoxSSAODepth->setValue(imageProp.ssaoDepth);
    ui->doubleSpinBoxSSAOBias->setValue(imageProp.ssaoBias);
    ui->doubleSpinBoxSSAOIntensity->setValue(imageProp.ssaoIntensity);


    bLoading = false;
    ui->horizontalSliderConversionBMPreSmoothRadius->setValue(imageProp.conversionBaseMapPreSmoothRadius*10);
}
