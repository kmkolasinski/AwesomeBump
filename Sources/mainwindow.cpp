#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

extern QString _find_data_dir(const QString& path);

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

    statusLabel = new QLabel("Memory left:");

    glImage          = new GLImage(this);
    glWidget         = new GLWidget(this,glImage);



    connect(glImage,SIGNAL(rendered()),this,SLOT(initializeImages()));

    diffuseImageProp  = new FormImageProp(this,glImage);
    normalImageProp   = new FormImageProp(this,glImage);
    specularImageProp = new FormImageProp(this,glImage);
    heightImageProp   = new FormImageProp(this,glImage);
    occlusionImageProp= new FormImageProp(this,glImage);
    roughnessImageProp= new FormImageProp(this,glImage);
    metallicImageProp = new FormImageProp(this,glImage);
    grungeImageProp   = new FormImageProp(this,glImage);
    unitySupport = new UnitySupportGui(this);


    materialManager = new FormMaterialIndicesManager(this,glImage);

    // setting pointers to images

    materialManager->imagesPointers[0]  = diffuseImageProp;
    materialManager->imagesPointers[1]  = normalImageProp;
    materialManager->imagesPointers[2]  = specularImageProp;
    materialManager->imagesPointers[3]  = heightImageProp;
    materialManager->imagesPointers[4]  = occlusionImageProp;
    materialManager->imagesPointers[5]  = roughnessImageProp;
    materialManager->imagesPointers[6]  = metallicImageProp;


    // Setting pointers to 3D view (this pointer are used to bindTextures).
    glWidget->setPointerToTexture(&diffuseImageProp->getImageProporties()  ->fbo,DIFFUSE_TEXTURE);
    glWidget->setPointerToTexture(&normalImageProp->getImageProporties()   ->fbo,NORMAL_TEXTURE);
    glWidget->setPointerToTexture(&specularImageProp->getImageProporties() ->fbo,SPECULAR_TEXTURE);
    glWidget->setPointerToTexture(&heightImageProp->getImageProporties()   ->fbo,HEIGHT_TEXTURE);
    glWidget->setPointerToTexture(&occlusionImageProp->getImageProporties()->fbo,OCCLUSION_TEXTURE);
    glWidget->setPointerToTexture(&roughnessImageProp->getImageProporties()->fbo,ROUGHNESS_TEXTURE);
    glWidget->setPointerToTexture(&metallicImageProp->getImageProporties()->fbo ,METALLIC_TEXTURE);
    glWidget->setPointerToTexture(&materialManager->getImageProporties()->fbo,MATERIAL_TEXTURE);
    //glWidget->setPointerToTexture(&grungeImageProp->getImageProporties()->fbo,GRUNGE_TEXTURE);

    // Selecting type of image for each texture
    diffuseImageProp  ->getImageProporties()->imageType = DIFFUSE_TEXTURE;
    normalImageProp   ->getImageProporties()->imageType = NORMAL_TEXTURE;
    specularImageProp ->getImageProporties()->imageType = SPECULAR_TEXTURE;
    heightImageProp   ->getImageProporties()->imageType = HEIGHT_TEXTURE;
    occlusionImageProp->getImageProporties()->imageType = OCCLUSION_TEXTURE;
    roughnessImageProp->getImageProporties()->imageType = ROUGHNESS_TEXTURE;
    metallicImageProp ->getImageProporties()->imageType = METALLIC_TEXTURE;

    materialManager->getImageProporties()   ->imageType = MATERIAL_TEXTURE;
    grungeImageProp->getImageProporties()   ->imageType = GRUNGE_TEXTURE;


    // disabling some options for each texture
    specularImageProp->setSpecularControlChecked();
    specularImageProp->getImageProporties()->bSpeclarControl = true;
    specularImageProp->hideBMGroupBox();
    specularImageProp->hideNormalInputGroup();
    specularImageProp->hideHeightInputGroup();
    specularImageProp->hideOcclusionInputGroup();
    specularImageProp->hideNormalStepBar();
    specularImageProp->hideSelectiveBlurBox();
    specularImageProp->hideRoughnessInputGroup();


    diffuseImageProp->hideNormalInputGroup();
    diffuseImageProp->hideSpecularInputGroup();
    diffuseImageProp->hideHeightInputGroup();
    diffuseImageProp->hideSpecularGroupBox();
    diffuseImageProp->hideNormalStepBar();
    diffuseImageProp->hideOcclusionInputGroup();
    diffuseImageProp->hideSelectiveBlurBox();
    diffuseImageProp->hideRoughnessInputGroup();


    normalImageProp->hideSpecularInputGroup();
    normalImageProp->hideBMGroupBox();
    normalImageProp->hideSpecularGroupBox();
    normalImageProp->hideOcclusionInputGroup();
    normalImageProp->hideHeightProcessingBox();
    normalImageProp->hideGrayScaleControl();
    normalImageProp->hideSelectiveBlurBox();
    normalImageProp->hideHeightInputGroup();
    normalImageProp->hideRoughnessInputGroup();
    normalImageProp->showNormalMixerGroup();
    normalImageProp->hideGrungeBlendinModeComboBox();
    normalImageProp->showGrungeMainImageWeightSlider();

    heightImageProp->hideSpecularInputGroup();
    heightImageProp->hideNormalInputGroup();
    heightImageProp->hideBMGroupBox();
    heightImageProp->hideSpecularGroupBox();
    heightImageProp->hideNormalStepBar();
    heightImageProp->hideOcclusionInputGroup();
    heightImageProp->hideGrayScaleControl();
    heightImageProp->hideRoughnessInputGroup();


    occlusionImageProp->hideSpecularInputGroup();
    occlusionImageProp->hideSpecularGroupBox();
    occlusionImageProp->hideBMGroupBox();
    occlusionImageProp->hideNormalInputGroup();
    occlusionImageProp->hideHeightInputGroup();
    occlusionImageProp->hideNormalStepBar();
    occlusionImageProp->hideGrayScaleControl();
    occlusionImageProp->hideSelectiveBlurBox();
    occlusionImageProp->hideRoughnessInputGroup();

    
    roughnessImageProp->hideSpecularInputGroup();
    roughnessImageProp->hideNormalStepBar();
    roughnessImageProp->hideNormalInputGroup();
    roughnessImageProp->hideHeightInputGroup();
    roughnessImageProp->hideOcclusionInputGroup();
    roughnessImageProp->hideBMGroupBox();
    roughnessImageProp->hideSelectiveBlurBox();

    roughnessImageProp->hideGrayScaleControl();

    metallicImageProp->hideSpecularInputGroup();
    metallicImageProp->hideNormalStepBar();
    metallicImageProp->hideNormalInputGroup();
    metallicImageProp->hideHeightInputGroup();
    metallicImageProp->hideOcclusionInputGroup();
    metallicImageProp->hideBMGroupBox();
    metallicImageProp->hideSelectiveBlurBox();

    grungeImageProp->hideRoughnessInputGroup();
    grungeImageProp->hideSpecularInputGroup();
    grungeImageProp->hideNormalStepBar();
    grungeImageProp->hideNormalInputGroup();
    grungeImageProp->hideHeightInputGroup();
    grungeImageProp->hideOcclusionInputGroup();
    grungeImageProp->hideBMGroupBox();
    grungeImageProp->hideSelectiveBlurBox();
    grungeImageProp->hideSpecularGroupBox();
    grungeImageProp->showGrungeSettingsGroup();



    glImage ->targetImageNormal    = normalImageProp   ->getImageProporties();
    glImage ->targetImageHeight    = heightImageProp   ->getImageProporties();
    glImage ->targetImageSpecular  = specularImageProp ->getImageProporties();
    glImage ->targetImageOcclusion = occlusionImageProp->getImageProporties();
    glImage ->targetImageDiffuse   = diffuseImageProp  ->getImageProporties();
    glImage ->targetImageRoughness = roughnessImageProp->getImageProporties();
    glImage ->targetImageMetallic  = metallicImageProp ->getImageProporties();
    glImage ->targetImageMaterial  = materialManager   ->getImageProporties();
    glImage ->targetImageGrunge    = grungeImageProp   ->getImageProporties();
    // ------------------------------------------------------
    //                      GUI setup
    // ------------------------------------------------------
    ui->setupUi(this);
    ui->statusbar->addWidget(statusLabel);

    // Settings container
    settingsContainer = new FormSettingsContainer;
    ui->verticalLayout2DImage->addWidget(settingsContainer);
    settingsContainer->hide();
    connect(settingsContainer,SIGNAL(reloadConfigFile()),this,SLOT(loadSettings()));
    connect(settingsContainer,SIGNAL(forceSaveCurrentConfig()),this,SLOT(saveSettings()));
    connect(ui->pushButtonProjectManager,SIGNAL(toggled(bool)),settingsContainer,SLOT(setVisible(bool)));

    // -------------------------------------------------------
    // 3D settings widget
    // -------------------------------------------------------
    dock3Dsettings = new DockWidget3DSettings(this,glWidget);
    ui->verticalLayout3DImage->addWidget(dock3Dsettings);
    setDockNestingEnabled(true);
    connect(dock3Dsettings,SIGNAL(signalSelectedShadingModel(int)),this,SLOT(selectShadingModel(int)));
    // show hide 3D settings
    connect(ui->pushButton3DSettings ,SIGNAL(toggled(bool)),dock3Dsettings,SLOT(setVisible(bool)));


    ui->verticalLayout3DImage->addWidget(glWidget);
    ui->verticalLayout2DImage->addWidget(glImage);




    ui->verticalLayoutDiffuseImage  ->addWidget(diffuseImageProp);
    ui->verticalLayoutNormalImage   ->addWidget(normalImageProp);
    ui->verticalLayoutSpecularImage ->addWidget(specularImageProp);
    ui->verticalLayoutHeightImage   ->addWidget(heightImageProp);
    ui->verticalLayoutOcclusionImage->addWidget(occlusionImageProp);
    ui->verticalLayoutRoughnessImage->addWidget(roughnessImageProp);
    ui->verticalLayoutMetallicImage ->addWidget(metallicImageProp);
    ui->verticalLayoutMaterialIndicesImage->addWidget(materialManager);
    ui->verticalLayoutGrungeImage   ->addWidget(grungeImageProp);
    ui->verticalLayoutUnity->addWidget(unitySupport);

    ui->tabWidget->setCurrentIndex(TAB_SETTINGS);
    
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(updateImage(int)));
    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),this,SLOT(updateImage(int)));
    
    // imageChange and imageLoaded signals
    connect(diffuseImageProp    ,SIGNAL(imageChanged()),this,SLOT(updateDiffuseImage()));
    connect(normalImageProp     ,SIGNAL(imageChanged()),this,SLOT(updateNormalImage()));
    connect(specularImageProp   ,SIGNAL(imageChanged()),this,SLOT(updateSpecularImage()));
    connect(heightImageProp     ,SIGNAL(imageChanged()),this,SLOT(updateHeightImage()));
    connect(occlusionImageProp  ,SIGNAL(imageChanged()),this,SLOT(updateOcclusionImage()));
    connect(roughnessImageProp  ,SIGNAL(imageChanged()),this,SLOT(updateRoughnessImage()));
    connect(metallicImageProp   ,SIGNAL(imageChanged()),this,SLOT(updateMetallicImage()));
    connect(grungeImageProp     ,SIGNAL(imageChanged()),this,SLOT(updateGrungeImage()));
    // grunge
    connect(grungeImageProp,SIGNAL(toggleGrungeSettings(bool)),diffuseImageProp     ,SLOT(toggleGrungeImageSettingsGroup(bool)));
    connect(grungeImageProp,SIGNAL(toggleGrungeSettings(bool)),normalImageProp      ,SLOT(toggleGrungeImageSettingsGroup(bool)));
    connect(grungeImageProp,SIGNAL(toggleGrungeSettings(bool)),specularImageProp    ,SLOT(toggleGrungeImageSettingsGroup(bool)));
    connect(grungeImageProp,SIGNAL(toggleGrungeSettings(bool)),heightImageProp      ,SLOT(toggleGrungeImageSettingsGroup(bool)));
    connect(grungeImageProp,SIGNAL(toggleGrungeSettings(bool)),occlusionImageProp   ,SLOT(toggleGrungeImageSettingsGroup(bool)));
    connect(grungeImageProp,SIGNAL(toggleGrungeSettings(bool)),roughnessImageProp   ,SLOT(toggleGrungeImageSettingsGroup(bool)));
    connect(grungeImageProp,SIGNAL(toggleGrungeSettings(bool)),metallicImageProp    ,SLOT(toggleGrungeImageSettingsGroup(bool)));

    // Material Manager slots
    connect(materialManager,SIGNAL(materialChanged()),this,SLOT(replotAllImages()));   
    connect(materialManager,SIGNAL(materialsToggled(bool)),ui->tabTilling,SLOT(setDisabled(bool)));
    connect(glWidget,SIGNAL(materialColorPicked(QColor)),materialManager,SLOT(chooseMaterialByColor(QColor)));


    connect(diffuseImageProp  ,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(normalImageProp   ,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(specularImageProp ,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(heightImageProp   ,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(occlusionImageProp,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(roughnessImageProp,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    connect(metallicImageProp ,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));
    //connect(grungeImageProp   ,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));

    connect(materialManager ,SIGNAL(imageLoaded(int,int)),this,SLOT(applyResizeImage(int,int)));

    // image reload settings signal
    connect(diffuseImageProp   ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(normalImageProp    ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(specularImageProp  ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(heightImageProp    ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(occlusionImageProp ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(roughnessImageProp ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(metallicImageProp  ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));
    connect(grungeImageProp    ,SIGNAL(reloadSettingsFromConfigFile(TextureTypes)),this,SLOT(loadImageSettings(TextureTypes)));

    // conversion signals
    connect(normalImageProp   ,SIGNAL(conversionHeightToNormalApplied()) ,this,SLOT(convertFromHtoN()));
    connect(heightImageProp   ,SIGNAL(conversionNormalToHeightApplied()) ,this,SLOT(convertFromNtoH()));
    connect(diffuseImageProp  ,SIGNAL(conversionBaseConversionApplied()),this,SLOT(convertFromBase()));
    connect(occlusionImageProp,SIGNAL(conversionHeightNormalToOcclusionApplied()),this,SLOT(convertFromHNtoOcc()));


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


    connect(ui->pushButtonReplotAll           ,SIGNAL(released()),this,SLOT(replotAllImages()));
    connect(ui->pushButtonResetCameraPosition ,SIGNAL(released()),glWidget,SLOT(resetCameraPosition()));
    connect(ui->pushButtonChangeCamPosition   ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleChangeCamPosition(bool)));
    connect(glWidget,SIGNAL(changeCamPositionApplied(bool)),ui->pushButtonChangeCamPosition   ,SLOT(setChecked(bool)));


    connect(ui->pushButtonToggleDiffuse       ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleDiffuseView(bool)));
    connect(ui->pushButtonToggleSpecular      ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleSpecularView(bool)));
    connect(ui->pushButtonToggleOcclusion     ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleOcclusionView(bool)));
    connect(ui->pushButtonToggleNormal        ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleNormalView(bool)));
    connect(ui->pushButtonToggleHeight        ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleHeightView(bool)));
    connect(ui->pushButtonToggleRoughness     ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleRoughnessView(bool)));
    connect(ui->pushButtonToggleMetallic      ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleMetallicView(bool)));
    connect(ui->pushButtonSaveCurrentSettings ,SIGNAL(released()),this,SLOT(saveSettings()));
    connect(ui->comboBoxImageOutputFormat     ,SIGNAL(activated(int)),this,SLOT(setOutputFormat(int)));


    // Other staff

    ui->progressBar->setValue(0);

    connect(ui->actionReplot             ,SIGNAL(triggered()),this,SLOT(replotAllImages()));
    connect(ui->actionShowDiffuseImage   ,SIGNAL(triggered()),this,SLOT(selectDiffuseTab()));
    connect(ui->actionShowNormalImage    ,SIGNAL(triggered()),this,SLOT(selectNormalTab()));
    connect(ui->actionShowSpecularImage  ,SIGNAL(triggered()),this,SLOT(selectSpecularTab()));
    connect(ui->actionShowHeightImage    ,SIGNAL(triggered()),this,SLOT(selectHeightTab()));
    connect(ui->actionShowOcclusiontImage,SIGNAL(triggered()),this,SLOT(selectOcclusionTab()));
    connect(ui->actionShowRoughnessImage ,SIGNAL(triggered()),this,SLOT(selectRoughnessTab()));
    connect(ui->actionShowMetallicImage  ,SIGNAL(triggered()),this,SLOT(selectMetallicTab()));
    connect(ui->actionShowMaterialsImage ,SIGNAL(triggered()),this,SLOT(selectMaterialsTab()));
    connect(ui->actionShowGrungeTexture  ,SIGNAL(triggered()),this,SLOT(selectGrungeTab()));



    connect(ui->checkBoxSaveDiffuse ,SIGNAL(toggled(bool)),this,SLOT(showHideTextureTypes(bool)));
    connect(ui->checkBoxSaveNormal  ,SIGNAL(toggled(bool)),this,SLOT(showHideTextureTypes(bool)));
    connect(ui->checkBoxSaveSpecular,SIGNAL(toggled(bool)),this,SLOT(showHideTextureTypes(bool)));
    connect(ui->checkBoxSaveHeight,SIGNAL(toggled(bool)),this,SLOT(showHideTextureTypes(bool)));
    connect(ui->checkBoxSaveOcclusion,SIGNAL(toggled(bool)),this,SLOT(showHideTextureTypes(bool)));
    connect(ui->checkBoxSaveRoughness,SIGNAL(toggled(bool)),this,SLOT(showHideTextureTypes(bool)));
    connect(ui->checkBoxSaveMetallic,SIGNAL(toggled(bool)),this,SLOT(showHideTextureTypes(bool)));




    connect(ui->actionShowSettingsImage ,SIGNAL(triggered()),this,SLOT(selectGeneralSettingsTab()));
    connect(ui->actionShowUVsTab        ,SIGNAL(triggered()),this,SLOT(selectUVsTab()));
    connect(ui->actionFitToScreen       ,SIGNAL(triggered()),this,SLOT(fitImage()));

    // perspective tool
    connect(ui->pushButtonResetTransform            ,SIGNAL(released()),this,SLOT(resetTransform()));
    connect(ui->comboBoxPerspectiveTransformMethod  ,SIGNAL(activated(int)),glImage,SLOT(selectPerspectiveTransformMethod(int)));
    connect(ui->comboBoxSeamlessMode                ,SIGNAL(activated(int)),this,SLOT(selectSeamlessMode(int)));
    connect(ui->comboBoxSeamlessContrastInputImage  ,SIGNAL(activated(int)),this,SLOT(selectContrastInputImage(int)));

    // uv seamless algorithms
    connect(ui->checkBoxUVTranslationsFirst,SIGNAL(clicked()),this,SLOT(updateSliders()));

    connect(ui->horizontalSliderMakeSeamlessRadius,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderMakeSeamlessRadius,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));

    connect(ui->horizontalSliderSeamlessContrastStrenght,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderSeamlessContrastStrenght,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));

    connect(ui->horizontalSliderSeamlessContrastPower,SIGNAL(sliderReleased()),this,SLOT(updateSliders()));
    connect(ui->horizontalSliderSeamlessContrastPower,SIGNAL(valueChanged(int)),this,SLOT(updateSpinBoxes(int)));

    QButtonGroup *groupSimpleDirectionMode = new QButtonGroup( this );
    groupSimpleDirectionMode->addButton( ui->radioButtonSeamlessSimpleDirXY);
    groupSimpleDirectionMode->addButton( ui->radioButtonSeamlessSimpleDirX);
    groupSimpleDirectionMode->addButton( ui->radioButtonSeamlessSimpleDirY);
    connect(ui->radioButtonSeamlessSimpleDirXY ,SIGNAL(released()),this,SLOT(updateSliders()));
    connect(ui->radioButtonSeamlessSimpleDirX ,SIGNAL(released()),this,SLOT(updateSliders()));
    connect(ui->radioButtonSeamlessSimpleDirY,SIGNAL(released()),this,SLOT(updateSliders()));

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


    // apply UVs tranformations
    connect(ui->pushButtonApplyUVtransformations,SIGNAL(released()),this,SLOT(applyCurrentUVsTransformations()));

    ui->groupBoxSimpleSeamlessMode->hide();
    ui->groupBoxMirrorMode->hide();
    ui->groupBoxRandomPatchesMode->hide();


    // color picking
    connect(metallicImageProp ,SIGNAL(toggleColorPickingApplied(bool)),glImage,SLOT(toggleColorPicking(bool)));
    connect(glImage           ,SIGNAL(colorPicked(QVector4D)),metallicImageProp,SLOT(colorPicked(QVector4D)));

    connect(roughnessImageProp ,SIGNAL(toggleColorPickingApplied(bool)),glImage,SLOT(toggleColorPicking(bool)));
    connect(glImage           ,SIGNAL(colorPicked(QVector4D)),roughnessImageProp,SLOT(colorPicked(QVector4D)));

    connect(diffuseImageProp  ,SIGNAL(toggleColorPickingApplied(bool)),glImage,SLOT(toggleColorPicking(bool)));
    connect(glImage           ,SIGNAL(colorPicked(QVector4D)),diffuseImageProp,SLOT(colorPicked(QVector4D)));


    // 2D imate tool box settings
    QActionGroup *group = new QActionGroup( this );
    group->addAction( ui->actionTranslateUV );
    group->addAction( ui->actionGrabCorners);
    group->addAction( ui->actionScaleXY );
    ui->actionTranslateUV->setChecked(true);
    connect(ui->actionTranslateUV,SIGNAL(triggered()),this,SLOT(setUVManipulationMethod()));
    connect(ui->actionGrabCorners,SIGNAL(triggered()),this,SLOT(setUVManipulationMethod()));
    connect(ui->actionScaleXY    ,SIGNAL(triggered()),this,SLOT(setUVManipulationMethod()));

    // other settings:
    connect(ui->spinBoxMouseSensitivity    ,SIGNAL(valueChanged(int)),glWidget,SLOT(setCameraMouseSensitivity(int)));
    connect(ui->spinBoxFontSize            ,SIGNAL(valueChanged(int)),this,SLOT(changeGUIFontSize(int)));
    connect(ui->checkBoxToggleMouseLoop    ,SIGNAL(toggled(bool)),glWidget,SLOT(toggleMouseWrap(bool)));
    connect(ui->checkBoxToggleMouseLoop    ,SIGNAL(toggled(bool)),glImage ,SLOT(toggleMouseWrap(bool)));
    connect(ui->checkBoxSaveUnityMetallic , SIGNAL(toggled(bool)), this, SLOT(on_checkBoxSaveUnityMetallic_stateChanged(bool)));




#ifdef Q_OS_MAC
    if(ui->statusbar && !ui->statusbar->testAttribute(Qt::WA_MacNormalSize)) ui->statusbar->setAttribute(Qt::WA_MacSmallSize);
#endif

    // Checking for GUI styles
    QStringList guiStyleList = QStyleFactory::keys();
    qDebug() << "Supported GUI styles: " << guiStyleList.join(", ");
    ui->comboBoxGUIStyle->addItems(guiStyleList);

    qDebug() << "Loading settings:" ;
    // Now we can load settings




    loadSettings();


    // Loading default (initial) textures
    diffuseImageProp   ->setImage(QImage(QString(":/resources/logo_D.png")));

    normalImageProp    ->setImage(QImage(QString(":/resources/logo_N.png")));
    specularImageProp  ->setImage(QImage(QString(":/resources/logo_D.png")));
    heightImageProp    ->setImage(QImage(QString(":/resources/logo_H.png")));
    occlusionImageProp ->setImage(QImage(QString(":/resources/logo_O.png")));
    roughnessImageProp ->setImage(QImage(QString(":/resources/logo_R.png")));
    metallicImageProp  ->setImage(QImage(QString(":/resources/logo_M.png")));
    grungeImageProp    ->setImage(QImage(QString(":/resources/logo_R.png")));
    materialManager    ->setImage(QImage(QString(":/resources/logo_R.png")));


    diffuseImageProp   ->setImageName(ui->lineEditOutputName->text());
    normalImageProp    ->setImageName(ui->lineEditOutputName->text());
    heightImageProp    ->setImageName(ui->lineEditOutputName->text());
    specularImageProp  ->setImageName(ui->lineEditOutputName->text());
    occlusionImageProp ->setImageName(ui->lineEditOutputName->text());
    roughnessImageProp ->setImageName(ui->lineEditOutputName->text());
    metallicImageProp  ->setImageName(ui->lineEditOutputName->text());
    grungeImageProp    ->setImageName(ui->lineEditOutputName->text());

    // Setting the active image
    glImage->setActiveImage(diffuseImageProp->getImageProporties());


    aboutAction = new QAction(QIcon(":/resources/cube.png"), tr("&About %1").arg(qApp->applicationName()), this);
    aboutAction->setToolTip(tr("Show information about AwesomeBump"));
    aboutAction->setMenuRole(QAction::AboutQtRole);
    aboutAction->setMenuRole(QAction::AboutRole);

    shortcutsAction = new QAction(QString("Shortcuts"),this);

    aboutQtAction = new QAction(QIcon(":/resources/QtLogo.png"), tr("About &Qt"), this);
    aboutQtAction->setToolTip(tr("Show information about Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);



    logAction = new QAction("Show log file",this);
    dialogLogger    = new DialogLogger(this);
    dialogShortcuts = new DialogShortcuts(this);
    dialogLogger->setModal(true);
    dialogShortcuts->setModal(true);

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(logAction, SIGNAL(triggered()), dialogLogger, SLOT(showLog()));
    connect(shortcutsAction, SIGNAL(triggered()), dialogShortcuts, SLOT(show()));


    QMenu *help = menuBar()->addMenu(tr("&Help"));
    help->addAction(aboutAction);
    help->addAction(aboutQtAction);
    help->addAction(logAction);
    help->addAction(shortcutsAction);

    QAction *action = ui->toolBar->toggleViewAction();
    ui->menubar->addAction(action);

}

MainWindow::~MainWindow()
{
    delete dialogLogger;
    delete dialogShortcuts;
    delete materialManager;
    delete settingsContainer;
    delete dock3Dsettings;
    delete diffuseImageProp;
    delete normalImageProp;
    delete specularImageProp;
    delete heightImageProp;
    delete occlusionImageProp;
    delete roughnessImageProp;
    delete grungeImageProp;
    delete metallicImageProp;

    delete statusLabel;
    delete glImage;
    delete glWidget;
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
    settings.setValue("gui_style",ui->comboBoxGUIStyle->currentText());
    settings.setValue("font_size",ui->spinBoxFontSize->value());
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

    // skip grunge map if conversion is enabled
    if(glImage->getConversionType() != CONVERT_FROM_D_TO_O){        
        updateImage(GRUNGE_TEXTURE);
        //glImage->update();
    }

    updateImage(DIFFUSE_TEXTURE);
    //glImage->update();

    updateImage(ROUGHNESS_TEXTURE);
    //glImage->update();

    updateImage(METALLIC_TEXTURE);
   // glImage->update();

    updateImage(HEIGHT_TEXTURE);
   // glImage->update();

    // recalulate normal at the end
    updateImage(NORMAL_TEXTURE);
   // glImage->update();
    // then ambient occlusion
    updateImage(OCCLUSION_TEXTURE);
    //glImage->update();

    updateImage(SPECULAR_TEXTURE);
   // glImage->update();

    updateImage(MATERIAL_TEXTURE);
   // glImage->update();



    glImage->enableShadowRender(false);

    glImage->setActiveImage(lastActive);
    glWidget->update();
    
    QGLContext* glContext = (QGLContext *) glWidget->context();
    GLCHK( glContext->makeCurrent() );

#ifndef Q_OS_MAC
    GpuInfo glGpu(glContext);
    QString menu_text;
    
    GLint gpuMemTotal = glGpu.getTotalMem();
    GLint gpuMemAvail = glGpu.getAvailMem();
    if(gpuMemTotal > 0)
    {
        menu_text = QString(" Memory Used:") + QString::number(float(gpuMemTotal - gpuMemAvail) / 1024.0f) + QString("[MB]")
                      + QString(" Memory Free:") + QString::number(float(gpuMemAvail) / 1024.0f) + QString("[MB]")
                      + QString(" Total Memory:") + QString::number(float(gpuMemTotal) / 1024.0f) + QString("[MB]");
    }
    else
    {
        menu_text = QString(" Memory Free:") + QString::number(float(gpuMemAvail) / 1024.0f) + QString("[MB]");
    }

    statusLabel->setText(menu_text);
#endif
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

void MainWindow::selectRoughnessTab(){
    ui->tabWidget->setCurrentIndex(5);
    updateImage(5);
}

void MainWindow::selectMetallicTab(){
    ui->tabWidget->setCurrentIndex(6);
    updateImage(6);
}

void MainWindow::selectMaterialsTab(){
    ui->tabWidget->setCurrentIndex(7);
    updateImage(7);
}
void MainWindow::selectGrungeTab(){
    ui->tabWidget->setCurrentIndex(8);
    updateImage(8);
}


void MainWindow::selectGeneralSettingsTab(){
    ui->tabWidget->setCurrentIndex(TAB_SETTINGS);
}
void MainWindow::selectUVsTab(){
    ui->tabWidget->setCurrentIndex(TAB_SETTINGS+1);
}


void MainWindow::fitImage(){
    glImage->resetView();
    glImage->repaint();
}


void MainWindow::showHideTextureTypes(bool){

    //qDebug() << "Toggle processing images";

    bool value = ui->checkBoxSaveDiffuse->isChecked();
    diffuseImageProp->getImageProporties()->bSkipProcessing = !value;
    ui->tabWidget->setTabEnabled(DIFFUSE_TEXTURE,value);
    ui->pushButtonToggleDiffuse->setVisible(value);
    ui->pushButtonToggleDiffuse->setChecked(value);
    ui->actionShowDiffuseImage->setVisible(value);

    value = ui->checkBoxSaveNormal->isChecked();
    normalImageProp->getImageProporties()->bSkipProcessing = !value;
    ui->tabWidget->setTabEnabled(NORMAL_TEXTURE,value);
    ui->pushButtonToggleNormal->setVisible(value);
    ui->pushButtonToggleNormal->setChecked(value);
    ui->actionShowNormalImage->setVisible(value);

    value = ui->checkBoxSaveHeight->isChecked();
    occlusionImageProp->getImageProporties()->bSkipProcessing = !value;
    ui->tabWidget->setTabEnabled(OCCLUSION_TEXTURE,value);
    ui->pushButtonToggleOcclusion->setVisible(value);
    ui->pushButtonToggleOcclusion->setChecked(value);
    ui->actionShowOcclusiontImage->setVisible(value);

    value = ui->checkBoxSaveOcclusion->isChecked();
    heightImageProp->getImageProporties()->bSkipProcessing = !value;
    ui->tabWidget->setTabEnabled(HEIGHT_TEXTURE,value);
    ui->pushButtonToggleHeight->setVisible(value);
    ui->pushButtonToggleHeight->setChecked(value);
    ui->actionShowHeightImage->setVisible(value);

    value = ui->checkBoxSaveSpecular->isChecked();
    specularImageProp->getImageProporties()->bSkipProcessing = !value;
    ui->tabWidget->setTabEnabled(SPECULAR_TEXTURE,value);
    ui->pushButtonToggleSpecular->setVisible(value);
    ui->pushButtonToggleSpecular->setChecked(value);
    ui->actionShowSpecularImage->setVisible(value);

    value = ui->checkBoxSaveRoughness->isChecked();
    roughnessImageProp->getImageProporties()->bSkipProcessing = !value;
    ui->tabWidget->setTabEnabled(ROUGHNESS_TEXTURE,value);
    ui->pushButtonToggleRoughness->setVisible(value);
    ui->pushButtonToggleRoughness->setChecked(value);
    ui->actionShowRoughnessImage->setVisible(value);

    value = ui->checkBoxSaveMetallic->isChecked();
    metallicImageProp->getImageProporties()->bSkipProcessing = !value;
    ui->tabWidget->setTabEnabled(METALLIC_TEXTURE,value);
    ui->pushButtonToggleMetallic->setVisible(value);
    ui->pushButtonToggleMetallic->setChecked(value);
    ui->actionShowMetallicImage->setVisible(value);

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

    diffuseImageProp   ->setImageName(ui->lineEditOutputName->text());
    normalImageProp    ->setImageName(ui->lineEditOutputName->text());
    heightImageProp    ->setImageName(ui->lineEditOutputName->text());
    specularImageProp  ->setImageName(ui->lineEditOutputName->text());
    occlusionImageProp ->setImageName(ui->lineEditOutputName->text());
    roughnessImageProp ->setImageName(ui->lineEditOutputName->text());
    metallicImageProp  ->setImageName(ui->lineEditOutputName->text());

    replotAllImages();
    setCursor(Qt::WaitCursor);
    QCoreApplication::processEvents();
    ui->progressBar->setValue(0);

    if(ui->checkBoxSaveUnityMetallic->isChecked())
    {
        ui->labelProgressInfo->setText("Computing Unity's metallic map");
        QImage metallic = metallicImageProp->getImageProporties()->getImage();
        QImage rough = roughnessImageProp->getImageProporties()->getImage();
        QImage unityMetallicMap(metallic.width(), metallic.height(), QImage::Format_ARGB32);

        computeUnityMetallic(metallic, rough, unityMetallicMap);
        ui->labelProgressInfo->setText("Saving Unity's metallic map...");
        saveMapToPng(dir,unityMetallicMap);

    }

	if (!bSaveCompressedFormImages) {
		ui->labelProgressInfo->setText("Saving diffuse image...");
		if (bSaveCheckedImages*ui->checkBoxSaveDiffuse->isChecked() || !bSaveCheckedImages) {
			diffuseImageProp->saveFileToDir(dir);
		}
		ui->progressBar->setValue(15);


		ui->labelProgressInfo->setText("Saving normal image...");
		if (bSaveCheckedImages*ui->checkBoxSaveNormal->isChecked() || !bSaveCheckedImages) {
			normalImageProp->saveFileToDir(dir);

		}
		ui->progressBar->setValue(30);
		ui->labelProgressInfo->setText("Saving specular image...");
		if (bSaveCheckedImages*ui->checkBoxSaveSpecular->isChecked() || !bSaveCheckedImages) {
			specularImageProp->saveFileToDir(dir);

		}
		ui->progressBar->setValue(45);

		ui->labelProgressInfo->setText("Saving height image...");
		if (bSaveCheckedImages*ui->checkBoxSaveHeight->isChecked() || !bSaveCheckedImages) {
			occlusionImageProp->saveFileToDir(dir);
		}

		ui->progressBar->setValue(60);
		ui->labelProgressInfo->setText("Saving occlusion image...");
		if (bSaveCheckedImages*ui->checkBoxSaveOcclusion->isChecked() || !bSaveCheckedImages) {
			heightImageProp->saveFileToDir(dir);
		}

		ui->progressBar->setValue(75);
		ui->labelProgressInfo->setText("Saving roughness image...");

		if (bSaveCheckedImages*ui->checkBoxSaveRoughness->isChecked() || !bSaveCheckedImages) {
			roughnessImageProp->saveFileToDir(dir);
		}

		ui->progressBar->setValue(90);
		ui->labelProgressInfo->setText("Saving metallic image...");
		if (bSaveCheckedImages*ui->checkBoxSaveMetallic->isChecked() || !bSaveCheckedImages) {
			metallicImageProp->saveFileToDir(dir);
		}
        ui->progressBar->setValue(100);
    }



    else{ // if using compressed format
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
        ui->labelProgressInfo->setText("Saving diffuse image..."); // Shouldn't this be Saving normal image?
        normalImageProp->saveImageToDir(dir,newNormalImage);

    }// end of saveAsCompressedFormat

    QCoreApplication::processEvents();
    ui->progressBar->setValue(100);
    ui->labelProgressInfo->setText("Done!");
    setCursor(Qt::ArrowCursor);


    return true;
}


void MainWindow::computeUnityMetallic(const QImage& metallic, const QImage& roughness, QImage& out) const
{

    if(!metallicImageProp->isCheckedGrayScale())
    {
        return;
    }

	int height = metallic.height();
	int width = metallic.width();
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
            QRgb* line_m = (QRgb *)metallic.scanLine(y);
            QRgb* line_r = (QRgb *)roughness.scanLine(y);
            QRgb* line_out = (QRgb *)out.scanLine(y);

            line_m += x; line_r += x; line_out += x;
            int m_value = qGreen(*line_m);

            *line_out = qRgba(m_value, m_value,m_value, 255-qGreen(*line_r)); /*1.Since every pixel's channel value is the same, I take the green channel.
                                                                                                    Unity needs the inversed roughness map (glossiness setup)
                                                                                                 2. Unity is using Blinn–Phong shading model which produces different results. ):
                                                                                                    But overall they look good*/


		}
	}
}

void MainWindow::saveMapToPng(const QString& dir, const QImage& map) const
{
    bool b = map.save(dir+"/"+ui->lineEditOutputName->text()+ui->lineEditPostfixUnityMetallic->text()+".png");
    qDebug() << "Map saved to " << dir << ": " << b;
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

    // replot normal if height was changed in attached mode
    if(specularImageProp->getImageProporties()->inputImageType == INPUT_FROM_DIFFUSE_OUTPUT){
        glImage->enableShadowRender(true);
        updateImage(SPECULAR_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(DIFFUSE_TEXTURE);
    }

    // replot normal if height was changed in attached mode
    if(roughnessImageProp->getImageProporties()->inputImageType == INPUT_FROM_DIFFUSE_OUTPUT){
        glImage->enableShadowRender(true);
        updateImage(ROUGHNESS_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(DIFFUSE_TEXTURE);
    }

    // replot normal if height was changed in attached mode
    if(metallicImageProp->getImageProporties()->inputImageType == INPUT_FROM_DIFFUSE_OUTPUT){
        glImage->enableShadowRender(true);
        updateImage(METALLIC_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(DIFFUSE_TEXTURE);
    }

    glWidget->repaint();
}
void MainWindow::updateNormalImage(){
    ui->lineEditOutputName->setText(normalImageProp->getImageName());
    glImage->repaint();

    // replot normal if  was changed in attached mode
    if(occlusionImageProp->getImageProporties()->inputImageType == INPUT_FROM_HO_NO){
        glImage->enableShadowRender(true);
        updateImage(OCCLUSION_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(NORMAL_TEXTURE);
    }

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
    if(normalImageProp->getImageProporties()->inputImageType == INPUT_FROM_HEIGHT_OUTPUT){
        glImage->enableShadowRender(true);
        updateImage(NORMAL_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(HEIGHT_TEXTURE);
    }
    // replot normal if  was changed in attached mode
    if(specularImageProp->getImageProporties()->inputImageType == INPUT_FROM_HEIGHT_OUTPUT){
        glImage->enableShadowRender(true);
        updateImage(SPECULAR_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(HEIGHT_TEXTURE);
    }

    // replot normal if  was changed in attached mode
    if(occlusionImageProp->getImageProporties()->inputImageType == INPUT_FROM_HI_NI||
       occlusionImageProp->getImageProporties()->inputImageType == INPUT_FROM_HO_NO){
        glImage->enableShadowRender(true);
        updateImage(OCCLUSION_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(HEIGHT_TEXTURE);
    }

    // replot normal if  was changed in attached mode
    if(roughnessImageProp->getImageProporties()->inputImageType == INPUT_FROM_HEIGHT_OUTPUT){
        glImage->enableShadowRender(true);
        updateImage(ROUGHNESS_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(HEIGHT_TEXTURE);
    }
    // replot normal if  was changed in attached mode
    if(metallicImageProp->getImageProporties()->inputImageType == INPUT_FROM_HEIGHT_OUTPUT){
        glImage->enableShadowRender(true);
        updateImage(METALLIC_TEXTURE);
        //glImage->updateGL();
        glImage->enableShadowRender(false);
        // set height tab back again
        updateImage(HEIGHT_TEXTURE);
    }
    glWidget->repaint();
}

void MainWindow::updateOcclusionImage(){
    ui->lineEditOutputName->setText(occlusionImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();
}

void MainWindow::updateRoughnessImage(){
    ui->lineEditOutputName->setText(roughnessImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();
}

void MainWindow::updateMetallicImage(){
    ui->lineEditOutputName->setText(metallicImageProp->getImageName());
    glImage->repaint();
    glWidget->repaint();
}

void MainWindow::updateGrungeImage(){

    bool test = (grungeImageProp->getImageProporties()->bGrungeReplotAllWhenChanged == true);
    //test *= (grungeImageProp->getImageProporties()->grungeOverallWeight > 0.0);

    //if replot enabled and grunge weight > 0 then replot all textures
    if(test){
        replotAllImages();

    }else{ // otherwise replot only the grunge map
        glImage->repaint();
        glWidget->repaint();
    }
}

void MainWindow::updateImageInformation(){

    ui->labelCurrentImageWidth ->setNum(diffuseImageProp->getImageProporties()->fbo->width());
    ui->labelCurrentImageHeight->setNum(diffuseImageProp->getImageProporties()->fbo->height());
}

void MainWindow::initializeGL(){  
    static bool one_time = false;
    // Context is vallid at this moment
    if (!one_time){
      one_time = true;

      qDebug() << "calling" << Q_FUNC_INFO;
      
      // Loading default (initial) textures
      diffuseImageProp  ->setImage(QImage(QString(":/resources/logo_D.png")));
      normalImageProp   ->setImage(QImage(QString(":/resources/logo_N.png")));
      specularImageProp ->setImage(QImage(QString(":/resources/logo_D.png")));
      heightImageProp   ->setImage(QImage(QString(":/resources/logo_H.png")));
      occlusionImageProp->setImage(QImage(QString(":/resources/logo_O.png")));
      roughnessImageProp->setImage(QImage(QString(":/resources/logo_R.png")));
      metallicImageProp ->setImage(QImage(QString(":/resources/logo_M.png")));
      grungeImageProp   ->setImage(QImage(QString(":/resources/logo_R.png")));

      diffuseImageProp  ->setImageName(ui->lineEditOutputName->text());
      normalImageProp   ->setImageName(ui->lineEditOutputName->text());
      heightImageProp   ->setImageName(ui->lineEditOutputName->text());
      specularImageProp ->setImageName(ui->lineEditOutputName->text());
      occlusionImageProp->setImageName(ui->lineEditOutputName->text());
      roughnessImageProp->setImageName(ui->lineEditOutputName->text());
      metallicImageProp ->setImageName(ui->lineEditOutputName->text());
      grungeImageProp   ->setImageName(ui->lineEditOutputName->text());
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
    //glImage->update();
    glImage->setActiveImage(lastActive);

}

void MainWindow::updateImage(int tType){
    switch(tType){
        case(DIFFUSE_TEXTURE ):
            glImage->setActiveImage(diffuseImageProp->getImageProporties());
            diffuseImageProp->cancelColorPicking();
            break;
        case(NORMAL_TEXTURE  ):
            glImage->setActiveImage(normalImageProp->getImageProporties());
            normalImageProp->cancelColorPicking();
            break;
        case(SPECULAR_TEXTURE):
            glImage->setActiveImage(specularImageProp->getImageProporties());
            specularImageProp->cancelColorPicking();
            break;
        case(HEIGHT_TEXTURE  ):
            glImage->setActiveImage(heightImageProp->getImageProporties());
            heightImageProp->cancelColorPicking();
            break;
        case(OCCLUSION_TEXTURE  ):
            glImage->setActiveImage(occlusionImageProp->getImageProporties());
            occlusionImageProp->cancelColorPicking();
            break;
        case(ROUGHNESS_TEXTURE  ):
            glImage->setActiveImage(roughnessImageProp->getImageProporties());
            roughnessImageProp->cancelColorPicking();
            break;
        case(METALLIC_TEXTURE  ):
            glImage->setActiveImage(metallicImageProp->getImageProporties());
            metallicImageProp->cancelColorPicking();
            break;
        case(MATERIAL_TEXTURE  ):
            glImage->setActiveImage(materialManager->getImageProporties());
            metallicImageProp->cancelColorPicking();
            break;
        case(GRUNGE_TEXTURE  ):
            glImage->setActiveImage(grungeImageProp->getImageProporties());
            grungeImageProp->cancelColorPicking();
            break;
        default: // Settings
            return;
    }
    //glImage->toggleColorPicking(false);
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

    int materiaIndex = FBOImageProporties::currentMaterialIndeks;
    materialManager->disableMaterials();

    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
    for(int i = 0 ; i < MAX_TEXTURES_TYPE ; i++){
        if( i != GRUNGE_TEXTURE){ // grunge map does not scale like other images
            glImage->resizeFBO(width,height);
            updateImage(i);
        }
    }
    glImage->enableShadowRender(false);
    glImage->setActiveImage(lastActive);
    replotAllImages();
    updateImageInformation();
    glWidget->repaint();
    // replot all material group after image resize

    FBOImageProporties::currentMaterialIndeks = materiaIndex;
    if(materialManager->isEnabled()){
       materialManager->toggleMaterials(true);
    }
}

void MainWindow::applyResizeImage(int width, int height){
    QCoreApplication::processEvents();

    qDebug() << "Image resize applied. Current image size is (" << width << "," << height << ")" ;
    int materiaIndex = FBOImageProporties::currentMaterialIndeks;
    materialManager->disableMaterials();
    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
    for(int i = 0 ; i < MAX_TEXTURES_TYPE ; i++){
        if( i != GRUNGE_TEXTURE){
            glImage->resizeFBO(width,height);
            updateImage(i);
        }
    }
    glImage->enableShadowRender(false);
    glImage->setActiveImage(lastActive);
    replotAllImages();
    updateImageInformation();
    glWidget->repaint();

    // replot all material group after image resize
    FBOImageProporties::currentMaterialIndeks = materiaIndex;
    if(materialManager->isEnabled()){
       materialManager->toggleMaterials(true);
    }
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
    int materiaIndex = FBOImageProporties::currentMaterialIndeks;
    materialManager->disableMaterials();
    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);
    for(int i = 0 ; i < MAX_TEXTURES_TYPE ; i++){
        glImage->resizeFBO(width,height);
        updateImage(i);
    }
    glImage->enableShadowRender(false);
    glImage->setActiveImage(lastActive);
    replotAllImages();
    updateImageInformation();
    glWidget->repaint();

    // replot all material group after image resize
    FBOImageProporties::currentMaterialIndeks = materiaIndex;
    if(materialManager->isEnabled()){
       materialManager->toggleMaterials(true);
    }

}

void MainWindow::applyCurrentUVsTransformations(){
    // get current diffuse image (with applied UVs transformations)
    QImage diffuseImage = diffuseImageProp->getImageProporties()->getImage();
    // reset all the transformations
    ui->comboBoxSeamlessMode->setCurrentIndex(0);
    selectSeamlessMode(0);
    resetTransform();
    // set it as default
    diffuseImageProp->setImage(diffuseImage);
    // generate all textures based on new one
    bool bConvValue = diffuseImageProp->getImageProporties()->bConversionBaseMap;
    diffuseImageProp->getImageProporties()->bConversionBaseMap = true;
    convertFromBase();
    diffuseImageProp->getImageProporties()->bConversionBaseMap = bConvValue;
}

void MainWindow::selectSeamlessMode(int mode){
    // some gui interaction -> hide and show
    ui->groupBoxSimpleSeamlessMode->hide();
    ui->groupBoxMirrorMode->hide();
    ui->groupBoxRandomPatchesMode->hide();
    ui->groupBoxUVContrastSettings->setDisabled(false);
    ui->horizontalSliderSeamlessContrastPower->setEnabled(true);
    ui->comboBoxSeamlessContrastInputImage->setEnabled(true);
    ui->doubleSpinBoxSeamlessContrastPower->setEnabled(true);
    switch(mode){
    case(SEAMLESS_NONE):
        break;
    case(SEAMLESS_SIMPLE):
        ui->groupBoxSimpleSeamlessMode->show();
        ui->labelContrastStrenght->setText("Contrast strenght");
        break;
    case(SEAMLESS_MIRROR):
        ui->groupBoxMirrorMode->show();
        ui->groupBoxUVContrastSettings->setDisabled(true);
        break;
    case(SEAMLESS_RANDOM):
        ui->groupBoxRandomPatchesMode->show();
        ui->labelContrastStrenght->setText("Radius");
        ui->doubleSpinBoxSeamlessContrastPower->setEnabled(false);
        ui->horizontalSliderSeamlessContrastPower->setEnabled(false);
        ui->comboBoxSeamlessContrastInputImage->setEnabled(false);
        break;
    default:
        break;
    }
    glImage->selectSeamlessMode((SeamlessMode)mode);

    replotAllImages();
}

void MainWindow::selectContrastInputImage(int mode){


    switch(mode){
    case(0):
        FBOImageProporties::seamlessContrastInputType = INPUT_FROM_HEIGHT_INPUT;
        break;
    case(1):
        FBOImageProporties::seamlessContrastInputType = INPUT_FROM_DIFFUSE_INPUT;
        break;
    case(2):
        FBOImageProporties::seamlessContrastInputType = INPUT_FROM_NORMAL_INPUT;
        break;
    case(3):
        FBOImageProporties::seamlessContrastInputType = INPUT_FROM_OCCLUSION_INPUT;
        break;
    default:
        break;
    }
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


void MainWindow::updateSpinBoxes(int){
    ui->doubleSpinBoxMakeSeamless->setValue(ui->horizontalSliderMakeSeamlessRadius->value()/100.0);

    // random tilling mode
    ui->doubleSpinBoxRandomPatchesAngle      ->setValue(ui->horizontalSliderRandomPatchesRotate     ->value());
    ui->doubleSpinBoxRandomPatchesInnerRadius->setValue(ui->horizontalSliderRandomPatchesInnerRadius->value()/100.0);
    ui->doubleSpinBoxRandomPatchesOuterRadius->setValue(ui->horizontalSliderRandomPatchesOuterRadius->value()/100.0);
    //seamless strenght
    ui->doubleSpinBoxSeamlessContrastStrenght->setValue(ui->horizontalSliderSeamlessContrastStrenght->value()/100.0);
    ui->doubleSpinBoxSeamlessContrastPower->setValue(ui->horizontalSliderSeamlessContrastPower->value()/100.0);
}

void MainWindow::selectShadingModel(int i){

      if(i == 0) ui->tabWidget->setTabText(5,"Roughness");
      if(i == 1) ui->tabWidget->setTabText(5,"Glossiness");
}


void MainWindow::convertFromHtoN(){   
    glImage->setConversionType(CONVERT_FROM_H_TO_N);
    glImage->enableShadowRender(true);
    glImage->setActiveImage(heightImageProp->getImageProporties());
    glImage->enableShadowRender(true);
    glImage->setConversionType(CONVERT_FROM_H_TO_N);
    glImage->setActiveImage(normalImageProp->getImageProporties());
    glImage->enableShadowRender(false);
    glImage->setConversionType(CONVERT_NONE);

    replotAllImages();

    qDebug() << "Conversion from height to normal applied";
}

void MainWindow::convertFromNtoH(){
    glImage->setConversionType(CONVERT_FROM_H_TO_N);// fake conversion
    glImage->enableShadowRender(true);
    glImage->setActiveImage(heightImageProp->getImageProporties());
    glImage->setConversionType(CONVERT_FROM_N_TO_H);
    glImage->enableShadowRender(true);
    glImage->setActiveImage(normalImageProp->getImageProporties());
    glImage->setConversionType(CONVERT_FROM_N_TO_H);
    glImage->enableShadowRender(true);
    glImage->setActiveImage(heightImageProp->getImageProporties());
    glImage->enableShadowRender(false);
    replotAllImages();

    qDebug() << "Conversion from normal to height applied";
}


void MainWindow::convertFromBase(){
    qDebug() << "Conversion from Base to others started";
    normalImageProp   ->setImageName(diffuseImageProp->getImageName());
    heightImageProp   ->setImageName(diffuseImageProp->getImageName());
    specularImageProp ->setImageName(diffuseImageProp->getImageName());
    occlusionImageProp->setImageName(diffuseImageProp->getImageName());
    roughnessImageProp->setImageName(diffuseImageProp->getImageName());
    metallicImageProp ->setImageName(diffuseImageProp->getImageName());
    glImage->setConversionType(CONVERT_FROM_D_TO_O);
    glImage->updateGLNow();
    glImage->setConversionType(CONVERT_FROM_D_TO_O);
    replotAllImages();
    qDebug() << "Conversion from Base to others applied";
}

void MainWindow::convertFromHNtoOcc(){

    glImage->setConversionType(CONVERT_FROM_HN_TO_OC);
    glImage->enableShadowRender(true);
    glImage->setActiveImage(heightImageProp->getImageProporties());

    glImage->setConversionType(CONVERT_FROM_HN_TO_OC);
    glImage->enableShadowRender(true);
    glImage->setActiveImage(normalImageProp->getImageProporties());

    glImage->setConversionType(CONVERT_FROM_HN_TO_OC);
    glImage->enableShadowRender(true);
    glImage->setActiveImage(occlusionImageProp->getImageProporties());
    glImage->enableShadowRender(false);
    replotAllImages();

    qDebug() << "Conversion from Height and Normal to Occlusion applied";
}

void MainWindow::updateSliders(){
    updateSpinBoxes(0);
    FBOImageProporties::seamlessSimpleModeRadius          = ui->doubleSpinBoxMakeSeamless->value();
    FBOImageProporties::seamlessContrastStrenght          = ui->doubleSpinBoxSeamlessContrastStrenght->value();
    FBOImageProporties::seamlessContrastPower             = ui->doubleSpinBoxSeamlessContrastPower->value();

    FBOImageProporties::seamlessRandomTiling.common_phase = ui->doubleSpinBoxRandomPatchesAngle->value()/180.0*3.1415926;
    FBOImageProporties::seamlessRandomTiling.inner_radius = ui->doubleSpinBoxRandomPatchesInnerRadius->value();
    FBOImageProporties::seamlessRandomTiling.outer_radius = ui->doubleSpinBoxRandomPatchesOuterRadius->value();


    FBOImageProporties::bSeamlessTranslationsFirst = ui->checkBoxUVTranslationsFirst->isChecked();
    // choosing the proper mirror mode
    if(ui->radioButtonMirrorModeXY->isChecked()) FBOImageProporties::seamlessMirroModeType = 0;
    if(ui->radioButtonMirrorModeX ->isChecked()) FBOImageProporties::seamlessMirroModeType = 1;
    if(ui->radioButtonMirrorModeY ->isChecked()) FBOImageProporties::seamlessMirroModeType = 2;

    // choosing the proper simple mode direction
    if(ui->radioButtonSeamlessSimpleDirXY->isChecked()) FBOImageProporties::seamlessSimpleModeDirection = 0;
    if(ui->radioButtonSeamlessSimpleDirX ->isChecked()) FBOImageProporties::seamlessSimpleModeDirection = 1;
    if(ui->radioButtonSeamlessSimpleDirY ->isChecked()) FBOImageProporties::seamlessSimpleModeDirection = 2;

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
    settings.setValue("t_"+abbr+"_bSkipProcessing" ,image->getImageProporties()->bSkipProcessing);


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
    settings.setValue("t_"+abbr+"_heightOffsetValue"                ,image->getImageProporties()->heightOffsetValue);
    settings.setValue("t_"+abbr+"_bHeightEnableNormalization"       ,image->getImageProporties()->bHeightEnableNormalization);



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
    settings.setValue("t_"+abbr+"_baseMapAngleCorrection"           ,image->getImageProporties()->baseMapAngleCorrection);
    settings.setValue("t_"+abbr+"_baseMapAngleWeight"               ,image->getImageProporties()->baseMapAngleWeight);

    settings.setValue("t_"+abbr+"_conversionBaseMapheightMaxX"       ,image->getImageProporties()->conversionBaseMapheightMax.x());
    settings.setValue("t_"+abbr+"_conversionBaseMapheightMaxY"       ,image->getImageProporties()->conversionBaseMapheightMax.y());
    settings.setValue("t_"+abbr+"_conversionBaseMapheightMaxZ"       ,image->getImageProporties()->conversionBaseMapheightMax.z());
    settings.setValue("t_"+abbr+"_conversionBaseMapheightMinX"       ,image->getImageProporties()->conversionBaseMapheightMin.x());
    settings.setValue("t_"+abbr+"_conversionBaseMapheightMinY"       ,image->getImageProporties()->conversionBaseMapheightMin.y());
    settings.setValue("t_"+abbr+"_conversionBaseMapheightMinZ"       ,image->getImageProporties()->conversionBaseMapheightMin.z());
    settings.setValue("t_"+abbr+"_conversionBaseMapHeightMinMaxTolerance"       ,image->getImageProporties()->conversionBaseMapHeightMinMaxTolerance);

    settings.setValue("t_"+abbr+"_bConversionBaseMapShowHeightTexture",image->getImageProporties()->bConversionBaseMapShowHeightTexture);

    if(image->getImageProporties()->imageType == DIFFUSE_TEXTURE){
        for(int i = 0; i < 4 ; i++){
        QString level = "_Level"+QString::number(i);
        settings.setValue("t_"+abbr+"_conversionBaseMapAmplitude"      +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapAmplitude);
        settings.setValue("t_"+abbr+"_conversionBaseMapFlatness"       +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapFlatness);
        settings.setValue("t_"+abbr+"_conversionBaseMapNoIters"        +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapNoIters);
        settings.setValue("t_"+abbr+"_conversionBaseMapNoIters"        +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapNoIters);
        settings.setValue("t_"+abbr+"_conversionBaseMapFilterRadius"   +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapFilterRadius);
        settings.setValue("t_"+abbr+"_conversionBaseMapMixNormals"     +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapMixNormals);
        settings.setValue("t_"+abbr+"_conversionBaseMapPreSmoothRadius"+level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapPreSmoothRadius);
        settings.setValue("t_"+abbr+"_conversionBaseMapBlending"       +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapBlending);
        settings.setValue("t_"+abbr+"conversionBaseMapWeight"          +level ,image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapWeight);
        }
    }
    if(image->getImageProporties()->imageType == OCCLUSION_TEXTURE){
        settings.setValue("t_"+abbr+"_ssaoNoIters"                      ,image->getImageProporties()->ssaoNoIters);
        settings.setValue("t_"+abbr+"_ssaoBias"                         ,image->getImageProporties()->ssaoBias);
        settings.setValue("t_"+abbr+"_ssaoDepth"                        ,image->getImageProporties()->ssaoDepth);
        settings.setValue("t_"+abbr+"_ssaoIntensity"                    ,image->getImageProporties()->ssaoIntensity);
    }
    if(image->getImageProporties()->imageType == HEIGHT_TEXTURE){
        settings.setValue("t_"+abbr+"_selectiveBlurType"                ,image->getImageProporties()->selectiveBlurType);
        settings.setValue("t_"+abbr+"_bSelectiveBlurPreviewMask"        ,image->getImageProporties()->bSelectiveBlurPreviewMask);
        settings.setValue("t_"+abbr+"_bSelectiveBlurInvertMask"         ,image->getImageProporties()->bSelectiveBlurInvertMask);
        settings.setValue("t_"+abbr+"_bSelectiveBlurEnable"             ,image->getImageProporties()->bSelectiveBlurEnable);

        settings.setValue("t_"+abbr+"_selectiveBlurBlending"            ,image->getImageProporties()->selectiveBlurBlending);
        settings.setValue("t_"+abbr+"_selectiveBlurMaskRadius"          ,image->getImageProporties()->selectiveBlurMaskRadius);
        settings.setValue("t_"+abbr+"_selectiveBlurDOGRadius"           ,image->getImageProporties()->selectiveBlurDOGRadius);
        settings.setValue("t_"+abbr+"_selectiveBlurDOGConstrast"        ,image->getImageProporties()->selectiveBlurDOGConstrast);
        settings.setValue("t_"+abbr+"_selectiveBlurDOGAmplifier"        ,image->getImageProporties()->selectiveBlurDOGAmplifier);
        settings.setValue("t_"+abbr+"_selectiveBlurBlending"            ,image->getImageProporties()->selectiveBlurBlending);
        settings.setValue("t_"+abbr+"_selectiveBlurDOGOffset"           ,image->getImageProporties()->selectiveBlurDOGOffset);

        settings.setValue("t_"+abbr+"_selectiveBlurMinValue"            ,image->getImageProporties()->selectiveBlurMinValue);
        settings.setValue("t_"+abbr+"_selectiveBlurMaxValue"            ,image->getImageProporties()->selectiveBlurMaxValue);
        settings.setValue("t_"+abbr+"_selectiveBlurDetails"             ,image->getImageProporties()->selectiveBlurDetails);
        settings.setValue("t_"+abbr+"_selectiveBlurOffsetValue"         ,image->getImageProporties()->selectiveBlurOffsetValue);
    }
    settings.setValue("t_"+abbr+"_inputImageType"                   ,image->getImageProporties()->inputImageType);
    settings.setValue("t_"+abbr+"_roughnessDepth"                   ,image->getImageProporties()->roughnessDepth);


    settings.setValue("t_"+abbr+"_aoCancellation"                   ,image->getImageProporties()->aoCancellation);
    settings.setValue("t_"+abbr+"_removeShadingLFBlending"          ,image->getImageProporties()->removeShadingLFBlending);
    settings.setValue("t_"+abbr+"_removeShadingLFRadius"            ,image->getImageProporties()->removeShadingLFRadius);
    settings.setValue("t_"+abbr+"_colorHue"                         ,image->getImageProporties()->colorHue);


    settings.setValue("t_"+abbr+"_bRoughnessEnableColorPicking"     ,image->getImageProporties()->bRoughnessEnableColorPicking);
    settings.setValue("t_"+abbr+"_bRoughnessColorPickingToggled"    ,image->getImageProporties()->bRoughnessColorPickingToggled);
    settings.setValue("t_"+abbr+"_bRoughnessInvertColorMask"        ,image->getImageProporties()->bRoughnessInvertColorMask);

    settings.setValue("t_"+abbr+"_roughnessTreshold"                ,image->getImageProporties()->roughnessTreshold);
    settings.setValue("t_"+abbr+"_roughnessAmplifier"               ,image->getImageProporties()->roughnessAmplifier);
    settings.setValue("t_"+abbr+"_bRoughnessSurfaceEnable"          ,image->getImageProporties()->bRoughnessSurfaceEnable);

    settings.setValue("t_"+abbr+"_roughnessColorOffset"             ,image->getImageProporties()->roughnessColorOffset);
    settings.setValue("t_"+abbr+"_roughnessColorGlobalOffset"       ,image->getImageProporties()->roughnessColorGlobalOffset);
    settings.setValue("t_"+abbr+"_roughnessColorAmplifier"          ,image->getImageProporties()->roughnessColorAmplifier);

    settings.setValue("t_"+abbr+"_selectiveBlurNoIters"             ,image->getImageProporties()->selectiveBlurNoIters);
    settings.setValue("t_"+abbr+"_selectiveBlurMaskInputImageType"  ,image->getImageProporties()->selectiveBlurMaskInputImageType);
    settings.setValue("t_"+abbr+"_colorPickerMethod"                ,image->getImageProporties()->colorPickerMethod);
    settings.setValue("t_"+abbr+"_pickedColorR"                     ,image->getImageProporties()->pickedColor.x());
    settings.setValue("t_"+abbr+"_pickedColorG"                     ,image->getImageProporties()->pickedColor.y());
    settings.setValue("t_"+abbr+"_pickedColorB"                     ,image->getImageProporties()->pickedColor.z());

    if(image->getImageProporties()->imageType == GRUNGE_TEXTURE){
        settings.setValue("t_"+abbr+"_grungeOverallWeight"  ,image->getImageProporties()->grungeOverallWeight);
        settings.setValue("t_"+abbr+"_grungeSeed"           ,image->getImageProporties()->grungeSeed);
        settings.setValue("t_"+abbr+"_grungeRadius"         ,image->getImageProporties()->grungeRadius);
        settings.setValue("t_"+abbr+"_grungeNormalWarp"     ,image->getImageProporties()->grungeNormalWarp);
        settings.setValue("t_"+abbr+"_bGrungeEnableRandomTranslations",image->getImageProporties()->bGrungeEnableRandomTranslations);
        settings.setValue("t_"+abbr+"_bGrungeReplotAllWhenChanged",image->getImageProporties()->bGrungeReplotAllWhenChanged);
    }
    settings.setValue("t_"+abbr+"_grungeBlendingMode"   ,image->getImageProporties()->grungeBlendingMode);
    settings.setValue("t_"+abbr+"_grungeImageWeight"    ,image->getImageProporties()->grungeImageWeight);
    settings.setValue("t_"+abbr+"_grungeMainImageWeight",image->getImageProporties()->grungeMainImageWeight);


}

void MainWindow::loadImageSettings(QString abbr,FormImageProp* image){

    QSettings settings(QString(AB_INI), QSettings::IniFormat);


    image->getImageProporties()->bSkipProcessing                    = settings.value("t_"+abbr+"_bSkipProcessing",false).toBool();

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
    image->getImageProporties()->heightOffsetValue                  = settings.value("t_"+abbr+"_heightOffsetValue",0.0).toFloat();
    image->getImageProporties()->bHeightEnableNormalization         = settings.value("t_"+abbr+"_bHeightEnableNormalization",true).toBool();


    image->getImageProporties()->conversionHNDepth                  = settings.value("t_"+abbr+"_conversionHNDepth",10.0).toFloat();
    //image->getImageProporties()->bConversionHN                      = settings.value("t_"+abbr+"_bConversionHN",false).toBool();
    //image->getImageProporties()->bConversionNH                      = settings.value("t_"+abbr+"_bConversionNH",false).toBool();

    image->getImageProporties()->conversionNHItersHuge              = settings.value("t_"+abbr+"_conversionNHItersHuge",10).toInt();
    image->getImageProporties()->conversionNHItersVeryLarge         = settings.value("t_"+abbr+"_conversionNHItersVeryLarge",10).toInt();
    image->getImageProporties()->conversionNHItersLarge             = settings.value("t_"+abbr+"_conversionNHItersLarge",10).toInt();
    image->getImageProporties()->conversionNHItersMedium            = settings.value("t_"+abbr+"_conversionNHItersMedium",10).toInt();
    image->getImageProporties()->conversionNHItersSmall             = settings.value("t_"+abbr+"_conversionNHItersSmall",10).toInt();
    image->getImageProporties()->conversionNHItersVerySmall         = settings.value("t_"+abbr+"_conversionNHItersVerySmall",10).toInt();

    image->getImageProporties()->bConversionBaseMapShowHeightTexture= settings.value("t_"+abbr+"_bConversionBaseMapShowHeightTexture",false).toBool();
    image->getImageProporties()->baseMapAngleCorrection             = settings.value("t_"+abbr+"_baseMapAngleCorrection",0.0).toFloat();
    image->getImageProporties()->baseMapAngleWeight                 = settings.value("t_"+abbr+"_baseMapAngleWeight",0.0).toFloat();

    image->getImageProporties()->conversionBaseMapheightMax.setX(settings.value("t_"+abbr+"_conversionBaseMapheightMaxX",-1.0).toFloat());
    image->getImageProporties()->conversionBaseMapheightMax.setY(settings.value("t_"+abbr+"_conversionBaseMapheightMaxY", 0.0).toFloat());
    image->getImageProporties()->conversionBaseMapheightMax.setZ(settings.value("t_"+abbr+"_conversionBaseMapheightMaxZ", 0.0).toFloat());

    image->getImageProporties()->conversionBaseMapheightMin.setX(settings.value("t_"+abbr+"_conversionBaseMapheightMinX",-1.0).toFloat());
    image->getImageProporties()->conversionBaseMapheightMin.setY(settings.value("t_"+abbr+"_conversionBaseMapheightMinY", 0.0).toFloat());
    image->getImageProporties()->conversionBaseMapheightMin.setZ(settings.value("t_"+abbr+"_conversionBaseMapheightMinZ", 0.0).toFloat());

    image->getImageProporties()->conversionBaseMapHeightMinMaxTolerance = settings.value("t_"+abbr+"_conversionBaseMapHeightMinMaxTolerance", 0.0).toFloat();



    if(image->getImageProporties()->imageType == DIFFUSE_TEXTURE){
        for(int i = 0; i < 4 ; i++){
        QString level = "_Level"+QString::number(i);

        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapAmplitude         = settings.value("t_"+abbr+"_conversionBaseMapAmplitude"+level,-1.0).toFloat();
        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapFlatness          = settings.value("t_"+abbr+"_conversionBaseMapFlatness"+level,0.0).toFloat();
        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapNoIters           = settings.value("t_"+abbr+"_conversionBaseMapNoIters"+level,2).toInt();
        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapFilterRadius      = settings.value("t_"+abbr+"_conversionBaseMapFilterRadius"+level,0).toInt();
        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapMixNormals        = settings.value("t_"+abbr+"_conversionBaseMapMixNormals"+level,0.5).toFloat();
        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapPreSmoothRadius   = settings.value("t_"+abbr+"_conversionBaseMapPreSmoothRadius"+level,0).toFloat();
        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapBlending          = settings.value("t_"+abbr+"_conversionBaseMapBlending"+level,0.5).toFloat();
        image->getImageProporties()->baseMapConvLevels[i].conversionBaseMapWeight          = settings.value("t_"+abbr+"conversionBaseMapWeight"+level,50.0).toFloat();

        }
    }

    if(image->getImageProporties()->imageType == OCCLUSION_TEXTURE){
        image->getImageProporties()->ssaoNoIters                        = settings.value("t_"+abbr+"_ssaoNoIters",20.0).toFloat();
        image->getImageProporties()->ssaoBias                           = settings.value("t_"+abbr+"_ssaoBias",-1.5).toFloat();
        image->getImageProporties()->ssaoDepth                          = settings.value("t_"+abbr+"_ssaoDepth",0.3).toFloat();
        image->getImageProporties()->ssaoIntensity                      = settings.value("t_"+abbr+"_ssaoIntensity",1.0).toFloat();
    }
    image->getImageProporties()->aoCancellation                     = settings.value("t_"+abbr+"_aoCancellation",0.0).toFloat();
    image->getImageProporties()->removeShadingLFBlending            = settings.value("t_"+abbr+"_removeShadingLFBlending",0.0).toFloat();
    image->getImageProporties()->removeShadingLFRadius              = settings.value("t_"+abbr+"_removeShadingLFRadius",0.0).toFloat();
    image->getImageProporties()->colorHue                           = settings.value("t_"+abbr+"_colorHue",0.0).toFloat();

    if(image->getImageProporties()->imageType == HEIGHT_TEXTURE){
        image->getImageProporties()->selectiveBlurType                  = (SelectiveBlurType)settings.value("t_"+abbr+"_selectiveBlurType",0).toInt();
        image->getImageProporties()->bSelectiveBlurPreviewMask          = settings.value("t_"+abbr+"_bSelectiveBlurPreviewMask",false).toBool();
        image->getImageProporties()->bSelectiveBlurInvertMask           = settings.value("t_"+abbr+"_bSelectiveBlurInvertMask",false).toBool();
        image->getImageProporties()->bSelectiveBlurEnable               = settings.value("t_"+abbr+"_bSelectiveBlurEnable",false).toBool();

        image->getImageProporties()->selectiveBlurBlending              = settings.value("t_"+abbr+"_selectiveBlurBlending",0.3).toFloat();
        image->getImageProporties()->selectiveBlurMaskRadius            = settings.value("t_"+abbr+"_selectiveBlurMaskRadius",5).toInt();
        image->getImageProporties()->selectiveBlurDOGRadius             = settings.value("t_"+abbr+"_selectiveBlurDOGRadius",5).toInt();
        image->getImageProporties()->selectiveBlurDOGConstrast          = settings.value("t_"+abbr+"_selectiveBlurDOGConstrast",0.0).toFloat();
        image->getImageProporties()->selectiveBlurDOGAmplifier          = settings.value("t_"+abbr+"_selectiveBlurDOGAmplifier",5.0).toFloat();
        image->getImageProporties()->selectiveBlurDOGOffset             = settings.value("t_"+abbr+"_selectiveBlurDOGOffset",0.0).toFloat();

        image->getImageProporties()->selectiveBlurMinValue              = settings.value("t_"+abbr+"_selectiveBlurMinValue",0.0).toFloat();
        image->getImageProporties()->selectiveBlurMaxValue              = settings.value("t_"+abbr+"_selectiveBlurMaxValue",1.0).toFloat();
        image->getImageProporties()->selectiveBlurDetails               = settings.value("t_"+abbr+"_selectiveBlurDetails",0).toInt();
        image->getImageProporties()->selectiveBlurOffsetValue           = settings.value("t_"+abbr+"_selectiveBlurOffsetValue",0.0).toFloat();
    }

    image->getImageProporties()->inputImageType                     = (SourceImageType)settings.value("t_"+abbr+"_inputImageType",0).toInt();
    image->getImageProporties()->roughnessDepth                     = settings.value("t_"+abbr+"_roughnessDepth",0.3).toFloat();
    image->getImageProporties()->roughnessTreshold                  = settings.value("t_"+abbr+"_roughnessTreshold",0.0).toFloat();
    image->getImageProporties()->roughnessAmplifier                 = settings.value("t_"+abbr+"_roughnessAmplifier",0.0).toFloat();
    image->getImageProporties()->bRoughnessSurfaceEnable            = settings.value("t_"+abbr+"_bRoughnessSurfaceEnable",false).toBool();

    image->getImageProporties()->bRoughnessEnableColorPicking       = settings.value("t_"+abbr+"_bRoughnessEnableColorPicking",false).toBool();
    image->getImageProporties()->bRoughnessColorPickingToggled      = settings.value("t_"+abbr+"_bRoughnessColorPickingToggled",false).toBool();


    image->getImageProporties()->pickedColor.setX(settings.value("t_"+abbr+"_pickedColorR",0.0).toFloat());
    image->getImageProporties()->pickedColor.setY(settings.value("t_"+abbr+"_pickedColorG",0.0).toFloat());
    image->getImageProporties()->pickedColor.setZ(settings.value("t_"+abbr+"_pickedColorB",0.0).toFloat());

    image->getImageProporties()->colorPickerMethod              = (ColorPickerMethod)settings.value("t_"+abbr+"_colorPickerMethod",0).toInt();
    image->getImageProporties()->bRoughnessInvertColorMask      = settings.value("t_"+abbr+"_bRoughnessInvertColorMask",false).toBool();
    image->getImageProporties()->roughnessColorOffset           = settings.value("t_"+abbr+"_roughnessColorOffset",0.0).toFloat();
    image->getImageProporties()->roughnessColorGlobalOffset     = settings.value("t_"+abbr+"_roughnessColorGlobalOffset",0.0).toFloat();
    image->getImageProporties()->roughnessColorAmplifier        = settings.value("t_"+abbr+"_roughnessColorAmplifier",1.0).toFloat();

    image->getImageProporties()->selectiveBlurMaskInputImageType= (SourceImageType)settings.value("t_"+abbr+"_selectiveBlurMaskInputImageType",0).toInt();
    image->getImageProporties()->selectiveBlurNoIters           = settings.value("t_"+abbr+"_selectiveBlurNoIters",1).toInt();

    if(image->getImageProporties()->imageType == GRUNGE_TEXTURE){
        //image->getImageProporties()->grungeOverallWeight        = settings.value("t_"+abbr+"_grungeOverallWeight",0.0).toFloat();
        image->getImageProporties()->grungeRadius               = settings.value("t_"+abbr+"_grungeRadius",0.0).toFloat();
        image->getImageProporties()->grungeNormalWarp           = settings.value("t_"+abbr+"_grungeNormalWarp",0.0).toFloat();
        image->getImageProporties()->grungeSeed                 = settings.value("t_"+abbr+"_grungeSeed",0).toInt();
        image->getImageProporties()->bGrungeEnableRandomTranslations = settings.value("t_"+abbr+"_bGrungeEnableRandomTranslations",0.0).toBool();
        image->getImageProporties()->bGrungeReplotAllWhenChanged     = settings.value("t_"+abbr+"_bGrungeReplotAllWhenChanged",0.0).toBool();

    }
    image->getImageProporties()->grungeBlendingMode   = settings.value("t_"+abbr+"_grungeBlendingMode",0).toInt();
    image->getImageProporties()->grungeImageWeight    = settings.value("t_"+abbr+"_grungeImageWeight",0.0).toFloat();
    image->getImageProporties()->grungeMainImageWeight= settings.value("t_"+abbr+"_grungeMainImageWeight",50.0).toFloat();


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
        case(ROUGHNESS_TEXTURE):
            loadImageSettings("r",roughnessImageProp);
            break;
        case(METALLIC_TEXTURE):
            loadImageSettings("m",metallicImageProp);
            break;
        case(GRUNGE_TEXTURE):
            loadImageSettings("g",grungeImageProp);
            break;
        default: qWarning() << "Trying to load non supported image! Given textureType:" << type;
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

    settings.setValue("tab_win_w",ui->tabWidget->width());
    settings.setValue("tab_win_h",ui->tabWidget->height());



    settings.setValue("recent_dir",recentDir.absolutePath());
    settings.setValue("recent_mesh_dir",recentMeshDir.absolutePath());

    PostfixNames::diffuseName   = ui->lineEditPostfixDiffuse->text();
    PostfixNames::normalName    = ui->lineEditPostfixNormal->text();
    PostfixNames::specularName  = ui->lineEditPostfixSpecular->text();
    PostfixNames::heightName    = ui->lineEditPostfixHeight->text();
    PostfixNames::occlusionName = ui->lineEditPostfixOcclusion->text();
    PostfixNames::roughnessName = ui->lineEditPostfixRoughness->text();
    PostfixNames::metallicName  = ui->lineEditPostfixMetallic->text();


    settings.setValue("d_postfix",ui->lineEditPostfixDiffuse->text());
    settings.setValue("n_postfix",ui->lineEditPostfixNormal->text());
    settings.setValue("s_postfix",ui->lineEditPostfixSpecular->text());
    settings.setValue("h_postfix",ui->lineEditPostfixHeight->text());
    settings.setValue("o_postfix",ui->lineEditPostfixOcclusion->text());
    settings.setValue("r_postfix",ui->lineEditPostfixRoughness->text());
    settings.setValue("m_postfix",ui->lineEditPostfixMetallic->text());


    settings.setValue("d_enable",ui->checkBoxSaveDiffuse->isChecked());
    settings.setValue("n_enable",ui->checkBoxSaveNormal->isChecked());
    settings.setValue("s_enable",ui->checkBoxSaveSpecular->isChecked());
    settings.setValue("o_enable",ui->checkBoxSaveOcclusion->isChecked());
    settings.setValue("h_enable",ui->checkBoxSaveHeight->isChecked());
    settings.setValue("m_enable",ui->checkBoxSaveMetallic->isChecked());
    settings.setValue("r_enable",ui->checkBoxSaveRoughness->isChecked());


    settings.setValue("recent_dir"      ,recentDir.absolutePath());
    settings.setValue("recent_mesh_dir" ,recentMeshDir.absolutePath());
    settings.setValue("gui_style"       ,ui->comboBoxGUIStyle->currentText());

    // UV Settings
    settings.setValue("uv_tiling_type",ui->comboBoxSeamlessMode->currentIndex());
    settings.setValue("uv_tiling_radius",ui->horizontalSliderMakeSeamlessRadius->value());
    settings.setValue("uv_tiling_mirror_x",ui->radioButtonMirrorModeX->isChecked());
    settings.setValue("uv_tiling_mirror_y",ui->radioButtonMirrorModeY->isChecked());
    settings.setValue("uv_tiling_mirror_xy",ui->radioButtonMirrorModeXY->isChecked());
    settings.setValue("uv_tiling_random_inner_radius",ui->horizontalSliderRandomPatchesInnerRadius->value());
    settings.setValue("uv_tiling_random_outer_radius",ui->horizontalSliderRandomPatchesOuterRadius->value());
    settings.setValue("uv_tiling_random_rotate",ui->horizontalSliderRandomPatchesRotate->value());
    // UV contrast etc
    settings.setValue("uv_translations_first",ui->checkBoxUVTranslationsFirst->isChecked());
    settings.setValue("uv_contrast_strength",ui->doubleSpinBoxSeamlessContrastStrenght->value());
    settings.setValue("uv_contrast_power",ui->doubleSpinBoxSeamlessContrastPower->value());
    settings.setValue("uv_contrast_input_image",ui->comboBoxSeamlessContrastInputImage->currentIndex());
    settings.setValue("uv_tiling_simple_dir_xy",ui->radioButtonSeamlessSimpleDirXY->isChecked());
    settings.setValue("uv_tiling_simple_dir_x",ui->radioButtonSeamlessSimpleDirX->isChecked());
    settings.setValue("uv_tiling_simple_dir_y",ui->radioButtonSeamlessSimpleDirY->isChecked());


    // other parameters
    settings.setValue("use_texture_interpolation",ui->checkBoxUseLinearTextureInterpolation->isChecked());
    settings.setValue("mouse_sensitivity",ui->spinBoxMouseSensitivity->value());
    settings.setValue("font_size",ui->spinBoxFontSize->value());
    settings.setValue("mouse_loop",ui->checkBoxToggleMouseLoop->isChecked());


    dock3Dsettings->saveSettings();
    saveImageSettings("d",diffuseImageProp);
    saveImageSettings("n",normalImageProp);
    saveImageSettings("s",specularImageProp);
    saveImageSettings("h",heightImageProp);
    saveImageSettings("o",occlusionImageProp);
    saveImageSettings("r",roughnessImageProp);
    saveImageSettings("m",metallicImageProp);
    saveImageSettings("g",grungeImageProp);

}

void MainWindow::changeGUIFontSize(int value){
    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(value);
    QApplication::setFont(font);
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
        ui->tabWidget->resize(settings.value("tab_win_w",200).toInt(),
                              settings.value("tab_win_h",600).toInt());
    }

    PostfixNames::diffuseName   = settings.value("d_postfix","_d").toString();
    PostfixNames::normalName    = settings.value("n_postfix","_n").toString();
    PostfixNames::specularName  = settings.value("s_postfix","_s").toString();
    PostfixNames::heightName    = settings.value("h_postfix","_h").toString();
    PostfixNames::occlusionName = settings.value("o_postfix","_o").toString();
    PostfixNames::roughnessName = settings.value("m_postfix","_m").toString();
    PostfixNames::metallicName  = settings.value("r_postfix","_r").toString();

    ui->checkBoxSaveDiffuse->setChecked( settings.value("d_enable",true).toBool());
    ui->checkBoxSaveNormal->setChecked( settings.value("n_enable",true).toBool());
    ui->checkBoxSaveSpecular->setChecked( settings.value("s_enable",true).toBool());
    ui->checkBoxSaveOcclusion->setChecked( settings.value("o_enable",true).toBool());
    ui->checkBoxSaveHeight->setChecked( settings.value("h_enable",true).toBool());
    ui->checkBoxSaveMetallic->setChecked( settings.value("m_enable",true).toBool());
    ui->checkBoxSaveRoughness->setChecked( settings.value("r_enable",true).toBool());

    showHideTextureTypes(true);

    ui->lineEditPostfixDiffuse  ->setText(PostfixNames::diffuseName);
    ui->lineEditPostfixNormal   ->setText(PostfixNames::normalName);
    ui->lineEditPostfixSpecular ->setText(PostfixNames::specularName);
    ui->lineEditPostfixHeight   ->setText(PostfixNames::heightName);
    ui->lineEditPostfixOcclusion->setText(PostfixNames::occlusionName);
    ui->lineEditPostfixRoughness->setText(PostfixNames::roughnessName);
    ui->lineEditPostfixMetallic ->setText(PostfixNames::metallicName);


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

    ui->radioButtonSeamlessSimpleDirXY->setChecked(settings.value("uv_tiling_simple_dir_xy",true).toBool());
    ui->radioButtonSeamlessSimpleDirX->setChecked(settings.value("uv_tiling_simple_dir_x",false).toBool());
    ui->radioButtonSeamlessSimpleDirY->setChecked(settings.value("uv_tiling_simple_dir_y",false).toBool());

    ui->checkBoxUVTranslationsFirst->setChecked(settings.value("uv_translations_first",true).toBool());
    ui->horizontalSliderSeamlessContrastStrenght->setValue(settings.value("uv_contrast_strength",0.0).toFloat()*100);
    ui->horizontalSliderSeamlessContrastPower->setValue(settings.value("uv_contrast_power",0.0).toFloat()*100);

    ui->comboBoxSeamlessContrastInputImage->setCurrentIndex(settings.value("uv_contrast_input_image",0).toInt());

    // other settings
    ui->spinBoxMouseSensitivity->setValue(settings.value("mouse_sensitivity",50).toInt());
    ui->spinBoxFontSize->setValue(settings.value("font_size",10).toInt());
    ui->checkBoxToggleMouseLoop->setChecked(settings.value("mouse_loop",true).toBool());

    dock3Dsettings->loadSettings();
    updateSliders();


    loadImageSettings("d",diffuseImageProp);
    loadImageSettings("n",normalImageProp);
    loadImageSettings("s",specularImageProp);
    loadImageSettings("h",heightImageProp);
    loadImageSettings("o",occlusionImageProp);
    loadImageSettings("r",roughnessImageProp);
    loadImageSettings("m",metallicImageProp);
    loadImageSettings("g",grungeImageProp);


    QString name = settings.value("settings_name","Default").toString();
    ui->pushButtonProjectManager->setText("Project manager ("+name + ")");
    replotAllImages();

    glImage ->repaint();
    glWidget->repaint();
    bFirstTime = false;

}

void MainWindow::about()
{
    QMessageBox::about(this, tr(AWESOME_BUMP_VERSION),
                       tr("AwesomeBump is an open source program designed to generate normal, "
                          "height, specular or ambient occlusion, roughness and metallic textures from a single image. "
                          "Since the image processing is done in 99% on GPU  the program runs very fast "
                          "and all the parameters can be changed in real time.\n \n"
                          "Program written by: \n Krzysztof Kolasinski (Copyright 2015)\n"));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr(AWESOME_BUMP_VERSION));
}

void MainWindow::on_checkBoxSaveUnityMetallic_stateChanged(bool toggled)
{
    if(toggled && !metallicImageProp->isCheckedGrayScale())
    {
        QMessageBox mgs(QMessageBox::Icon::Warning,"Warning","Unity can't make use of colored maps. Thus the metallic map has to be grayscaled."
                                                             "\nBefore you save your images, please make sure to have checked the option 'Grayscale' in the metallic window,"
                                                             "else the map won't be generated");
        mgs.exec();
    }
}
