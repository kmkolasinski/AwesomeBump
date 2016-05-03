#include "Dialog3DGeneralSettings.h"
#include "ui_Dialog3DGeneralSettings.h"

QtnPropertySetFilters3D* Dialog3DGeneralSettings::settings3D = NULL;
GLSLShaderParser* Dialog3DGeneralSettings::currentRenderShader        = NULL;
GLSLParsedShaderContainer* Dialog3DGeneralSettings::glslParsedShaders = NULL;

Dialog3DGeneralSettings::Dialog3DGeneralSettings(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PropertyDialog)
{
    ui->setupUi(this);
    hide();
    ui->widget->setParts(QtnPropertyWidgetPartsDescriptionPanel);


    QtnPropertySetFilters3D* settings = new QtnPropertySetFilters3D(this);

    settings3D = settings;

    ui->widget->setPropertySet(settings);
    // ---------------------------------------------------
    //              Read settings from file
    // ---------------------------------------------------
    QFile file("Configs/settings3D.dat");
    file.open(QIODevice::ReadOnly);
    if(file.isOpen()){
        QString property;
        QTextStream outstream(&file);
        property = outstream.readAll();
//        qDebug() << property;
        settings3D->fromStr(property);
    }

    connect(ui->pushButtonCancel,SIGNAL(pressed()),this,SLOT(cancelSettings()));
    connect(ui->pushButtonOK    ,SIGNAL(pressed()),this,SLOT(acceptSettings()));
    connect(ui->pushButtonRecompileShaders,SIGNAL(released()),this,SLOT(recompileCustomShader()));
    connect(settings,SIGNAL(propertyDidChange(const QtnPropertyBase*,const QtnPropertyBase*,QtnPropertyChangeReason)),
                this,SLOT(propertyChanged(const QtnPropertyBase*,const QtnPropertyBase*,QtnPropertyChangeReason)));


    glslParsedShaders = new GLSLParsedShaderContainer; // parse and creat list of avaiable shaders in Render folder

    // Create list of available shaders
    QStringList shaderList;
    for(int i = 0 ; i < glslParsedShaders->glslParsedShaders.size(); i++){
        shaderList << glslParsedShaders->glslParsedShaders.at(i)->shaderName;
    }
    ui->comboBoxShadersList->addItems(shaderList);
    // Setup pointer and comboBox
    int lastIndex = settings->ParsedShader.LastShaderIndex.value();
    ui->comboBoxShadersList->setCurrentIndex(lastIndex);
    currentRenderShader  = glslParsedShaders->glslParsedShaders[lastIndex];

    connect(ui->comboBoxShadersList,SIGNAL(currentIndexChanged(int)),this,SLOT(shaderChanged(int)));

}

void Dialog3DGeneralSettings::propertyChanged(const QtnPropertyBase* changedProperty,
                                              const QtnPropertyBase* firedProperty,
                                             QtnPropertyChangeReason reason){
    if (reason & QtnPropertyChangeReasonValue){
        emit signalPropertyChanged();
    }
}

void Dialog3DGeneralSettings::recompileCustomShader(){
    emit signalRecompileCustomShader();
}

void Dialog3DGeneralSettings::shaderChanged(int index){
    currentRenderShader  = glslParsedShaders->glslParsedShaders[index];
    updateParsedShaders();
    emit signalPropertyChanged();
}

// Each time button ok is pressed the settings are saved
// to local file. This file is used during the application
// initialization to load last settings.
void Dialog3DGeneralSettings::closeEvent(QCloseEvent *event){
    cancelSettings();
}

void Dialog3DGeneralSettings::show(){
    QtnPropertySet* properties = ui->widget->propertySet();
    // copy of properties
    cpyPropertySet = properties->createCopy(this);
    Q_ASSERT(cpyPropertySet);
    this->showNormal();
}

void Dialog3DGeneralSettings::cancelSettings(){
    this->reject();
    QtnPropertySet* properties = ui->widget->propertySet();
    properties->copyValues(cpyPropertySet, QtnPropertyStateInvisible);
}
void Dialog3DGeneralSettings::acceptSettings(){
    this->accept();
    saveSettings();
}

void Dialog3DGeneralSettings::saveSettings(){
    QFile file("Configs/settings3D.dat");
    file.open(QIODevice::WriteOnly);
    QString property;
    settings3D->ParsedShader.LastShaderIndex.setValue(ui->comboBoxShadersList->currentIndex());
    settings3D->toStr(property);
    QTextStream outstream(&file);
    outstream << property;
}

void Dialog3DGeneralSettings::updateParsedShaders(){
    GLSLShaderParser* parsedShader = currentRenderShader;
    int maxParams      = settings3D->ParsedShader.MaxParams;
    int noParsedParams = parsedShader->uniforms.size();
    // Check if parsed number uniform is greater than supported number of params
    // if YES display warning message.
    if(noParsedParams > maxParams){
        QMessageBox msgBox;
        msgBox.setText("Error!");
        msgBox.setInformativeText("Custom shader with name:"+parsedShader->shaderName+
                                  " has more than maxiumum allowed number of user-defined uniforms. \n"+
                                  "Current number of parameters:"+QString::number(noParsedParams)+".\n"+
                                  "Supported number:"+QString::number(maxParams)+".\n"+
                                  "Custom shader has been linked but may not work properly.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }
    // hide all by default
    for(int i = 0 ; i < maxParams ; i++) {
        QtnPropertyFloat* p = (QtnPropertyFloat*)(settings3D->ParsedShader.findChildProperty(i+1));
        p->switchState(QtnPropertyStateInvisible,true);
    }

    // update property set based on parsed fragment shader
    for(int i = 0 ; i < qMin(noParsedParams,maxParams) ; i++){
        QtnPropertyFloat* p = (QtnPropertyFloat*)(settings3D->ParsedShader.findChildProperty(i+1));
        UniformData& uniform = parsedShader->uniforms[i];
        p->switchState(QtnPropertyStateInvisible,false);
        p->setDescription(uniform.description);
        p->setDisplayName(uniform.name);
        p->setValue(uniform.value);
        p->setMaxValue(uniform.max);
        p->setMinValue(uniform.min);
        p->setStepValue(uniform.step);
    }
}

void Dialog3DGeneralSettings::setUniforms(){
    GLSLShaderParser* parsedShader = currentRenderShader;
    int maxParams      = settings3D->ParsedShader.MaxParams;
    int noParsedParams = parsedShader->uniforms.size();
    // update property set based on parsed fragment shader
    for(int i = 0 ; i < qMin(noParsedParams,maxParams) ; i++){
        QtnPropertyFloat* p = (QtnPropertyFloat*)(settings3D->ParsedShader.findChildProperty(i+1));
        UniformData& uniform = parsedShader->uniforms[i];
        uniform.value = (float)p->value();
    }
    parsedShader->setParsedUniforms();
}

Dialog3DGeneralSettings::~Dialog3DGeneralSettings()
{
    qDebug() << "calling" << Q_FUNC_INFO;
    settings3D = NULL;
    delete glslParsedShaders;
    glslParsedShaders = NULL;
    delete ui;
}
