#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QtnProperty/Core/Property.h>

#include "Filters3D.peg.h"
#include "utils/glslshaderparser.h"
#include "utils/glslparsedshadercontainer.h"

namespace Ui {
class PropertyDialog;
}

class Dialog3DGeneralSettings : public QDialog
{
    Q_OBJECT
    
public:
    Dialog3DGeneralSettings(QWidget* parent);
    ~Dialog3DGeneralSettings();
    void closeEvent(QCloseEvent* event);

    void saveSettings();   // save current properties to file
public slots:
    void show();           // opens settings window
    void cancelSettings(); // restore last settings when window is cancelled
    void acceptSettings(); // save current settings to file when OK button is pressed
    void propertyChanged(const QtnPropertyBase*changedProperty, const QtnPropertyBase*firedProperty, QtnPropertyChangeReason reason);
    void recompileCustomShader();
    void shaderChanged(int index);
    static void updateParsedShaders();
    static void setUniforms();
signals:
    void signalPropertyChanged();
    void signalRecompileCustomShader();
private:
    QtnPropertyDelegateInfo delegate;
    Ui::PropertyDialog *ui;
    QtnPropertySet* cpyPropertySet;//keeps last settings before window was open
public:
    static QtnPropertySetFilters3D* settings3D;
    static GLSLShaderParser* currentRenderShader;
    static GLSLParsedShaderContainer* glslParsedShaders;

};

#endif // MYDIALOG_H
