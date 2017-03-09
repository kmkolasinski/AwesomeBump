#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QAction>
#include <QDir>

#include "CommonObjects.h"

class QLabel;

class GLWidget;
class GLImage;
class FormImageProp;
class FormMaterialIndicesManager;
class FormSettingsContainer;
class DockWidget3DSettings;
class Dialog3DGeneralSettings;
class DialogLogger;
class DialogShortcuts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    QSize sizeHint() const;
    ~MainWindow();

protected:

     void closeEvent(QCloseEvent *event);
     void resizeEvent(QResizeEvent* event);
     void showEvent(QShowEvent* event);

signals:

	void initProgress(int perc);
	void initMessage(const QString &msg);

public slots:

    void aboutQt();
    void about();
  
	void initializeApp();

    void initializeGL();
    void initializeImages();

    void saveImages();
    void saveCheckedImages();
    void saveCompressedForm();
    void saveSettings();
    void changeGUIFontSize(int);
    // loading the application setting from ini file
    void loadSettings();
    // the same as above but Image is choosen by proper switch using the given type
    void loadImageSettings(TextureTypes type);
    void showSettingsManager();
    void setOutputFormat(int index);
    void replotAllImages();
    void materialsToggled(bool toggle);
    void checkWarnings();


    // repaint views after selecting tab
    void selectDiffuseTab();
    void selectNormalTab();
    void selectSpecularTab();
    void selectHeightTab();
    void selectOcclusionTab();
    void selectRoughnessTab();
    void selectMetallicTab();
    void selectMaterialsTab();
    void selectGrungeTab();
    void selectGeneralSettingsTab();
    void selectUVsTab();
    void fitImage();// resize 2D image

    // disabling textures
    void showHideTextureTypes(bool);

    // repaint views after changes
    void updateDiffuseImage();
    void updateNormalImage();
    void updateSpecularImage();
    void updateHeightImage();
    void updateOcclusionImage();
    void updateRoughnessImage();
    void updateMetallicImage();
    void updateGrungeImage();
    // repaint selected tab
    void updateImage(int tab);
    void updateImageInformation();

    // image properties
    void changeWidth (int size); // change the combobox index
    void changeHeight(int size);
    void scaleWidth(double);
    void scaleHeight(double);
    void applyResizeImage();
    void applyResizeImage(int width, int height);
    void applyScaleImage();
    void applyCurrentUVsTransformations();// copy current image to diffuse and convert to others


    // Setting the global parameters
    void updateSpinBoxes(int);
    void selectShadingModel(int i);

    // Conversion functions
    void convertFromHtoN();
    void convertFromNtoH();
    void convertFromBase();
    void convertFromHNtoOcc();

    // UV tools
    void updateSliders();
    // Perspective tool    
    void resetTransform();
    void setUVManipulationMethod();

    void selectSeamlessMode(int mode);
    void randomizeAngles();// in random mode
    void resetRandomPatches();
    void selectContrastInputImage(int mode);

private:
    // batch tool
    void selectSourceImages();
    void selectOutputPath();
    void runBatch();
    // saves all textures to given directory
    bool saveAllImages(const QString &dir);

    // Pointers
    Ui::MainWindow *ui;
    GLWidget* glWidget;

    GLImage* glImage;
    
    bool bSaveCheckedImages;
    bool bSaveCompressedFormImages;

    QDir recentDir;
    QDir recentMeshDir; // path to last loaded OBJ Mesh folder

    FormImageProp* diffuseImageProp;
    FormImageProp* normalImageProp;
    FormImageProp* specularImageProp;
    FormImageProp* heightImageProp;
    FormImageProp* occlusionImageProp;
    FormImageProp* roughnessImageProp;
    FormImageProp* metallicImageProp;
    FormImageProp* grungeImageProp;
    // Material manager
    FormMaterialIndicesManager* materialManager;

    // Settings container
    FormSettingsContainer *settingsContainer;
    QtnPropertySetAwesomeBump* abSettings;// use qtn to keep all settings in one place
    // 3D settings manager
    DockWidget3DSettings *dock3Dsettings;

    // 3D shading & display settings dialog
    Dialog3DGeneralSettings* dialog3dGeneralSettings;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *logAction; // show logger
    QAction *shortcutsAction; // show key shortcuts

    QLabel  *statusLabel;

    DialogLogger* dialogLogger;
    DialogShortcuts* dialogShortcuts;
    QSettings defaults;

    // page navigation
    QComboBox *pageSel;
    void configureToolbarAndStatusline();

private:
    void setTabText(int index, const char *title);
    void setTabEnabled(int index, bool value);
};

class CloneAction : public QAction {
    Q_OBJECT
  public:
    CloneAction(QAction *original, QObject *parent = 0) : QAction(parent), m_orig(original) {
      connect(this, SIGNAL(triggered()), original, SLOT(trigger()));      // trigger on triggered
      connect(this, SIGNAL(toggle(bool)), original, SLOT(toggle(bool)));  // trigger on toggled

      connect(original, SIGNAL(changed()), this, SLOT(__update()));       // update on change
      connect(original, SIGNAL(destroyed()), this, SLOT(deleteLater()));  // delete on destroyed
      __update();
    }
  private slots:
    void __update() {
      static QStringList props = QStringList() << "text" << "iconText" << "enabled" << "checkable" << "checked";
      foreach(const QString prop, props)
        setProperty(qPrintable(prop), m_orig->property(qPrintable(prop)));
    }
  private:
    QAction *m_orig;
};

#endif // MAINWINDOW_H

