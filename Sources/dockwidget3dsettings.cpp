#include "dockwidget3dsettings.h"
#include "ui_dockwidget3dsettings.h"
extern QString _find_data_dir(const QString& path);

DockWidget3DSettings::DockWidget3DSettings(QWidget *parent, GLWidget* ptr_gl) :
    QDockWidget(parent),ptr_glWidget(ptr_gl),
    ui(new Ui::DockWidget3DSettings)
{
    ui->setupUi(this);
    close();
    setContentsMargins(0,0,0,0);
    //setWindowFlags(Qt::Widget);


    // Connect all the sliders and other widgets
    connect(ui->horizontalSliderDepthScale ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));
    connect(ui->horizontalSliderUVScale    ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));
    connect(ui->horizontalSliderUVXOffset  ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));
    connect(ui->horizontalSliderUVYOffset  ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));

    connect(ui->horizontalSliderSpecularI     ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));
    connect(ui->horizontalSliderDiffuseI      ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));
    connect(ui->horizontalSliderLightPower    ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));
    connect(ui->horizontalSliderLightRadius   ,SIGNAL(valueChanged(int)),this,SLOT(updateSettings(int)));

    connect(ui->comboBoxPerformanceNoRays     ,SIGNAL(activated(int)),this,SLOT(updateSettings(int)));
    connect(ui->comboBoxPerformanceNoTessSub  ,SIGNAL(activated(int)),this,SLOT(updateSettings(int)));
    connect(ui->checkBoxPerformanceCullFace   ,SIGNAL(clicked()),this,SLOT(updateSettings()));
    connect(ui->checkBoxPerformanceSimplePBR  ,SIGNAL(clicked()),this,SLOT(updateSettings()));
    connect(ui->checkBoxBloomEffect           ,SIGNAL(clicked()),this,SLOT(updateSettings()));
    connect(ui->checkBoxDOFEffect             ,SIGNAL(clicked()),this,SLOT(updateSettings()));
    connect(ui->checkBoxLensFlaresEffect      ,SIGNAL(clicked()),this,SLOT(updateSettings()));
    connect(ui->checkBoxShowTriangleEdges     ,SIGNAL(clicked()),this,SLOT(updateSettings()));

    connect(ui->comboBoxShadingModel          ,SIGNAL(activated(int)),    this,SLOT(selectShadingModel(int)));
    connect(ui->comboBoxShadingType           ,SIGNAL(activated(int)),    this,SLOT(updateSettings(int)));

    // loading 3d mesh signal and eviromental maps
    connect(ui->pushButtonLoadMesh            ,SIGNAL(released()),        ptr_glWidget,SLOT(loadMeshFromFile()));
    connect(ui->comboBoxChooseOBJModel        ,SIGNAL(activated(QString)),ptr_glWidget,SLOT(chooseMeshFile(QString)));
    connect(ui->comboBoxSkyBox                ,SIGNAL(activated(QString)),ptr_glWidget,SLOT(chooseSkyBox(QString)));


    // send current settings to glWidget
    connect(this,SIGNAL(signalSettingsChanged(Display3DSettings)),ptr_glWidget,SLOT(updatePerformanceSettings(Display3DSettings)));
    // ------------------------------------------------------- //
    //               Loading cub maps folders
    // ------------------------------------------------------- //
    qDebug() << "Loading cubemaps folders:";
    QDir currentDir(_find_data_dir(QString(RESOURCE_BASE) + "Core/2D/skyboxes"));
    currentDir.setFilter(QDir::Dirs);
    QStringList entries = currentDir.entryList();
    qDebug() << "Looking for enviromental maps in Core/2D/skyboxes:";
    for( QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry ){
        QString dirname=*entry;
        if(dirname != tr(".") && dirname != tr("..")){
            qDebug() << "Enviromental map:" << dirname;
            ui->comboBoxSkyBox->addItem(dirname);
        }
    }// end of for
    // setting cube map for glWidget
    ptr_glWidget->chooseSkyBox(ui->comboBoxSkyBox->currentText(),true);



}
QSize DockWidget3DSettings::sizeHint() const
{
    return QSize(200, 60);
}

void DockWidget3DSettings::updateSettings(int){

    settings.bUseCullFace       = ui->checkBoxPerformanceCullFace   ->isChecked();
    settings.bUseSimplePBR      = ui->checkBoxPerformanceSimplePBR  ->isChecked();
    settings.noPBRRays          = ui->comboBoxPerformanceNoRays     ->currentText().toInt();
    settings.noTessSubdivision  = ui->comboBoxPerformanceNoTessSub  ->currentText().toInt();
    settings.bBloomEffect       = ui->checkBoxBloomEffect           ->isChecked();
    settings.bDofEffect         = ui->checkBoxDOFEffect             ->isChecked();
    settings.bShowTriangleEdges = ui->checkBoxShowTriangleEdges     ->isChecked();
    settings.bLensFlares        = ui->checkBoxLensFlaresEffect      ->isChecked();

    settings.depthScale  = ui->horizontalSliderDepthScale->value()/50.0;
    settings.uvScale     = ui->horizontalSliderUVScale->value()/10.0;


    ui->doubleSpinBoxDepthScale  ->setValue(ui->horizontalSliderDepthScale->value()/100.0);
    ui->doubleSpinBoxUVScale     ->setValue(ui->horizontalSliderUVScale   ->value()/10.0);
    ui->doubleSpinBoxUVXOffset   ->setValue(ui->horizontalSliderUVXOffset ->value()/100.0);
    ui->doubleSpinBoxUVYOffset   ->setValue(ui->horizontalSliderUVYOffset ->value()/100.0);

    ui->doubleSpinBoxLightPower ->setValue(ui->horizontalSliderLightPower  ->value()/100.0);
    ui->doubleSpinBoxLightRadius->setValue(ui->horizontalSliderLightRadius->value()/100.0);

    settings.lightPower  = ui->doubleSpinBoxLightPower->value();
    settings.lightRadius = ui->doubleSpinBoxLightRadius->value();
    settings.uvOffset = QVector2D(ui->doubleSpinBoxUVXOffset->value(),ui->doubleSpinBoxUVYOffset->value());

    ui->doubleSpinBoxSpecularI->setValue(ui->horizontalSliderSpecularI->value()/50.0);
    ui->doubleSpinBoxDiffuseI ->setValue(ui->horizontalSliderDiffuseI ->value()/50.0);
    settings.specularIntensity = ui->doubleSpinBoxSpecularI->value();
    settings.diffuseIntensity  = ui->doubleSpinBoxDiffuseI ->value();

    settings.shadingType  = (ShadingType)  ui->comboBoxShadingType->currentIndex();
    settings.shadingModel = (ShadingModel) ui->comboBoxShadingModel->currentIndex();
    emit signalSettingsChanged(settings);
}

void DockWidget3DSettings::selectShadingModel(int i){
      updateSettings();
      emit signalSelectedShadingModel(i);
}
void DockWidget3DSettings::saveSettings(){
    QSettings settings(QString(AB_INI), QSettings::IniFormat);
    settings.setValue("3d_depth",ui->horizontalSliderDepthScale->value()/100.0);

    settings.setValue("bUseCullFace",ui->checkBoxPerformanceCullFace->isChecked());
    settings.setValue("bUseSimplePBR",ui->checkBoxPerformanceSimplePBR->isChecked());
    settings.setValue("noPBRRays",ui->comboBoxPerformanceNoRays->currentIndex());
    settings.setValue("noTessSubdivision",ui->comboBoxPerformanceNoTessSub->currentIndex());
    settings.setValue("bBloomEffect",ui->checkBoxBloomEffect->isChecked());
    settings.setValue("bLensFlaresEffect",ui->checkBoxLensFlaresEffect->isChecked());
    settings.setValue("bDofEffect",ui->checkBoxDOFEffect->isChecked());
    updateSettings();
}

void DockWidget3DSettings::loadSettings(){
    QSettings settings(QString(AB_INI), QSettings::IniFormat);

    // 3D settings:
    ui->horizontalSliderDepthScale  ->setValue(settings.value("3d_depth","0.25").toFloat()*100);
    ui->checkBoxPerformanceCullFace ->setChecked(settings.value("bUseCullFace",false).toBool());
    ui->checkBoxPerformanceSimplePBR->setChecked(settings.value("bUseSimplePBR",false).toBool());
    ui->checkBoxBloomEffect         ->setChecked(settings.value("bBloomEffect",true).toBool());
    ui->checkBoxDOFEffect           ->setChecked(settings.value("bDofEffect",true).toBool());
    ui->checkBoxLensFlaresEffect    ->setChecked(settings.value("bLensFlaresEffect",true).toBool());
    ui->comboBoxPerformanceNoRays   ->setCurrentIndex(settings.value("noPBRRays",0).toInt());
    ui->comboBoxPerformanceNoTessSub->setCurrentIndex(settings.value("noTessSubdivision",0).toInt());
    updateSettings();
}

DockWidget3DSettings::~DockWidget3DSettings()
{
    delete ui;
}
