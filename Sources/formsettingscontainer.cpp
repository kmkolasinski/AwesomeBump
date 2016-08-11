#include "formsettingscontainer.h"
#include "ui_formsettingscontainer.h"

FormSettingsContainer::FormSettingsContainer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingsContainer)
{
    ui->setupUi(this);
    connect(ui->pushButtonConfirm,SIGNAL(released()),this,SLOT(addNewSettingsField()));
    connect(ui->pushButtonConfirm,SIGNAL(released()),this,SLOT(toggleAdding()));
    connect(ui->pushButtonCancel ,SIGNAL(released()),this,SLOT(toggleAdding()));
    connect(ui->lineEditFilterPreset,SIGNAL(textEdited(QString)),this,SLOT(filterPresets(QString)));
    ui->groupBoxAddingOptions->hide();
    ui->verticalLayoutSettingsList->setAlignment(Qt::AlignTop);


    QSettings list_settings("Configs/config_list",QSettings::IniFormat);
    FormSettingsField::settingsGlobalID =  list_settings.value("last_id",0).toInt();


    QStringList nameFilter("*.ini");
    QDir directory("Configs/");
    QStringList iniFiles = directory.entryList(nameFilter);
    qDebug() << "Reading the list of available configs settings:";
    // reading configs
    for (int i = 0; i < iniFiles.size(); ++i){
        if(iniFiles[i] == "gui.ini") continue;
        FormSettingsField* sfield = new FormSettingsField("Configs/"+iniFiles[i],this);
        connect(sfield,SIGNAL(emitDeleteSettings(FormSettingsField*)),this,SLOT(removeSetting(FormSettingsField*)));
        connect(sfield,SIGNAL(emitLoadSettings(FormSettingsField*))  ,this,SLOT(reloadSettings(FormSettingsField*)));
        connect(sfield,SIGNAL(emitSaveSettings())  ,this,SLOT(saveSettings()));
        connect(sfield,SIGNAL(emitLoadAndConvert()),this,SLOT(loadAndConvert()));
        ui->verticalLayoutSettingsList->addWidget(sfield);


        settingsList.push_back(sfield);
    }

}

FormSettingsContainer::~FormSettingsContainer()
{
    qDebug() << "Removing settings tool.";
    for(int i = 0 ; i < settingsList.size() ; i++) delete settingsList[i];
    delete ui;
}

void FormSettingsContainer::toggleAdding(){
    ui->pushButtonAddNew->setChecked(false);
}



void FormSettingsContainer::addNewSettingsField(){
    FormSettingsField* sfield = new FormSettingsField(ui->lineEditName->text(),ui->textEditDescription->toPlainText());
    connect(sfield,SIGNAL(emitDeleteSettings(FormSettingsField*)),this,SLOT(removeSetting(FormSettingsField*)));
    connect(sfield,SIGNAL(emitLoadSettings(FormSettingsField*))  ,this,SLOT(reloadSettings(FormSettingsField*)));
    connect(sfield,SIGNAL(emitSaveSettings())  ,this,SLOT(saveSettings()));
    connect(sfield,SIGNAL(emitLoadAndConvert()),this,SLOT(loadAndConvert()));

    ui->verticalLayoutSettingsList->addWidget(sfield);
    settingsList.push_back(sfield);
}


void FormSettingsContainer::removeSetting(FormSettingsField* field){
    // removing from widget
    ui->verticalLayoutSettingsList->removeWidget(field);
    field->hide();
    repaint();
    // removing from list    
    settingsList.remove(settingsList.indexOf(field));
    // removing pointer
    delete field;
}

void FormSettingsContainer::reloadSettings(FormSettingsField* field){
    for(int i = 0 ; i < settingsList.size() ; i++){
        settingsList[i]->resetBackGroundColor();
    }
    emit reloadConfigFile();
}

void FormSettingsContainer::loadAndConvert(){
    emit emitLoadAndConvert();
}

void FormSettingsContainer::saveSettings(){
    for(int i = 0 ; i < settingsList.size() ; i++){
        settingsList[i]->resetBackGroundColor();
    }
    emit forceSaveCurrentConfig();
}

void FormSettingsContainer::filterPresets(QString filter){

    // check if string filter is present in presets names
    for(int i = 0 ; i < settingsList.size() ; i++){
        if(settingsList[i]->getName().contains(QRegExp(".*"+filter+".*",Qt::CaseInsensitive))){
            settingsList[i]->show();
        }else{
            settingsList[i]->hide();
        }
    }// end of for
}
