#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"
#include "glimageeditor.h"
#include "formimageprop.h"
#include "formsettingscontainer.h"
#include "formmaterialindicesmanager.h"
#include "dialoglogger.h"
#include "dialogshortcuts.h"
#include "dockwidget3dsettings.h"

#include "gpuinfo.h"
#include <Property.h>
#include <PropertySet.h>
#include "properties/Dialog3DGeneralSettings.h"

#include <iostream>

extern QString _find_data_dir(const QString& resource);

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
    abSettings                  = new QtnPropertySetAwesomeBump(this);
    
    ui->setupUi(this);

    statusLabel = new QLabel("GPU memory status: n/a");
#ifdef Q_OS_MAC
    if(!statusLabel->testAttribute(Qt::WA_MacNormalSize)) statusLabel->setAttribute(Qt::WA_MacSmallSize);
#endif

    glImage          = new GLImage(this);
    glWidget         = new GLWidget(this,glImage);
}


#define INIT_PROGRESS(p,m) \
	emit initProgress(p); \
    emit initMessage(m); \
    qApp->processEvents()


void MainWindow::initializeApp()
{
    connect(glImage,SIGNAL(rendered()),this,SLOT(initializeImages()));
    qDebug() << "Initialization: Build image properties";
    INIT_PROGRESS(10, "Build image properties");


    diffuseImageProp  = new FormImageProp(this,glImage);
    normalImageProp   = new FormImageProp(this,glImage);
    specularImageProp = new FormImageProp(this,glImage);
    heightImageProp   = new FormImageProp(this,glImage);
    occlusionImageProp= new FormImageProp(this,glImage);
    roughnessImageProp= new FormImageProp(this,glImage);
    metallicImageProp = new FormImageProp(this,glImage);
    grungeImageProp   = new FormImageProp(this,glImage);

    materialManager = new FormMaterialIndicesManager(this,glImage);

    qDebug() << "Initialization: Setup image properties";
    INIT_PROGRESS(20, "Setup image properties");

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

    diffuseImageProp->setupPopertiesGUI();
    normalImageProp->setupPopertiesGUI();
    specularImageProp->setupPopertiesGUI();
    heightImageProp->setupPopertiesGUI();
    occlusionImageProp->setupPopertiesGUI();
    roughnessImageProp->setupPopertiesGUI();
    metallicImageProp->setupPopertiesGUI();
	// materialManager->setupPopertiesGUI();
    grungeImageProp->setupPopertiesGUI();

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


    glImage ->targetImageNormal    = normalImageProp   ->getImageProporties();
    glImage ->targetImageHeight    = heightImageProp   ->getImageProporties();
    glImage ->targetImageSpecular  = specularImageProp ->getImageProporties();
    glImage ->targetImageOcclusion = occlusionImageProp->getImageProporties();
    glImage ->targetImageDiffuse   = diffuseImageProp  ->getImageProporties();
    glImage ->targetImageRoughness = roughnessImageProp->getImageProporties();
    glImage ->targetImageMetallic  = metallicImageProp ->getImageProporties();
    glImage ->targetImageMaterial  = materialManager   ->getImageProporties();
    glImage ->targetImageGrunge    = grungeImageProp   ->getImageProporties();

    qDebug() << "Initialization: GUI setup";
    INIT_PROGRESS(30, "GUI setup");

    // ------------------------------------------------------
    //                      GUI setup
    // ------------------------------------------------------
    ui->statusbar->addWidget(statusLabel);



    // Settings container
    settingsContainer = new FormSettingsContainer;
    ui->verticalLayout2DImage->addWidget(settingsContainer);
    settingsContainer->hide();
    connect(settingsContainer,SIGNAL(reloadConfigFile()),this,SLOT(loadSettings()));
    connect(settingsContainer,SIGNAL(emitLoadAndConvert()),this,SLOT(convertFromBase()));
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

    dialog3dGeneralSettings = new Dialog3DGeneralSettings(this);
    connect(ui->pushButton3DGeneralSettings,SIGNAL(released()),dialog3dGeneralSettings,SLOT(show()));
    connect(dialog3dGeneralSettings,SIGNAL(signalPropertyChanged()),glWidget,SLOT(repaint()));
    connect(dialog3dGeneralSettings,SIGNAL(signalRecompileCustomShader()),glWidget,SLOT(recompileRenderShader()));

    ui->verticalLayout3DImage->addWidget(glWidget);
    ui->verticalLayout2DImage->addWidget(glImage);

    qDebug() << "Initialization: Adding widgets.";
    INIT_PROGRESS(40, "Adding widgets.");

    ui->verticalLayoutDiffuseImage  ->addWidget(diffuseImageProp);
    ui->verticalLayoutNormalImage   ->addWidget(normalImageProp);
    ui->verticalLayoutSpecularImage ->addWidget(specularImageProp);
    ui->verticalLayoutHeightImage   ->addWidget(heightImageProp);
    ui->verticalLayoutOcclusionImage->addWidget(occlusionImageProp);
    ui->verticalLayoutRoughnessImage->addWidget(roughnessImageProp);
    ui->verticalLayoutMetallicImage ->addWidget(metallicImageProp);
    ui->verticalLayoutMaterialIndicesImage->addWidget(materialManager);
    ui->verticalLayoutGrungeImage   ->addWidget(grungeImageProp);


    ui->tabWidget->setCurrentIndex(TAB_SETTINGS);
    
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(updateImage(int)));
    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),this,SLOT(updateImage(int)));
    
    // imageChange and imageLoaded signals
    connect(diffuseImageProp    ,SIGNAL(imageChanged()),this,SLOT(checkWarnings()));
    connect(grungeImageProp     ,SIGNAL(imageChanged()),this,SLOT(checkWarnings()));
    connect(occlusionImageProp  ,SIGNAL(imageChanged()),this,SLOT(checkWarnings()));


    connect(diffuseImageProp    ,SIGNAL(imageChanged()),glImage,SLOT(imageChanged()));
    connect(roughnessImageProp  ,SIGNAL(imageChanged()),glImage,SLOT(imageChanged()));
    connect(metallicImageProp   ,SIGNAL(imageChanged()),glImage,SLOT(imageChanged()));

    connect(diffuseImageProp    ,SIGNAL(imageChanged()),this,SLOT(updateDiffuseImage()));
    connect(normalImageProp     ,SIGNAL(imageChanged()),this,SLOT(updateNormalImage()));
    connect(specularImageProp   ,SIGNAL(imageChanged()),this,SLOT(updateSpecularImage()));
    connect(heightImageProp     ,SIGNAL(imageChanged()),this,SLOT(updateHeightImage()));
    connect(occlusionImageProp  ,SIGNAL(imageChanged()),this,SLOT(updateOcclusionImage()));
    connect(roughnessImageProp  ,SIGNAL(imageChanged()),this,SLOT(updateRoughnessImage()));
    connect(metallicImageProp   ,SIGNAL(imageChanged()),this,SLOT(updateMetallicImage()));
    connect(grungeImageProp     ,SIGNAL(imageChanged()),this,SLOT(updateGrungeImage()));

    qDebug() << "Initialization: Connections and actions.";
    INIT_PROGRESS(50, "Connections and actions.");

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
    connect(materialManager,SIGNAL(materialsToggled(bool)),this,SLOT(materialsToggled(bool))); // disable conversion tool
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

    qDebug() << "Initialization: Perspective tool connections.";
    INIT_PROGRESS(60, "Perspective tool connections.");

    // perspective tool
    connect(ui->pushButtonResetTransform            ,SIGNAL(released()),this,SLOT(resetTransform()));
    connect(ui->comboBoxPerspectiveTransformMethod  ,SIGNAL(activated(int)),glImage,SLOT(selectPerspectiveTransformMethod(int)));
    connect(ui->comboBoxSeamlessMode                ,SIGNAL(activated(int)),this,SLOT(selectSeamlessMode(int)));
    connect(ui->comboBoxSeamlessContrastInputImage  ,SIGNAL(activated(int)),this,SLOT(selectContrastInputImage(int)));

    qDebug() << "Initialization: UV seamless connections.";
    INIT_PROGRESS(70, "UV seamless connections.");

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
    connect(diffuseImageProp    ,SIGNAL(pickImageColor( QtnPropertyABColor*)),glImage,SLOT(pickImageColor( QtnPropertyABColor*)));
    connect(roughnessImageProp  ,SIGNAL(pickImageColor( QtnPropertyABColor*)),glImage,SLOT(pickImageColor( QtnPropertyABColor*)));
    connect(metallicImageProp   ,SIGNAL(pickImageColor( QtnPropertyABColor*)),glImage,SLOT(pickImageColor( QtnPropertyABColor*)));

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

    // batch settings
    connect(ui->pushButtonImageBatchSource ,SIGNAL(pressed()),this,SLOT(selectSourceImages()));
    connect(ui->pushButtonImageBatchOutput ,SIGNAL(pressed()),this,SLOT(selectOutputPath()));
    connect(ui->pushButtonImageBatchRun ,SIGNAL(pressed()),this,SLOT(runBatch()));




#ifdef Q_OS_MAC
    if(ui->statusbar && !ui->statusbar->testAttribute(Qt::WA_MacNormalSize)) ui->statusbar->setAttribute(Qt::WA_MacSmallSize);
#endif

    // Checking for GUI styles
    QStringList guiStyleList = QStyleFactory::keys();
    qDebug() << "Supported GUI styles: " << guiStyleList.join(", ");
    ui->comboBoxGUIStyle->addItems(guiStyleList);

    ui->labelFontSize->setVisible(false);
    ui->spinBoxFontSize->setVisible(false);
    qDebug() << "Loading settings:" ;
    // Now we can load settings
    loadSettings();

    qDebug() << "Initialization: Loading default (initial) textures.";
    INIT_PROGRESS(80, "Loading default (initial) textures.");

    // Loading default (initial) textures
    diffuseImageProp   ->setImage(QImage(QString(":/resources/logo/logo_D.png")));

    normalImageProp    ->setImage(QImage(QString(":/resources/logo/logo_N.png")));
    specularImageProp  ->setImage(QImage(QString(":/resources/logo/logo_D.png")));
    heightImageProp    ->setImage(QImage(QString(":/resources/logo/logo_H.png")));
    occlusionImageProp ->setImage(QImage(QString(":/resources/logo/logo_O.png")));
    roughnessImageProp ->setImage(QImage(QString(":/resources/logo/logo_R.png")));
    metallicImageProp  ->setImage(QImage(QString(":/resources/logo/logo_M.png")));
    grungeImageProp    ->setImage(QImage(QString(":/resources/logo/logo_R.png")));
    materialManager    ->setImage(QImage(QString(":/resources/logo/logo_R.png")));


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

    INIT_PROGRESS(90, "Updating main menu items.");

    aboutAction = new QAction(QIcon(":/resources/icons/cube.png"), tr("&About %1").arg(qApp->applicationName()), this);
    aboutAction->setToolTip(tr("Show information about AwesomeBump"));
    aboutAction->setMenuRole(QAction::AboutQtRole);
    aboutAction->setMenuRole(QAction::AboutRole);

    shortcutsAction = new QAction(QString("Shortcuts"),this);

    aboutQtAction = new QAction(QIcon(":/resources/icons/Qt.png"), tr("About &Qt"), this);
    aboutQtAction->setToolTip(tr("Show information about Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);



    logAction = new QAction("Show log file",this);
    dialogLogger    = new DialogLogger(this);
    dialogShortcuts = new DialogShortcuts(this);
    //dialogLogger->setModal(true);
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


    selectDiffuseTab();

    // Hide warning icons
    ui->pushButtonMaterialWarning  ->setVisible(false);
    ui->pushButtonConversionWarning->setVisible(false);
    ui->pushButtonGrungeWarning->setVisible(false);
    ui->pushButtonUVWarning->setVisible(false);
    ui->pushButtonOccWarning->setVisible(false);

    qDebug() << "Initialization: Done - UI ready.";
    INIT_PROGRESS(100, tr("Done - UI ready."));


}

MainWindow::~MainWindow()
{
    delete dialogLogger;
    delete dialogShortcuts;
    delete materialManager;
    delete settingsContainer;
    delete dock3Dsettings;
    delete dialog3dGeneralSettings;
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
    delete abSettings;
    delete ui;

}
void MainWindow::closeEvent(QCloseEvent *event) {
    QWidget::closeEvent( event );

    settingsContainer->close();
    glWidget->close();
    glImage->close();


}

void MainWindow::resizeEvent(QResizeEvent* event){
  QWidget::resizeEvent( event );
  replotAllImages();
}

void MainWindow::showEvent(QShowEvent* event){
  QWidget::showEvent( event );
  replotAllImages();
}

void MainWindow::replotAllImages(){
    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->enableShadowRender(true);

    // skip grunge map if conversion is enabled
    if(glImage->getConversionType() != CONVERT_FROM_D_TO_O){        
        updateImage(GRUNGE_TEXTURE);
    }

    updateImage(DIFFUSE_TEXTURE);
    updateImage(ROUGHNESS_TEXTURE);
    updateImage(METALLIC_TEXTURE);
    updateImage(HEIGHT_TEXTURE);
    // recalulate normal at the end
    updateImage(NORMAL_TEXTURE);
    // then ambient occlusion
    updateImage(OCCLUSION_TEXTURE);
    updateImage(SPECULAR_TEXTURE);
    updateImage(MATERIAL_TEXTURE);

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
        menu_text = QString("GPU memory used:") + QString::number(float(gpuMemTotal - gpuMemAvail) / 1024.0f) + QString("[MB]")
                  + QString(" GPU memory free:") + QString::number(float(gpuMemAvail) / 1024.0f) + QString("[MB]")
                  + QString(" GPU total memory:") + QString::number(float(gpuMemTotal) / 1024.0f) + QString("[MB]");
    }
    else
    {
        menu_text = QString("GPU memory free:") + QString::number(float(gpuMemAvail) / 1024.0f) + QString("[MB]");
    }

    statusLabel->setText(menu_text);
#endif
}

void MainWindow::materialsToggled(bool toggle){
    static bool bLastValue;
    ui->pushButtonMaterialWarning->setVisible(toggle);
    ui->pushButtonUVWarning->setVisible(FBOImageProporties::seamlessMode != SEAMLESS_NONE);
    if(toggle){

        bLastValue = diffuseImageProp->imageProp.properties->BaseMapToOthers.EnableConversion;
        diffuseImageProp->imageProp.properties->BaseMapToOthers.EnableConversion = false;
        ui->pushButtonUVWarning->setVisible(false);
        if(bLastValue) replotAllImages();
    }else{
        diffuseImageProp->imageProp.properties->BaseMapToOthers.EnableConversion = bLastValue;
    }
    diffuseImageProp->imageProp.properties->BaseMapToOthers.switchState(QtnPropertyStateInvisible,toggle);

}


void MainWindow::checkWarnings(){
    ui->pushButtonConversionWarning->setVisible(FBOImageProporties::bConversionBaseMap);
    ui->pushButtonGrungeWarning->setVisible(grungeImageProp->imageProp.properties->Grunge.OverallWeight.value() > 0);
    ui->pushButtonUVWarning->setVisible(FBOImageProporties::seamlessMode != SEAMLESS_NONE);

    bool bOccTest = (occlusionImageProp->imageProp.inputImageType == INPUT_FROM_HO_NO) ||
                (occlusionImageProp->imageProp.inputImageType == INPUT_FROM_HI_NI);
    ui->pushButtonOccWarning->setVisible(bOccTest);
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

    if(!bSaveCompressedFormImages){
        ui->labelProgressInfo->setText("Saving diffuse image...");
        if(bSaveCheckedImages*ui->checkBoxSaveDiffuse->isChecked() || !bSaveCheckedImages ){
            diffuseImageProp ->saveFileToDir(dir);
        }
        ui->progressBar->setValue(15);


        ui->labelProgressInfo->setText("Saving normal image...");
        if(bSaveCheckedImages*ui->checkBoxSaveNormal->isChecked() || !bSaveCheckedImages ){
            normalImageProp  ->saveFileToDir(dir);

        }
        ui->progressBar->setValue(30);
        ui->labelProgressInfo->setText("Saving specular image...");
        if(bSaveCheckedImages*ui->checkBoxSaveSpecular->isChecked() || !bSaveCheckedImages ){
            specularImageProp->saveFileToDir(dir);

        }
        ui->progressBar->setValue(45);

        ui->labelProgressInfo->setText("Saving height image...");
        if(bSaveCheckedImages*ui->checkBoxSaveHeight->isChecked() || !bSaveCheckedImages ){
            occlusionImageProp  ->saveFileToDir(dir);
        }

        ui->progressBar->setValue(60);
        ui->labelProgressInfo->setText("Saving occlusion image...");
        if(bSaveCheckedImages*ui->checkBoxSaveOcclusion->isChecked() || !bSaveCheckedImages ){
            heightImageProp  ->saveFileToDir(dir);
        }

        ui->progressBar->setValue(75);
        ui->labelProgressInfo->setText("Saving roughness image...");
        if(bSaveCheckedImages*ui->checkBoxSaveRoughness->isChecked() || !bSaveCheckedImages ){
            roughnessImageProp  ->saveFileToDir(dir);
        }

        ui->progressBar->setValue(90);
        ui->labelProgressInfo->setText("Saving metallic image...");
        if(bSaveCheckedImages*ui->checkBoxSaveMetallic->isChecked() || !bSaveCheckedImages ){
            metallicImageProp ->saveFileToDir(dir);
        }
        ui->progressBar->setValue(100);

    }else{ // if using compressed format
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

    bool test = (grungeImageProp->getImageProporties()->properties->Grunge.ReplotAll == true);

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
      diffuseImageProp  ->setImage(QImage(QString(":/resources/logo/logo_D.png")));
      normalImageProp   ->setImage(QImage(QString(":/resources/logo/logo_N.png")));
      specularImageProp ->setImage(QImage(QString(":/resources/logo/logo_D.png")));
      heightImageProp   ->setImage(QImage(QString(":/resources/logo/logo_H.png")));
      occlusionImageProp->setImage(QImage(QString(":/resources/logo/logo_O.png")));
      roughnessImageProp->setImage(QImage(QString(":/resources/logo/logo_R.png")));
      metallicImageProp ->setImage(QImage(QString(":/resources/logo/logo_M.png")));
      grungeImageProp   ->setImage(QImage(QString(":/resources/logo/logo_R.png")));

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
    FormImageProp* imageProp = NULL;

    switch(tType){
        case(DIFFUSE_TEXTURE ):
            imageProp = diffuseImageProp;
            glImage->setActiveImage(diffuseImageProp->getImageProporties());            
            break;
        case(NORMAL_TEXTURE  ):
            imageProp = normalImageProp;
            glImage->setActiveImage(normalImageProp->getImageProporties());            
            break;
        case(SPECULAR_TEXTURE):
            imageProp = specularImageProp;
            glImage->setActiveImage(specularImageProp->getImageProporties());            
            break;
        case(HEIGHT_TEXTURE  ):
            imageProp = heightImageProp;
            glImage->setActiveImage(heightImageProp->getImageProporties());            
            break;
        case(OCCLUSION_TEXTURE  ):
            imageProp = occlusionImageProp;
            glImage->setActiveImage(occlusionImageProp->getImageProporties());            
            break;
        case(ROUGHNESS_TEXTURE  ):
            imageProp = roughnessImageProp;
            glImage->setActiveImage(roughnessImageProp->getImageProporties());            
            break;
        case(METALLIC_TEXTURE  ):
            imageProp = metallicImageProp;
            glImage->setActiveImage(metallicImageProp->getImageProporties());            
            break;
        case(MATERIAL_TEXTURE  ):            
            glImage->setActiveImage(materialManager->getImageProporties());            
            break;
        case(GRUNGE_TEXTURE  ):
            imageProp = grungeImageProp;
            glImage->setActiveImage(grungeImageProp->getImageProporties());            
            break;
        default: // Settings
            return;
    }
    if (imageProp->bLoading != NULL){
        imageProp->bLoading = false;
    }
    glWidget->update();
}

void MainWindow::changeWidth (int size=0){

    if(ui->pushButtonResizePropTo->isChecked()){
        ui->comboBoxResizeHeight->setCurrentText(ui->comboBoxResizeWidth->currentText());
    }
}
void MainWindow::changeHeight(int size=0){

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
    checkWarnings();
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

void MainWindow::selectSourceImages(){

    QString startPath;
    if(recentDir.exists()) startPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();
    else  startPath = recentDir.absolutePath();

    QString source = QFileDialog::getExistingDirectory(this, tr("Select source directory"),
                                                startPath,
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    QDir dir(source);
    qDebug() << "Selecting source folder for batch processing: " << source;

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp" << "*.tga";
    QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);

    ui->listWidgetImageBatch->clear();
    foreach (QFileInfo fileInfo, fileInfoList) {
       qDebug() << "Found:" << fileInfo.absoluteFilePath();
       ui->listWidgetImageBatch->addItem(fileInfo.fileName());
    }
    ui->lineEditImageBatchSource->setText(source);
}

void MainWindow::selectOutputPath(){

    QString startPath;
    if(recentDir.exists()) startPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();
    else  startPath = recentDir.absolutePath();

    QString path = QFileDialog::getExistingDirectory(this, tr("Select source directory"),
                                                startPath,
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->lineEditImageBatchOutput->setText(path);
}

void MainWindow::runBatch(){

    QString sourceFolder = ui->lineEditImageBatchSource->text();
    QString outputFolder = ui->lineEditImageBatchOutput->text();

    // check if output path exists
    if(!QDir(outputFolder).exists() || outputFolder == ""){
        QMessageBox msgBox;
        msgBox.setText("Info");
        msgBox.setInformativeText("Output path is not provided");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }

    qDebug() << "Starting batch mode: this may take some time";


    while(ui->listWidgetImageBatch->count() > 0){
        QListWidgetItem* item = ui->listWidgetImageBatch->takeItem(0);
        ui->labelBatchProgress->setText("Images left: " + QString::number(ui->listWidgetImageBatch->count()+1));
        ui->labelBatchProgress->repaint();
        QCoreApplication::processEvents();

        QString imageName = item->text();
        ui->lineEditOutputName->setText(imageName);
        QString imagePath = sourceFolder + "/" + imageName;

        qDebug() << "Processing image: " << imagePath;
        diffuseImageProp->loadFile(imagePath);
        convertFromBase();
        saveAllImages(outputFolder);

        delete item;
        ui->listWidgetImageBatch->repaint();
        QCoreApplication::processEvents();
    }

    ui->labelBatchProgress->setText("Done...");

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

      if(i == 0) ui->tabWidget->setTabText(5,"Rgnss");
      if(i == 1) ui->tabWidget->setTabText(5,"Gloss");
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
    FBOImageProporties* lastActive = glImage->getActiveImage();
    glImage->setActiveImage(diffuseImageProp->getImageProporties());
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

    glImage->setActiveImage(lastActive);
    glWidget->update();
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
    // choosing the propper mirror mode
    if(ui->radioButtonMirrorModeXY->isChecked()) FBOImageProporties::seamlessMirroModeType = 0;
    if(ui->radioButtonMirrorModeX ->isChecked()) FBOImageProporties::seamlessMirroModeType = 1;
    if(ui->radioButtonMirrorModeY ->isChecked()) FBOImageProporties::seamlessMirroModeType = 2;

    // choosing the propper simple mode direction
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
	return QSize(abSettings->d_win_w,abSettings->d_win_h);
}

void MainWindow::loadImageSettings(TextureTypes type){

    switch(type){
        case(DIFFUSE_TEXTURE):            
            diffuseImageProp    ->imageProp.properties->copyValues(&abSettings->Diffuse);
            break;
        case(NORMAL_TEXTURE):
            normalImageProp     ->imageProp.properties->copyValues(&abSettings->Normal);
            break;
        case(SPECULAR_TEXTURE):
            specularImageProp   ->imageProp.properties->copyValues(&abSettings->Specular);
            break;
        case(HEIGHT_TEXTURE):
            heightImageProp     ->imageProp.properties->copyValues(&abSettings->Height);
            break;
        case(OCCLUSION_TEXTURE):
            occlusionImageProp  ->imageProp.properties->copyValues(&abSettings->Occlusion);
            break;
        case(ROUGHNESS_TEXTURE):
            roughnessImageProp  ->imageProp.properties->copyValues(&abSettings->Roughness);
            break;
        case(METALLIC_TEXTURE):
            metallicImageProp   ->imageProp.properties->copyValues(&abSettings->Metallic);
            break;
        case(GRUNGE_TEXTURE):
            grungeImageProp     ->imageProp.properties->copyValues(&abSettings->Grunge);
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

    abSettings->d_win_w =  this->width();
    abSettings->d_win_h =  this->height();
    abSettings->tab_win_w = ui->tabWidget->width();
    abSettings->tab_win_h = ui->tabWidget->height();
    abSettings->recent_dir      = recentDir.absolutePath();
    abSettings->recent_mesh_dir = recentMeshDir.absolutePath();

    PostfixNames::diffuseName   = ui->lineEditPostfixDiffuse->text();
    PostfixNames::normalName    = ui->lineEditPostfixNormal->text();
    PostfixNames::specularName  = ui->lineEditPostfixSpecular->text();
    PostfixNames::heightName    = ui->lineEditPostfixHeight->text();
    PostfixNames::occlusionName = ui->lineEditPostfixOcclusion->text();
    PostfixNames::roughnessName = ui->lineEditPostfixRoughness->text();
    PostfixNames::metallicName  = ui->lineEditPostfixMetallic->text();

    abSettings->d_postfix=ui->lineEditPostfixDiffuse->text();
    abSettings->d_postfix=ui->lineEditPostfixDiffuse->text();
    abSettings->n_postfix=ui->lineEditPostfixNormal->text();
    abSettings->s_postfix=ui->lineEditPostfixSpecular->text();
    abSettings->h_postfix=ui->lineEditPostfixHeight->text();
    abSettings->o_postfix=ui->lineEditPostfixOcclusion->text();
    abSettings->r_postfix=ui->lineEditPostfixRoughness->text();
    abSettings->m_postfix=ui->lineEditPostfixMetallic->text();

    abSettings->gui_style=ui->comboBoxGUIStyle->currentText();


    // UV Settings
    abSettings->uv_tiling_type=ui->comboBoxSeamlessMode->currentIndex();
    abSettings->uv_tiling_radius=ui->horizontalSliderMakeSeamlessRadius->value();
    abSettings->uv_tiling_mirror_x=ui->radioButtonMirrorModeX->isChecked();
    abSettings->uv_tiling_mirror_y=ui->radioButtonMirrorModeY->isChecked();
    abSettings->uv_tiling_mirror_xy=ui->radioButtonMirrorModeXY->isChecked();
    abSettings->uv_tiling_random_inner_radius=ui->horizontalSliderRandomPatchesInnerRadius->value();
    abSettings->uv_tiling_random_outer_radius=ui->horizontalSliderRandomPatchesOuterRadius->value();
    abSettings->uv_tiling_random_rotate=ui->horizontalSliderRandomPatchesRotate->value();
    // UV contrast etc
    abSettings->uv_translations_first=ui->checkBoxUVTranslationsFirst->isChecked();
    abSettings->uv_contrast_strength=ui->doubleSpinBoxSeamlessContrastStrenght->value();
    abSettings->uv_contrast_power=ui->doubleSpinBoxSeamlessContrastPower->value();
    abSettings->uv_contrast_input_image=ui->comboBoxSeamlessContrastInputImage->currentIndex();
    abSettings->uv_tiling_simple_dir_xy=ui->radioButtonSeamlessSimpleDirXY->isChecked();
    abSettings->uv_tiling_simple_dir_x=ui->radioButtonSeamlessSimpleDirX->isChecked();
    abSettings->uv_tiling_simple_dir_y=ui->radioButtonSeamlessSimpleDirY->isChecked();

    // other parameters
    abSettings->use_texture_interpolation=ui->checkBoxUseLinearTextureInterpolation->isChecked();
    abSettings->mouse_sensitivity=ui->spinBoxMouseSensitivity->value();
    abSettings->font_size=ui->spinBoxFontSize->value();
    abSettings->mouse_loop=ui->checkBoxToggleMouseLoop->isChecked();

    dock3Dsettings->saveSettings(abSettings);


    abSettings->Diffuse  .copyValues(diffuseImageProp   ->imageProp.properties);
    abSettings->Specular .copyValues(specularImageProp  ->imageProp.properties);
    abSettings->Normal   .copyValues(normalImageProp    ->imageProp.properties);
    abSettings->Occlusion.copyValues(occlusionImageProp ->imageProp.properties);
    abSettings->Height   .copyValues(heightImageProp    ->imageProp.properties);
    abSettings->Metallic .copyValues(metallicImageProp  ->imageProp.properties);
    abSettings->Roughness.copyValues(roughnessImageProp ->imageProp.properties);
    abSettings->Grunge   .copyValues(grungeImageProp    ->imageProp.properties);


    // Disable possibility to save conversion status ???
//    abSettings->Diffuse.BaseMapToOthers.EnableConversion.setValue(false);

    QFile file( QString(AB_INI) );
    if( !file.open( QIODevice::WriteOnly ) )
         return;
    QTextStream stream(&file);
    QString data;
    abSettings->toStr(data);
    stream << data;

}

void MainWindow::changeGUIFontSize(int value){
    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(value);
    QApplication::setFont(font);
}

void MainWindow::setOutputFormat(int index=0){

    PostfixNames::outputFormat = ui->comboBoxImageOutputFormat->currentText();
}

void MainWindow::loadSettings(){
    static bool bFirstTime = true;

    qDebug() << "Calling" << Q_FUNC_INFO << " loading from " << QString(AB_INI);
    diffuseImageProp->bLoading = true;

    QFile file( QString(AB_INI) );
    if( !file.open( QIODevice::ReadOnly ) )
         return;

    QTextStream stream(&file);
    QString data;

    stream.readLine(); //skip one line
    data = stream.readAll();
    abSettings->fromStr(data);

    QString name = abSettings->settings_name.value();
    ui->pushButtonProjectManager->setText("Project manager (" + name + ")");

    diffuseImageProp    ->imageProp.properties->copyValues(&abSettings->Diffuse);
    specularImageProp   ->imageProp.properties->copyValues(&abSettings->Specular);
    normalImageProp     ->imageProp.properties->copyValues(&abSettings->Normal);
    occlusionImageProp  ->imageProp.properties->copyValues(&abSettings->Occlusion);
    heightImageProp     ->imageProp.properties->copyValues(&abSettings->Height);
    metallicImageProp   ->imageProp.properties->copyValues(&abSettings->Metallic);
    roughnessImageProp  ->imageProp.properties->copyValues(&abSettings->Roughness);
    grungeImageProp     ->imageProp.properties->copyValues(&abSettings->Grunge);

    // update general settings
    if(bFirstTime){
        this->resize(abSettings->d_win_w,abSettings->d_win_h);
        ui->tabWidget->resize(abSettings->tab_win_w,abSettings->tab_win_h);
    }
    PostfixNames::diffuseName   = abSettings->d_postfix;
    PostfixNames::normalName    = abSettings->n_postfix;
    PostfixNames::specularName  = abSettings->s_postfix;
    PostfixNames::heightName    = abSettings->h_postfix;
    PostfixNames::occlusionName = abSettings->o_postfix;
    PostfixNames::roughnessName = abSettings->m_postfix;
    PostfixNames::metallicName  = abSettings->r_postfix;

    showHideTextureTypes(true);

    ui->lineEditPostfixDiffuse  ->setText(PostfixNames::diffuseName);
    ui->lineEditPostfixNormal   ->setText(PostfixNames::normalName);
    ui->lineEditPostfixSpecular ->setText(PostfixNames::specularName);
    ui->lineEditPostfixHeight   ->setText(PostfixNames::heightName);
    ui->lineEditPostfixOcclusion->setText(PostfixNames::occlusionName);
    ui->lineEditPostfixRoughness->setText(PostfixNames::roughnessName);
    ui->lineEditPostfixMetallic ->setText(PostfixNames::metallicName);


    recentDir     = abSettings->recent_dir;
    recentMeshDir = abSettings->recent_mesh_dir;

    ui->checkBoxUseLinearTextureInterpolation->setChecked(abSettings->use_texture_interpolation);
    FBOImages::bUseLinearInterpolation = ui->checkBoxUseLinearTextureInterpolation->isChecked();
    ui->comboBoxGUIStyle->setCurrentText(abSettings->gui_style);

    // UV Settings
    ui->comboBoxSeamlessMode->setCurrentIndex(abSettings->uv_tiling_type);
    selectSeamlessMode(ui->comboBoxSeamlessMode->currentIndex());
    ui->horizontalSliderMakeSeamlessRadius->setValue(abSettings->uv_tiling_radius);
    ui->radioButtonMirrorModeX->setChecked(abSettings->uv_tiling_mirror_x);
    ui->radioButtonMirrorModeY->setChecked(abSettings->uv_tiling_mirror_y);
    ui->radioButtonMirrorModeXY->setChecked(abSettings->uv_tiling_mirror_xy);
    ui->horizontalSliderRandomPatchesInnerRadius->setValue(abSettings->uv_tiling_random_inner_radius);
    ui->horizontalSliderRandomPatchesOuterRadius->setValue(abSettings->uv_tiling_random_outer_radius);
    ui->horizontalSliderRandomPatchesRotate->setValue(abSettings->uv_tiling_random_rotate);

    ui->radioButtonSeamlessSimpleDirXY->setChecked(abSettings->uv_tiling_simple_dir_xy);
    ui->radioButtonSeamlessSimpleDirX->setChecked(abSettings->uv_tiling_simple_dir_x);
    ui->radioButtonSeamlessSimpleDirY->setChecked(abSettings->uv_tiling_simple_dir_y);

    ui->checkBoxUVTranslationsFirst->setChecked(abSettings->uv_translations_first);
    ui->horizontalSliderSeamlessContrastStrenght->setValue(abSettings->uv_contrast_strength*100);
    ui->horizontalSliderSeamlessContrastPower->setValue(abSettings->uv_contrast_power*100);
    ui->comboBoxSeamlessContrastInputImage->setCurrentIndex(abSettings->uv_contrast_input_image);

    // other settings

    ui->spinBoxMouseSensitivity->setValue(abSettings->mouse_sensitivity);
    ui->spinBoxFontSize->setValue(abSettings->font_size);
    ui->checkBoxToggleMouseLoop->setChecked(abSettings->mouse_loop);

    updateSliders();

    dock3Dsettings->loadSettings(abSettings);
    heightImageProp->reloadSettings();
    diffuseImageProp->bLoading = false;

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
                          "and all the parameters can be changed in real time.\n "
                          "Program written by: \n Krzysztof Kolasinski and Pawel Piecuch (Copyright 2015-2016) with collaboration \n"
                          "with other people! See project collaborators list on github. "));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, tr(AWESOME_BUMP_VERSION));
}
