#ifndef FORMSETTINGSFIELD_H
#define FORMSETTINGSFIELD_H

#include <QWidget>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QSettings>
namespace Ui {
class FormSettingsField;
}

class FormSettingsField : public QWidget
{
    Q_OBJECT

public:
    // Create new Settings
    FormSettingsField(QString _name,QString _description, QWidget *parent = 0);
    // Load Settings From File
    FormSettingsField(QString _config_name, QWidget *parent = 0);
    const QString& getName();
    ~FormSettingsField();
public slots:
    void deleteSettings();// remove files
    void saveSettings();  // copy config.ini file to current settings file
    void loadSettings();  // copy settings file to main config.ini file
    void loadAndConvert(); // same as loadSettings but also convert images from diffuse
    void dataChanged();
    void nameChanged(QString text);
    void resetBackGroundColor();


signals:
    void emitDeleteSettings(FormSettingsField*);
    void emitSaveSettings();
    void emitLoadSettings(FormSettingsField*);
    void emitLoadAndConvert();
private:

    Ui::FormSettingsField *ui;
    QString settingsPath;
    QString name;
    QString description;
public:
static int settingsGlobalID;
};

#endif // FORMSETTINGSFIELD_H
