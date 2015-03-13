#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{


    recentDir                   = NULL;
    recentMeshDir               = NULL;
    bSaveCheckedImages          = false;
    bSaveCompressedFormImages   = false;
    FormImageProp::recentDir    = &recentDir;
    GLWidget::recentMeshDir     = &recentMeshDir;

    QGLFormat glFormat(QGL::SampleBuffers);

#ifdef Q_OS_MAC
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setVersion( 4, 1 );
#endif
    QGLFormat::setDefaultFormat(glFormat);

    glImage          = new GLImage(this);
    glWidget         = new GLWidget(this,glImage);

    connect(glImage,SIGNAL(rendered()),this,SLOT(initializeImages()));


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
    glWidget->setPointerToTexture(&diffuseImageProp->getImageProporties()  ->fbo,DIFFUSE_TEXTURE);
    glWidget->setPointerToTexture(&normalImageProp->getImageProporties()   ->fbo,NORMAL_TEXTURE);
    glWidget->setPointerToTexture(&specularImageProp->getImageProporties() ->fbo,SPECULAR_TEXTURE);
    glWidget->setPointerToTexture(&heightImageProp->getImageProporties()   ->fbo,HEIGHT_TEXTURE);
    glWidget->setPointerToTexture(&occlusionImageProp->getImageProporties()->fbo,OCCLUSION_TEXTURE);

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
    glImage ->targetImageDiffuse   = diffuseImageProp  ->getImageProporties();

    // ------------------------------------------------------
    //                      GUI setup
    // ------------------------------------------------------
    ui->setupUi(this);
    ui->widget3DSettings->hide();

    // Settings container
    settingsContainer = new FormSettingsContainer;
    ui->verticalLayout2DImage->addWidget(settingsContainer);
    settingsContainer->hide();
    connect(settingsContainer,SIGNAL(reloadConfigFile()),this,SLOT(loadSettings()));
    connect(settingsContainer,SIGNAL(forceSaveCurrentConfig()),this,SLOT(saveSettings()));
    connect(ui->pushButtonProjectManager,SIGNAL(toggled(bool)),settingsContainer,SLOT(setVisible(bool)));


    ui->verticalLayout3DImage->addWidget(glWidget);
    ui->verticalLayout2DImage->addWidget(glImage);


    ui->verticalLayoutDiffuseImage  ->addWidget(diffuseImageProp);
    ui->verticalLayoutNormalImage   ->addWidget(normalImageProp);
    ui->verticalLayoutSpecularImage ->addWidget(specularImageProp);
    ui->verticalLayoutHeightImage   ->addWidget(heightImageProp);
    ui->verticalLayoutOcclusionImage->addWidget(occlusionImageProp);

    ui->tabWidget->setCurrentIndex(TAB_SETTINGS);
    
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(updateImage(int)));
    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),this,SLOT(updateImage(int)));
    
    // imageChange and imageLoaded signals
    connect(diffuseImageProp,SIGNAL(imageChanged()),this,SLOT(updateDiffuseImage()));
    connect(normalImageProp,SIGNAL(imageChanged()),this,SLOT(updateNormalImage()));
    connect(specularImageProp,SIGNAL(imageChanged()),this,SLOT(updateSpecularImage()));
    connect(heightImageProp,SIGNAL(imageChanged()),this,SLOT(updateHeightImage()));
    connect(occlusionImageProp,SIGNAL(imageChanged()),this,SLOT(updateOcclusionImage()));


    connect(diffuseImageProp,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(normalImageProp,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(specularImageProp,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(heightImageProp,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(occlusionImageProp,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));


    // image reload settings signal
    connect(diffuseImageProp   ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(normalImageProp    ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(specularImageProp  ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(heightImageProp    ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(occlusionImageProp ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));

    // conversion signals
    connect(heightImageProp,SIGNAL(conversionHeightToNormalApplied()) ,this,SLOT(convertFromHtoN()));
    connect(heightImageProp,SIGNAL(repaintNormalTexture()) ,this,SLOT(repaintNormalImage()));
    
    connect(normalImageProp,SIGNAL(conversionNormalToHeightApplied()) ,this,SLOT(convertFromNtoH()));
    connect(diffuseImageProp,SIGNAL(conversionBaseConversionApplied()),this,SLOT(convertFromBase()));

    // Global setting signals
    // sliders
    connect(ui->horizontalSliderDepthScale ,SIGNAL(valueChanged(int)),glWidget,SLOT(setDepthScale(int)));
    connect(ui->horizontalSliderUVScale    ,SIGNAL(valueChanged(int)),glWidget,SLOT(setUVScale(int)));
    connect(ui->horizontalSliderDepthScale ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVScale    ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVXOffset  ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));
    connect(ui->horizontalSliderUVYOffset  ,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));

    // Save signals
    connect(ui->pushButtonSaveAll,SIGNAL(released()),this,SLOT(saveImages()));
    connect(ui->pushButtonSaveChecked,SIGNAL(released()),this,SLOT(saveCheckedImages()));
    connect(ui->pushButtonSaveAs,SIGNAL(released()),this,SLOT(saveCompressedForm()));

    // image properties signals
    connect(ui->comboBoxResizeWidth   ,SIGNAL(currentIndexChanged(int)),this,SLOT(changeWidth(int)));
    connect(ui->comboBoxResizeHeight  ,SIGNAL(currentIndexChanged(int)),this,SLOT(changeHeight(int)));

    connect(ui->doubleSpinBoxRescaleWidth  ,SIGNAL(valueChanged(double)),this,SLOT(scaleWidth(double)));
    connect(ui->doubleSpinBoxRescaleHeight ,SIGNAL(valueChanged(double)),this,SLOT(scaleHeight(double)));

    connect(ui->pushButtonResizeApply ,SIGNAL(released()),this,SLOT(applyResizeImage()));
    connect(ui->pushButtonRescaleApply,SIGNAL(released()),this,SLOT(applyScaleImage()));

    // Other signals - 3D settings
    connect(ui->pushButtonReplotAll           ,SIGNAL(released()),this,SLOT(replotAllImages()));
    connect(ui->pushButtonToggleDiffuse       ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleDiffuseView(bool)));
    connect(ui->pushButtonToggleSpecular      ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleSpecularView(bool)));
    connect(ui->pushButtonToggleOcclusion     ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleOcclusionView(bool)));
    connect(ui->pushButtonToggleNormal        ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleNormalView(bool)));
    connect(ui->pushButtonToggleHeight        ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleHeightView(bool)));
    connect(ui->pushButtonSaveCurrentSettings ,SIGNAL(released()),this,SLOT(saveSettings()));
    connect(ui->horizontalSliderSpecularI     ,SIGNAL(valueChanged(int)),this,SLOT(setSpecularIntensity(int)));
    connect(ui->horizontalSliderDiffuseI      ,SIGNAL(valueChanged(int)),this,SLOT(setDiffuseIntensity(int)));
    connect(ui->comboBoxImageOutputFormat     ,SIGNAL(activated(int)),this,SLOT(setOutputFormat(int)));

    // loading 3d mesh signal
    connect(ui->pushButtonLoadMesh            ,SIGNAL(released()),glWidget,SLOT(loadMeshFromFile()));
    connect(ui->comboBoxChooseOBJModel        ,SIGNAL(activated(QString)),glWidget,SLOT(chooseMeshFile(QString)));
    connect(ui->comboBoxShadingType           ,SIGNAL(activated(int)),glWidget,SLOT(selectShadingType(int)));
    // Other staff

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

#ifdef Q_OS_MAC
    if(ui->statusbar && !ui->statusbar->testAttribute(Qt::WA_MacNormalSize)) ui->statusbar->setAttribute(Qt::WA_MacSmallSize);
#endif

    // Checking for GUI styles
    QStringList guiStyleList = QStyleFactory::keys();
    qDebug() << "Supported GUI styles: " << guiStyleList.join(", ");
    ui->comboBoxGUIStyle->addItems(guiStyleList);

    // Now we can load settings
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


    aboutAction = new QAction(QIcon(":/content/cube.png"), tr("&About %1").arg(qApp->applicationName()), this);
    aboutAction->setToolTip(tr("Show information about AwesomeBump"));
    aboutAction->setMenuRole(QAction::AboutQtRole);
    aboutAction->setMenuRole(QAction::AboutRole);
    aboutQtAction = new QAction(QIcon(":/content/QtLogo.png"), tr("About &Qt"), this);
    aboutQtAction->setToolTip(tr("Show information about Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));


    QMenu *help = menuBar()->addMenu(tr("&Help"));
    help->addAction(aboutAction);
    help->addAction(aboutQtAction);

    QAction *action = ui->toolBar->toggleViewAction();
    ui->menubar->addAction(action);


}

MainWindow::~MainWindow()
{
  delete settingsContainer;
  delete ui;
}
void MainWindow::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent( event );
    
    qDebug() << "calling" << Q_FUNC_INFO;

    QSettings settings(QString(AB_INI), QSettings::IniFormat);
    settings.setValue("d_win_w",this->width());
    settings.setValue("d_win_h",this->height());
    settings.setValue("recent_dir",recentDir.absolutePath());
    settings.setValue("recent_mesh_dir",recentMeshDir.absolutePath());

    settingsContainer->close();
    glWidget->close();
    glImage->close();


}

void MainWindow::resizeEvent(QResizeEvent* event){
  QWidget::resizeEvent( event );
  replotAllImages();
  qDebug() << "calling " << Q_FUNC_INFO;
}

void MainWindow::showEvent(QShowEvent* event){
  QWidget::showEvent( event );
  qDebug() << "calling" << Q_FUNC_INFO;
  replotAllImages();
}

void MainWindow::replotAllImages(){
    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
    for(int i = 0 ; i < MAX_TEXTURES_TYPE ; i++){
        // skip normal and ambient occlusion
        if(i!=NORMAL_TEXTURE && i!=OCCLUSION_TEXTURE) updateImage(i);
        glImage->update();
    }
    // recalulate normal at the end
    updateImage(NORMAL_TEXTURE);
    glImage->update();
    // then ambient occlusion
    updateImage(OCCLUSION_TEXTURE);
    glImage->update();
    glImage->enableShadowRender(false);

    glImage->setActiveImage(lastActive);
    glWidget->update();
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
    ui->tabWidget->setCurrentIndex(TAB_SETTINGS);
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

    qDebug() << Q_FUNC_INFO << "Saving to dir:" << fileInfo.absoluteFilePath();

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
        QCoreApplication::processEvents();
        glImage->makeCurrent();

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

void MainWindow::initializeGL(){  
    static bool one_time = false;
    // Context is vallid at this moment
    if (!one_time){
      one_time = true;

      qDebug() << "calling" << Q_FUNC_INFO;
      
      // Loading default (initial) textures
      diffuseImageProp  ->setImage(QImage(QString(":/content/logo_D.png")));
      normalImageProp   ->setImage(QImage(QString(":/content/logo_N.png")));
      specularImageProp ->setImage(QImage(QString(":/content/logo_D.png")));
      heightImageProp   ->setImage(QImage(QString(":/content/logo_H.png")));
      occlusionImageProp->setImage(QImage(QString(":/content/logo_O.png")));

      diffuseImageProp  ->setImageName(ui->lineEditOutputName->text());
      normalImageProp   ->setImageName(ui->lineEditOutputName->text());
      heightImageProp   ->setImageName(ui->lineEditOutputName->text());
      specularImageProp ->setImageName(ui->lineEditOutputName->text());
      occlusionImageProp->setImageName(ui->lineEditOutputName->text());
      // Setting the active image
      glImage->setActiveImage(diffuseImageProp->getImageProporties());
    }
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

    updateImage(OCCLUSION_TEXTURE);
    glImage->update();
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
        default: // Settings
            return;
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

void MainWindow::applyResizeImage(int width, int height){
    QCoreApplication::processEvents();
    //int width  = ui->comboBoxResizeWidth->currentText().toInt();
    //int height = ui->comboBoxResizeHeight->currentText().toInt();
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
    if(ui->actionTranslateUV->isChecked()) glImage->selectUVManipulationMethod(UV_TRANSLATE);
    if(ui->actionGrabCorners->isChecked()) glImage->selectUVManipulationMethod(UV_GRAB_CORNERS);
    if(ui->actionScaleXY->isChecked())     glImage->selectUVManipulationMethod(UV_SCALE_XY);
}

QSize MainWindow::sizeHint() const
{
    QSettings settings(QString(AB_INI), QSettings::IniFormat);
    return QSize(settings.value("d_win_w",800).toInt(),settings.value("d_win_h",600).toInt());
}

void MainWindow::saveImageSettings(QString abbr,FormImageProp* image){


    QSettings settings(QString(AB_INI), QSettings::IniFormat);

    settings.setValue("t_"+abbr+"_bGrayScale"                       ,image->getImageProporties()->bGrayScale);
    settings.setValue("t_"+abbr+"_grayScaleR"                       ,image->getImageProporties()->grayScalePreset.R);
    settings.setValue("t_"+abbr+"_grayScaleG"                       ,image->getImageProporties()->grayScalePreset.G);
    settings.setValue("t_"+abbr+"_grayScaleB"                       ,image->getImageProporties()->grayScalePreset.B);
 
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

    settings.setValue("t_"+abbr+"_heightMinValue"                   ,image->getImageProporties()->heightMinValue);
    settings.setValue("t_"+abbr+"_heightMaxValue"                   ,image->getImageProporties()->heightMaxValue);
    settings.setValue("t_"+abbr+"_heightAveragingRadius"            ,image->getImageProporties()->heightAveragingRadius);

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

    QSettings settings(QString(AB_INI), QSettings::IniFormat);
    image->getImageProporties()->bGrayScale                         = settings.value("t_"+abbr+"_bGrayScale",false).toBool();
    image->getImageProporties()->grayScalePreset.R                  = settings.value("t_"+abbr+"_grayScaleR",0.333).toFloat();
    image->getImageProporties()->grayScalePreset.G                  = settings.value("t_"+abbr+"_grayScaleG",0.333).toFloat();
    image->getImageProporties()->grayScalePreset.B                  = settings.value("t_"+abbr+"_grayScaleB",0.333).toFloat();


    image->getImageProporties()->bInvertR                           = settings.value("t_"+abbr+"_bInvertR",false).toBool();
    image->getImageProporties()->bInvertG                           = settings.value("t_"+abbr+"_bInvertG",false).toBool();
    image->getImageProporties()->bInvertB                           = settings.value("t_"+abbr+"_bInvertB",false).toBool();
    image->getImageProporties()->bRemoveShading                     = settings.value("t_"+abbr+"_bRemoveShading",false).toBool();
    image->getImageProporties()->noRemoveShadingGaussIter           = settings.value("t_"+abbr+"_noRemoveShadingGaussIter",0).toInt();
    image->getImageProporties()->noBlurPasses                       = settings.value("t_"+abbr+"_noBlurPasses",0).toInt();
    image->getImageProporties()->bSpeclarControl                    = settings.value("t_"+abbr+"_bSpeclarControl",false).toBool();
    if(image->getImageProporties()->imageType == SPECULAR_TEXTURE)// enable specular control
         image->getImageProporties()->bSpeclarControl               = settings.value("t_"+abbr+"_bSpeclarControl",true).toBool();
    image->getImageProporties()->specularRadius                     = settings.value("t_"+abbr+"_specularRadius",10).toInt();
    image->getImageProporties()->specularW1                         = settings.value("t_"+abbr+"_specularW1",0.1).toFloat();
    image->getImageProporties()->specularW2                         = settings.value("t_"+abbr+"_specularW2",10.0).toFloat();
    image->getImageProporties()->specularContrast                   = settings.value("t_"+abbr+"_specularContrast",0.0).toFloat();
    image->getImageProporties()->specularAmplifier                  = settings.value("t_"+abbr+"_specularAmplifier",-3.0).toFloat();
    image->getImageProporties()->specularBrightness                 = settings.value("t_"+abbr+"_specularBrightness",0.0).toFloat();
    image->getImageProporties()->smallDetails                       = settings.value("t_"+abbr+"_smallDetails",0.0).toFloat();
    image->getImageProporties()->mediumDetails                      = settings.value("t_"+abbr+"_mediumDetails",0.0).toFloat();
    image->getImageProporties()->detailDepth                        = settings.value("t_"+abbr+"_detailDepth",1.0).toFloat();
    image->getImageProporties()->sharpenBlurAmount                  = settings.value("t_"+abbr+"_sharpenBlurAmount",0).toInt();
    image->getImageProporties()->normalsStep                        = settings.value("t_"+abbr+"_normalsStep",1.0).toFloat();

    image->getImageProporties()->heightAveragingRadius              = settings.value("t_"+abbr+"_heightAveragingRadius",0.0).toFloat();
    image->getImageProporties()->heightMinValue                     = settings.value("t_"+abbr+"_heightMinValue",0.0).toFloat();
    image->getImageProporties()->heightMaxValue                     = settings.value("t_"+abbr+"_heightMaxValue",1.0).toFloat();


    image->getImageProporties()->conversionHNDepth                  = settings.value("t_"+abbr+"_conversionHNDepth",10.0).toFloat();
    //image->getImageProporties()->bConversionHN                      = settings.value("t_"+abbr+"_bConversionHN",false).toBool();
    //image->getImageProporties()->bConversionNH                      = settings.value("t_"+abbr+"_bConversionNH",false).toBool();

    image->getImageProporties()->conversionNHItersHuge              = settings.value("t_"+abbr+"_conversionNHItersHuge",10).toInt();
    image->getImageProporties()->conversionNHItersVeryLarge         = settings.value("t_"+abbr+"_conversionNHItersVeryLarge",10).toInt();
    image->getImageProporties()->conversionNHItersLarge             = settings.value("t_"+abbr+"_conversionNHItersLarge",10).toInt();
    image->getImageProporties()->conversionNHItersMedium            = settings.value("t_"+abbr+"_conversionNHItersMedium",10).toInt();
    image->getImageProporties()->conversionNHItersSmall             = settings.value("t_"+abbr+"_conversionNHItersSmall",10).toInt();
    image->getImageProporties()->conversionNHItersVerySmall         = settings.value("t_"+abbr+"_conversionNHItersVerySmall",10).toInt();

    //image->getImageProporties()->bConversionBaseMap                 = settings.value("t_"+abbr+"_bConversionBaseMap",false).toBool();
    image->getImageProporties()->conversionBaseMapAmplitude         = settings.value("t_"+abbr+"_conversionBaseMapAmplitude",-1.0).toFloat();
    image->getImageProporties()->conversionBaseMapFlatness          = settings.value("t_"+abbr+"_conversionBaseMapFlatness",0.0).toFloat();
    image->getImageProporties()->conversionBaseMapNoIters           = settings.value("t_"+abbr+"_conversionBaseMapNoIters",2).toInt();
    image->getImageProporties()->conversionBaseMapFilterRadius      = settings.value("t_"+abbr+"_conversionBaseMapFilterRadius",0).toInt();
    image->getImageProporties()->conversionBaseMapMixNormals        = settings.value("t_"+abbr+"_conversionBaseMapMixNormals",0.5).toFloat();
    image->getImageProporties()->conversionBaseMapPreSmoothRadius   = settings.value("t_"+abbr+"_conversionBaseMapPreSmoothRadius",0).toFloat();
    image->getImageProporties()->conversionBaseMapBlending          = settings.value("t_"+abbr+"_conversionBaseMapBlending",0.5).toFloat();
    image->getImageProporties()->ssaoNoIters                        = settings.value("t_"+abbr+"_ssaoNoIters",20.0).toFloat();
    image->getImageProporties()->ssaoBias                           = settings.value("t_"+abbr+"_ssaoBias",-1.5).toFloat();
    image->getImageProporties()->ssaoDepth                          = settings.value("t_"+abbr+"_ssaoDepth",0.3).toFloat();
    image->getImageProporties()->ssaoIntensity                      = settings.value("t_"+abbr+"_ssaoIntensity",1.0).toFloat();
    image->reloadSettings();

}

void MainWindow::loadImageSettings(TextureTypes type){

    switch(type){
        case(DIFFUSE_TEXTURE):
            loadImageSettings("d",diffuseImageProp);
            break;
        case(NORMAL_TEXTURE):
            loadImageSettings("n",normalImageProp);
            break;
        case(SPECULAR_TEXTURE):
            loadImageSettings("s",specularImageProp);
            break;
        case(HEIGHT_TEXTURE):
            loadImageSettings("h",heightImageProp);
            break;
        case(OCCLUSION_TEXTURE):
            loadImageSettings("o",occlusionImageProp);
            break;
    }
    glImage ->repaint();
    glWidget->repaint();
}

void MainWindow::showSettingsManager(){
    settingsContainer->show();
}

void MainWindow::saveSettings(){
    qDebug() << "Calling" << Q_FUNC_INFO << "Saving to :"<< QString(AB_INI);
  
    QSettings settings(QString(AB_INI), QSettings::IniFormat);
    settings.setValue("d_win_w",this->width());
    settings.setValue("d_win_h",this->height());
    settings.setValue("recent_dir",recentDir.absolutePath());
    settings.setValue("recent_mesh_dir",recentMeshDir.absolutePath());

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
    settings.setValue("recent_mesh_dir",recentMeshDir.absolutePath());
    settings.setValue("gui_style",ui->comboBoxGUIStyle->currentText());

    // UV Settings
    settings.setValue("uv_tiling_type",ui->comboBoxSeamlessMode->currentIndex());
    settings.setValue("uv_tiling_radius",ui->horizontalSliderMakeSeamlessRadius->value());
    settings.setValue("uv_tiling_mirror_x",ui->radioButtonMirrorModeX->isChecked());
    settings.setValue("uv_tiling_mirror_y",ui->radioButtonMirrorModeY->isChecked());
    settings.setValue("uv_tiling_mirror_xy",ui->radioButtonMirrorModeXY->isChecked());
    settings.setValue("uv_tiling_random_inner_radius",ui->horizontalSliderRandomPatchesInnerRadius->value());
    settings.setValue("uv_tiling_random_outer_radius",ui->horizontalSliderRandomPatchesOuterRadius->value());
    settings.setValue("uv_tiling_random_rotate",ui->horizontalSliderRandomPatchesRotate->value());

    settings.setValue("h_attachNormal",FBOImageProporties::bAttachNormalToHeightMap);
    settings.setValue("use_texture_interpolation",ui->checkBoxUseLinearTextureInterpolation->isChecked());

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
    static bool bFirstTime = true;
    qDebug() << "Calling" << Q_FUNC_INFO << " loading from " << QString(AB_INI);

    QSettings settings(QString(AB_INI), QSettings::IniFormat);

    if(bFirstTime){
        this->resize(settings.value("d_win_w",800).toInt(),settings.value("d_win_h",600).toInt());
    }

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


    FBOImageProporties::bAttachNormalToHeightMap = settings.value("h_attachNormal",true).toBool();
    heightImageProp->toggleAttachToNormal(FBOImageProporties::bAttachNormalToHeightMap);
    recentDir     = settings.value("recent_dir","").toString();
    recentMeshDir = settings.value("recent_mesh_dir","").toString();

    ui->checkBoxUseLinearTextureInterpolation->setChecked(settings.value("use_texture_interpolation",true).toBool());
    FBOImages::bUseLinearInterpolation = ui->checkBoxUseLinearTextureInterpolation->isChecked();
    ui->comboBoxGUIStyle->setCurrentText(settings.value("gui_style","default").toString());


    // UV Settings
    ui->comboBoxSeamlessMode->setCurrentIndex(settings.value("uv_tiling_type",0).toInt());
    selectSeamlessMode(ui->comboBoxSeamlessMode->currentIndex());
    ui->horizontalSliderMakeSeamlessRadius->setValue(settings.value("uv_tiling_radius",50).toInt());
    ui->radioButtonMirrorModeX->setChecked(settings.value("uv_tiling_mirror_x",false).toBool());
    ui->radioButtonMirrorModeY->setChecked(settings.value("uv_tiling_mirror_y",false).toBool());
    ui->radioButtonMirrorModeXY->setChecked(settings.value("uv_tiling_mirror_xy",true).toBool());
    ui->horizontalSliderRandomPatchesInnerRadius->setValue(settings.value("uv_tiling_random_inner_radius",50).toInt());
    ui->horizontalSliderRandomPatchesOuterRadius->setValue(settings.value("uv_tiling_random_outer_radius",50).toInt());
    ui->horizontalSliderRandomPatchesRotate->setValue(settings.value("uv_tiling_random_rotate",50).toInt());



    loadImageSettings("d",diffuseImageProp);
    loadImageSettings("n",normalImageProp);
    loadImageSettings("s",specularImageProp);
    loadImageSettings("h",heightImageProp);
    loadImageSettings("o",occlusionImageProp);

    replotAllImages();
    glImage ->repaint();
    glWidget->repaint();
    bFirstTime = false;
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Awesome Bump"), tr("AwesomeBump is an open source program designed to generate normal, height, specular or ambient occlusion textures from a single image. Since the image processing is done in 99% on GPU  the program runs very fast and all the parameters can be changed in real time."));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr("Awesome Bump"));
}
