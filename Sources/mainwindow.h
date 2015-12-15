#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDir>
#include <QColor>
#include "glwidget.h"
#include "glimageeditor.h"
#include "formimageprop.h"
#include "formsettingscontainer.h"
#include "formmaterialindicesmanager.h"
#include "CommonObjects.h"
#include "dialoglogger.h"
#include "dialogshortcuts.h"
#include "dockwidget3dsettings.h"
#include "gpuinfo.h"
#include "unitysupportgui.h"

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

public slots:

    void aboutQt();
    void about();
  
    void initializeGL();
    void initializeImages();

    void saveImages();
    void saveCheckedImages();
    void saveCompressedForm();
    void saveSettings();
    void changeGUIFontSize(int);
    // loading the application setting from ini file
    void loadSettings();
    // the same but loading configs
    void loadImageSettings(QString abbr,FormImageProp* image);
    // the same as above but Image is choosen by proper switch using the given type
    void loadImageSettings(TextureTypes type);
    void showSettingsManager();

    void setOutputFormat(int index);
    void replotAllImages();

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

private slots:
    void on_checkBoxSaveUnityMetallic_stateChanged(bool toggled);

private:
    // saves current settings of given image to config file. The param: abbr is e.g for diffuse image: "d"
    void saveImageSettings(QString abbr,FormImageProp* image);
    // saves all textures to given directory
    bool saveAllImages(const QString &dir);

    // Support for Unity's metallic maps;
    void saveMapToPng(const QString &dir, const QImage& map ) const;

    // This method is called during the saving process.
    void computeUnityMetallic(const QImage& metallic, const QImage& roughness, QImage& out) const;

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
    UnitySupportGui* unitySupport;
    // Material manager
    FormMaterialIndicesManager* materialManager;

    // Settings container
    FormSettingsContainer *settingsContainer;
    // 3D settings manager
    DockWidget3DSettings *dock3Dsettings;

    QAction *aboutQtAction;
    QAction *aboutAction;
    QAction *logAction; // show logger
    QAction *shortcutsAction; // show key shortcuts

    QLabel  *statusLabel;
    DialogLogger* dialogLogger;
    DialogShortcuts* dialogShortcuts;
    QSettings defaults;

};

#endif // MAINWINDOW_H

