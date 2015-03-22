#ifndef FORMIMAGEPROP_H
#define FORMIMAGEPROP_H

#include <QWidget>
#include <QImage>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QImageReader>
#include <QStandardPaths>

#include "CommonObjects.h"
#include "dialogheightcalculator.h"



namespace Ui {
class FormImageProp;
}

class FormImageProp : public QWidget
{
    Q_OBJECT

public:
    explicit FormImageProp(QMainWindow *parent = 0, QGLWidget* qlW_ptr = 0);
    void saveFileToDir(const QString &dir);
    void saveImageToDir(const QString &dir,QImage& image);
    void setImage(QImage image);
    void setImageName(QString name);
    void setPtrToGLWidget(QGLWidget* ptr){ imageProp.glWidget_ptr = ptr;  }
    FBOImageProporties* getImageProporties(){return &imageProp;}

    QString getImageName();


    void hideHeightInputGroup();
    void hideBMGroupBox();
    void hideSpecularGroupBox();
    void hideNormalStepBar();
    void hideOcclusionInputGroup();
    void hideHeightProcessingBox();
    void hideSelectiveBlurBox();
    void hideGrayScaleControl();
    void setSpecularControlChecked();
    void reloadSettings();
    // hide input groups
    void hideNormalInputGroup();
    void hideSpecularInputGroup();
    void hideRoughnessInputGroup();


    ~FormImageProp();
public slots:
    void open();
    void save();
    void reloadImageSettings();

    void updateComboBoxes(int index);
    void updateGuiSpinBoxesAndLabes(int);
    void updateSlidersOnRelease();
    void updateGuiCheckBoxes();


    void applyHeightToNormalConversion();
    void applyNormalToHeightConversion();
    void applyBaseConversionConversion();    
    void applyHeightNormalToOcclusionConversion();


    void showHeightCalculatorDialog();

    void toggleColorPicking(bool toggle);
    void togglePreviewSelectiveBlurMask(bool toggle);
    void colorPicked(QVector4D);
    void cancelColorPicking();


signals:
    void reloadSettingsFromConfigFile(TextureTypes type);
    void imageChanged();
    void imageLoaded(int width,int height);
    void conversionHeightToNormalApplied();
    void conversionNormalToHeightApplied();
    void conversionBaseConversionApplied();
    void conversionHeightNormalToOcclusionApplied();
    void recalculateOcclusion();
    void toggleColorPickingApplied(bool toggle);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
private:
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);

    bool bLoading;
    Ui::FormImageProp *ui;
    QImage image;
    QString imageName;
    FBOImageProporties imageProp;
    DialogHeightCalculator *heightCalculator; // height calculator tool

public:
    static QDir* recentDir;
};

#endif // FORMIMAGEPROP_H

