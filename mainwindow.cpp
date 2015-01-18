#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    recentDir                   = NULL;
    bSaveCheckedImages          = false;
    bSaveCompressedFormImages   = false;
    FormImageProp::recentDir    = &recentDir;
    QGLFormat glFormat(QGL::SampleBuffers);

#ifdef Q_OS_MAC
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setVersion( 4, 1 );
#endif
    QGLFormat::setDefaultFormat(glFormat);

    glImage          = new GLImage(this);
    glWidget         = new GLWidget(this,glImage);


    QGLContext* glContext = (QGLContext *) glWidget->context();
    glContext->makeCurrent();    
    qDebug() << "Widget OpenGL: " << glContext->format().majorVersion() << "." << glContext->format().minorVersion() ;
    qDebug() << "Context valid: " << glContext->isValid() ;
    qDebug() << "OpenGL information: " ;
    qDebug() << "VENDOR: "       << (const char*)glGetString(GL_VENDOR) ;
    qDebug() << "RENDERER: "     << (const char*)glGetString(GL_RENDERER) ;
    qDebug() << "VERSION: "      << (const char*)glGetString(GL_VERSION) ;
    qDebug() << "GLSL VERSION: " << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) ;

    qDebug() << "OpenGLVersionFlags(): " << QGLFormat::OpenGLVersionFlags();

    if((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_2) == 0)
    {
       qDebug() << "GL 3.2 Not supported" ;
       exit(-1);
    }

    diffuseImageProp  = new FormImageProp(this,glImage);
    normalImageProp   = new FormImageProp(this,glImage);
    specularImageProp = new FormImageProp(this,glImage);
    heightImageProp   = new FormImageProp(this,glImage);
    occlusionImageProp= new FormImageProp(this,glImage);

    // Setting pointers to 3D view (this pointer are used to bindTextures).
    glWidget->setPointerToTexture(&diffuseImageProp  ->getImageProporties() ->fbo,DIFFUSE_TEXTURE);
    glWidget->setPointerToTexture(&normalImageProp   ->getImageProporties() ->fbo,NORMAL_TEXTURE);
    glWidget->setPointerToTexture(&specularImageProp ->getImageProporties() ->fbo,SPECULAR_TEXTURE);
    glWidget->setPointerToTexture(&heightImageProp   ->getImageProporties() ->fbo,HEIGHT_TEXTURE);
    glWidget->setPointerToTexture(&occlusionImageProp->getImageProporties() ->fbo,OCCLUSION_TEXTURE);

    // Selecting type of image for each texture
    diffuseImageProp  ->getImageProporties()->imageType = DIFFUSE_TEXTURE;
    normalImageProp   ->getImageProporties()->imageType = NORMAL_TEXTURE;
    specularImageProp ->getImageProporties()->imageType = SPECULAR_TEXTURE;
    heightImageProp   ->getImageProporties()->imageType = HEIGHT_TEXTURE;
    occlusionImageProp->getImageProporties()->imageType = OCCLUSION_TEXTURE;

    // disabling some options for each texture
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
    normalImageProp->hideHeightProcessingBox();
    normalImageProp->hideGrayScaleControl();

    heightImageProp->hideBMGroupBox();
    heightImageProp->hideNHGroupBox();
    heightImageProp->hideSpecularGroupBox();
    heightImageProp->hideNormalStepBar();
    heightImageProp->hideSSAOBar();
    heightImageProp->hideGrayScaleControl();

    occlusionImageProp->hideSpecularGroupBox();
    occlusionImageProp->hideBMGroupBox();
    occlusionImageProp->hideHNGroupBox();
    occlusionImageProp->hideNHGroupBox();
    occlusionImageProp->hideNormalStepBar();
    occlusionImageProp->hideGrayScaleControl();

    glImage ->targetImageNormal    = normalImageProp   ->getImageProporties();
    glImage ->targetImageHeight    = heightImageProp   ->getImageProporties();
    glImage ->targetImageSpecular  = specularImageProp ->getImageProporties();
    glImage ->targetImageOcclusion = occlusionImageProp->getImageProporties();

    // ------------------------------------------------------
    //                      GUI setup
    // ------------------------------------------------------
    ui->setupUi(this);
    ui->widget_2->hide();
    ui->verticalLayout3DImage->addWidget(glWidget);
    ui->verticalLayout2DImage->addWidget(glImage);


    ui->verticalLayoutDiffuseImage  ->addWidget(diffuseImageProp);
    ui->verticalLayoutNormalImage   ->addWidget(normalImageProp);
    ui->verticalLayoutSpecularImage ->addWidget(specularImageProp);
    ui->verticalLayoutHeightImage   ->addWidget(heightImageProp);
    ui->verticalLayoutOcclusionImage->addWidget(occlusionImageProp);

    connect(glWidget,SIGNAL(rendered()),this,SLOT(initializeImages()));
    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),this,SLOT(updateImage(int)));
    // imageChange signals
    connect(diffuseImageProp  ,SIGNAL(imageChanged()),this,SLOT(updateDiffuseImage()));
    connect(normalImageProp   ,SIGNAL(imageChanged()),this,SLOT(updateNormalImage()));
    connect(specularImageProp ,SIGNAL(imageChanged()),this,SLOT(updateSpecularImage()));
    connect(heightImageProp   ,SIGNAL(imageChanged()),this,SLOT(updateHeightImage()));
    connect(occlusionImageProp,SIGNAL(imageChanged()),this,SLOT(updateOcclusionImage()));
    // conversion signals
    connect(heightImageProp,SIGNAL(conversionHeightToNormalApplied()) ,this,SLOT(convertFromHtoN()));
    connect(heightImageProp,SIGNAL(repaintNormalTexture()) ,this,SLOT(repaintNormalImage()));

    connect(normalImageProp ,SIGNAL(conversionNormalToHeightApplied()),this,SLOT(convertFromNtoH()));
    connect(diffuseImageProp,SIGNAL(conversionBaseConversionApplied()),this,SLOT(convertFromBase()));

    // Global setting signals
    // sliders
    connect(ui->horizontalSliderDepthScale  ,SIGNAL(valueChanged(int)),glWidget,SLOT(setDepthScale(int)));
    connect(ui->horizontalSliderUVScale     ,SIGNAL(valueChanged(int)),glWidget,SLOT(setUVScale(int)));    
    connect(ui->horizontalSliderDepthScale  ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVScale     ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVXOffset   ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVYOffset   ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));

    // Save signals
    connect(ui->pushButtonSaveAll    ,SIGNAL(released()),this,SLOT(saveImages()));
    connect(ui->pushButtonSaveChecked,SIGNAL(released()),this,SLOT(saveCheckedImages()));
    connect(ui->pushButtonSaveAs     ,SIGNAL(released()),this,SLOT(saveCompressedForm()));


    // image properties signals
    connect(ui->comboBoxResizeWidth   ,SIGNAL(currentIndexChanged(int)),this,SLOT(changeWidth(int)));
    connect(ui->comboBoxResizeHeight  ,SIGNAL(currentIndexChanged(int)),this,SLOT(changeHeight(int)));

    connect(ui->doubleSpinBoxRescaleWidth  ,SIGNAL(valueChanged(double)),this,SLOT(scaleWidth(double)));
    connect(ui->doubleSpinBoxRescaleHeight ,SIGNAL(valueChanged(double)),this,SLOT(scaleHeight(double)));

    connect(ui->pushButtonResizeApply ,SIGNAL(released()),this,SLOT(applyResizeImage()));
    connect(ui->pushButtonRescaleApply,SIGNAL(released()),this,SLOT(applyScaleImage()));


    // Other signals
    connect(ui->pushButtonReplotAll         ,SIGNAL(released()),this,SLOT(replotAllImages()));
    connect(ui->pushButtonToggleDiffuse     ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleDiffuseView(bool)));
    connect(ui->pushButtonToggleSpecular    ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleSpecularView(bool)));
    connect(ui->pushButtonToggleOcclusion   ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleOcclusionView(bool)));
    connect(ui->pushButtonSaveCurrentSettings,SIGNAL(released()),this,SLOT(saveSettings()));
    connect(ui->horizontalSliderSpecularI   ,SIGNAL(valueChanged(int)),this,SLOT(setSpecularIntensity(int)));
    connect(ui->horizontalSliderDiffuseI    ,SIGNAL(valueChanged(int)),this,SLOT(setDiffuseIntensity(int)));
    connect(ui->comboBoxImageOutputFormat   ,SIGNAL(activated(int)),this,SLOT(setOutputFormat(int)));


    ui->progressBar->setValue(0);

    connect(ui->actionReplot            ,SIGNAL(triggered()),this,SLOT(replotAllImages()));
    connect(ui->actionShowDiffuseImage  ,SIGNAL(triggered()),this,SLOT(selectDiffuseTab()));
    connect(ui->actionShowNormalImage   ,SIGNAL(triggered()),this,SLOT(selectNormalTab()));
    connect(ui->actionShowSpecularImage ,SIGNAL(triggered()),this,SLOT(selectSpecularTab()));
    connect(ui->actionShowHeightImage   ,SIGNAL(triggered()),this,SLOT(selectHeightTab()));
    connect(ui->actionShowOcclusiontImage,SIGNAL(triggered()),this,SLOT(selectOcclusionTab()));
    connect(ui->actionShowSettingsImage ,SIGNAL(triggered()),this,SLOT(selectGeneralSettingsTab()));
    connect(ui->actionFitToScreen       ,SIGNAL(triggered()),this,SLOT(fitImage()));


    // perspective tool
    connect(ui->pushButtonResetTransform            ,SIGNAL(released()),this,SLOT(resetTransform()));
    connect(ui->comboBoxPerspectiveTransformMethod  ,SIGNAL(activated(int)),glImage,SLOT(selectPerspectiveTransformMethod(int)));
    connect(ui->comboBoxSeamlessMode                ,SIGNAL(activated(int)),this,SLOT(selectSeamlessMode(int)));

    // uv seamless algorithms
    connect(ui->horizontalSliderMakeSeamlessRadius,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderMakeSeamlessRadius,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    QButtonGroup *groupMirroMode = new QButtonGroup( this );
    groupMirroMode->addButton( ui->radioButtonMirrorModeX);
    groupMirroMode->addButton( ui->radioButtonMirrorModeY);
    groupMirroMode->addButton( ui->radioButtonMirrorModeXY);
    connect(ui->radioButtonMirrorModeX ,SIGNAL(released()),this,SLOT(updateSliders()));
    connect(ui->radioButtonMirrorModeY ,SIGNAL(released()),this,SLOT(updateSliders()));
    connect(ui->radioButtonMirrorModeXY,SIGNAL(released()),this,SLOT(updateSliders()));

    // random mode
    connect(ui->pushButtonRandomPatchesRandomize,SIGNAL(released()),this,SLOT(randomizeAngles()));
    connect(ui->pushButtonRandomPatchesReset,SIGNAL(released()),this,SLOT(resetRandomPatches()));
    connect(ui->horizontalSliderRandomPatchesRotate,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderRandomPatchesInnerRadius,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderRandomPatchesOuterRadius,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));

    connect(ui->horizontalSliderRandomPatchesRotate,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderRandomPatchesInnerRadius,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderRandomPatchesOuterRadius,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));


    ui->groupBoxSimpleSeamlessMode->hide();
    ui->groupBoxMirrorMode->hide();
    ui->groupBoxRandomPatchesMode->hide();


    // 2D imate tool box settings
    QActionGroup *group = new QActionGroup( this );
    group->addAction( ui->actionTranslateUV );
    group->addAction( ui->actionGrabCorners);
    group->addAction( ui->actionScaleXY );
    ui->actionTranslateUV->setChecked(true);
    connect(ui->actionTranslateUV,SIGNAL(triggered()),this,SLOT(setUVManipulationMethod()));
    connect(ui->actionGrabCorners,SIGNAL(triggered()),this,SLOT(setUVManipulationMethod()));
    connect(ui->actionScaleXY    ,SIGNAL(triggered()),this,SLOT(setUVManipulationMethod()));

    loadSettings();
    // Loading default (initial) textures
    diffuseImageProp   ->setImage(QImage(QString(":/content/logo_D.png")));
    normalImageProp    ->setImage(QImage(QString(":/content/logo_N.png")));
    specularImageProp  ->setImage(QImage(QString(":/content/logo_D.png")));
    heightImageProp    ->setImage(QImage(QString(":/content/logo_H.png")));
    occlusionImageProp ->setImage(QImage(QString(":/content/logo_O.png")));

    diffuseImageProp   ->setImageName(ui->lineEditOutputName->text());
    normalImageProp    ->setImageName(ui->lineEditOutputName->text());
    heightImageProp    ->setImageName(ui->lineEditOutputName->text());
    specularImageProp  ->setImageName(ui->lineEditOutputName->text());
    occlusionImageProp ->setImageName(ui->lineEditOutputName->text());

    // Setting the active image
    glImage->setActiveImage(diffuseImageProp->getImageProporties());



#ifdef Q_OS_MAC
    if(ui->statusbar && !ui->statusbar->testAttribute(Qt::WA_MacNormalSize)) ui->statusbar->setAttribute(Qt::WA_MacSmallSize);
#endif
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
    settings.setValue("recent_dir",recentDir.absolutePath());
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
        // skip normal and ambient occlusion
        if(i!=1 && i!=4)updateImage(i);
        glImage->repaint();
    }
    // recalulate normal at the end
    updateImage(1);
    glImage->repaint();
    // then ambient occlusion
    updateImage(4);
    glImage->repaint();
    glImage->enableShadowRender(false);

    glImage->setActiveImage(lastActive);
    glWidget->repaint();
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

void MainWindow::fitImage(){
    glImage->resetView();
    glImage->repaint();
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

        // putting height or specular color to alpha channel according to compression type
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
    updateImageInformation();
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

    // replot normal if height was changed in attached mode
    if(FBOImageProporties::bAttachNormalToHeightMap){
        glImage->enableShadowRender(true);
        updateImage(1);
        glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(3);
    }
    glWidget->repaint();
}

void MainWindow::updateOcclusionImage(){
    ui->lineEditOutputName->setText(occlusionImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();

}

void MainWindow::repaintNormalImage(){
    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
        updateImage(1);
        glImage->repaint();
    glImage->enableShadowRender(false);
    glImage->setActiveImage(lastActive);
    glWidget->repaint();
}


void MainWindow::updateImageInformation(){
    ui->labelCurrentImageWidth ->setNum(diffuseImageProp->getImageProporties()->ref_fbo->width());
    ui->labelCurrentImageHeight->setNum(diffuseImageProp->getImageProporties()->ref_fbo->height());
}

void MainWindow::initializeImages(){
    static bool bInitializedFirstDraw = false;

    if(bInitializedFirstDraw) return;
    bInitializedFirstDraw = true;
    qDebug() << "MainWindow::Initialization";
    QCoreApplication::processEvents();

    replotAllImages();
    // SSAO recalculation
    FBOImageProporties* lastActive = glImage->getActiveImage();

    updateImage(4);
    glImage->repaint();
    glImage->setActiveImage(lastActive);

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

void MainWindow::changeWidth (int size){
    if(ui->pushButtonResizePropTo->isChecked()){
        ui->comboBoxResizeHeight->setCurrentText(ui->comboBoxResizeWidth->currentText());
    }
}
void MainWindow::changeHeight(int size){
    if(ui->pushButtonResizePropTo->isChecked()){
        ui->comboBoxResizeWidth->setCurrentText(ui->comboBoxResizeHeight->currentText());
    }
}



void MainWindow::applyResizeImage(){
    QCoreApplication::processEvents();
    int width  = ui->comboBoxResizeWidth->currentText().toInt();
    int height = ui->comboBoxResizeHeight->currentText().toInt();
    qDebug() << "Image resize applied. Current image size is (" << width << "," << height << ")" ;

    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
    for(int i = 0 ; i < 5 ; i++){
        updateImage(i);
        glImage->resizeFBO(width,height);
    }
    glImage->enableShadowRender(false);
    glImage->setActiveImage(lastActive);
    replotAllImages();
    updateImageInformation();
    glWidget->repaint();
}

void MainWindow::scaleWidth(double){
    if(ui->pushButtonRescalePropTo->isChecked()){
        ui->doubleSpinBoxRescaleHeight->setValue(ui->doubleSpinBoxRescaleWidth->value());
    }
}
void MainWindow::scaleHeight(double){
    if(ui->pushButtonRescalePropTo->isChecked()){
        ui->doubleSpinBoxRescaleWidth->setValue(ui->doubleSpinBoxRescaleHeight->value());
    }
}

void MainWindow::applyScaleImage(){
    QCoreApplication::processEvents();
    float scale_width   = ui->doubleSpinBoxRescaleWidth ->value();
    float scale_height  = ui->doubleSpinBoxRescaleHeight->value();
    int width  = diffuseImageProp->getImageProporties()->scr_tex_width *scale_width;
    int height = diffuseImageProp->getImageProporties()->scr_tex_height*scale_height;

    qDebug() << "Image rescale applied. Current image size is (" << width << "," << height << ")" ;

    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
    for(int i = 0 ; i < 5 ; i++){
        updateImage(i);
        glImage->resizeFBO(width,height);
    }
    glImage->enableShadowRender(false);
    glImage->setActiveImage(lastActive);
    replotAllImages();
    updateImageInformation();
    glWidget->repaint();

}


void MainWindow::selectSeamlessMode(int mode){
    // some gui interaction -> hide and show
    ui->groupBoxSimpleSeamlessMode->hide();
    ui->groupBoxMirrorMode->hide();
    ui->groupBoxRandomPatchesMode->hide();
    switch(mode){
    case(SEAMLESS_NONE):

        break;
    case(SEAMLESS_SIMPLE):
        ui->groupBoxSimpleSeamlessMode->show();
        break;
    case(SEAMLESS_MIRROR):
        ui->groupBoxMirrorMode->show();
        break;
    case(SEAMLESS_RANDOM):
        ui->groupBoxRandomPatchesMode->show();
        break;
    default:
        break;
    }
    glImage->selectSeamlessMode((SeamlessMode)mode);
    replotAllImages();
}

void MainWindow::randomizeAngles(){
    FBOImageProporties::seamlessRandomTiling.randomize();
    replotAllImages();
}
void MainWindow::resetRandomPatches(){
    FBOImageProporties::seamlessRandomTiling = RandomTilingMode();
    ui->horizontalSliderRandomPatchesRotate     ->setValue(FBOImageProporties::seamlessRandomTiling.common_phase);
    ui->horizontalSliderRandomPatchesInnerRadius->setValue(FBOImageProporties::seamlessRandomTiling.inner_radius*100.0);
    ui->horizontalSliderRandomPatchesOuterRadius->setValue(FBOImageProporties::seamlessRandomTiling.outer_radius*100.0);
    updateSpinBoxes(0);
    replotAllImages();
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
    ui->doubleSpinBoxDepthScale  ->setValue(ui->horizontalSliderDepthScale->value()/100.0);
    ui->doubleSpinBoxUVScale     ->setValue(ui->horizontalSliderUVScale   ->value()/10.0);
    ui->doubleSpinBoxUVXOffset   ->setValue(ui->horizontalSliderUVXOffset ->value()/100.0);
    ui->doubleSpinBoxUVYOffset   ->setValue(ui->horizontalSliderUVYOffset ->value()/100.0);

    // random tiling mode
    ui->doubleSpinBoxRandomPatchesAngle      ->setValue(ui->horizontalSliderRandomPatchesRotate     ->value());
    ui->doubleSpinBoxRandomPatchesInnerRadius->setValue(ui->horizontalSliderRandomPatchesInnerRadius->value()/100.0);
    ui->doubleSpinBoxRandomPatchesOuterRadius->setValue(ui->horizontalSliderRandomPatchesOuterRadius->value()/100.0);

    glWidget->setUVScaleOffset(ui->doubleSpinBoxUVXOffset->value(),ui->doubleSpinBoxUVYOffset->value());
}


void MainWindow::convertFromHtoN(){   
    glImage->setConversionType(CONVERT_FROM_H_TO_N);
    glImage->repaint();
    replotAllImages();
    glWidget->repaint();
    qDebug() << "Conversion from height to normal applied";
}

void MainWindow::convertFromNtoH(){
    glImage->setConversionType(CONVERT_FROM_N_TO_H);
    glImage->repaint();
    replotAllImages();
    glWidget->repaint();
    qDebug() << "Conversion from normal to height applied";
}


void MainWindow::convertFromBase(){
    normalImageProp   ->setImageName(diffuseImageProp->getImageName());
    heightImageProp   ->setImageName(diffuseImageProp->getImageName());
    specularImageProp ->setImageName(diffuseImageProp->getImageName());
    occlusionImageProp->setImageName(diffuseImageProp->getImageName());
    glImage->setConversionType(CONVERT_FROM_D_TO_O);
    replotAllImages();
    qDebug() << "Conversion from Base to others applied";
}


void MainWindow::updateSliders(){
    updateSpinBoxes(0);
    FBOImageProporties::seamlessSimpleModeRadius          = ui->doubleSpinBoxMakeSeamless->value();
    FBOImageProporties::seamlessRandomTiling.common_phase = ui->doubleSpinBoxRandomPatchesAngle->value()/180.0*3.1415926;
    FBOImageProporties::seamlessRandomTiling.inner_radius = ui->doubleSpinBoxRandomPatchesInnerRadius->value();
    FBOImageProporties::seamlessRandomTiling.outer_radius = ui->doubleSpinBoxRandomPatchesOuterRadius->value();

    // choosing the proper mirror mode
    if(ui->radioButtonMirrorModeXY->isChecked()) FBOImageProporties::seamlessMirroModeType = 0;
    if(ui->radioButtonMirrorModeX ->isChecked()) FBOImageProporties::seamlessMirroModeType = 1;
    if(ui->radioButtonMirrorModeY ->isChecked()) FBOImageProporties::seamlessMirroModeType = 2;


    glImage ->repaint();
    glWidget->repaint();

}




void MainWindow::resetTransform(){
    QVector2D corner(0,0);
    glImage->updateCornersPosition(corner,corner,corner,corner);
    glImage->updateCornersWeights(0,0,0,0);
    replotAllImages();
}



void MainWindow::setUVManipulationMethod(){
    if(ui->actionTranslateUV->isChecked()) glImage->selectUVManipulationMethod(uvTranslate);
    if(ui->actionGrabCorners->isChecked()) glImage->selectUVManipulationMethod(uvGrabCorners);
    if(ui->actionScaleXY->isChecked())     glImage->selectUVManipulationMethod(uvScaleXY);
}

QSize MainWindow::sizeHint() const
{
    QSettings settings("config.ini", QSettings::IniFormat);
    return QSize(settings.value("d_win_w","800").toInt(),settings.value("d_win_h","600").toInt());
}

void MainWindow::saveImageSettings(QString abbr,FormImageProp* image){


    QSettings settings("config.ini", QSettings::IniFormat);

    settings.setValue("t_"+abbr+"_bGrayScale"                       ,image->getImageProporties()->bGrayScale);
    settings.setValue("t_"+abbr+"_bInvertR"                         ,image->getImageProporties()->bInvertR);
    settings.setValue("t_"+abbr+"_bInvertG"                         ,image->getImageProporties()->bInvertG);
    settings.setValue("t_"+abbr+"_bInvertB"                         ,image->getImageProporties()->bInvertB);
    settings.setValue("t_"+abbr+"_bRemoveShading"                   ,image->getImageProporties()->bRemoveShading);
    settings.setValue("t_"+abbr+"_noRemoveShadingGaussIter"         ,image->getImageProporties()->noRemoveShadingGaussIter);
    settings.setValue("t_"+abbr+"_noBlurPasses"                     ,image->getImageProporties()->noBlurPasses);
    settings.setValue("t_"+abbr+"_bSpeclarControl"                  ,image->getImageProporties()->bSpeclarControl);
    settings.setValue("t_"+abbr+"_specularRadius"                   ,image->getImageProporties()->specularRadius);
    settings.setValue("t_"+abbr+"_specularW1"                       ,image->getImageProporties()->specularW1);
    settings.setValue("t_"+abbr+"_specularW2"                       ,image->getImageProporties()->specularW2);
    settings.setValue("t_"+abbr+"_specularContrast"                 ,image->getImageProporties()->specularContrast);
    settings.setValue("t_"+abbr+"_specularAmplifier"                ,image->getImageProporties()->specularAmplifier);
    settings.setValue("t_"+abbr+"_specularBrightness"               ,image->getImageProporties()->specularBrightness);
    settings.setValue("t_"+abbr+"_smallDetails"                     ,image->getImageProporties()->smallDetails);
    settings.setValue("t_"+abbr+"_mediumDetails"                    ,image->getImageProporties()->mediumDetails);
    settings.setValue("t_"+abbr+"_detailDepth"                      ,image->getImageProporties()->detailDepth);
    settings.setValue("t_"+abbr+"_sharpenBlurAmount"                ,image->getImageProporties()->sharpenBlurAmount);
    settings.setValue("t_"+abbr+"_normalsStep"                      ,image->getImageProporties()->normalsStep);
    settings.setValue("t_"+abbr+"_conversionHNDepth"                ,image->getImageProporties()->conversionHNDepth);
    settings.setValue("t_"+abbr+"_bConversionHN"                    ,image->getImageProporties()->bConversionHN);
    settings.setValue("t_"+abbr+"_bConversionNH"                    ,image->getImageProporties()->bConversionNH);
    settings.setValue("t_"+abbr+"_conversionNHItersHuge"            ,image->getImageProporties()->conversionNHItersHuge);
    settings.setValue("t_"+abbr+"_conversionNHItersVeryLarge"       ,image->getImageProporties()->conversionNHItersVeryLarge);
    settings.setValue("t_"+abbr+"_conversionNHItersLarge"           ,image->getImageProporties()->conversionNHItersLarge);
    settings.setValue("t_"+abbr+"_conversionNHItersMedium"          ,image->getImageProporties()->conversionNHItersMedium);
    settings.setValue("t_"+abbr+"_conversionNHItersVerySmall"       ,image->getImageProporties()->conversionNHItersVerySmall);
    settings.setValue("t_"+abbr+"_conversionNHItersSmall"           ,image->getImageProporties()->conversionNHItersSmall);
    settings.setValue("t_"+abbr+"_bConversionBaseMap"               ,image->getImageProporties()->bConversionBaseMap);
    settings.setValue("t_"+abbr+"_conversionBaseMapAmplitude"       ,image->getImageProporties()->conversionBaseMapAmplitude);
    settings.setValue("t_"+abbr+"_conversionBaseMapFlatness"        ,image->getImageProporties()->conversionBaseMapFlatness);
    settings.setValue("t_"+abbr+"_conversionBaseMapNoIters"         ,image->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_"+abbr+"_conversionBaseMapNoIters"         ,image->getImageProporties()->conversionBaseMapNoIters);
    settings.setValue("t_"+abbr+"_conversionBaseMapFilterRadius"    ,image->getImageProporties()->conversionBaseMapFilterRadius);
    settings.setValue("t_"+abbr+"_conversionBaseMapMixNormals"      ,image->getImageProporties()->conversionBaseMapMixNormals);
    settings.setValue("t_"+abbr+"_conversionBaseMapPreSmoothRadius" ,image->getImageProporties()->conversionBaseMapPreSmoothRadius);
    settings.setValue("t_"+abbr+"_conversionBaseMapBlending"        ,image->getImageProporties()->conversionBaseMapBlending);
    settings.setValue("t_"+abbr+"_ssaoNoIters"                      ,image->getImageProporties()->ssaoNoIters);
    settings.setValue("t_"+abbr+"_ssaoBias"                         ,image->getImageProporties()->ssaoBias);
    settings.setValue("t_"+abbr+"_ssaoDepth"                        ,image->getImageProporties()->ssaoDepth);
    settings.setValue("t_"+abbr+"_ssaoIntensity"                    ,image->getImageProporties()->ssaoIntensity);

}

void MainWindow::loadImageSettings(QString abbr,FormImageProp* image){

    QSettings settings("config.ini", QSettings::IniFormat);
    image->getImageProporties()->bGrayScale                         = settings.value("t_"+abbr+"_bGrayScale","false").toBool();
    image->getImageProporties()->bInvertR                           = settings.value("t_"+abbr+"_bInvertR","false").toBool();
    image->getImageProporties()->bInvertG                           = settings.value("t_"+abbr+"_bInvertG","false").toBool();
    image->getImageProporties()->bInvertB                           = settings.value("t_"+abbr+"_bInvertB","false").toBool();
    image->getImageProporties()->bRemoveShading                     = settings.value("t_"+abbr+"_bRemoveShading","false").toBool();
    image->getImageProporties()->noRemoveShadingGaussIter           = settings.value("t_"+abbr+"_noRemoveShadingGaussIter","0").toInt();
    image->getImageProporties()->noBlurPasses                       = settings.value("t_"+abbr+"_noBlurPasses","0").toInt();
    image->getImageProporties()->bSpeclarControl                    = settings.value("t_"+abbr+"_bSpeclarControl","false").toBool();
    image->getImageProporties()->specularRadius                     = settings.value("t_"+abbr+"_specularRadius","1").toInt();
    image->getImageProporties()->specularW1                         = settings.value("t_"+abbr+"_specularW1","0.0").toFloat();
    image->getImageProporties()->specularW2                         = settings.value("t_"+abbr+"_specularW2","0.0").toFloat();
    image->getImageProporties()->specularContrast                   = settings.value("t_"+abbr+"_specularContrast","0.0").toFloat();
    image->getImageProporties()->specularAmplifier                  = settings.value("t_"+abbr+"_specularAmplifier","0.0").toFloat();
    image->getImageProporties()->specularBrightness                 = settings.value("t_"+abbr+"_specularBrightness","0.0").toFloat();
    image->getImageProporties()->smallDetails                       = settings.value("t_"+abbr+"_smallDetails","0.0").toFloat();
    image->getImageProporties()->mediumDetails                      = settings.value("t_"+abbr+"_mediumDetails","0.0").toFloat();
    image->getImageProporties()->detailDepth                        = settings.value("t_"+abbr+"_detailDepth","0.0").toFloat();
    image->getImageProporties()->sharpenBlurAmount                  = settings.value("t_"+abbr+"_sharpenBlurAmount","0").toInt();
    image->getImageProporties()->normalsStep                        = settings.value("t_"+abbr+"_normalsStep","0.0").toFloat();
    image->getImageProporties()->conversionHNDepth                  = settings.value("t_"+abbr+"_conversionHNDepth","0.0").toFloat();
    image->getImageProporties()->bConversionHN                      = settings.value("t_"+abbr+"_bConversionHN","false").toBool();
    image->getImageProporties()->bConversionNH                      = settings.value("t_"+abbr+"_bConversionNH","false").toBool();

    image->getImageProporties()->conversionNHItersHuge              = settings.value("t_"+abbr+"_conversionNHItersHuge","10").toInt();
    image->getImageProporties()->conversionNHItersVeryLarge         = settings.value("t_"+abbr+"_conversionNHItersVeryLarge","10").toInt();
    image->getImageProporties()->conversionNHItersLarge             = settings.value("t_"+abbr+"_conversionNHItersLarge","10").toInt();
    image->getImageProporties()->conversionNHItersMedium            = settings.value("t_"+abbr+"_conversionNHItersMedium","10").toInt();
    image->getImageProporties()->conversionNHItersSmall             = settings.value("t_"+abbr+"_conversionNHItersSmall","10").toInt();
    image->getImageProporties()->conversionNHItersVerySmall         = settings.value("t_"+abbr+"_conversionNHItersVerySmall","10").toInt();

    image->getImageProporties()->bConversionBaseMap                 = settings.value("t_"+abbr+"_bConversionBaseMap","false").toBool();
    image->getImageProporties()->conversionBaseMapAmplitude         = settings.value("t_"+abbr+"_conversionBaseMapAmplitude","-100.0").toFloat();
    image->getImageProporties()->conversionBaseMapFlatness          = settings.value("t_"+abbr+"_conversionBaseMapFlatness","0.0").toFloat();
    image->getImageProporties()->conversionBaseMapNoIters           = settings.value("t_"+abbr+"_conversionBaseMapNoIters","0").toInt();
    image->getImageProporties()->conversionBaseMapFilterRadius      = settings.value("t_"+abbr+"_conversionBaseMapFilterRadius","0").toInt();
    image->getImageProporties()->conversionBaseMapMixNormals        = settings.value("t_"+abbr+"_conversionBaseMapMixNormals","0").toFloat();
    image->getImageProporties()->conversionBaseMapPreSmoothRadius   = settings.value("t_"+abbr+"_conversionBaseMapPreSmoothRadius","0").toFloat();
    image->getImageProporties()->conversionBaseMapBlending          = settings.value("t_"+abbr+"_conversionBaseMapBlending","0").toFloat();
    image->getImageProporties()->ssaoNoIters                        = settings.value("t_"+abbr+"_ssaoNoIters","2").toFloat();
    image->getImageProporties()->ssaoBias                           = settings.value("t_"+abbr+"_ssaoBias","0.5").toFloat();
    image->getImageProporties()->ssaoDepth                          = settings.value("t_"+abbr+"_ssaoDepth","0.5").toFloat();
    image->getImageProporties()->ssaoIntensity                      = settings.value("t_"+abbr+"_ssaoIntensity","-0.3").toFloat();
    image->reloadSettings();

}

void MainWindow::saveSettings(){
    qDebug() << "<MainWindow>:: saving settings";
    QSettings settings("config.ini", QSettings::IniFormat);
    PostfixNames::diffuseName   = ui->lineEditPostfixDiffuse->text();
    PostfixNames::normalName    = ui->lineEditPostfixNormal->text();
    PostfixNames::specularName  = ui->lineEditPostfixSpecular->text();
    PostfixNames::heightName    = ui->lineEditPostfixHeight->text();
    PostfixNames::occlusionName = ui->lineEditPostfixOcclusion->text();

    settings.setValue("3d_depth",ui->horizontalSliderDepthScale->value()/100.0);
    settings.setValue("d_postfix",ui->lineEditPostfixDiffuse->text());
    settings.setValue("n_postfix",ui->lineEditPostfixNormal->text());
    settings.setValue("s_postfix",ui->lineEditPostfixSpecular->text());
    settings.setValue("h_postfix",ui->lineEditPostfixHeight->text());
    settings.setValue("o_postfix",ui->lineEditPostfixOcclusion->text());
    settings.setValue("recent_dir",recentDir.absolutePath());

    settings.setValue("h_attachNormal",FBOImageProporties::bAttachNormalToHeightMap);

    saveImageSettings("d",diffuseImageProp);
    saveImageSettings("n",normalImageProp);
    saveImageSettings("s",specularImageProp);
    saveImageSettings("h",heightImageProp);
    saveImageSettings("o",occlusionImageProp);

}

void MainWindow::setOutputFormat(int index){
    PostfixNames::outputFormat = ui->comboBoxImageOutputFormat->currentText();
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

    ui->horizontalSliderDepthScale->setValue(settings.value("3d_depth","0.25").toFloat()*100);
    ui->lineEditPostfixDiffuse  ->setText(PostfixNames::diffuseName);
    ui->lineEditPostfixNormal   ->setText(PostfixNames::normalName);
    ui->lineEditPostfixSpecular ->setText(PostfixNames::specularName);
    ui->lineEditPostfixHeight   ->setText(PostfixNames::heightName);
    ui->lineEditPostfixOcclusion->setText(PostfixNames::occlusionName);


    FBOImageProporties::bAttachNormalToHeightMap = settings.value("h_attachNormal","").toBool();
    heightImageProp->toggleAttachToNormal(FBOImageProporties::bAttachNormalToHeightMap);
    recentDir = settings.value("recent_dir","").toString();

    loadImageSettings("d",diffuseImageProp);
    loadImageSettings("n",normalImageProp);
    loadImageSettings("s",specularImageProp);
    loadImageSettings("h",heightImageProp);
    loadImageSettings("o",occlusionImageProp);

}
