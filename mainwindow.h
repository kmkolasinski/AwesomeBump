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

    void selectDiffuseTab();
    void selectNormalTab();
    void selectSpecularTab();
    void selectHeightTab();
    void selectOcclusionTab();
    void selectGeneralSettingsTab();

    void updateDiffuseImage();
    void updateNormalImage();
    void updateSpecularImage();
    void updateHeightImage();
    void updateOcclusionImage();

    void updateImage(int tab);
    void enableMakeSeamless(bool);
    void setMakeSeamlessRadius(int);
    void setSpecularIntensity(int);
    void setDiffuseIntensity(int);
    void updateSpinBoxes(int);

    void convertFromHtoN();
    void convertFromNtoH();
    void convertFromBase();
    void recalculateOcclusion();

    void about();
    void aboutQt();

private:
    void loadSettings();
    bool saveAllImages(const QString &dir);

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

    QAction *aboutQtAction;
    QAction *aboutAction;

};

#endif // MAINWINDOW_H
