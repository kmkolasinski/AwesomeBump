#include "formsettingsfield.h"
#include "ui_formsettingsfield.h"

int FormSettingsField::settingsGlobalID = 0;

FormSettingsField::FormSettingsField(QString _name,QString _description,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettingsField)
{
    ui->setupUi(this);
    ui->textEditDescription->hide();
    emit emitSaveSettings();
    name         = _name;
    description  = _description;
    settingsPath = "Configs/"+QString::number(settingsGlobalID)+"_"+name+".ini";
    settingsGlobalID++;
    ui->textEditDescription->setText(description);
    ui->lineEditName->setText(name);
    connect(ui->pushButtonDelete,SIGNAL(released()),this,SLOT(deleteSettings()));
    connect(ui->pushButtonSave  ,SIGNAL(released()),this,SLOT(saveSettings()));
    connect(ui->pushButtonLoad  ,SIGNAL(released()),this,SLOT(loadSettings()));

    connect(ui->lineEditName        ,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));
    connect(ui->textEditDescription ,SIGNAL(textChanged()),this,SLOT(dataChanged()));

    QFile::copy("config.ini",settingsPath);
    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.setValue("settings_name",name);
    settings.setValue("settings_description",description);
    settings.setValue("settings_path",settingsPath);


    QSettings list_settings("Configs/config_list",QSettings::IniFormat);
    list_settings.setValue("last_id",settingsGlobalID);

}

FormSettingsField::FormSettingsField(QString config_name, QWidget *parent):
    QWidget(parent),
    ui(new Ui::FormSettingsField)
{

    ui->setupUi(this);
    ui->textEditDescription->hide();
    qDebug() << "Loading Settings:" << config_name ;
    QSettings settings(config_name, QSettings::IniFormat);
    name         = settings.value("settings_name",QString("name")).toString();
    description  = settings.value("settings_description",QString("Nothing here???")).toString();
    settingsPath = settings.value("settings_path",QString("nope")).toString();


    ui->textEditDescription->setText(description);
    ui->lineEditName->setText(name);
    connect(ui->pushButtonDelete,SIGNAL(released()),this,SLOT(deleteSettings()));
    connect(ui->pushButtonSave  ,SIGNAL(released()),this,SLOT(saveSettings()));
    connect(ui->pushButtonLoad  ,SIGNAL(released()),this,SLOT(loadSettings()));
    connect(ui->pushButtonLoadAndConv  ,SIGNAL(released()),this,SLOT(loadAndConvert()));

    connect(ui->lineEditName        ,SIGNAL(textChanged(QString)),this,SLOT(nameChanged(QString)));
    connect(ui->textEditDescription ,SIGNAL(textChanged()),this,SLOT(dataChanged()));
}

FormSettingsField::~FormSettingsField()
{
    delete ui;
}

const QString& FormSettingsField::getName(){
    return name;
}

void FormSettingsField::deleteSettings(){
    QFile::remove(settingsPath);
    qDebug() << "Removing settings file:" << settingsPath ;
    emit emitDeleteSettings(this);
}
void FormSettingsField::saveSettings(){

    emit emitSaveSettings();

    description = ui->textEditDescription->toPlainText();
    name        = ui->lineEditName->text();
    ui->pushButtonSave->setText("");


    QFile::remove(settingsPath);
    bool test = QFile::copy("config.ini",settingsPath);

    qDebug() << "Saving current config file to:" << settingsPath + " from config.ini with result:" << test;

    QSettings settings(settingsPath, QSettings::IniFormat);
    settings.setValue("settings_name",name);
    settings.setValue("settings_description",description);
    settings.setValue("settings_path",settingsPath);

    QPalette p(palette());
    p.setColor(QPalette::Background,QColor(200,200,200) );
    setAutoFillBackground(true);
    setPalette(p);
}
void FormSettingsField::loadSettings(){
    QFile::remove("config.ini");
    bool test = QFile::copy(settingsPath,"config.ini");
    qDebug() << "Copying from file " << settingsPath + " to config.ini with result:" << test;
    emit emitLoadSettings(this);

    QPalette p(palette());
    p.setColor(QPalette::Background,QColor(200,200,200) );
    setAutoFillBackground(true);
    setPalette(p);
}

void FormSettingsField::loadAndConvert(){
    loadSettings();
    emit emitLoadAndConvert();
}


void FormSettingsField::resetBackGroundColor(){
    QPalette p(palette());
    p.setColor(QPalette::Background, Qt::transparent);
    setAutoFillBackground(false);
    setPalette(p);
}
void FormSettingsField::dataChanged(){
    ui->pushButtonSave->setText("Save*");
}
void FormSettingsField::nameChanged(QString text){
    ui->pushButtonSave->setText("Save*");
}

