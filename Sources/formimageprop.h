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


#include "formimagebase.h"
#include "dialogheightcalculator.h"
#include "formbasemapconversionlevels.h"


namespace Ui {
class FormImageProp;
}

class FormImageProp : public FormImageBase
{
    Q_OBJECT

public:
    explicit FormImageProp(QMainWindow *parent = 0, QGLWidget* qlW_ptr = 0);
    void setImage(QImage image);
    void setPtrToGLWidget(QGLWidget* ptr){ imageProp.glWidget_ptr = ptr;  }


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
    void showNormalMixerGroup();


    ~FormImageProp();

public slots:


    void reloadImageSettings();

    void updateComboBoxes(int index);
    void updateGuiSpinBoxesAndLabes(int);
    void updateSlidersOnRelease();
    void updateGuiCheckBoxes();
    void updateSlidersNow(int);


    void applyHeightToNormalConversion();
    void applyNormalToHeightConversion();
    void applyBaseConversionConversion();    
    void applyHeightNormalToOcclusionConversion();


    void showHeightCalculatorDialog();

    void toggleColorPicking(bool toggle);
    void togglePreviewSelectiveBlurMask(bool toggle);
    void colorPicked(QVector4D);
    void cancelColorPicking();

    // normal mixer
    void openNormalMixerImage();
    void pasteNormalFromClipBoard();
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


private:
    bool loadFile(const QString &fileName);
    void pasteImageFromClipboard(QImage& _image);

    bool bLoading;
    Ui::FormImageProp *ui;
    DialogHeightCalculator      *heightCalculator;     // height calculator tool
    FormBaseMapConversionLevels* baseMapConvLevels[4]; // for levels of mipmaps

public:

    bool bOpenNormalMapMixer;


};

#endif // FORMIMAGEPROP_H

