#ifndef FORMSETTINGSCONTAINER_H
#define FORMSETTINGSCONTAINER_H

#include <QWidget>
#include <QDebug>
#include <QDir>
#include "formsettingsfield.h"
#include <QVector>

namespace Ui {
class FormSettingsContainer;
}

class FormSettingsContainer : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingsContainer(QWidget *parent = 0);
    ~FormSettingsContainer();

public slots:
    void addNewSettingsField(); // create a new settings record
    void toggleAdding();
    void removeSetting(FormSettingsField* field); // destroys also files
    void reloadSettings(FormSettingsField* field);
    void saveSettings();
    void loadAndConvert();
    void filterPresets(QString filter);
signals:
    void reloadConfigFile(); // force main window to read config.ini again
    void forceSaveCurrentConfig(); // current configs will be save to config.ini file
    void emitLoadAndConvert(); // load settings and convert images
private:

    Ui::FormSettingsContainer *ui;
    QVector<FormSettingsField*> settingsList;
};

#endif // FORMSETTINGSCONTAINER_H
