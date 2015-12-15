#include "formimageprop.h"
#include "ui_formimageprop.h"




FormImageProp::FormImageProp(QMainWindow *parent, QGLWidget* qlW_ptr) :
    FormImageBase(parent),
    ui(new Ui::FormImageProp)
{
    ui->setupUi(this);

    bOpenNormalMapMixer   = false;

    imageProp.glWidget_ptr = qlW_ptr;
    
    connect(ui->pushButtonOpenImage,SIGNAL(released()),this,SLOT(open()));
    connect(ui->pushButtonSaveImage,SIGNAL(released()),this,SLOT(save()));
    connect(ui->pushButtonCopyToClipboard,SIGNAL(released()),this,SLOT(copyToClipboard()));
    connect(ui->pushButtonPasteFromClipboard,SIGNAL(released()),this,SLOT(pasteFromClipboard()));

    connect(ui->pushButtonRestoreSettings,SIGNAL(released()),this,SLOT(reloadImageSettings()));

    connect(ui->checkBoxRemoveShading,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxGrayScale,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    // gray scale properties
    connect(ui->comboBoxGrayScalePresets,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));
    connect(ui->horizontalSliderGrayScaleR,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrayScaleG,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrayScaleB,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));


    connect(ui->checkBoxInvertB,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxInvertR,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxInvertG,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    // standard enchancement
    connect(ui->horizontalSliderRemoveShadingGaussIter,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderAOCancelation         ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderRemoveShadingLFBlending,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderRemoveShadingLFRadius,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));



    connect(ui->horizontalSliderBlurNoPasses          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSmallDetails          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderMediumDetails         ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderDetailDepth           ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderColorHue              ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    // specular manipulation tool
    connect(ui->checkBoxSpecularControl             ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderSpeculatW1          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularW2          ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularRadius      ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularContrast    ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularAmplifier   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSpecularBrightness  ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->horizontalSliderSpeculatW1           ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularW2           ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularRadius       ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularRadius       ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularContrast     ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularAmplifier    ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSpecularBrightness   ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));

    connect(ui->horizontalSliderSharpenBlur,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalsStep,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    // height conversion buttons
    connect(ui->horizontalSliderConversionHNDepth   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->pushButtonConverToNormal            ,SIGNAL(released()),this,SLOT(applyHeightToNormalConversion()));
    connect(ui->pushButtonShowDepthCalculator       ,SIGNAL(released()),this,SLOT(showHeightCalculatorDialog()));


    // normal convertion buttons and sliders

    connect(ui->horizontalSliderNormalToHeightItersHuge     ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersVeryLarge,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersLarge    ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersMedium   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersSmall    ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalToHeightItersVerySmall,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->pushButtonConvertToHeight,SIGNAL(released()),this,SLOT(applyNormalToHeightConversion()));

    // base map convertion
    connect(ui->checkBoxEnableBaseMapToOthers               ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxEnableBaseMapHeightTexture          ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderBaseToOthersAngleCorrection ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderBaseToOthersAngleWeight     ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->pushButtonBaseMapToOthersHMaxVal,SIGNAL(toggled(bool)),this,SLOT(toggleColorPicking(bool)));
    connect(ui->pushButtonBaseMapToOthersHMinVal,SIGNAL(toggled(bool)),this,SLOT(toggleColorPicking(bool)));
    connect(ui->pushButtonBaseMapToOthersHReset ,SIGNAL(pressed()),this,SLOT(resetBaseMapMinMaxColors()));
    connect(ui->horizontalSliderBaseMapToOthersHRangeTolerance ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->pushButtonConvertToNormalAndHeight,SIGNAL(released()),this,SLOT(applyBaseConversionConversion()));

    // ssao calculation
    connect(ui->horizontalSliderSSAOBias        ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAODepth       ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAOIntensity   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSSAONoIters     ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->horizontalSliderSSAOBias        ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSSAODepth       ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSSAOIntensity   ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderSSAONoIters     ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));

    connect(ui->pushButtonConvertOcclusionFromHN,SIGNAL(released()),this,SLOT(applyHeightNormalToOcclusionConversion()));



    // levels properties
    connect(ui->horizontalSliderHeightProcMinValue,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));    
    connect(ui->horizontalSliderHeightProcMaxValue,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderHeightAveRadius   ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderHeightOffsetValue ,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->checkBoxHeightProcEnableNormalization ,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));

    connect(ui->horizontalSliderHeightProcMinValue,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderHeightProcMaxValue,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));
    connect(ui->horizontalSliderHeightAveRadius   ,SIGNAL(sliderMoved(int)),this,SLOT(updateGuiSpinBoxesAndLabes(int)));

    // selective blur buttons and sliders
    connect(ui->pushButtonSelectiveBlurPreviewMask,SIGNAL(released()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->pushButtonSelectiveBlurPreviewMask,SIGNAL(toggled(bool)),this,SLOT(togglePreviewSelectiveBlurMask(bool)));
    connect(ui->checkBoxSelectiveBlurInvertMask,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxSelectiveBlurEnable,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));

    connect(ui->comboBoxSelectiveBlurTypes,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));

    connect(ui->horizontalSliderSelectiveBlurBlending,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurMaskRadius,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurDOGAmplifier,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurDOGContrast,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurDOGOffset,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurDOGRadius,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->horizontalSliderSelectiveBlurMinValue,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurMaxValue,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurDetails,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderSelectiveBlurOffset,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->comboBoxMaskInputImage,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));
    connect(ui->horizontalSliderSelectiveBlurNoIters,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    // roughness settings
    connect(ui->horizontalSliderRoughnessDepth,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderRoughnessTreshold,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderRoughnessAmplifier,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->radioButtonRoughnessEnable,SIGNAL(clicked()),this,SLOT(cancelColorPicking()));
    connect(ui->radioButtonEnableColorPicking,SIGNAL(clicked()),this,SLOT(cancelColorPicking()));
    connect(ui->radioButtonRoughnessNoneEffect,SIGNAL(clicked()),this,SLOT(cancelColorPicking()));

    connect(ui->radioButtonRoughnessEnable,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->radioButtonEnableColorPicking,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->radioButtonRoughnessNoneEffect,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));


    connect(ui->pushButtonRoughnessPickColor,SIGNAL(toggled(bool)),this,SLOT(toggleColorPicking(bool)));
    connect(ui->comboBoxColorPickerMethod,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));
    connect(ui->horizontalSliderRoughnessColorOffset,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderRoughnessColorGlobalOffset,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderRoughnessColorAmplifier,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->checkBoxRoughnessColorInvert,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));



    // input image boxes
    connect(ui->comboBoxNormalInputImage,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));
    connect(ui->comboBoxSpecularInputImage,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));
    connect(ui->comboBoxOcclusionInputImage,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));
    connect(ui->comboBoxRoughnessInputImage,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));

    // normal map mixer
    connect(ui->pushButtonNormalMixerLoadImage,SIGNAL(released())        ,this,SLOT(openNormalMixerImage()));
    connect(ui->pushButtonNormalMixerPasteFromCB,SIGNAL(released())        ,this,SLOT(pasteNormalFromClipBoard()));

    connect(ui->checkBoxNormalMixerEnable,SIGNAL(clicked())              ,this,SLOT(updateGuiCheckBoxes()));
    connect(ui->horizontalSliderNormalMixerDepth,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalMixerAngle,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalMixerScale,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalMixerScale,SIGNAL(valueChanged(int)),this,SLOT(updateSlidersNow(int)));
    connect(ui->horizontalSliderNormalMixerPosX,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderNormalMixerPosY,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    // grunge
    connect(ui->horizontalSliderGrungeOverallWeight,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrungeSeed,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrungeRadius,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrungeRadius,SIGNAL(valueChanged(int)),this,SLOT(updateSlidersNow(int)));

    connect(ui->horizontalSliderGrungeNormalWarp,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));

    connect(ui->checkBoxGrungeRandomTranslations,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));
    connect(ui->checkBoxGrungeReplotAllAfterChange,SIGNAL(clicked()),this,SLOT(updateGuiCheckBoxes()));

    connect(ui->horizontalSliderGrungeImageWeight,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->horizontalSliderGrungeMainImageWeight,SIGNAL(sliderReleased()),this,SLOT(updateSlidersOnRelease()));
    connect(ui->comboBoxGrungeBlendingMode,SIGNAL(activated(int)),this,SLOT(updateComboBoxes(int)));
    // this actually invert all color components
    connect(ui->checkBoxGrungeInvert,SIGNAL(toggled(bool)),this,SLOT(invertGrunge(bool)));
    connect(ui->comboBoxGrungePredefined,SIGNAL(activated(QString)),this,SLOT(loadPredefinedGrunge(QString)));

    setAcceptDrops(true);
    ui->groupBoxRemoveShading->hide();
    ui->checkBoxRemoveShading->hide();
    ui->groupBoxGrayScale->hide();
    ui->groupBoxSelectiveBlurOptions->hide();
    ui->groupBoxSelectiveBlurLevels->hide();
    heightCalculator = new DialogHeightCalculator;

    ui->labelHue->hide();
    ui->labelHueValue->hide();
    ui->horizontalSliderColorHue->hide();

    ui->groupBoxRoughnessSurfaceFilter->hide();
    ui->groupBoxRoughnessColorFilter_2->hide();
    ui->groupBoxBaseToOthersProcessing->hide();
    ui->groupBoxSpecularProcessing->hide();

    // conversion levels
    for(int i = 0; i < 4 ; i++){
        baseMapConvLevels[i] = new FormBaseMapConversionLevels;
        connect(baseMapConvLevels[i],SIGNAL(slidersChanged()),this,SLOT(updateSlidersOnRelease()));
    }
    ui->verticalLayoutBaseMapConvLevel0->addWidget(baseMapConvLevels[0]);
    ui->verticalLayoutBaseMapConvLevel1->addWidget(baseMapConvLevels[1]);
    ui->verticalLayoutBaseMapConvLevel2->addWidget(baseMapConvLevels[2]);
    ui->verticalLayoutBaseMapConvLevel3->addWidget(baseMapConvLevels[3]);

    for(int i = 0; i < 4 ; i++){
        baseMapConvLevels[i]->show();
    }

    // normal mixer
    ui->groupBoxNormalMixerSettings->hide();
    ui->groupBoxNormalMixer->hide();
    ui->groupBoxGrungeSettings->hide();
    ui->groupBoxGrungeImageSettings->hide();

    ui->labelGrungeImageWeight->hide();
    ui->labelGrungeImageWeight2->hide();
    ui->horizontalSliderGrungeMainImageWeight->hide();





    setMouseTracking(true);
    setFocus();
    setFocusPolicy(Qt::ClickFocus);
}

FormImageProp::~FormImageProp()
{

    delete heightCalculator;
    for(int i = 0; i < 4 ; i++) delete baseMapConvLevels[i];
    delete ui;
}



bool FormImageProp::loadFile(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QImage _image;

    // Targa support added
    if(fileInfo.completeSuffix().compare("tga") == 0){
        TargaImage tgaImage;
        _image = tgaImage.read(fileName);
    }else{
        QImageReader loadedImage(fileName);
        _image = loadedImage.read();
    }

    if (_image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }
    if(bOpenNormalMapMixer){
        qDebug() << "<FormImageProp> Open normal mixer image:" << fileName;

        imageProp.glWidget_ptr->makeCurrent();
        if(glIsTexture(imageProp.normalMixerInputTexId)) imageProp.glWidget_ptr->deleteTexture(imageProp.normalMixerInputTexId);
        imageProp.normalMixerInputTexId = imageProp.glWidget_ptr->bindTexture(_image,GL_TEXTURE_2D);
        ui->labelNormalMixerInfo->setText("Current image:"+ fileInfo.baseName());
        emit imageChanged();

    }else{
        qDebug() << "<FormImageProp> Open image:" << fileName;

        imageName = fileInfo.baseName();
        (*recentDir).setPath(fileName);
        image    = _image;
        imageProp.init(image);

        //emit imageChanged();
        emit imageLoaded(image.width(),image.height());
        if(imageProp.imageType == GRUNGE_TEXTURE)emit imageChanged();
    }
    return true;
}

void FormImageProp::pasteImageFromClipboard(QImage& _image){
    imageName = "clipboard_image";
    image     = _image;
    imageProp.init(image);
    emit imageLoaded(image.width(),image.height());
    if(imageProp.imageType == GRUNGE_TEXTURE)emit imageChanged();
}



void FormImageProp::setImage(QImage _image){
    image    = _image;
    if (imageProp.glWidget_ptr->isValid())
      imageProp.init(image);
    else
        qDebug() << Q_FUNC_INFO << "Invalid context.";
}


void FormImageProp::setSpecularControlChecked(){
    ui->checkBoxSpecularControl->setChecked(true);
}

void FormImageProp::updateComboBoxes(int index){
    index = ui->comboBoxGrayScalePresets->currentIndex();
    switch(index){
        case(0):
        imageProp.grayScalePreset.mode1();
        break;
        case(1):
        imageProp.grayScalePreset.mode2();
        break;
        default:break;
    };

    // updating selective blur groupboxes
    imageProp.selectiveBlurType = (SelectiveBlurType) ui->comboBoxSelectiveBlurTypes->currentIndex();


    switch(imageProp.selectiveBlurType){
        case(SELECTIVE_BLUR_DIFFERENCE_OF_GAUSSIANS):
            ui->groupBoxSelectiveBlurLevels->hide();
            ui->groupBoxSelectiveBlurDOG->show();
        break;
        case(SELECTIVE_BLUR_LEVELS):
            ui->groupBoxSelectiveBlurLevels->show();
            ui->groupBoxSelectiveBlurDOG->hide();
        break;
    };

    bLoading = true;
    ui->horizontalSliderGrayScaleR->setValue(imageProp.grayScalePreset.R*255);
    ui->horizontalSliderGrayScaleG->setValue(imageProp.grayScalePreset.G*255);
    ui->horizontalSliderGrayScaleB->setValue(imageProp.grayScalePreset.B*255);
    bLoading = false;

    // input image case study
    switch(imageProp.imageType){
        case(NORMAL_TEXTURE):
        // select propper input image for normals
        ui->pushButtonConverToNormal->setEnabled(false);

        switch(ui->comboBoxNormalInputImage->currentIndex()){
            case(0):
                imageProp.inputImageType = INPUT_FROM_NORMAL_INPUT ;
                ui->pushButtonConverToNormal->setEnabled(true);
                break;
            case(1): imageProp.inputImageType = INPUT_FROM_HEIGHT_INPUT; break;
            case(2): imageProp.inputImageType = INPUT_FROM_HEIGHT_OUTPUT; break;
        }
        break;
         // end of case NORMAL
        case(SPECULAR_TEXTURE):
        // select propper input image for specular

        switch(ui->comboBoxSpecularInputImage->currentIndex()){
            case(0):
                imageProp.inputImageType = INPUT_FROM_SPECULAR_INPUT;
                break;
            case(1): imageProp.inputImageType = INPUT_FROM_DIFFUSE_INPUT; break;
            case(2): imageProp.inputImageType = INPUT_FROM_DIFFUSE_OUTPUT; break;
            case(3): imageProp.inputImageType = INPUT_FROM_HEIGHT_INPUT; break;
            case(4): imageProp.inputImageType = INPUT_FROM_HEIGHT_OUTPUT; break;
        }
        break;
         // end of case SPECULAR
        case(OCCLUSION_TEXTURE):
        // select propper input image for occlusion
        ui->pushButtonConvertOcclusionFromHN->setEnabled(false);
        switch(ui->comboBoxOcclusionInputImage->currentIndex()){
            case(0):
                imageProp.inputImageType = INPUT_FROM_OCCLUSION_INPUT;
                ui->pushButtonConvertOcclusionFromHN->setEnabled(true);
                break;
            case(1): imageProp.inputImageType = INPUT_FROM_HI_NI; break;
            case(2): imageProp.inputImageType = INPUT_FROM_HO_NO; break;
        }
        break;
         // end of case OCCLUSION
        case(ROUGHNESS_TEXTURE):
        // select propper input image for roughness

        switch(ui->comboBoxRoughnessInputImage->currentIndex()){
            case(0):
                imageProp.inputImageType = INPUT_FROM_ROUGHNESS_INPUT;
                break;
            case(1): imageProp.inputImageType = INPUT_FROM_DIFFUSE_INPUT;  break;
            case(2): imageProp.inputImageType = INPUT_FROM_DIFFUSE_OUTPUT; break;

        }
        break;
         // end of case ROUGHNESS
        case(METALLIC_TEXTURE):
        // select propper input image for roughness

        switch(ui->comboBoxRoughnessInputImage->currentIndex()){
            case(0):
                imageProp.inputImageType = INPUT_FROM_METALLIC_INPUT;
                break;
            case(1): imageProp.inputImageType = INPUT_FROM_DIFFUSE_INPUT;  break;
            case(2): imageProp.inputImageType = INPUT_FROM_DIFFUSE_OUTPUT; break;

        }
        break;
         // end of case Metallic
        case(HEIGHT_TEXTURE):
        // select propper input image for roughness

        switch(ui->comboBoxMaskInputImage->currentIndex()){
            case(0):
                imageProp.selectiveBlurMaskInputImageType = INPUT_FROM_HEIGHT_OUTPUT;
                break;
            case(1): imageProp.selectiveBlurMaskInputImageType = INPUT_FROM_DIFFUSE_OUTPUT  ;  break;
            case(2): imageProp.selectiveBlurMaskInputImageType = INPUT_FROM_METALLIC_OUTPUT ;  break;
            case(3): imageProp.selectiveBlurMaskInputImageType = INPUT_FROM_ROUGHNESS_OUTPUT;  break;
        }
        break;
         // end of case Metallic
        default:break; // do nothing
    };


    // color picker
    imageProp.colorPickerMethod     = (ColorPickerMethod) ui->comboBoxColorPickerMethod->currentIndex();

    // grunge blending mode
    imageProp.grungeBlendingMode    =  ui->comboBoxGrungeBlendingMode->currentIndex();
    emit imageChanged();
}

void FormImageProp::updateGuiSpinBoxesAndLabes(int){
    if(bLoading == true) return;

    ui->doubleSpinBoxSpecularW1         ->setValue(ui->horizontalSliderSpeculatW1           ->value()/100.0);
    ui->doubleSpinBoxSpecularW2         ->setValue(ui->horizontalSliderSpecularW2           ->value()/100.0);
    ui->doubleSpinBoxSpecularRadius     ->setValue(ui->horizontalSliderSpecularRadius       ->value()*1.0);
    ui->doubleSpinBoxSpecularContrast   ->setValue(ui->horizontalSliderSpecularContrast     ->value()/100.0-1.0);
    ui->doubleSpinBoxSpecularAmplifier  ->setValue(ui->horizontalSliderSpecularAmplifier    ->value()/10.0);
    ui->doubleSpinBoxSpecularBrightness ->setValue(ui->horizontalSliderSpecularBrightness   ->value()/100.0);
    ui->doubleSpinBoxConversionHNDepth  ->setValue(ui->horizontalSliderConversionHNDepth    ->value()/5.0);
    // gray scale preset
    imageProp.grayScalePreset.R = ui->horizontalSliderGrayScaleR->value()/255.0;
    imageProp.grayScalePreset.G = ui->horizontalSliderGrayScaleG->value()/255.0;
    imageProp.grayScalePreset.B = ui->horizontalSliderGrayScaleB->value()/255.0;

    imageProp.specularW1        = ui->doubleSpinBoxSpecularW1       ->value();
    imageProp.specularW2        = ui->doubleSpinBoxSpecularW2       ->value();
    imageProp.specularRadius    = ui->doubleSpinBoxSpecularRadius   ->value();
    imageProp.specularContrast  = ui->doubleSpinBoxSpecularContrast ->value();
    imageProp.specularAmplifier = ui->doubleSpinBoxSpecularAmplifier->value();
    imageProp.specularBrightness= ui->doubleSpinBoxSpecularBrightness->value();

    imageProp.noRemoveShadingGaussIter = ui->horizontalSliderRemoveShadingGaussIter->value();
    imageProp.aoCancellation           = ui->horizontalSliderAOCancelation->value()/100.0;

    imageProp.removeShadingLFBlending = ui->horizontalSliderRemoveShadingLFBlending->value()/100.0;
    imageProp.removeShadingLFRadius   = ui->horizontalSliderRemoveShadingLFRadius->value();

    imageProp.noBlurPasses      = ui->horizontalSliderBlurNoPasses ->value();
    imageProp.smallDetails      = ui->horizontalSliderSmallDetails ->value()/100.0;
    imageProp.mediumDetails     = ui->horizontalSliderMediumDetails->value()/100.0;
    imageProp.colorHue          = ui->horizontalSliderColorHue->value()/180.0;


    imageProp.detailDepth       = ui->horizontalSliderDetailDepth->value()/20.0;
    imageProp.sharpenBlurAmount = ui->horizontalSliderSharpenBlur->value();
    imageProp.normalsStep       = ui->horizontalSliderNormalsStep->value()/100.0;

    imageProp.conversionHNDepth         = ui->doubleSpinBoxConversionHNDepth->value();

    imageProp.conversionNHItersHuge     = ui->horizontalSliderNormalToHeightItersHuge->value();
    imageProp.conversionNHItersVeryLarge= ui->horizontalSliderNormalToHeightItersVeryLarge->value();
    imageProp.conversionNHItersLarge    = ui->horizontalSliderNormalToHeightItersLarge->value();
    imageProp.conversionNHItersMedium   = ui->horizontalSliderNormalToHeightItersMedium->value();
    imageProp.conversionNHItersSmall    = ui->horizontalSliderNormalToHeightItersSmall->value();
    imageProp.conversionNHItersVerySmall= ui->horizontalSliderNormalToHeightItersVerySmall->value();

    // update conversion levels
    if(imageProp.imageType == DIFFUSE_TEXTURE){
        for(int i = 0; i < 4 ; i++){
            baseMapConvLevels[i]->getSlidersValues(imageProp.baseMapConvLevels[i]);
        }
    }

    imageProp.baseMapAngleCorrection                 = ui->horizontalSliderBaseToOthersAngleCorrection->value();
    imageProp.baseMapAngleWeight                     = ui->horizontalSliderBaseToOthersAngleWeight->value();
    imageProp.conversionBaseMapHeightMinMaxTolerance = ui->horizontalSliderBaseMapToOthersHRangeTolerance->value();

    imageProp.ssaoNoIters   = ui->horizontalSliderSSAONoIters->value();
    imageProp.ssaoDepth     = ui->horizontalSliderSSAODepth->value()/100.0;
    imageProp.ssaoIntensity = ui->horizontalSliderSSAOIntensity->value()/100.0;
    imageProp.ssaoBias      = ui->horizontalSliderSSAOBias->value()/100.0;

    ui->doubleSpinBoxSSAONoIters->setValue(imageProp.ssaoNoIters);
    ui->doubleSpinBoxSSAODepth  ->setValue(imageProp.ssaoDepth);
    ui->doubleSpinBoxSSAOBias   ->setValue(imageProp.ssaoBias);
    ui->doubleSpinBoxSSAOIntensity->setValue(imageProp.ssaoIntensity);

    imageProp.heightMinValue        = ui->horizontalSliderHeightProcMinValue->value()/200.0;
    imageProp.heightMaxValue        = ui->horizontalSliderHeightProcMaxValue->value()/200.0;
    imageProp.heightAveragingRadius = ui->horizontalSliderHeightAveRadius->value();
    imageProp.heightOffsetValue     = ui->horizontalSliderHeightOffsetValue->value()/100.0;

    ui->labelHeightProcMinValue->setText(QString::number(imageProp.heightMinValue));
    ui->labelHeightProcMaxValue->setText(QString::number(imageProp.heightMaxValue));


    // selective blur
    imageProp.selectiveBlurBlending     = ui->horizontalSliderSelectiveBlurBlending->value()/100.0;
    imageProp.selectiveBlurMaskRadius   = ui->horizontalSliderSelectiveBlurMaskRadius->value() ;
    imageProp.selectiveBlurDOGRadius    = ui->horizontalSliderSelectiveBlurDOGRadius->value();
    imageProp.selectiveBlurDOGConstrast = ui->horizontalSliderSelectiveBlurDOGContrast->value()/100.0;
    imageProp.selectiveBlurDOGAmplifier = ui->horizontalSliderSelectiveBlurDOGAmplifier->value()/10.0;
    imageProp.selectiveBlurDOGOffset    = ui->horizontalSliderSelectiveBlurDOGOffset->value()/255.0;

    imageProp.selectiveBlurMinValue        = ui->horizontalSliderSelectiveBlurMinValue->value()/255.0;
    imageProp.selectiveBlurMaxValue        = ui->horizontalSliderSelectiveBlurMaxValue->value()/255.0;
    imageProp.selectiveBlurDetails         = ui->horizontalSliderSelectiveBlurDetails->value();
    imageProp.selectiveBlurOffsetValue     = ui->horizontalSliderSelectiveBlurOffset->value()/255.0;
    imageProp.selectiveBlurNoIters         = ui->horizontalSliderSelectiveBlurNoIters->value();

    // roughness settings
    imageProp.roughnessDepth    = ui->horizontalSliderRoughnessDepth->value()/100.0;
    imageProp.roughnessTreshold = ui->horizontalSliderRoughnessTreshold->value()/100.0;
    imageProp.roughnessAmplifier= ui->horizontalSliderRoughnessAmplifier->value()/100.0;

    imageProp.roughnessColorOffset= ui->horizontalSliderRoughnessColorOffset->value()/100.0;
    imageProp.roughnessColorAmplifier= ui->horizontalSliderRoughnessColorAmplifier->value()/100.0;
    imageProp.roughnessColorGlobalOffset= ui->horizontalSliderRoughnessColorGlobalOffset->value()/255.0;



    imageProp.normalMixerDepth    = ui->horizontalSliderNormalMixerDepth->value();
    imageProp.normalMixerScale    = ui->horizontalSliderNormalMixerScale->value()/10.0;
    imageProp.normalMixerAngle    = 3.1415926*ui->horizontalSliderNormalMixerAngle->value()/180.0;
    imageProp.normalMixerPosX     = ui->horizontalSliderNormalMixerPosX->value()/100.0;
    imageProp.normalMixerPosY     = ui->horizontalSliderNormalMixerPosY->value()/100.0;

    if(imageProp.imageType == GRUNGE_TEXTURE){
        imageProp.grungeOverallWeight = ui->horizontalSliderGrungeOverallWeight->value();
        imageProp.grungeSeed          = ui->horizontalSliderGrungeSeed->value();
        imageProp.grungeRadius        = ui->horizontalSliderGrungeRadius->value();
        imageProp.grungeNormalWarp    = ui->horizontalSliderGrungeNormalWarp->value();
        ui->labelGrungeRadius->setText(QString::number(imageProp.grungeRadius/25.0));
    }
    imageProp.grungeImageWeight       = ui->horizontalSliderGrungeImageWeight->value();
    imageProp.grungeMainImageWeight   = ui->horizontalSliderGrungeMainImageWeight->value();


    if(ui->horizontalSliderGrungeOverallWeight->value() == 0){
        emit  toggleGrungeSettings(false);
    }else{
        emit  toggleGrungeSettings(true);
    }

    ui->labelNormalMixerScale->setText(QString::number(imageProp.normalMixerScale));
}

void FormImageProp::updateSlidersOnRelease(){
    if(bLoading == true) return;
    updateGuiSpinBoxesAndLabes(0);
    emit imageChanged();
}


void FormImageProp::updateSlidersNow(int){

    imageProp.normalMixerScale    = ui->horizontalSliderNormalMixerScale->value()/10.0;
    ui->labelNormalMixerScale->setText(QString::number(imageProp.normalMixerScale));

    imageProp.grungeRadius        = ui->horizontalSliderGrungeRadius->value();
    ui->labelGrungeRadius->setText(QString::number(imageProp.grungeRadius/25.0));
}

void FormImageProp::updateGuiCheckBoxes(){
    if(bLoading == true) return;

    imageProp.bRemoveShading  = ui->checkBoxRemoveShading->isChecked();
    imageProp.bSpeclarControl = ui->checkBoxSpecularControl->isChecked();

    imageProp.bGrayScale            = ui->checkBoxGrayScale->isChecked();
    imageProp.bInvertB              = ui->checkBoxInvertB->isChecked();
    imageProp.bInvertR              = ui->checkBoxInvertR->isChecked();
    imageProp.bInvertG              = ui->checkBoxInvertG->isChecked();



    imageProp.bConversionBaseMap    = ui->checkBoxEnableBaseMapToOthers->isChecked();
    imageProp.bConversionBaseMapShowHeightTexture  = ui->checkBoxEnableBaseMapHeightTexture->isChecked();

    imageProp.bSelectiveBlurPreviewMask = ui->pushButtonSelectiveBlurPreviewMask->isChecked();
    imageProp.bSelectiveBlurInvertMask  = ui->checkBoxSelectiveBlurInvertMask   ->isChecked();
    imageProp.bSelectiveBlurEnable      = ui->checkBoxSelectiveBlurEnable       ->isChecked();


    imageProp.bRoughnessSurfaceEnable           = ui->radioButtonRoughnessEnable->isChecked();
    imageProp.bRoughnessEnableColorPicking      = ui->radioButtonEnableColorPicking->isChecked();
    imageProp.bRoughnessInvertColorMask         = ui->checkBoxRoughnessColorInvert->isChecked();

    imageProp.bHeightEnableNormalization        = ui->checkBoxHeightProcEnableNormalization->isChecked();
    imageProp.bNormalMixerEnabled               = ui->checkBoxNormalMixerEnable->isChecked();


    if(imageProp.imageType == GRUNGE_TEXTURE){
        imageProp.bGrungeEnableRandomTranslations = ui->checkBoxGrungeRandomTranslations->isChecked();
        imageProp.bGrungeReplotAllWhenChanged     = ui->checkBoxGrungeReplotAllAfterChange->isChecked();
    }


    if(imageProp.bRoughnessEnableColorPicking){
         if(!imageProp.bRoughnessColorPickingToggled)ui->groupBoxGeneral->setDisabled(true);
    }else{
         ui->groupBoxGeneral->setDisabled(false);
    }

    emit imageChanged();
}

void FormImageProp::applyHeightToNormalConversion(){
    emit conversionHeightToNormalApplied();
}
void FormImageProp::applyNormalToHeightConversion(){
    emit conversionNormalToHeightApplied();
}

void FormImageProp::applyBaseConversionConversion(){
    emit conversionBaseConversionApplied();
}

void FormImageProp::applyHeightNormalToOcclusionConversion(){
    emit conversionHeightNormalToOcclusionApplied();
}

void FormImageProp::resetBaseMapMinMaxColors(){
    // negative X component means no action, this is cecked in the
    // GLSL code.
    imageProp.conversionBaseMapheightMax = QVector3D(-1,0,0);
    imageProp.conversionBaseMapheightMin = QVector3D(-1,0,0);
    // Take care about the buttons and colors
    ui->pushButtonBaseMapToOthersHMinVal->setChecked(false);
    ui->pushButtonBaseMapToOthersHMaxVal->setChecked(false);
    ui->pushButtonBaseMapToOthersHMaxVal->setPalette(QApplication::palette());
    ui->pushButtonBaseMapToOthersHMinVal->setPalette(QApplication::palette());
    emit imageChanged();

}

void FormImageProp::showHeightCalculatorDialog(){

     //heightCalculator->setImageSize(imageProp.ref_fbo->width(),imageProp.ref_fbo->height());
     heightCalculator->setImageSize(imageProp.fbo->width(),imageProp.fbo->height());
     unsigned int result = heightCalculator->exec();
     if(result == QDialog::Accepted){
        ui->horizontalSliderConversionHNDepth->setValue(heightCalculator->getDepthInPixels()*5);
        updateSlidersOnRelease();
        qDebug() << "Height map::Depth calculated:" << heightCalculator->getDepthInPixels();
     }
}

void FormImageProp::toggleColorPicking(bool toggle){
    imageProp.bRoughnessColorPickingToggled = toggle;
    // Roughness or Metallic map edition
    if(imageProp.bRoughnessEnableColorPicking && toggle){
        QPalette palette = ui->pushButtonRoughnessPickColor->palette();
        palette.setColor(ui->pushButtonRoughnessPickColor->backgroundRole(), QColor(255, 0, 0, 127));
        ui->pushButtonRoughnessPickColor->setPalette(palette);
        ui->pushButtonRoughnessPickColor->setText("pick color");

        emit toggleColorPickingApplied(toggle);

    }
    if(toggle == false){
        QPalette palette = ui->pushButtonRoughnessPickColor->palette();
        palette.setColor(ui->pushButtonRoughnessPickColor->backgroundRole(), QColor(0, 255, 0, 127));
        ui->pushButtonRoughnessPickColor->setPalette(palette);
        ui->pushButtonRoughnessPickColor->setText("click to pick");


        // Same for base map tab
        ui->pushButtonBaseMapToOthersHMaxVal->setText("Max");
        ui->pushButtonBaseMapToOthersHMaxVal->setChecked(false);
        ui->pushButtonBaseMapToOthersHMinVal->setText("Min");
        ui->pushButtonBaseMapToOthersHMinVal->setChecked(false);

    }

    // When conversion from BaseMap to others is enabled:
    if(imageProp.bConversionBaseMap){
        if(ui->pushButtonBaseMapToOthersHMaxVal->isChecked()){
            ui->pushButtonBaseMapToOthersHMaxVal->setText("Pick!");
            ui->pushButtonBaseMapToOthersHMinVal->setChecked(false); // toggle off the second button
        }
        if(ui->pushButtonBaseMapToOthersHMinVal->isChecked()){
            ui->pushButtonBaseMapToOthersHMinVal->setText("Pick!");
            ui->pushButtonBaseMapToOthersHMaxVal->setChecked(false); // toggle off the second button
        }

        emit toggleColorPickingApplied(toggle);
    }

    emit imageChanged();
}

void FormImageProp::colorPicked(QVector4D color){

    if(imageProp.bRoughnessColorPickingToggled){
        ui->pushButtonRoughnessPickColor->setChecked(false);
        imageProp.bRoughnessColorPickingToggled = false;
        imageProp.pickedColor                   = color.toVector3D()/255.0;
        QPalette palette = ui->labelRoughnessPickedColor->palette();
        palette.setColor(ui->labelRoughnessPickedColor->backgroundRole(), QColor(color.x(),color.y(),color.z()));
        palette.setColor(ui->labelRoughnessPickedColor->foregroundRole(), QColor(color.x(),color.y(),color.z()));
        ui->labelRoughnessPickedColor->setPalette(palette);
        update();
        emit imageChanged();
    }

    // When conversion from BaseMap to others is enabled:
    if(imageProp.bConversionBaseMap){
        if(ui->pushButtonBaseMapToOthersHMaxVal->isChecked()){
            ui->pushButtonBaseMapToOthersHMaxVal->setChecked(false);
            imageProp.conversionBaseMapheightMax = color.toVector3D()/255.0;
            QPalette palette = ui->pushButtonBaseMapToOthersHMaxVal->palette();
            palette.setColor(ui->pushButtonBaseMapToOthersHMaxVal->backgroundRole(), QColor(color.x(),color.y(),color.z()));
            palette.setColor(ui->pushButtonBaseMapToOthersHMaxVal->foregroundRole(), QColor(255-color.x(),255-color.y(),255-color.z()));
            ui->pushButtonBaseMapToOthersHMaxVal->setPalette(palette);
        }

        if(ui->pushButtonBaseMapToOthersHMinVal->isChecked()){
            ui->pushButtonBaseMapToOthersHMinVal->setChecked(false);
            imageProp.conversionBaseMapheightMin = color.toVector3D()/255.0;
            QPalette palette = ui->pushButtonBaseMapToOthersHMinVal->palette();
            palette.setColor(ui->pushButtonBaseMapToOthersHMinVal->backgroundRole(), QColor(color.x(),color.y(),color.z()));
            palette.setColor(ui->pushButtonBaseMapToOthersHMinVal->foregroundRole(), QColor(255-color.x(),255-color.y(),255-color.z()));
            ui->pushButtonBaseMapToOthersHMinVal->setPalette(palette);
        }
        update();
        emit imageChanged();
    }

}

void FormImageProp::cancelColorPicking(){
    imageProp.bRoughnessColorPickingToggled = false;
    ui->pushButtonRoughnessPickColor->setChecked(false);

}

void FormImageProp::openNormalMixerImage(){
    bOpenNormalMapMixer = true;
    open();
    bOpenNormalMapMixer = false;
}

void FormImageProp::pasteNormalFromClipBoard(){
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {
        qDebug() << "<FormImageProp> Normal image :"+
                    PostfixNames::getTextureName(imageProp.imageType)+
                    " loaded from clipboard.";
        QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        QImage _image = pixmap.toImage();

        imageProp.glWidget_ptr->makeCurrent();
        if(glIsTexture(imageProp.normalMixerInputTexId)) imageProp.glWidget_ptr->deleteTexture(imageProp.normalMixerInputTexId);
        imageProp.normalMixerInputTexId = imageProp.glWidget_ptr->bindTexture(_image,GL_TEXTURE_2D);
        ui->labelNormalMixerInfo->setText("Current image: (clipboard source)");
        emit imageChanged();

    }
}

void FormImageProp::togglePreviewSelectiveBlurMask(bool toggle){
    QPalette palette = ui->pushButtonSelectiveBlurPreviewMask->palette();
    palette.setColor(ui->pushButtonSelectiveBlurPreviewMask->backgroundRole(), QColor(255*int(toggle), 255*int(!toggle), 0, 127));
    if(toggle) ui->pushButtonSelectiveBlurPreviewMask->setText("Disable preview");
    else ui->pushButtonSelectiveBlurPreviewMask->setText("Preview mask");
    ui->pushButtonSelectiveBlurPreviewMask->setPalette(palette);
}

void FormImageProp::hideSpecularGroupBox(){
    ui->groupBoxSpecular->hide();
}

void FormImageProp::hideNormalInputGroup(){
    ui->groupBoxNormalInputImage->hide();
}
void FormImageProp::hideSpecularInputGroup(){
    ui->groupBoxSpecularInputImage->hide();
}

void FormImageProp::hideRoughnessInputGroup(){
    ui->groupBoxRoughnessInputImage->hide();
}

void FormImageProp::showNormalMixerGroup(){
    ui->groupBoxNormalMixer->show();
}

void FormImageProp::showGrungeSettingsGroup(){
    ui->groupBoxGrungeSettings->show();
    // ------------------------------------------------------- //
    //               Loading grunge maps folders
    // ------------------------------------------------------- //
    qDebug() << "Loading cubemaps folders:";
    QDir currentDir(QString(RESOURCE_BASE) + "Core/2D/grunge");
    currentDir.setFilter(QDir::Files);
    QStringList entries = currentDir.entryList();
    for( QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry ){
        QString dirname=*entry;
        if(dirname != tr(".") && dirname != tr("..")){
            qDebug() << "Loading grunge map:" << dirname;
            ui->comboBoxGrungePredefined->addItem(dirname);
        }
    }// end of for
    // setting cube map for glWidget
}

void FormImageProp::showGrungeMainImageWeightSlider(){
    ui->labelGrungeImageWeight->show();
    ui->labelGrungeImageWeight2->show();
    ui->horizontalSliderGrungeMainImageWeight->show();
}

void FormImageProp::hideGrungeBlendinModeComboBox(){
    ui->comboBoxGrungeBlendingMode->hide();
    ui->labelGrungeBlendingMode->hide();
}

void FormImageProp::toggleGrungeImageSettingsGroup(bool toggle){
    if(toggle) ui->groupBoxGrungeImageSettings->show();
    else ui->groupBoxGrungeImageSettings->hide();
}

void FormImageProp::invertGrunge(bool toggle){

    ui->checkBoxInvertB->setChecked(toggle);
    ui->checkBoxInvertG->setChecked(toggle);
    ui->checkBoxInvertR->setChecked(toggle);
    updateGuiCheckBoxes();
}

void FormImageProp::loadPredefinedGrunge(QString image){
    loadFile(QString(RESOURCE_BASE) + "Core/2D/grunge/" + image);
}

void FormImageProp::hideOcclusionInputGroup(){
    ui->groupBoxOcclusionInputImage->hide();
}

void FormImageProp::hideHeightInputGroup(){
    ui->groupBoxHeightInputImage->hide();
}
void FormImageProp::hideBMGroupBox(){
    ui->groupBoxBM->hide();
}

void FormImageProp::hideNormalStepBar(){
    ui->labelNormalStep->hide();
    ui->labelNormalStepValue->hide();
    ui->horizontalSliderNormalsStep->hide();
}


void FormImageProp::hideHeightProcessingBox(){
    ui->groupBoxHeightProcessing->hide();
}

void FormImageProp::hideSelectiveBlurBox(){
    ui->groupBoxSelectiveBlur->hide();
    ui->groupBoxSelectiveBlurDOG->hide();
    ui->groupBoxSelectiveBlurLevels->hide();
    ui->groupBoxSelectiveBlurOptions->hide();
}


void FormImageProp::hideGrayScaleControl(){
    ui->checkBoxGrayScale->hide();
}

bool FormImageProp::isCheckedGrayScale(){
    return ui->checkBoxGrayScale->isChecked();
}

void FormImageProp::reloadSettings(){
    bLoading = true;
    ui->horizontalSliderGrayScaleR->setValue(imageProp.grayScalePreset.R*255);
    ui->horizontalSliderGrayScaleG->setValue(imageProp.grayScalePreset.G*255);
    ui->horizontalSliderGrayScaleB->setValue(imageProp.grayScalePreset.B*255);

    ui->checkBoxSpecularControl->setChecked(imageProp.bSpeclarControl);
    ui->checkBoxRemoveShading  ->setChecked(imageProp.bRemoveShading);
    ui->checkBoxGrayScale      ->setChecked(imageProp.bGrayScale);

    ui->checkBoxInvertB->setChecked(imageProp.bInvertB);
    ui->checkBoxInvertR->setChecked(imageProp.bInvertR);
    ui->checkBoxInvertG->setChecked(imageProp.bInvertG);

    //ui->checkBoxEnableHeightToNormal    ->setChecked(imageProp.bConversionHN);
    //ui->checkBoxEnableNormalToHeight    ->setChecked(imageProp.bConversionNH);
    //
    ui->checkBoxEnableBaseMapHeightTexture    ->setChecked(imageProp.bConversionBaseMapShowHeightTexture);
    ui->horizontalSliderRemoveShadingGaussIter->setValue(imageProp.noRemoveShadingGaussIter);


    ui->horizontalSliderSpeculatW1        ->setValue(imageProp.specularW1*100);
    ui->horizontalSliderSpecularW2        ->setValue(imageProp.specularW2*100);
    ui->horizontalSliderSpecularRadius    ->setValue(imageProp.specularRadius);
    ui->horizontalSliderSpecularContrast  ->setValue(imageProp.specularContrast*100+100);
    ui->horizontalSliderSpecularAmplifier ->setValue(imageProp.specularAmplifier*10);
    ui->horizontalSliderSpecularBrightness->setValue(imageProp.specularBrightness*100);


    ui->horizontalSliderBlurNoPasses    ->setValue(imageProp.noBlurPasses);
    ui->horizontalSliderSmallDetails    ->setValue(imageProp.smallDetails*100);
    ui->horizontalSliderMediumDetails   ->setValue(imageProp.mediumDetails*100);

    ui->horizontalSliderDetailDepth->setValue(imageProp.detailDepth*20);
    ui->horizontalSliderSharpenBlur->setValue(imageProp.sharpenBlurAmount);
    ui->horizontalSliderNormalsStep->setValue(imageProp.normalsStep*100);

    ui->horizontalSliderHeightAveRadius     ->setValue(imageProp.heightAveragingRadius);
    ui->horizontalSliderHeightProcMaxValue  ->setValue(imageProp.heightMaxValue*200);
    ui->horizontalSliderHeightProcMinValue  ->setValue(imageProp.heightMinValue*200);
    ui->horizontalSliderHeightOffsetValue   ->setValue(imageProp.heightOffsetValue*100);
    ui->checkBoxHeightProcEnableNormalization->setChecked(imageProp.bHeightEnableNormalization);

    ui->labelHeightProcMinValue             ->setText(QString::number(imageProp.heightMinValue));
    ui->labelHeightProcMaxValue             ->setText(QString::number(imageProp.heightMaxValue));

    ui->horizontalSliderConversionHNDepth           ->setValue(imageProp.conversionHNDepth*5);
    ui->doubleSpinBoxConversionHNDepth              ->setValue(imageProp.conversionHNDepth);

    ui->horizontalSliderNormalToHeightItersHuge     ->setValue(imageProp.conversionNHItersHuge);
    ui->horizontalSliderNormalToHeightItersVeryLarge->setValue(imageProp.conversionNHItersVeryLarge);
    ui->horizontalSliderNormalToHeightItersLarge    ->setValue(imageProp.conversionNHItersLarge);
    ui->horizontalSliderNormalToHeightItersMedium   ->setValue(imageProp.conversionNHItersMedium);
    ui->horizontalSliderNormalToHeightItersVerySmall->setValue(imageProp.conversionNHItersVerySmall);
    ui->horizontalSliderNormalToHeightItersSmall    ->setValue(imageProp.conversionNHItersSmall);

    // update conversion levels
    if(imageProp.imageType == DIFFUSE_TEXTURE){
        for(int i = 0; i < 4 ; i++){
            baseMapConvLevels[i]->updateSliders(imageProp.baseMapConvLevels[i]);
        }
    }
    ui->horizontalSliderBaseToOthersAngleCorrection   ->setValue(imageProp.baseMapAngleCorrection);
    ui->horizontalSliderBaseToOthersAngleWeight       ->setValue(imageProp.baseMapAngleWeight);

    ui->horizontalSliderBaseMapToOthersHRangeTolerance->setValue(imageProp.conversionBaseMapHeightMinMaxTolerance);


    ui->horizontalSliderSSAONoIters     ->setValue(imageProp.ssaoNoIters);
    ui->horizontalSliderSSAOBias        ->setValue(imageProp.ssaoBias*100);
    ui->horizontalSliderSSAOIntensity   ->setValue(imageProp.ssaoIntensity*100);
    ui->horizontalSliderSSAODepth       ->setValue(imageProp.ssaoDepth*100);

    ui->doubleSpinBoxSSAONoIters        ->setValue(imageProp.ssaoNoIters);
    ui->doubleSpinBoxSSAODepth          ->setValue(imageProp.ssaoDepth);
    ui->doubleSpinBoxSSAOBias           ->setValue(imageProp.ssaoBias);
    ui->doubleSpinBoxSSAOIntensity      ->setValue(imageProp.ssaoIntensity);


    ui->horizontalSliderAOCancelation   ->setValue(imageProp.aoCancellation*100);

    ui->horizontalSliderRemoveShadingLFBlending ->setValue(imageProp.removeShadingLFBlending*100);
    ui->horizontalSliderRemoveShadingLFRadius   ->setValue(imageProp.removeShadingLFRadius);

    ui->horizontalSliderColorHue        ->setValue(imageProp.colorHue*180);


    ui->horizontalSliderSelectiveBlurBlending->setValue(imageProp.selectiveBlurBlending*100);
    ui->horizontalSliderSelectiveBlurMaskRadius->setValue(imageProp.selectiveBlurMaskRadius);
    ui->horizontalSliderSelectiveBlurDOGRadius->setValue(imageProp.selectiveBlurDOGRadius);
    ui->horizontalSliderSelectiveBlurDOGContrast->setValue(imageProp.selectiveBlurDOGConstrast*100);
    ui->horizontalSliderSelectiveBlurDOGAmplifier->setValue(imageProp.selectiveBlurDOGAmplifier*10);
    ui->horizontalSliderSelectiveBlurDOGOffset->setValue(imageProp.selectiveBlurDOGOffset*255);

    ui->horizontalSliderSelectiveBlurMinValue      ->setValue(imageProp.selectiveBlurMinValue*255);
    ui->horizontalSliderSelectiveBlurMaxValue      ->setValue(imageProp.selectiveBlurMaxValue*255);
    ui->horizontalSliderSelectiveBlurOffset        ->setValue(imageProp.selectiveBlurOffsetValue*255);
    ui->horizontalSliderSelectiveBlurDetails       ->setValue(imageProp.selectiveBlurDetails);

    ui->horizontalSliderRoughnessDepth      ->setValue(imageProp.roughnessDepth*100.0);
    ui->horizontalSliderRoughnessTreshold   ->setValue(imageProp.roughnessTreshold*100.0);
    ui->horizontalSliderRoughnessAmplifier  ->setValue(imageProp.roughnessAmplifier*100.0);


    ui->comboBoxSelectiveBlurTypes->setCurrentIndex(imageProp.selectiveBlurType);    

    ui->checkBoxSelectiveBlurEnable->setChecked(imageProp.bSelectiveBlurEnable);
    ui->checkBoxSelectiveBlurInvertMask->setChecked(imageProp.bSelectiveBlurInvertMask);
    ui->pushButtonSelectiveBlurPreviewMask->setChecked(imageProp.bSelectiveBlurPreviewMask);

    ui->radioButtonRoughnessEnable->setChecked(imageProp.bRoughnessSurfaceEnable);
    ui->radioButtonEnableColorPicking->setChecked(imageProp.bRoughnessEnableColorPicking);
    if(!imageProp.bRoughnessSurfaceEnable && !imageProp.bRoughnessEnableColorPicking){
        ui->radioButtonRoughnessNoneEffect->setChecked(true);
        ui->groupBoxGeneral->setEnabled(true);
    }
    if(imageProp.bRoughnessEnableColorPicking) ui->groupBoxGeneral->setDisabled(true);

    imageProp.bRoughnessColorPickingToggled = false;

    QColor color(255*imageProp.pickedColor.x(),255*imageProp.pickedColor.y(),255*imageProp.pickedColor.z());
    QPalette palette = ui->labelRoughnessPickedColor->palette();
    palette.setColor(ui->labelRoughnessPickedColor->backgroundRole(), color);
    palette.setColor(ui->labelRoughnessPickedColor->foregroundRole(), color);
    ui->labelRoughnessPickedColor->setPalette(palette);


    ui->checkBoxRoughnessColorInvert->setChecked(imageProp.bRoughnessInvertColorMask);
    ui->horizontalSliderRoughnessColorOffset->setValue(imageProp.roughnessColorOffset*100);
    ui->horizontalSliderRoughnessColorGlobalOffset->setValue(imageProp.roughnessColorGlobalOffset*255);
    ui->horizontalSliderRoughnessColorAmplifier->setValue(imageProp.roughnessColorAmplifier*100);
    ui->horizontalSliderSelectiveBlurNoIters->setValue(imageProp.selectiveBlurNoIters);
    ui->comboBoxColorPickerMethod->setCurrentIndex(imageProp.colorPickerMethod);

    // grunge map
    ui->horizontalSliderGrungeImageWeight    ->setValue(imageProp.grungeImageWeight);
    ui->horizontalSliderGrungeMainImageWeight->setValue(imageProp.grungeMainImageWeight);
    ui->comboBoxGrungeBlendingMode->setCurrentIndex(imageProp.grungeBlendingMode);
    if(imageProp.imageType == GRUNGE_TEXTURE){

       // ui->horizontalSliderGrungeOverallWeight ->setValue(imageProp.grungeOverallWeight);
        ui->horizontalSliderGrungeRadius        ->setValue(imageProp.grungeRadius);
        ui->horizontalSliderGrungeSeed          ->setValue(imageProp.grungeSeed);
        ui->horizontalSliderGrungeNormalWarp    ->setValue(imageProp.grungeNormalWarp);
        ui->checkBoxGrungeRandomTranslations->setChecked(imageProp.bGrungeEnableRandomTranslations);
        ui->checkBoxGrungeReplotAllAfterChange->setChecked(imageProp.bGrungeReplotAllWhenChanged);

    }



    // input image case study
    switch(imageProp.imageType){
        case(NORMAL_TEXTURE):
        // select propper input image for normals
        ui->pushButtonConverToNormal->setEnabled(false);

        switch(imageProp.inputImageType){
            case(INPUT_FROM_NORMAL_INPUT):
                ui->comboBoxNormalInputImage->setCurrentIndex(0);
                ui->pushButtonConverToNormal->setEnabled(true);
                break;
            case(INPUT_FROM_HEIGHT_INPUT): ui->comboBoxNormalInputImage->setCurrentIndex(1); ; break;
            case(INPUT_FROM_HEIGHT_OUTPUT): ui->comboBoxNormalInputImage->setCurrentIndex(2); ; break;
            default: break;
        }
        break;
         // end of case NORMAL
        case(SPECULAR_TEXTURE):
        // select propper input image for specular

        switch(imageProp.inputImageType){
            case(INPUT_FROM_SPECULAR_INPUT): ui->comboBoxSpecularInputImage->setCurrentIndex(0); break;
            case(INPUT_FROM_DIFFUSE_INPUT) : ui->comboBoxSpecularInputImage->setCurrentIndex(0); break;
            case(INPUT_FROM_DIFFUSE_OUTPUT): ui->comboBoxSpecularInputImage->setCurrentIndex(1); break;
            case(INPUT_FROM_HEIGHT_INPUT)  : ui->comboBoxSpecularInputImage->setCurrentIndex(2); break;
            case(INPUT_FROM_HEIGHT_OUTPUT) : ui->comboBoxSpecularInputImage->setCurrentIndex(3); break;
            default: break;
        }
        break;
         // end of case SPECULAR
        case(OCCLUSION_TEXTURE):
        // select propper input image for occlusion
        ui->pushButtonConvertOcclusionFromHN->setEnabled(false);
        switch(imageProp.inputImageType){
            case(INPUT_FROM_OCCLUSION_INPUT):
                ui->comboBoxOcclusionInputImage->setCurrentIndex(0);
                ui->pushButtonConvertOcclusionFromHN->setEnabled(true);
                break;
            case(INPUT_FROM_HI_NI): ui->comboBoxOcclusionInputImage->setCurrentIndex(1); break;
            case(INPUT_FROM_HO_NO): ui->comboBoxOcclusionInputImage->setCurrentIndex(2); break;
            default: break;
        }
        break;
         // end of case OCCLUSION
        case(ROUGHNESS_TEXTURE):
        // select propper input image for roughness

        switch(imageProp.inputImageType){
            case(INPUT_FROM_ROUGHNESS_INPUT): ui->comboBoxRoughnessInputImage->setCurrentIndex(0);  break;
            case(INPUT_FROM_DIFFUSE_INPUT)  : ui->comboBoxRoughnessInputImage->setCurrentIndex(1);  break;
            case(INPUT_FROM_DIFFUSE_OUTPUT) : ui->comboBoxRoughnessInputImage->setCurrentIndex(2);  break;
            default: break;

        }
        break;
         // end of case ROUGHNESS
        case(METALLIC_TEXTURE):
        // select propper input image for roughness

        switch(imageProp.inputImageType){
            case(INPUT_FROM_METALLIC_INPUT): ui->comboBoxRoughnessInputImage->setCurrentIndex(0);  break;
            case(INPUT_FROM_DIFFUSE_INPUT) : ui->comboBoxRoughnessInputImage->setCurrentIndex(1);  break;
            case(INPUT_FROM_DIFFUSE_OUTPUT): ui->comboBoxRoughnessInputImage->setCurrentIndex(2);  break;
            default: break;
        }
        break;
         // end of case Metallic
        case(HEIGHT_TEXTURE):
        // select propper input image for roughness

        switch(imageProp.selectiveBlurMaskInputImageType){
            case(INPUT_FROM_HEIGHT_OUTPUT)   : ui->comboBoxMaskInputImage->setCurrentIndex(0);  break;
            case(INPUT_FROM_DIFFUSE_OUTPUT)  : ui->comboBoxMaskInputImage->setCurrentIndex(1);  break;
            case(INPUT_FROM_METALLIC_OUTPUT) : ui->comboBoxMaskInputImage->setCurrentIndex(2);  break;
            case(INPUT_FROM_ROUGHNESS_OUTPUT): ui->comboBoxMaskInputImage->setCurrentIndex(3);  break;
            default: break;
        }
        break;
         // end of case Metallic
        default:break; // do nothing
    };




    bLoading = false;

    // forcing gray scale for specular image
    if(imageProp.imageType == SPECULAR_TEXTURE){
        ui->checkBoxGrayScale->setChecked(true);
        imageProp.bGrayScale = true;
    }

    if(imageProp.imageType != HEIGHT_TEXTURE &&
       imageProp.imageType != NORMAL_TEXTURE &&
       imageProp.imageType != OCCLUSION_TEXTURE &&
       imageProp.imageType != ROUGHNESS_TEXTURE){

        // hue manipulation
        ui->labelHue->show();
        ui->labelHueValue->show();
        ui->horizontalSliderColorHue->show();
    }
    // Shading removal only for diffuse texture
    if(imageProp.imageType == DIFFUSE_TEXTURE ){
        ui->checkBoxRemoveShading->show();
    }

}

void FormImageProp::reloadImageSettings(){
    emit reloadSettingsFromConfigFile(imageProp.imageType);
}


void FormImageProp::pasteFromClipboard(){
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {
        qDebug() << "<FormImageProp> Image :"+
                    PostfixNames::getTextureName(imageProp.imageType)+
                    " loaded from clipboard.";
        QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
        QImage image = pixmap.toImage();
        pasteImageFromClipboard(image);

    }
}
void FormImageProp::copyToClipboard(){

    qDebug() << "<FormImageProp> Image :"+
                PostfixNames::getTextureName(imageProp.imageType)+
                " copied to clipboard.";

    QApplication::processEvents();
    image = imageProp.getImage();
    QApplication::clipboard()->setImage(image,QClipboard::Clipboard);
}

