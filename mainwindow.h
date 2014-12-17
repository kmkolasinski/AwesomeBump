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
#include "CommonObjects.h"

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
     void showEvent(QShowEvent* event);

public slots:

    void initializeImages();

    void saveImages();
    void saveCheckedImages();
    void saveCompressedForm();
    void saveSettings();

    void replotAllImages();

    // repaint views after selecting tab
    void selectDiffuseTab();
    void selectNormalTab();
    void selectSpecularTab();
    void selectHeightTab();
    void selectOcclusionTab();
    void selectGeneralSettingsTab();

    // repaint views after changes
    void updateDiffuseImage();
    void updateNormalImage();
    void updateSpecularImage();
    void updateHeightImage();
    void updateOcclusionImage();
    // repaint selected tab
    void updateImage(int tab);

    // Setting the global parameters
    void enableMakeSeamless(bool);
    void setMakeSeamlessRadius(int);
    void setSpecularIntensity(int);
    void setDiffuseIntensity(int);
    void updateSpinBoxes(int);

    // Conversion functions
    void convertFromHtoN();
    void convertFromNtoH();
    void convertFromBase();
    void recalculateOcclusion();



private:
    // saves current settings of given image to config file. The param: abbr is e.g for diffuse image: "d"
    void saveImageSettings(QString abbr,FormImageProp* image);
    // the same but loading configs
    void loadImageSettings(QString abbr,FormImageProp* image);
    // loading the application setting from config.ini file
    void loadSettings();
    // saves all textures to given directory
    bool saveAllImages(const QString &dir);

    // Pointers
    Ui::MainWindow *ui;
    GLWidget *glWidget;
    GLImage * glImage;
    bool bSaveCheckedImages;
    bool bSaveCompressedFormImages;

    QDir recentDir;

    FormImageProp* diffuseImageProp;
    FormImageProp* normalImageProp;
    FormImageProp* specularImageProp;
    FormImageProp* heightImageProp;
    FormImageProp* occlusionImageProp;


};

#endif // MAINWINDOW_H
