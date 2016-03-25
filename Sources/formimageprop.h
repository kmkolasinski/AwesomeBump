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

    void setupPopertiesGUI();



    void hideHeightInputGroup();
    void hideBMGroupBox();
    void hideSpecularGroupBox();
    void hideNormalStepBar();
    void hideOcclusionInputGroup();
    void hideGeneralGroup();
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
    void showGrungeSettingsGroup();
    void showGrungeMainImageWeightSlider();
    void hideGrungeBlendinModeComboBox();


    ~FormImageProp();

public slots:

    void propertyChanged(const QtnPropertyBase* changedProperty,
                         const QtnPropertyBase* firedProperty,
                         QtnPropertyChangeReason reason);
    void propertyFinishedEditing();
    void applyBaseConversion(const QtnPropertyButton* button); // convert Basemap to other textures
    void pasteNormalFromClipBoard(const QtnPropertyButton*);

    void reloadImageSettings();
    void pasteFromClipboard();
    void copyToClipboard();

    void updateComboBoxes(int index);
    void updateGuiSpinBoxesAndLabes(int);
    void updateSlidersOnRelease();

    void updateGuiCheckBoxes();
    void updateSlidersNow(int);


    void applyHeightToNormalConversion();
    void applyNormalToHeightConversion();
    void applyBaseConversionConversion();    
    void applyHeightNormalToOcclusionConversion();

    /**
     * Restore to default settings.
     */
    void resetBaseMapMinMaxColors();
    void showHeightCalculatorDialog();

    void toggleColorPicking(bool toggle);
    void togglePreviewSelectiveBlurMask(bool toggle);
    void colorPicked(QVector4D);
    void cancelColorPicking();
    void pickColorFromImage(QtnPropertyABColor *property);

    // normal mixer
    void openNormalMixerImage();
    void pasteNormalFromClipBoard();

    // grunge
    void toggleGrungeImageSettingsGroup(bool toggle);
    void invertGrunge(bool toggle);
    void loadPredefinedGrunge(QString);

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
    void pickImageColor( QtnPropertyABColor* property);
    void toggleGrungeSettings(bool toggle);


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

