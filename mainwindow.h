#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDir>
#include "glwidget.h"
#include "glimageeditor.h"
#include "formimageprop.h"
#include "formsettingscontainer.h"
#include "CommonObjects.h"

#define TAB_SETTINGS 5
#define TAB_TILING   6

#ifdef Q_OS_MAC
# define AB_INI "AwesomeBump.ini"
# define AB_LOG "AwesomeBump.log" // log created in current directory
# define AB_LOG_ALT (QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).arg(AB_LOG))
#else
    #define AB_INI "config.ini"
    #define AB_LOG "log.txt"
    #define AB_LOG_ALT "log.txt"
#endif

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
    void selectGeneralSettingsTab();
    void fitImage();// resize 2D image

    // repaint views after changes
    void updateDiffuseImage();
    void updateNormalImage();
    void repaintNormalImage(); // used in case preview mode
    void updateSpecularImage();
    void updateHeightImage();
    void updateOcclusionImage();
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

    // Setting the global parameters
    void setSpecularIntensity(int);
    void setDiffuseIntensity(int);
    void updateSpinBoxes(int);

    // Conversion functions
    void convertFromHtoN();
    void convertFromNtoH();
    void convertFromBase();

    // UV tools
    void updateSliders();
    // Perspective tool    
    void resetTransform();
    void setUVManipulationMethod();

    void selectSeamlessMode(int mode);
    void randomizeAngles();// in random mode
    void resetRandomPatches();



private:
    // saves current settings of given image to config file. The param: abbr is e.g for diffuse image: "d"
    void saveImageSettings(QString abbr,FormImageProp* image);
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

    // Settings container
    FormSettingsContainer *settingsContainer;

    QAction *aboutQtAction;
    QAction *aboutAction;

    QSettings defaults;
};

#endif // MAINWINDOW_H
