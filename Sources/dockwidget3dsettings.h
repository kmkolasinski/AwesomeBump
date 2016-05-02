#ifndef DOCKWIDGET3DSETTINGS_H
#define DOCKWIDGET3DSETTINGS_H

#include <QDockWidget>
#include "CommonObjects.h"
#include "glwidget.h"


namespace Ui {
class DockWidget3DSettings;
}

class DockWidget3DSettings : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * @brief DockWidget3DSettings - keeps the settings for 3D visualization, like specular intensity
     *                               depth scale, UV scaling and translation, some PBR settings etc.
     * @param parent - parent class
     * @param ptr_glWidget - pointer to GLWidget class, some of the control require communication
     *                       between both classes.
     */
    explicit DockWidget3DSettings(QWidget *parent, GLWidget *ptr_glWidget);
    ~DockWidget3DSettings();
public slots:
    /*
     * A universal function which reads all the currents values from GUI
     * and sending them to the glWidget to update 3D window.
     */

    void updateSettings(int i = 0);
    void selectShadingModel(int i);// This one sends special signal to change the names of the tabs in the MainWindow
    void saveSettings(QtnPropertySetAwesomeBump* settings);//save currents states to   config file
    void loadSettings(QtnPropertySetAwesomeBump* settings);//load currents states from config file
signals:
    void signalSelectedShadingModel(int i);// this signal change Tab name in mainWindow
    void signalSettingsChanged(Display3DSettings settings);
private:
    GLWidget *ptr_glWidget;
    Ui::DockWidget3DSettings *ui;
    QSize sizeHint() const;
public:
    Display3DSettings settings;
};

#endif // DOCKWIDGET3DSETTINGS_H
