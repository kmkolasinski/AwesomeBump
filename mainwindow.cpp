#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    recentDir = NULL;
    bSaveCheckedImages = false;
    bSaveCompressedFormImages = false;
    FormImageProp::recentDir = &recentDir;
    glImage          = new GLImage(this);

    //glWidget         = new GLWidget;
    glWidget         = new GLWidget(this,glImage);
    diffuseImageProp = new FormImageProp(this,glImage);
    normalImageProp  = new FormImageProp(this,glImage);
    specularImageProp= new FormImageProp(this,glImage);
    heightImageProp  = new FormImageProp(this,glImage);
    occlusionImageProp= new FormImageProp(this,glImage);


    glWidget->setPointerToTexture(&diffuseImageProp->getImageProporties() ->fbo,DIFFUSE_TEXTURE);
    glWidget->setPointerToTexture(&normalImageProp->getImageProporties()  ->fbo,NORMAL_TEXTURE);
    glWidget->setPointerToTexture(&specularImageProp->getImageProporties()->fbo,SPECULAR_TEXTURE);
    glWidget->setPointerToTexture(&heightImageProp->getImageProporties()  ->fbo,HEIGHT_TEXTURE);
    glWidget->setPointerToTexture(&occlusionImageProp->getImageProporties()->fbo,OCCLUSION_TEXTURE);


    diffuseImageProp  ->getImageProporties()->imageType = DIFFUSE_TEXTURE;
    normalImageProp   ->getImageProporties()->imageType = NORMAL_TEXTURE;
    specularImageProp ->getImageProporties()->imageType = SPECULAR_TEXTURE;
    heightImageProp   ->getImageProporties()->imageType = HEIGHT_TEXTURE;
    occlusionImageProp ->getImageProporties()->imageType = OCCLUSION_TEXTURE;

    specularImageProp->setSpecularControlChecked();
    specularImageProp->getImageProporties()->bSpeclarControl = true;
    specularImageProp->hideBMGroupBox();
    specularImageProp->hideHNGroupBox();
    specularImageProp->hideNHGroupBox();
    specularImageProp->hideSSAOBar();
    specularImageProp->hideNormalStepBar();

    diffuseImageProp->hideHNGroupBox();
    diffuseImageProp->hideNHGroupBox();
    diffuseImageProp->hideSpecularGroupBox();
    diffuseImageProp->hideNormalStepBar();
    diffuseImageProp->hideSSAOBar();

    normalImageProp->hideHNGroupBox();
    normalImageProp->hideBMGroupBox();
    normalImageProp->hideSpecularGroupBox();
    normalImageProp->hideSSAOBar();

    heightImageProp->hideBMGroupBox();
    heightImageProp->hideNHGroupBox();
    heightImageProp->hideSpecularGroupBox();
    heightImageProp->hideNormalStepBar();
    heightImageProp->hideSSAOBar();

    occlusionImageProp->hideSpecularGroupBox();
    occlusionImageProp->hideBMGroupBox();
    occlusionImageProp->hideHNGroupBox();
    occlusionImageProp->hideNHGroupBox();
    occlusionImageProp->hideNormalStepBar();
    glImage ->targetImageNormal = normalImageProp->getImageProporties();
    glImage ->targetImageHeight = heightImageProp->getImageProporties();


    ui->setupUi(this);
    ui->horizontalLayout->addWidget(glWidget);
    ui->horizontalLayout->addWidget(glImage);


    ui->verticalLayoutDiffuseImage->addWidget(diffuseImageProp);
    ui->verticalLayoutNormalImage->addWidget(normalImageProp);
    ui->verticalLayoutSpecularImage->addWidget(specularImageProp);
    ui->verticalLayoutHeightImage->addWidget(heightImageProp);
    ui->verticalLayoutOcclusionImage->addWidget(occlusionImageProp);

    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),this,SLOT(updateImage(int)));

    connect(diffuseImageProp,SIGNAL(imageChanged()),this,SLOT(updateDiffuseImage()));
    connect(normalImageProp,SIGNAL(imageChanged()),this,SLOT(updateNormalImage()));
    connect(specularImageProp,SIGNAL(imageChanged()),this,SLOT(updateSpecularImage()));
    connect(heightImageProp,SIGNAL(imageChanged()),this,SLOT(updateHeightImage()));
    connect(occlusionImageProp,SIGNAL(imageChanged()),this,SLOT(updateOcclusionImage()));


    connect(heightImageProp,SIGNAL(conversionHeightToNormalApplied()) ,this,SLOT(convertFromHtoN()));
    connect(normalImageProp,SIGNAL(conversionNormalToHeightApplied()) ,this,SLOT(convertFromNtoH()));
    connect(diffuseImageProp,SIGNAL(conversionBaseConversionApplied()),this,SLOT(convertFromBase()));
    connect(occlusionImageProp,SIGNAL(recalculateOcclusion()),this,SLOT(recalculateOcclusion()));





    connect(ui->checkBoxMakeSeamless,SIGNAL(toggled(bool)),this,SLOT(enableMakeSeamless(bool)));
    connect(ui->horizontalSliderMakeSeamlessRadius,SIGNAL(valueChanged(int)),this,SLOT(setMakeSeamlessRadius(int)));
    connect(ui->horizontalSliderDepthScale,SIGNAL(valueChanged(int)),glWidget,SLOT(setDepthScale(int)));
    connect(ui->horizontalSliderUVScale,SIGNAL(valueChanged(int)),glWidget,SLOT(setUVScale(int)));

    connect(ui->horizontalSliderMakeSeamlessRadius,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderDepthScale,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVScale,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVXOffset,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVYOffset,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));


    connect(ui->pushButtonSaveAll,SIGNAL(released()),this,SLOT(saveImages()));
    connect(ui->pushButtonSaveChecked,SIGNAL(released()),this,SLOT(saveCheckedImages()));
    connect(ui->pushButtonSaveAs,SIGNAL(released()),this,SLOT(saveCompressedForm()));

    connect(ui->pushButtonReplotAll,SIGNAL(released()),this,SLOT(replotAllImages()));
    connect(ui->pushButtonToggleDiffuse,SIGNAL(toggled(bool)),glWidget,SLOT(toggleDiffuseView(bool)));
    connect(ui->pushButtonToggleSpecular,SIGNAL(toggled(bool)),glWidget,SLOT(toggleSpecularView(bool)));
    connect(ui->pushButtonToggleOcclusion,SIGNAL(toggled(bool)),glWidget,SLOT(toggleOcclusionView(bool)));

    connect(ui->pushButtonSaveCurrentSettings,SIGNAL(released()),this,SLOT(saveSettings()));
    connect(ui->horizontalSliderSpecularI,SIGNAL(valueChanged(int)),this,SLOT(setSpecularIntensity(int)));
    connect(ui->horizontalSliderDiffuseI,SIGNAL(valueChanged(int)),this,SLOT(setDiffuseIntensity(int)));

    ui->progressBar->setValue(0);



    connect(ui->actionReplot,SIGNAL(triggered()),this,SLOT(replotAllImages()));
    connect(ui->actionShowDiffuseImage,SIGNAL(triggered()),this,SLOT(selectDiffuseTab()));
    connect(ui->actionShowNormalImage,SIGNAL(triggered()),this,SLOT(selectNormalTab()));
    connect(ui->actionShowSpecularImage,SIGNAL(triggered()),this,SLOT(selectSpecularTab()));
    connect(ui->actionShowHeightImage,SIGNAL(triggered()),this,SLOT(selectHeightTab()));
    connect(ui->actionShowOcclusiontImage,SIGNAL(triggered()),this,SLOT(selectOcclusionTab()));

    connect(ui->actionShowSettingsImage,SIGNAL(triggered()),this,SLOT(selectGeneralSettingsTab()));


    loadSettings();

    diffuseImageProp ->setImage(QImage(QString(":/content/logo_D.png")));
    normalImageProp  ->setImage(QImage(QString(":/content/logo_N.png")));
    specularImageProp->setImage(QImage(QString(":/content/logo_D.png")));
    heightImageProp  ->setImage(QImage(QString(":/content/logo_H.png")));
    occlusionImageProp->setImage(QImage(QString(":/content/logo_O.png")));
    glImage->setActiveImage(diffuseImageProp->getImageProporties());


}

MainWindow::~MainWindow()
{

    delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent( event );
    qDebug() << "<MainWindow> Close program.";

    QSettings settings("config.ini", QSettings::IniFormat);
    settings.setValue("d_win_w",this->width());
    settings.setValue("d_win_h",this->height());

    glWidget->close();
    glImage->close();

}
void MainWindow::showEvent(QShowEvent* event){
    QWidget::showEvent( event );
    qDebug() << "<MainWindow> Show window.";
    replotAllImages();


}

void MainWindow::replotAllImages(){
    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
    for(int i = 0 ; i < 5 ; i++){
        updateImage(i);
        glImage->repaint();
    }
    glImage->enableShadowRender(false);
    glImage->setActiveImage(lastActive);
}


void MainWindow::selectDiffuseTab(){
    ui->tabWidget->setCurrentIndex(0);
    updateImage(0);
}
void MainWindow::selectNormalTab(){
    ui->tabWidget->setCurrentIndex(1);
    updateImage(1);
}
void MainWindow::selectSpecularTab(){
    ui->tabWidget->setCurrentIndex(2);
    updateImage(2);
}
void MainWindow::selectHeightTab(){
    ui->tabWidget->setCurrentIndex(3);
    updateImage(3);
}
void MainWindow::selectOcclusionTab(){
    ui->tabWidget->setCurrentIndex(4);
    updateImage(4);
}

void MainWindow::selectGeneralSettingsTab(){
    ui->tabWidget->setCurrentIndex(5);
}

void MainWindow::saveImages(){

    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
                                                 recentDir.absolutePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if( dir != "" ) saveAllImages(dir);

}

bool MainWindow::saveAllImages(const QString &dir){
     QFileInfo fileInfo(dir);
    if (!fileInfo.exists()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot save to %1.").arg(QDir::toNativeSeparators(dir)));
        return false;
    }

    qDebug() << "<MainWindow>::Saving to dir:" << fileInfo.absoluteFilePath();

    diffuseImageProp  ->setImageName(ui->lineEditOutputName->text());
    normalImageProp   ->setImageName(ui->lineEditOutputName->text());
    heightImageProp   ->setImageName(ui->lineEditOutputName->text());
    specularImageProp ->setImageName(ui->lineEditOutputName->text());
    occlusionImageProp ->setImageName(ui->lineEditOutputName->text());

    replotAllImages();
    setCursor(Qt::WaitCursor);
    QCoreApplication::processEvents();
    ui->progressBar->setValue(0);

    if(!bSaveCompressedFormImages){
        ui->labelProgressInfo->setText("Saving diffuse image...");
        if(bSaveCheckedImages*ui->checkBoxSaveDiffuse->isChecked() || !bSaveCheckedImages ){
            diffuseImageProp ->saveFileToDir(dir);

        }
        ui->progressBar->setValue(20);


        ui->labelProgressInfo->setText("Saving normal image...");
        if(bSaveCheckedImages*ui->checkBoxSaveNormal->isChecked() || !bSaveCheckedImages ){
            normalImageProp  ->saveFileToDir(dir);

        }
        ui->progressBar->setValue(40);


        ui->labelProgressInfo->setText("Saving specular image...");
        if(bSaveCheckedImages*ui->checkBoxSaveSpecular->isChecked() || !bSaveCheckedImages ){
            specularImageProp->saveFileToDir(dir);

        }
        ui->progressBar->setValue(60);

        ui->labelProgressInfo->setText("Saving height image...");
        if(bSaveCheckedImages*ui->checkBoxSaveHeight->isChecked() || !bSaveCheckedImages ){
            occlusionImageProp  ->saveFileToDir(dir);
        }

         ui->progressBar->setValue(80);
        ui->labelProgressInfo->setText("Saving occlusion image...");
        if(bSaveCheckedImages*ui->checkBoxSaveOcclusion->isChecked() || !bSaveCheckedImages ){
            heightImageProp  ->saveFileToDir(dir);
        }
        ui->progressBar->setValue(100);

    }else{


        QGLFramebufferObject* diffuseFBOImage  = diffuseImageProp->getImageProporties()->fbo;
        QGLFramebufferObject* normalFBOImage   = normalImageProp->getImageProporties()->fbo;
        QGLFramebufferObject* specularFBOImage = specularImageProp->getImageProporties()->fbo;
        QGLFramebufferObject* heightFBOImage   = heightImageProp->getImageProporties()->fbo;

        QImage diffuseImage = diffuseFBOImage->toImage() ;
        QImage normalImage  = normalFBOImage->toImage();
        QImage heightImage  = specularFBOImage->toImage();
        QImage specularImage= heightFBOImage->toImage();

        ui->progressBar->setValue(20);
        ui->labelProgressInfo->setText("Preparing images...");

        QCoreApplication::processEvents();

        QImage newDiffuseImage = QImage(diffuseImage.width(), diffuseImage.height(), QImage::Format_ARGB32);
        QImage newNormalImage  = QImage(diffuseImage.width(), diffuseImage.height(), QImage::Format_ARGB32);



        unsigned char* newDiffuseBuffer  = newDiffuseImage.bits();
        unsigned char* newNormalBuffer   = newNormalImage.bits();
        unsigned char* srcDiffuseBuffer  = diffuseImage.bits();
        unsigned char* srcNormalBuffer   = normalImage.bits();
        unsigned char* srcSpecularBuffer = specularImage.bits();
        unsigned char* srcHeightBuffer   = heightImage.bits();

        //...

        int w = diffuseImage.width();
        int h = diffuseImage.height();

        switch(ui->comboBoxSaveAsOptions->currentIndex()){
            case( H_TO_D_AND_S_TO_N ):

            for(int i = 0; i <h; i++){
             for(int j = 0; j < w; j++){
              newDiffuseBuffer[4 * (i * w + j) + 0] = srcDiffuseBuffer[4 * (i * w + j)+0  ] ;
              newDiffuseBuffer[4 * (i * w + j) + 1] = srcDiffuseBuffer[4 * (i * w + j)+1  ] ;
              newDiffuseBuffer[4 * (i * w + j) + 2] = srcDiffuseBuffer[4 * (i * w + j)+2  ] ;
              newDiffuseBuffer[4 * (i * w + j) + 3] = srcHeightBuffer[4 * (i * w + j)+0  ] ;

              newNormalBuffer[4 * (i * w + j) + 0] = srcNormalBuffer[4 * (i * w + j)+0  ] ;
              newNormalBuffer[4 * (i * w + j) + 1] = srcNormalBuffer[4 * (i * w + j)+1  ] ;
              newNormalBuffer[4 * (i * w + j) + 2] = srcNormalBuffer[4 * (i * w + j)+2  ] ;
              newNormalBuffer[4 * (i * w + j) + 3] = srcSpecularBuffer[4 * (i * w + j)+0  ] ;
             }
            }

            break;
            case( S_TO_D_AND_H_TO_N):

            for(int i = 0; i <h; i++){
             for(int j = 0; j < w; j++){
              newDiffuseBuffer[4 * (i * w + j) + 0] = srcDiffuseBuffer[4 * (i * w + j)+0  ] ;
              newDiffuseBuffer[4 * (i * w + j) + 1] = srcDiffuseBuffer[4 * (i * w + j)+1  ] ;
              newDiffuseBuffer[4 * (i * w + j) + 2] = srcDiffuseBuffer[4 * (i * w + j)+2  ] ;
              newDiffuseBuffer[4 * (i * w + j) + 3] = srcSpecularBuffer[4 * (i * w + j)+0  ] ;

              newNormalBuffer[4 * (i * w + j) + 0] = srcNormalBuffer[4 * (i * w + j)+0  ] ;
              newNormalBuffer[4 * (i * w + j) + 1] = srcNormalBuffer[4 * (i * w + j)+1  ] ;
              newNormalBuffer[4 * (i * w + j) + 2] = srcNormalBuffer[4 * (i * w + j)+2  ] ;
              newNormalBuffer[4 * (i * w + j) + 3] = srcHeightBuffer[4 * (i * w + j)+0  ] ;
             }
            }
            break;
        }


        ui->progressBar->setValue(50);
        ui->labelProgressInfo->setText("Saving diffuse image...");

        diffuseImageProp->saveImageToDir(dir,newDiffuseImage);

        ui->progressBar->setValue(80);
        ui->labelProgressInfo->setText("Saving diffuse image...");
        normalImageProp->saveImageToDir(dir,newNormalImage);

    }// end of saveAsCompressedFormat

    QCoreApplication::processEvents();
    ui->progressBar->setValue(100);
    ui->labelProgressInfo->setText("Done!");
    setCursor(Qt::ArrowCursor);


    return true;
}

void MainWindow::saveCheckedImages(){
    bSaveCheckedImages = true;
    saveImages();
    bSaveCheckedImages = false;
}
void MainWindow::saveCompressedForm(){
    bSaveCompressedFormImages = true;
    saveImages();
    bSaveCompressedFormImages = false;
}

void MainWindow::updateDiffuseImage(){
    ui->lineEditOutputName->setText(diffuseImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();
}
void MainWindow::updateNormalImage(){
    ui->lineEditOutputName->setText(normalImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();
}
void MainWindow::updateSpecularImage(){
    ui->lineEditOutputName->setText(specularImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();
}
void MainWindow::updateHeightImage(){
    ui->lineEditOutputName->setText(heightImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();
}

void MainWindow::updateOcclusionImage(){
    ui->lineEditOutputName->setText(occlusionImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();

}

void MainWindow::initializeImages(){

}

void MainWindow::updateImage(int tType){
    switch(tType){
        case(DIFFUSE_TEXTURE ):
            glImage->setActiveImage(diffuseImageProp->getImageProporties());
            break;
        case(NORMAL_TEXTURE  ):
            glImage->setActiveImage(normalImageProp->getImageProporties());
            break;
        case(SPECULAR_TEXTURE):
            glImage->setActiveImage(specularImageProp->getImageProporties());
            break;
        case(HEIGHT_TEXTURE  ):
            glImage->setActiveImage(heightImageProp->getImageProporties());
            break;
        case(OCCLUSION_TEXTURE  ):
            glImage->setActiveImage(occlusionImageProp->getImageProporties());
            break;
    }
    glWidget->repaint();
}


void MainWindow::enableMakeSeamless(bool bVal){
    FBOImageProporties::bMakeSeamless = bVal;
    glImage->repaint();
    glWidget->repaint();
}
void MainWindow::setMakeSeamlessRadius(int iVal){
    FBOImageProporties::MakeSeamlessRadius = iVal/100.0;
    glImage->repaint();
    glWidget->repaint();
}

void MainWindow::setSpecularIntensity(int val){
    double d = val/50.0;
    ui->doubleSpinBoxSpecularI->setValue(d);
    glWidget->setSpecularIntensity(d);
}
void MainWindow::setDiffuseIntensity(int val){
    double d = val/50.0;
    ui->doubleSpinBoxDiffuseI->setValue(d);
    glWidget->setDiffuseIntensity(d);
}
void MainWindow::updateSpinBoxes(int){
    ui->doubleSpinBoxMakeSeamless->setValue(ui->horizontalSliderMakeSeamlessRadius->value()/100.0);
    ui->doubleSpinBoxDepthScale->setValue(ui->horizontalSliderDepthScale->value()/100.0);
    ui->doubleSpinBoxUVScale->setValue(ui->horizontalSliderUVScale->value()/10.0);
    ui->doubleSpinBoxUVXOffset->setValue(ui->horizontalSliderUVXOffset->value()/100.0);
    ui->doubleSpinBoxUVYOffset->setValue(ui->horizontalSliderUVYOffset->value()/100.0);

    glWidget->setUVScaleOffset(ui->doubleSpinBoxUVXOffset->value(),ui->doubleSpinBoxUVYOffset->value());
}


void MainWindow::convertFromHtoN(){
    qDebug() << "Conversion from height to normal applied";
    glImage->targetImage = normalImageProp->getImageProporties();
    glImage->repaint();
    glWidget->repaint();
}

void MainWindow::convertFromNtoH(){
    qDebug() << "Conversion from normal to height applied";
    glImage->targetImage = heightImageProp->getImageProporties();
    glImage->repaint();
    glWidget->repaint();
}


void MainWindow::convertFromBase(){
    qDebug() << "Conversion from Base";

    normalImageProp   ->setImageName(diffuseImageProp->getImageName());
    heightImageProp   ->setImageName(diffuseImageProp->getImageName());
    specularImageProp ->setImageName(diffuseImageProp->getImageName());
    occlusionImageProp->setImageName(diffuseImageProp->getImageName());

    glImage ->targetImage          = normalImageProp->getImageProporties();
    glImage ->targetImage2         = heightImageProp->getImageProporties();
    glImage ->targetImageSpecular  = specularImageProp->getImageProporties();
    glImage ->targetImageOcclusion = occlusionImageProp->getImageProporties();


    glImage ->repaint();
    glWidget->repaint();
}

void MainWindow::recalculateOcclusion(){
    glImage->enableRecalculateOcclusion(true);
    glImage ->repaint();
    glWidget->repaint();
}

QSize MainWindow::sizeHint() const
{
    QSettings settings("config.ini", QSettings::IniFormat);
    return QSize(settings.value("d_win_w","800").toInt(),settings.value("d_win_h","600").toInt());
}

void MainWindow::saveSettings(){
    qDebug() << "<MainWindow>:: saving settings";
    QSettings settings("config.ini", QSettings::IniFormat);
    PostfixNames::diffuseName   = ui->lineEditPostfixDiffuse->text();
    PostfixNames::normalName    = ui->lineEditPostfixNormal->text();
    PostfixNames::specularName  = ui->lineEditPostfixSpecular->text();
    PostfixNames::heightName    = ui->lineEditPostfixHeight->text();
    PostfixNames::occlusionName = ui->lineEditPostfixOcclusion->text();


    settings.setValue("d_postfix",ui->lineEditPostfixDiffuse->text());
    settings.setValue("n_postfix",ui->lineEditPostfixNormal->text());
    settings.setValue("s_postfix",ui->lineEditPostfixSpecular->text());
    settings.setValue("h_postfix",ui->lineEditPostfixHeight->text());
    settings.setValue("o_postfix",ui->lineEditPostfixOcclusion->text());
    settings.setValue("recent_dir",recentDir.absolutePath());




    // diffuse settings
    settings.setValue("t_d_bGrayScale",diffuseImageProp->getImageProporties()->bGrayScale);
    settings.setValue("t_d_bInvertR",diffuseImageProp->getImageProporties()->bInvertR);
    settings.setValue("t_d_bInvertG",diffuseImageProp->getImageProporties()->bInvertG);
    settings.setValue("t_d_bInvertB",diffuseImageProp->getImageProporties()->bInvertB);
    settings.setValue("t_d_bRemoveShading",diffuseImageProp->getImageProporties()->bRemoveShading);
    settings.setValue("t_d_noRemoveShadingGaussIter",diffuseImageProp->getImageProporties()->noRemoveShadingGaussIter);
    settings.setValue("t_d_noBlurPasses",diffuseImageProp->getImageProporties()->noBlurPasses);
    settings.setValue("t_d_bSpeclarControl",diffuseImageProp->getImageProporties()->bSpeclarControl);
    settings.setValue("t_d_specularRadius",diffuseImageProp->getImageProporties()->specularRadius);
    settings.setValue("t_d_specularW1",diffuseImageProp->getImageProporties()->specularW1);
    settings.setValue("t_d_specularW2",diffuseImageProp->getImageProporties()->specularW2);
    settings.setValue("t_d_specularContrast",diffuseImageProp->getImageProporties()->specularContrast);
    settings.setValue("t_d_specularAmplifier",diffuseImageProp->getImageProporties()->specularAmplifier);
    settings.setValue("t_d_smallDetails",diffuseImageProp->getImageProporties()->smallDetails);
    settings.setValue("t_d_mediumDetails",diffuseImageProp->getImageProporties()->mediumDetails);
    settings.setValue("t_d_detailDepth",diffuseImageProp->getImageProporties()->detailDepth);
    settings.setValue("t_d_sharpenBlurAmount",diffuseImageProp->getImageProporties()->sharpenBlurAmount);
    settings.setValue("t_d_normalsStep",diffuseImageProp->getImageProporties()->normalsStep);
    settings.setValue("t_d_conversionHNDepth",diffuseImageProp->getImageProporties()->conversionHNDepth);
    settings.setValue("t_d_bConversionHN",diffuseImageProp->getImageProporties()->bConversionHN);
    settings.setValue("t_d_bConversionNH",diffuseImageProp->getImageProporties()->bConversionNH);

    settings.setValue("t_d_conversionNHItersHuge",diffuseImageProp->getImageProporties()->conversionNHItersHuge);
    settings.setValue("t_d_conversionNHItersVeryLarge",diffuseImageProp->getImageProporties()->conversionNHItersVeryLarge);
    settings.setValue("t_d_conversionNHItersLarge",diffuseImageProp->getImageProporties()->conversionNHItersLarge);
    settings.setValue("t_d_conversionNHItersMedium",diffuseImageProp->getImageProporties()->conversionNHItersMedium);
    settings.setValue("t_d_conversionNHItersVerySmall",diffuseImageProp->getImageProporties()->conversionNHItersVerySmall);
    settings.setValue("t_d_conversionNHItersSmall",diffuseImageProp->getImageProporties()->conversionNHItersSmall);

    settings.setValue("t_d_bConversionBaseMap",diffuseImageProp->getImageProporties()->bConversionBaseMap);
    settings.setValue("t_d_conversionBaseMapAmplitude",diffuseImageProp->getImageProporties()->conversionBaseMapAmplitude);
    settings.setValue("t_d_conversionBaseMapFlatness",diffuseImageProp->getImageProporties()->conversionBaseMapFlatness);
    settings.setValue("t_d_conversionBaseMapNoIters",diffuseImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_d_conversionBaseMapNoIters",diffuseImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_d_conversionBaseMapFilterRadius",diffuseImageProp->getImageProporties()->conversionBaseMapFilterRadius);
    settings.setValue("t_d_conversionBaseMapMixNormals",diffuseImageProp->getImageProporties()->conversionBaseMapMixNormals);
    settings.setValue("t_d_conversionBaseMapPreSmoothRadius",diffuseImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius);
    settings.setValue("t_d_conversionBaseMapBlending",diffuseImageProp->getImageProporties()->conversionBaseMapBlending);


    settings.setValue("t_n_bGrayScale",normalImageProp->getImageProporties()->bGrayScale);
    settings.setValue("t_n_bInvertR",normalImageProp->getImageProporties()->bInvertR);
    settings.setValue("t_n_bInvertG",normalImageProp->getImageProporties()->bInvertG);
    settings.setValue("t_n_bInvertB",normalImageProp->getImageProporties()->bInvertB);
    settings.setValue("t_n_bRemoveShading",normalImageProp->getImageProporties()->bRemoveShading);
    settings.setValue("t_n_noRemoveShadingGaussIter",normalImageProp->getImageProporties()->noRemoveShadingGaussIter);
    settings.setValue("t_n_noBlurPasses",normalImageProp->getImageProporties()->noBlurPasses);
    settings.setValue("t_n_bSpeclarControl",normalImageProp->getImageProporties()->bSpeclarControl);
    settings.setValue("t_n_specularRadius",normalImageProp->getImageProporties()->specularRadius);
    settings.setValue("t_n_specularW1",normalImageProp->getImageProporties()->specularW1);
    settings.setValue("t_n_specularW2",normalImageProp->getImageProporties()->specularW2);
    settings.setValue("t_n_specularContrast",normalImageProp->getImageProporties()->specularContrast);
    settings.setValue("t_n_specularAmplifier",normalImageProp->getImageProporties()->specularAmplifier);
    settings.setValue("t_n_smallDetails",normalImageProp->getImageProporties()->smallDetails);
    settings.setValue("t_n_mediumDetails",normalImageProp->getImageProporties()->mediumDetails);
    settings.setValue("t_n_detailDepth",normalImageProp->getImageProporties()->detailDepth);
    settings.setValue("t_n_sharpenBlurAmount",normalImageProp->getImageProporties()->sharpenBlurAmount);
    settings.setValue("t_n_normalsStep",normalImageProp->getImageProporties()->normalsStep);
    settings.setValue("t_n_conversionHNDepth",normalImageProp->getImageProporties()->conversionHNDepth);
    settings.setValue("t_n_bConversionHN",normalImageProp->getImageProporties()->bConversionHN);
    settings.setValue("t_n_bConversionNH",normalImageProp->getImageProporties()->bConversionNH);
    settings.setValue("t_n_conversionNHItersHuge",normalImageProp->getImageProporties()->conversionNHItersHuge);
    settings.setValue("t_n_conversionNHItersVeryLarge",normalImageProp->getImageProporties()->conversionNHItersVeryLarge);
    settings.setValue("t_n_conversionNHItersLarge",normalImageProp->getImageProporties()->conversionNHItersLarge);
    settings.setValue("t_n_conversionNHItersMedium",normalImageProp->getImageProporties()->conversionNHItersMedium);
    settings.setValue("t_n_conversionNHItersVerySmall",normalImageProp->getImageProporties()->conversionNHItersVerySmall);
    settings.setValue("t_n_conversionNHItersSmall",normalImageProp->getImageProporties()->conversionNHItersSmall);
    settings.setValue("t_n_bConversionBaseMap",normalImageProp->getImageProporties()->bConversionBaseMap);
    settings.setValue("t_n_conversionBaseMapAmplitude",normalImageProp->getImageProporties()->conversionBaseMapAmplitude);
    settings.setValue("t_n_conversionBaseMapFlatness",normalImageProp->getImageProporties()->conversionBaseMapFlatness);
    settings.setValue("t_n_conversionBaseMapNoIters",normalImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_n_conversionBaseMapNoIters",normalImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_n_conversionBaseMapFilterRadius",normalImageProp->getImageProporties()->conversionBaseMapFilterRadius);
    settings.setValue("t_n_conversionBaseMapMixNormals",normalImageProp->getImageProporties()->conversionBaseMapMixNormals);
    settings.setValue("t_n_conversionBaseMapPreSmoothRadius",normalImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius);

    settings.setValue("t_s_bGrayScale",specularImageProp->getImageProporties()->bGrayScale);
    settings.setValue("t_s_bInvertR",specularImageProp->getImageProporties()->bInvertR);
    settings.setValue("t_s_bInvertG",specularImageProp->getImageProporties()->bInvertG);
    settings.setValue("t_s_bInvertB",specularImageProp->getImageProporties()->bInvertB);
    settings.setValue("t_s_bRemoveShading",specularImageProp->getImageProporties()->bRemoveShading);
    settings.setValue("t_s_noRemoveShadingGaussIter",specularImageProp->getImageProporties()->noRemoveShadingGaussIter);
    settings.setValue("t_s_noBlurPasses",specularImageProp->getImageProporties()->noBlurPasses);
    settings.setValue("t_s_bSpeclarControl",specularImageProp->getImageProporties()->bSpeclarControl);
    settings.setValue("t_s_specularRadius",specularImageProp->getImageProporties()->specularRadius);
    settings.setValue("t_s_specularW1",specularImageProp->getImageProporties()->specularW1);
    settings.setValue("t_s_specularW2",specularImageProp->getImageProporties()->specularW2);
    settings.setValue("t_s_specularContrast",specularImageProp->getImageProporties()->specularContrast);
    settings.setValue("t_s_specularAmplifier",specularImageProp->getImageProporties()->specularAmplifier);
    settings.setValue("t_s_smallDetails",specularImageProp->getImageProporties()->smallDetails);
    settings.setValue("t_s_mediumDetails",specularImageProp->getImageProporties()->mediumDetails);
    settings.setValue("t_s_detailDepth",specularImageProp->getImageProporties()->detailDepth);
    settings.setValue("t_s_sharpenBlurAmount",specularImageProp->getImageProporties()->sharpenBlurAmount);
    settings.setValue("t_s_normalsStep",specularImageProp->getImageProporties()->normalsStep);
    settings.setValue("t_s_conversionHNDepth",specularImageProp->getImageProporties()->conversionHNDepth);
    settings.setValue("t_s_bConversionHN",specularImageProp->getImageProporties()->bConversionHN);
    settings.setValue("t_s_bConversionNH",specularImageProp->getImageProporties()->bConversionNH);

    settings.setValue("t_s_bConversionBaseMap",specularImageProp->getImageProporties()->bConversionBaseMap);
    settings.setValue("t_s_conversionBaseMapAmplitude",specularImageProp->getImageProporties()->conversionBaseMapAmplitude);
    settings.setValue("t_s_conversionBaseMapFlatness",specularImageProp->getImageProporties()->conversionBaseMapFlatness);
    settings.setValue("t_s_conversionBaseMapNoIters",specularImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_s_conversionBaseMapNoIters",specularImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_s_conversionBaseMapFilterRadius",specularImageProp->getImageProporties()->conversionBaseMapFilterRadius);
    settings.setValue("t_s_conversionBaseMapMixNormals",specularImageProp->getImageProporties()->conversionBaseMapMixNormals);
    settings.setValue("t_s_conversionBaseMapPreSmoothRadius",specularImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius);

    settings.setValue("t_h_bGrayScale",heightImageProp->getImageProporties()->bGrayScale);
    settings.setValue("t_h_bInvertR",heightImageProp->getImageProporties()->bInvertR);
    settings.setValue("t_h_bInvertG",heightImageProp->getImageProporties()->bInvertG);
    settings.setValue("t_h_bInvertB",heightImageProp->getImageProporties()->bInvertB);
    settings.setValue("t_h_bRemoveShading",heightImageProp->getImageProporties()->bRemoveShading);
    settings.setValue("t_h_noRemoveShadingGaussIter",heightImageProp->getImageProporties()->noRemoveShadingGaussIter);
    settings.setValue("t_h_noBlurPasses",heightImageProp->getImageProporties()->noBlurPasses);
    settings.setValue("t_h_bSpeclarControl",heightImageProp->getImageProporties()->bSpeclarControl);
    settings.setValue("t_h_specularRadius",heightImageProp->getImageProporties()->specularRadius);
    settings.setValue("t_h_specularW1",heightImageProp->getImageProporties()->specularW1);
    settings.setValue("t_h_specularW2",heightImageProp->getImageProporties()->specularW2);
    settings.setValue("t_h_specularContrast",heightImageProp->getImageProporties()->specularContrast);
    settings.setValue("t_h_specularAmplifier",heightImageProp->getImageProporties()->specularAmplifier);
    settings.setValue("t_h_smallDetails",heightImageProp->getImageProporties()->smallDetails);
    settings.setValue("t_h_mediumDetails",heightImageProp->getImageProporties()->mediumDetails);
    settings.setValue("t_h_detailDepth",heightImageProp->getImageProporties()->detailDepth);
    settings.setValue("t_h_sharpenBlurAmount",heightImageProp->getImageProporties()->sharpenBlurAmount);
    settings.setValue("t_h_normalsStep",heightImageProp->getImageProporties()->normalsStep);
    settings.setValue("t_h_conversionHNDepth",heightImageProp->getImageProporties()->conversionHNDepth);
    settings.setValue("t_h_bConversionHN",heightImageProp->getImageProporties()->bConversionHN);
    settings.setValue("t_h_bConversionNH",heightImageProp->getImageProporties()->bConversionNH);

    settings.setValue("t_h_bConversionBaseMap",heightImageProp->getImageProporties()->bConversionBaseMap);
    settings.setValue("t_h_conversionBaseMapAmplitude",heightImageProp->getImageProporties()->conversionBaseMapAmplitude);
    settings.setValue("t_h_conversionBaseMapFlatness",heightImageProp->getImageProporties()->conversionBaseMapFlatness);
    settings.setValue("t_h_conversionBaseMapNoIters",heightImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_h_conversionBaseMapNoIters",heightImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_h_conversionBaseMapFilterRadius",heightImageProp->getImageProporties()->conversionBaseMapFilterRadius);
    settings.setValue("t_h_conversionBaseMapMixNormals",heightImageProp->getImageProporties()->conversionBaseMapMixNormals);
    settings.setValue("t_h_conversionBaseMapPreSmoothRadius",heightImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius);

    settings.setValue("t_o_bGrayScale",occlusionImageProp->getImageProporties()->bGrayScale);
    settings.setValue("t_o_bInvertR",occlusionImageProp->getImageProporties()->bInvertR);
    settings.setValue("t_o_bInvertG",occlusionImageProp->getImageProporties()->bInvertG);
    settings.setValue("t_o_bInvertB",occlusionImageProp->getImageProporties()->bInvertB);
    settings.setValue("t_o_bRemoveShading",occlusionImageProp->getImageProporties()->bRemoveShading);
    settings.setValue("t_o_noRemoveShadingGaussIter",occlusionImageProp->getImageProporties()->noRemoveShadingGaussIter);
    settings.setValue("t_o_noBlurPasses",occlusionImageProp->getImageProporties()->noBlurPasses);
    settings.setValue("t_o_bSpeclarControl",occlusionImageProp->getImageProporties()->bSpeclarControl);
    settings.setValue("t_o_specularRadius",occlusionImageProp->getImageProporties()->specularRadius);
    settings.setValue("t_o_specularW1",occlusionImageProp->getImageProporties()->specularW1);
    settings.setValue("t_o_specularW2",occlusionImageProp->getImageProporties()->specularW2);
    settings.setValue("t_o_specularContrast",occlusionImageProp->getImageProporties()->specularContrast);
    settings.setValue("t_o_specularAmplifier",occlusionImageProp->getImageProporties()->specularAmplifier);
    settings.setValue("t_o_smallDetails",occlusionImageProp->getImageProporties()->smallDetails);
    settings.setValue("t_o_mediumDetails",occlusionImageProp->getImageProporties()->mediumDetails);
    settings.setValue("t_o_detailDepth",occlusionImageProp->getImageProporties()->detailDepth);
    settings.setValue("t_o_sharpenBlurAmount",occlusionImageProp->getImageProporties()->sharpenBlurAmount);
    settings.setValue("t_o_normalsStep",occlusionImageProp->getImageProporties()->normalsStep);
    settings.setValue("t_o_conversionHNDepth",occlusionImageProp->getImageProporties()->conversionHNDepth);
    settings.setValue("t_o_bConversionHN",occlusionImageProp->getImageProporties()->bConversionHN);
    settings.setValue("t_o_bConversionNH",occlusionImageProp->getImageProporties()->bConversionNH);

    settings.setValue("t_o_bConversionBaseMap",occlusionImageProp->getImageProporties()->bConversionBaseMap);
    settings.setValue("t_o_conversionBaseMapAmplitude",occlusionImageProp->getImageProporties()->conversionBaseMapAmplitude);
    settings.setValue("t_o_conversionBaseMapFlatness",occlusionImageProp->getImageProporties()->conversionBaseMapFlatness);
    settings.setValue("t_o_conversionBaseMapNoIters",occlusionImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_o_conversionBaseMapNoIters",occlusionImageProp->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_o_conversionBaseMapFilterRadius",occlusionImageProp->getImageProporties()->conversionBaseMapFilterRadius);
    settings.setValue("t_o_conversionBaseMapMixNormals",occlusionImageProp->getImageProporties()->conversionBaseMapMixNormals);
    settings.setValue("t_o_conversionBaseMapPreSmoothRadius",occlusionImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius);


    settings.setValue("t_o_ssaoNoIters",occlusionImageProp->getImageProporties()->ssaoNoIters);
    settings.setValue("t_o_ssaoBias",occlusionImageProp->getImageProporties()->ssaoBias);
    settings.setValue("t_o_ssaoDepth",occlusionImageProp->getImageProporties()->ssaoDepth);
    settings.setValue("t_o_ssaoIntensity",occlusionImageProp->getImageProporties()->ssaoIntensity);


}


void MainWindow::loadSettings(){


    qDebug() << "<MainWindow>:: loading settings";
    QSettings settings("config.ini", QSettings::IniFormat);

    this->resize(settings.value("d_win_w","800").toInt(),settings.value("d_win_h","600").toInt());

    PostfixNames::diffuseName   = settings.value("d_postfix","_d").toString();
    PostfixNames::normalName    = settings.value("n_postfix","_n").toString();
    PostfixNames::specularName  = settings.value("s_postfix","_s").toString();
    PostfixNames::heightName    = settings.value("h_postfix","_h").toString();
    PostfixNames::occlusionName = settings.value("o_postfix","_o").toString();

    ui->lineEditPostfixDiffuse->setText(PostfixNames::diffuseName);
    ui->lineEditPostfixNormal->setText(PostfixNames::normalName);
    ui->lineEditPostfixSpecular->setText(PostfixNames::specularName);
    ui->lineEditPostfixHeight->setText(PostfixNames::heightName);
    ui->lineEditPostfixOcclusion->setText(PostfixNames::occlusionName);



    recentDir = settings.value("recent_dir","").toString();

    // diffuse settings
    diffuseImageProp->getImageProporties()->bGrayScale = settings.value("t_d_bGrayScale","false").toBool();
    diffuseImageProp->getImageProporties()->bInvertR = settings.value("t_d_bInvertR","false").toBool();
    diffuseImageProp->getImageProporties()->bInvertG = settings.value("t_d_bInvertG","false").toBool();
    diffuseImageProp->getImageProporties()->bInvertB = settings.value("t_d_bInvertB","false").toBool();
    diffuseImageProp->getImageProporties()->bRemoveShading = settings.value("t_d_bRemoveShading","false").toBool();
    diffuseImageProp->getImageProporties()->noRemoveShadingGaussIter = settings.value("t_d_noRemoveShadingGaussIter","0").toInt();
    diffuseImageProp->getImageProporties()->noBlurPasses    = settings.value("t_d_noBlurPasses","0").toInt();
    diffuseImageProp->getImageProporties()->bSpeclarControl = settings.value("t_d_bSpeclarControl","false").toBool();
    diffuseImageProp->getImageProporties()->specularRadius    = settings.value("t_d_specularRadius","1").toInt();
    diffuseImageProp->getImageProporties()->specularW1    = settings.value("t_d_specularW1","0.0").toFloat();
    diffuseImageProp->getImageProporties()->specularW2    = settings.value("t_d_specularW2","0.0").toFloat();
    diffuseImageProp->getImageProporties()->specularContrast     = settings.value("t_d_specularContrast","0.0").toFloat();
    diffuseImageProp->getImageProporties()->specularAmplifier    = settings.value("t_d_specularAmplifier","0.0").toFloat();
    diffuseImageProp->getImageProporties()->smallDetails     = settings.value("t_d_smallDetails","0.0").toFloat();
    diffuseImageProp->getImageProporties()->mediumDetails    = settings.value("t_d_mediumDetails","0.0").toFloat();
    diffuseImageProp->getImageProporties()->detailDepth      = settings.value("t_d_detailDepth","0.0").toFloat();
    diffuseImageProp->getImageProporties()->sharpenBlurAmount   = settings.value("t_d_sharpenBlurAmount","0").toInt();
    diffuseImageProp->getImageProporties()->normalsStep         = settings.value("t_d_normalsStep","0.0").toFloat();
    diffuseImageProp->getImageProporties()->conversionHNDepth         = settings.value("t_d_conversionHNDepth","0.0").toFloat();
    diffuseImageProp->getImageProporties()->bConversionHN         = settings.value("t_d_bConversionHN","false").toBool();
    diffuseImageProp->getImageProporties()->bConversionNH         = settings.value("t_d_bConversionNH","false").toBool();

    diffuseImageProp->getImageProporties()->bConversionBaseMap  = settings.value("t_d_bConversionBaseMap","false").toBool();
    diffuseImageProp->getImageProporties()->conversionBaseMapAmplitude      = settings.value("t_d_conversionBaseMapAmplitude","0.0").toFloat();
    diffuseImageProp->getImageProporties()->conversionBaseMapFlatness       = settings.value("t_d_conversionBaseMapFlatness","0.0").toFloat();
    diffuseImageProp->getImageProporties()->conversionBaseMapNoIters        = settings.value("t_d_conversionBaseMapNoIters","0").toInt();
    diffuseImageProp->getImageProporties()->conversionBaseMapFilterRadius   = settings.value("t_d_conversionBaseMapFilterRadius","0").toInt();
    diffuseImageProp->getImageProporties()->conversionBaseMapMixNormals          = settings.value("t_d_conversionBaseMapMixNormals","0").toFloat();
    diffuseImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius     = settings.value("t_d_conversionBaseMapPreSmoothRadius","0").toFloat();
    diffuseImageProp->getImageProporties()->conversionBaseMapBlending = settings.value("t_d_conversionBaseMapBlending","0").toFloat();

    diffuseImageProp->reloadSettings();

    normalImageProp->getImageProporties()->bGrayScale = settings.value("t_n_bGrayScale","false").toBool();
    normalImageProp->getImageProporties()->bInvertR = settings.value("t_n_bInvertR","false").toBool();
    normalImageProp->getImageProporties()->bInvertG = settings.value("t_n_bInvertG","false").toBool();
    normalImageProp->getImageProporties()->bInvertB = settings.value("t_n_bInvertB","false").toBool();
    normalImageProp->getImageProporties()->bRemoveShading = settings.value("t_n_bRemoveShading","false").toBool();
    normalImageProp->getImageProporties()->noRemoveShadingGaussIter = settings.value("t_n_noRemoveShadingGaussIter","0").toInt();
    normalImageProp->getImageProporties()->noBlurPasses    = settings.value("t_n_noBlurPasses","0").toInt();
    normalImageProp->getImageProporties()->bSpeclarControl = settings.value("t_n_bSpeclarControl","false").toBool();
    normalImageProp->getImageProporties()->specularRadius    = settings.value("t_n_specularRadius","1").toInt();
    normalImageProp->getImageProporties()->specularW1    = settings.value("t_n_specularW1","0.0").toFloat();
    normalImageProp->getImageProporties()->specularW2    = settings.value("t_n_specularW2","0.0").toFloat();
    normalImageProp->getImageProporties()->specularContrast     = settings.value("t_n_specularContrast","0.0").toFloat();
    normalImageProp->getImageProporties()->specularAmplifier    = settings.value("t_n_specularAmplifier","0.0").toFloat();
    normalImageProp->getImageProporties()->smallDetails     = settings.value("t_n_smallDetails","0.0").toFloat();
    normalImageProp->getImageProporties()->mediumDetails    = settings.value("t_n_mediumDetails","0.0").toFloat();
    normalImageProp->getImageProporties()->detailDepth      = settings.value("t_n_detailDepth","0.0").toFloat();
    normalImageProp->getImageProporties()->sharpenBlurAmount   = settings.value("t_n_sharpenBlurAmount","0").toInt();
    normalImageProp->getImageProporties()->normalsStep         = settings.value("t_n_normalsStep","0.0").toFloat();
    normalImageProp->getImageProporties()->conversionHNDepth         = settings.value("t_n_conversionHNDepth","0.0").toFloat();
    normalImageProp->getImageProporties()->bConversionHN         = settings.value("t_n_bConversionHN","false").toBool();
    normalImageProp->getImageProporties()->bConversionNH         = settings.value("t_n_bConversionNH","false").toBool();

    normalImageProp->getImageProporties()->conversionNHItersHuge   = settings.value("t_n_conversionNHItersHuge","0").toInt();
    normalImageProp->getImageProporties()->conversionNHItersVeryLarge   = settings.value("t_n_conversionNHItersVeryLarge","0").toInt();
    normalImageProp->getImageProporties()->conversionNHItersLarge   = settings.value("t_n_conversionNHItersLarge","0").toInt();
    normalImageProp->getImageProporties()->conversionNHItersMedium   = settings.value("t_n_conversionNHItersMedium","0").toInt();
    normalImageProp->getImageProporties()->conversionNHItersSmall   = settings.value("t_n_conversionNHItersSmall","0").toInt();
    normalImageProp->getImageProporties()->conversionNHItersVerySmall   = settings.value("t_n_conversionNHItersVerySmall","0").toInt();

    normalImageProp->getImageProporties()->bConversionBaseMap  = settings.value("t_n_bConversionBaseMap","false").toBool();
    normalImageProp->getImageProporties()->conversionBaseMapAmplitude      = settings.value("t_n_conversionBaseMapAmplitude","0.0").toFloat();
    normalImageProp->getImageProporties()->conversionBaseMapFlatness       = settings.value("t_n_conversionBaseMapFlatness","0.0").toFloat();
    normalImageProp->getImageProporties()->conversionBaseMapNoIters        = settings.value("t_n_conversionBaseMapNoIters","0").toInt();
    normalImageProp->getImageProporties()->conversionBaseMapFilterRadius   = settings.value("t_n_conversionBaseMapFilterRadius","0").toInt();
    normalImageProp->getImageProporties()->conversionBaseMapMixNormals          = settings.value("t_n_conversionBaseMapMixNormals","0").toFloat();
    normalImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius     = settings.value("t_n_conversionBaseMapPreSmoothRadius","0").toFloat();
    normalImageProp->reloadSettings();

    specularImageProp->getImageProporties()->bGrayScale = settings.value("t_s_bGrayScale","false").toBool();
    specularImageProp->getImageProporties()->bInvertR = settings.value("t_s_bInvertR","false").toBool();
    specularImageProp->getImageProporties()->bInvertG = settings.value("t_s_bInvertG","false").toBool();
    specularImageProp->getImageProporties()->bInvertB = settings.value("t_s_bInvertB","false").toBool();
    specularImageProp->getImageProporties()->bRemoveShading = settings.value("t_s_bRemoveShading","false").toBool();
    specularImageProp->getImageProporties()->noRemoveShadingGaussIter = settings.value("t_s_noRemoveShadingGaussIter","0").toInt();
    specularImageProp->getImageProporties()->noBlurPasses    = settings.value("t_s_noBlurPasses","0").toInt();
    specularImageProp->getImageProporties()->bSpeclarControl = settings.value("t_s_bSpeclarControl","false").toBool();
    specularImageProp->getImageProporties()->specularRadius    = settings.value("t_s_specularRadius","1").toInt();
    specularImageProp->getImageProporties()->specularW1    = settings.value("t_s_specularW1","0.0").toFloat();
    specularImageProp->getImageProporties()->specularW2    = settings.value("t_s_specularW2","0.0").toFloat();
    specularImageProp->getImageProporties()->specularContrast     = settings.value("t_s_specularContrast","0.0").toFloat();
    specularImageProp->getImageProporties()->specularAmplifier    = settings.value("t_s_specularAmplifier","0.0").toFloat();
    specularImageProp->getImageProporties()->smallDetails     = settings.value("t_s_smallDetails","0.0").toFloat();
    specularImageProp->getImageProporties()->mediumDetails    = settings.value("t_s_mediumDetails","0.0").toFloat();
    specularImageProp->getImageProporties()->detailDepth      = settings.value("t_s_detailDepth","0.0").toFloat();
    specularImageProp->getImageProporties()->sharpenBlurAmount   = settings.value("t_s_sharpenBlurAmount","0").toInt();
    specularImageProp->getImageProporties()->normalsStep         = settings.value("t_s_normalsStep","0.0").toFloat();
    specularImageProp->getImageProporties()->conversionHNDepth         = settings.value("t_s_conversionHNDepth","0.0").toFloat();
    specularImageProp->getImageProporties()->bConversionHN         = settings.value("t_s_bConversionHN","false").toBool();
    specularImageProp->getImageProporties()->bConversionNH         = settings.value("t_s_bConversionNH","false").toBool();

    specularImageProp->getImageProporties()->bConversionBaseMap  = settings.value("t_s_bConversionBaseMap","false").toBool();
    specularImageProp->getImageProporties()->conversionBaseMapAmplitude      = settings.value("t_s_conversionBaseMapAmplitude","0.0").toFloat();
    specularImageProp->getImageProporties()->conversionBaseMapFlatness       = settings.value("t_s_conversionBaseMapFlatness","0.0").toFloat();
    specularImageProp->getImageProporties()->conversionBaseMapNoIters        = settings.value("t_s_conversionBaseMapNoIters","0").toInt();
    specularImageProp->getImageProporties()->conversionBaseMapFilterRadius   = settings.value("t_s_conversionBaseMapFilterRadius","0").toInt();
    specularImageProp->getImageProporties()->conversionBaseMapMixNormals          = settings.value("t_s_conversionBaseMapMixNormals","0").toFloat();
    specularImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius     = settings.value("t_s_conversionBaseMapPreSmoothRadius","0").toFloat();
    specularImageProp->reloadSettings();

    heightImageProp->getImageProporties()->bGrayScale = settings.value("t_h_bGrayScale","false").toBool();
    heightImageProp->getImageProporties()->bInvertR = settings.value("t_h_bInvertR","false").toBool();
    heightImageProp->getImageProporties()->bInvertG = settings.value("t_h_bInvertG","false").toBool();
    heightImageProp->getImageProporties()->bInvertB = settings.value("t_h_bInvertB","false").toBool();
    heightImageProp->getImageProporties()->bRemoveShading = settings.value("t_h_bRemoveShading","false").toBool();
    heightImageProp->getImageProporties()->noRemoveShadingGaussIter = settings.value("t_h_noRemoveShadingGaussIter","0").toInt();
    heightImageProp->getImageProporties()->noBlurPasses    = settings.value("t_h_noBlurPasses","0").toInt();
    heightImageProp->getImageProporties()->bSpeclarControl = settings.value("t_h_bSpeclarControl","false").toBool();
    heightImageProp->getImageProporties()->specularRadius    = settings.value("t_h_specularRadius","1").toInt();
    heightImageProp->getImageProporties()->specularW1    = settings.value("t_h_specularW1","0.0").toFloat();
    heightImageProp->getImageProporties()->specularW2    = settings.value("t_h_specularW2","0.0").toFloat();
    heightImageProp->getImageProporties()->specularContrast     = settings.value("t_h_specularContrast","0.0").toFloat();
    heightImageProp->getImageProporties()->specularAmplifier    = settings.value("t_h_specularAmplifier","0.0").toFloat();
    heightImageProp->getImageProporties()->smallDetails     = settings.value("t_h_smallDetails","0.0").toFloat();
    heightImageProp->getImageProporties()->mediumDetails    = settings.value("t_h_mediumDetails","0.0").toFloat();
    heightImageProp->getImageProporties()->detailDepth      = settings.value("t_h_detailDepth","0.0").toFloat();
    heightImageProp->getImageProporties()->sharpenBlurAmount   = settings.value("t_h_sharpenBlurAmount","0").toInt();
    heightImageProp->getImageProporties()->normalsStep         = settings.value("t_h_normalsStep","0.0").toFloat();
    heightImageProp->getImageProporties()->conversionHNDepth         = settings.value("t_h_conversionHNDepth","0.0").toFloat();
    heightImageProp->getImageProporties()->bConversionHN         = settings.value("t_h_bConversionHN","false").toBool();
    heightImageProp->getImageProporties()->bConversionNH         = settings.value("t_h_bConversionNH","false").toBool();

    heightImageProp->getImageProporties()->bConversionBaseMap  = settings.value("t_h_bConversionBaseMap","false").toBool();
    heightImageProp->getImageProporties()->conversionBaseMapAmplitude      = settings.value("t_h_conversionBaseMapAmplitude","0.0").toFloat();
    heightImageProp->getImageProporties()->conversionBaseMapFlatness       = settings.value("t_h_conversionBaseMapFlatness","0.0").toFloat();
    heightImageProp->getImageProporties()->conversionBaseMapNoIters        = settings.value("t_h_conversionBaseMapNoIters","0").toInt();
    heightImageProp->getImageProporties()->conversionBaseMapFilterRadius   = settings.value("t_h_conversionBaseMapFilterRadius","0").toInt();
    heightImageProp->getImageProporties()->conversionBaseMapMixNormals          = settings.value("t_h_conversionBaseMapMixNormals","0").toFloat();
    heightImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius     = settings.value("t_h_conversionBaseMapPreSmoothRadius","0").toFloat();
    heightImageProp->reloadSettings();

    occlusionImageProp->getImageProporties()->bGrayScale = settings.value("t_o_bGrayScale","false").toBool();
      occlusionImageProp->getImageProporties()->bInvertR = settings.value("t_o_bInvertR","false").toBool();
      occlusionImageProp->getImageProporties()->bInvertG = settings.value("t_o_bInvertG","false").toBool();
      occlusionImageProp->getImageProporties()->bInvertB = settings.value("t_o_bInvertB","false").toBool();
      occlusionImageProp->getImageProporties()->bRemoveShading = settings.value("t_o_bRemoveShading","false").toBool();
      occlusionImageProp->getImageProporties()->noRemoveShadingGaussIter = settings.value("t_o_noRemoveShadingGaussIter","0").toInt();
      occlusionImageProp->getImageProporties()->noBlurPasses    = settings.value("t_o_noBlurPasses","0").toInt();
      occlusionImageProp->getImageProporties()->bSpeclarControl = settings.value("t_o_bSpeclarControl","false").toBool();
      occlusionImageProp->getImageProporties()->specularRadius    = settings.value("t_o_specularRadius","1").toInt();
      occlusionImageProp->getImageProporties()->specularW1    = settings.value("t_o_specularW1","0.0").toFloat();
      occlusionImageProp->getImageProporties()->specularW2    = settings.value("t_o_specularW2","0.0").toFloat();
      occlusionImageProp->getImageProporties()->specularContrast     = settings.value("t_o_specularContrast","0.0").toFloat();
      occlusionImageProp->getImageProporties()->specularAmplifier    = settings.value("t_o_specularAmplifier","0.0").toFloat();
      occlusionImageProp->getImageProporties()->smallDetails     = settings.value("t_o_smallDetails","0.0").toFloat();
      occlusionImageProp->getImageProporties()->mediumDetails    = settings.value("t_o_mediumDetails","0.0").toFloat();
      occlusionImageProp->getImageProporties()->detailDepth      = settings.value("t_o_detailDepth","0.0").toFloat();
      occlusionImageProp->getImageProporties()->sharpenBlurAmount   = settings.value("t_o_sharpenBlurAmount","0").toInt();
      occlusionImageProp->getImageProporties()->normalsStep         = settings.value("t_o_normalsStep","0.0").toFloat();
      occlusionImageProp->getImageProporties()->conversionHNDepth         = settings.value("t_o_conversionHNDepth","0.0").toFloat();
      occlusionImageProp->getImageProporties()->bConversionHN         = settings.value("t_o_bConversionHN","false").toBool();
      occlusionImageProp->getImageProporties()->bConversionNH         = settings.value("t_o_bConversionNH","false").toBool();

      occlusionImageProp->getImageProporties()->bConversionBaseMap  = settings.value("t_o_bConversionBaseMap","false").toBool();
      occlusionImageProp->getImageProporties()->conversionBaseMapAmplitude      = settings.value("t_o_conversionBaseMapAmplitude","0.0").toFloat();
      occlusionImageProp->getImageProporties()->conversionBaseMapFlatness       = settings.value("t_o_conversionBaseMapFlatness","0.0").toFloat();
      occlusionImageProp->getImageProporties()->conversionBaseMapNoIters        = settings.value("t_o_conversionBaseMapNoIters","0").toInt();
      occlusionImageProp->getImageProporties()->conversionBaseMapFilterRadius   = settings.value("t_o_conversionBaseMapFilterRadius","0").toInt();
      occlusionImageProp->getImageProporties()->conversionBaseMapMixNormals          = settings.value("t_o_conversionBaseMapMixNormals","0").toFloat();
      occlusionImageProp->getImageProporties()->conversionBaseMapPreSmoothRadius     = settings.value("t_o_conversionBaseMapPreSmoothRadius","0").toFloat();

      occlusionImageProp->getImageProporties()->ssaoNoIters = settings.value("t_o_ssaoNoIters","1").toFloat();
      occlusionImageProp->getImageProporties()->ssaoBias = settings.value("t_o_ssaoBias","1").toFloat();
      occlusionImageProp->getImageProporties()->ssaoDepth = settings.value("t_o_ssaoDepth","1").toFloat();
      occlusionImageProp->getImageProporties()->ssaoIntensity = settings.value("t_o_ssaoIntensity","1").toFloat();

      occlusionImageProp->reloadSettings();
}
