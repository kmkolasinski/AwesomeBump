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

    void hideHNGroupBox();
    void hideNHGroupBox();
    void hideBMGroupBox();
    void hideSpecularGroupBox();
    void hideNormalStepBar();
    void hideSSAOBar();
    void hideHeightProcessingBox();
    void hideGrayScaleControl();
    void setSpecularControlChecked();
    void reloadSettings();

    ~FormImageProp();
public slots:
    void open();
    void save();
    void reloadImageSettings();

    void updateGrayScalePreset(int index);
    void updateGuiSpinBoxesAndLabes(int);
    void updateSlidersOnRelease();
    void updateGuiCheckBoxes();


    void applyHeightToNormalConversion();
    void applyNormalToHeightConversion();
    void applyBaseConversionConversion();    
    void toggleHNPreviewMode();
    void toggleAttachToNormal(bool toggle);

    void showHeightCalculatorDialog();

signals:
    void reloadSettingsFromConfigFile(TextureTypes type);
    void imageChanged();
    void imageLoaded(int width,int height);
    void conversionHeightToNormalApplied();
    void conversionNormalToHeightApplied();
    void conversionBaseConversionApplied();
    void recalculateOcclusion();
    void repaintNormalTexture();

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

