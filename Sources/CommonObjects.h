#ifndef COMMONOBJECTS_H
#define COMMONOBJECTS_H

#include <QtOpenGL>
#include <QImage>
#include <cstdio>
#include <iostream>
#include "qopenglerrorcheck.h"
#include <QOpenGLFunctions_3_3_Core>

#define TAB_SETTINGS 9
#define TAB_TILING   10

#ifdef Q_OS_MAC
# define AB_INI "AwesomeBump.ini"
# define AB_LOG "AwesomeBump.log"
#else
# define AB_INI "config.ini"
# define AB_LOG "log.txt"
#endif



#define TEXTURE_FORMAT GL_RGB16F

#define KEY_SHOW_MATERIALS Qt::Key_S

//#define USE_OPENGL_330

#ifdef USE_OPENGL_330
#define AWESOME_BUMP_VERSION "AwesomeBump 4.0 Beta (2015) (openGL 330 release)"
#else
#define AWESOME_BUMP_VERSION "AwesomeBump 4.0 Beta (2015)"
#endif

using namespace std;

enum TextureTypes{
    DIFFUSE_TEXTURE = 0,
    NORMAL_TEXTURE ,
    SPECULAR_TEXTURE,
    HEIGHT_TEXTURE,
    OCCLUSION_TEXTURE,
    ROUGHNESS_TEXTURE,
    METALLIC_TEXTURE,
    MATERIAL_TEXTURE,
    GRUNGE_TEXTURE,
    MAX_TEXTURES_TYPE,
};

enum ConversionType{
    CONVERT_NONE = 0,
    CONVERT_FROM_H_TO_N,
    CONVERT_FROM_N_TO_H,
    CONVERT_FROM_D_TO_O, // diffuse to others
    CONVERT_FROM_HN_TO_OC,
    CONVERT_RESIZE
};

enum UVManipulationMethods{
    UV_TRANSLATE = 0,
    UV_GRAB_CORNERS,
    UV_SCALE_XY
};

enum ShadingType{
    SHADING_RELIEF_MAPPING = 0,
    SHADING_PARALLAX_NORMAL_MAPPING,
    SHADING_TESSELATION
};

enum ShadingModel{
    SHADING_MODEL_PBR = 0,
    SHADING_MODEL_BUMP_MAPPING
};
// Methods of making the texture seamless
enum SeamlessMode{
    SEAMLESS_NONE = 0,
    SEAMLESS_SIMPLE,
    SEAMLESS_MIRROR,
    SEAMLESS_RANDOM
};

// Compressed texture type
enum CompressedFromTypes{
    H_TO_D_AND_S_TO_N = 0,
    S_TO_D_AND_H_TO_N = 1
};

// Selective blur methods
enum SelectiveBlurType{
    SELECTIVE_BLUR_LEVELS = 0,
    SELECTIVE_BLUR_DIFFERENCE_OF_GAUSSIANS
};

enum TargaColorFormat{
    TARGA_BGR=0,
    TARGA_BGRA,
    TARGA_LUMINANCE
};

enum SourceImageType{
    INPUT_NONE = 0,
    INPUT_FROM_HEIGHT_INPUT,
    INPUT_FROM_HEIGHT_OUTPUT,
    INPUT_FROM_NORMAL_INPUT,
    INPUT_FROM_NORMAL_OUTPUT,
    INPUT_FROM_SPECULAR_INPUT,
    INPUT_FROM_SPECULAR_OUTPUT,
    INPUT_FROM_DIFFUSE_INPUT,
    INPUT_FROM_DIFFUSE_OUTPUT,
    INPUT_FROM_OCCLUSION_INPUT,
    INPUT_FROM_ROUGHNESS_INPUT,
    INPUT_FROM_ROUGHNESS_OUTPUT,
    INPUT_FROM_METALLIC_INPUT,
    INPUT_FROM_METALLIC_OUTPUT,
    INPUT_FROM_HI_NI,
    INPUT_FROM_HO_NO
};

enum ColorPickerMethod{
    COLOR_PICKER_METHOD_A = 0,
    COLOR_PICKER_METHOD_B ,
};

enum MaterialIndicesType{
    MATERIALS_DISABLED = -10,
    MATERIALS_ENABLED = -1
};



#define TARGA_HEADER_SIZE    0x12
#define TARGA_UNCOMP_RGB_IMG 0x02
#define TARGA_UNCOMP_BW_IMG  0x03

// Reading and writing to file TGA image
class TargaImage{
    public:
    // write QImage to tga file
    void write(QImage image, QString fileName);
    // return QImage from readed tga file
    QImage read(QString fileName);
    private:
    /**
     Read tga image to data.
     * @param filename - path to the image
     * @param width (output) width of the image
     * @param height (output) height of the image
     * @param format (output) format of the image (RGB,RGBA,LUMINANCE)
     * @param pixels (output) data of pixels
     * @return returns true if image was loaded.
     */
    bool load_targa (const char *filename, int &width, int &height,
                          TargaColorFormat &format, unsigned char *&pixels);
    // The same as above but write image to file
    bool save_targa (const char *filename, int width, int height,
                          TargaColorFormat format, unsigned char *pixels);


};

class PostfixNames{
public:
    static    QString   diffuseName;
    static    QString   normalName;
    static    QString   specularName;
    static    QString   heightName;
    static    QString   occlusionName;
    static    QString   roughnessName;
    static    QString   metallicName;
    static    QString   outputFormat;

    static QString getPostfix(TextureTypes tType){
        switch(tType){
            case(DIFFUSE_TEXTURE ):
                return diffuseName;
                break;
            case(NORMAL_TEXTURE  ):
                return normalName;
                break;
            case(SPECULAR_TEXTURE):
                return specularName;
                break;
            case(HEIGHT_TEXTURE  ):
                return heightName;
                break;
            case(OCCLUSION_TEXTURE  ):
                return occlusionName;
                break;
            case(ROUGHNESS_TEXTURE  ):
                return roughnessName;
                break;
            case(METALLIC_TEXTURE  ):
                return metallicName;
                break;

            default: return diffuseName;
        }
    }
    static QString getTextureName(TextureTypes tType){
        switch(tType){
            case(DIFFUSE_TEXTURE ):
                return "diffuse";
                break;
            case(NORMAL_TEXTURE  ):
                return "normal";
                break;
            case(SPECULAR_TEXTURE):
                return "specular";
                break;
            case(HEIGHT_TEXTURE  ):
                return "height";
                break;
            case(OCCLUSION_TEXTURE  ):
                return "occlusion";
                break;
            case(ROUGHNESS_TEXTURE  ):
                return "roughness";
                break;
            case(METALLIC_TEXTURE  ):
                return "metallic";
                break;
            case(MATERIAL_TEXTURE  ):
                return "material";
            case(GRUNGE_TEXTURE  ):
                return "grunge";
                break;
            default: return "default-diffuse";
        }
    }

};

struct GrayScalePreset{
    float R;
    float G;
    float B;
    int grayScaleMode;
    void mode1(){
        R = 1.0;
        G = 1.0;
        B = 1.0;        
        normalize();
        grayScaleMode = 0;
    }
    void mode2(){
        R = 0.3;
        G = 0.59;
        B = 0.11;
        normalize();
        grayScaleMode = 1;
    }
    void mode(){
        if(grayScaleMode == 0) mode1();
        if(grayScaleMode == 1) mode2();
    }
    QVector3D toQVector3D(){        
        return QVector3D(R,G,B);
    }
    void normalize(){
        float sum = R + G + B;
        if(fabs(sum) < 0.0001) sum = 1;
        R /= sum;
        G /= sum;
        B /= sum;
    }
};


struct RandomTilingMode{
  float angles[9];
  float common_phase;
  float inner_radius;
  float outer_radius;
  RandomTilingMode(){
      inner_radius = 0.2;
      outer_radius = 0.4;
      common_phase = 0.0;
      for(int i = 0; i < 9 ; i++){
          angles[i] = 0;
      }
  }
  // generate random angles
  void randomize(){
      static int seed = 312;
      qsrand(seed);
      seed = qrand() % 41211; // fake seed
      for(int i = 0; i < 9 ; i++){
           angles[i] = 2 * 3.1415269 * qrand() / (RAND_MAX + 0.0);
      }
  }
};

/**
 * All settings for the 3D widget
 */
class Display3DSettings{
public:

  float     depthScale;
  float     uvScale;
  QVector2D uvOffset;
  float     specularIntensity;
  float     diffuseIntensity;
  float     lightPower ;
  float     lightRadius;
  ShadingType  shadingType;
  ShadingModel shadingModel;


  // rendering quality settings
  bool bUseCullFace;
  bool bUseSimplePBR;
  int  noTessSubdivision;
  int  noPBRRays;
  bool bBloomEffect;
  bool bDofEffect;
  bool bShowTriangleEdges;
  bool bLensFlares;
  static float openGLVersion;
  Display3DSettings(){
        bUseCullFace  = false;
        bUseSimplePBR = false;
        noTessSubdivision  = 16;
        noPBRRays          = 15;
        bBloomEffect       = true;
        bDofEffect         = true;
        bShowTriangleEdges = false;
        bLensFlares        = true;
  }
};

// Wrapper for FBO initialization.
class FBOImages {
public:
    static void create(QGLFramebufferObject *&fbo,int width,int height,GLuint internal_format = TEXTURE_FORMAT){
       if(fbo)
       {
            fbo->release();
            delete fbo;
        }
        QGLFramebufferObjectFormat format;
        format.setInternalTextureFormat(internal_format);
        format.setTextureTarget(GL_TEXTURE_2D);
        format.setMipmap(true);
        fbo = new QGLFramebufferObject(width,height,format);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


        if(FBOImages::bUseLinearInterpolation){
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }else{
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        float aniso = 0.0;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
        GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
        qDebug() << "FBOImages::creating new FBO(" << width << "," << height << ") with id=" << fbo->texture() ;
    }
    static void resize(QGLFramebufferObject *&src,QGLFramebufferObject *&ref,GLuint internal_format = TEXTURE_FORMAT){
        if(src == NULL){
            GLCHK(FBOImages::create(src ,ref->width(),ref->height(),internal_format));
        }else if( ref->width()  == src->width() &&
            ref->height() == src->height() ){}else{
            GLCHK(FBOImages::create(src ,ref->width(),ref->height(),internal_format));
        }
    }
    static void resize(QGLFramebufferObject *&src,int width, int height,GLuint internal_format = TEXTURE_FORMAT){
        if(!src){
            GLCHK(FBOImages::create(src ,width,height,internal_format));
        }else if( width  == src->width() &&
            height == src->height() ){}else{
            GLCHK(FBOImages::create(src ,width,height,internal_format));
        }
    }
public:
    static bool bUseLinearInterpolation;

};


struct BaseMapConvLevelProperties{

    float conversionBaseMapAmplitude;
    float conversionBaseMapFlatness;
    int   conversionBaseMapNoIters;
    int   conversionBaseMapFilterRadius;
    float conversionBaseMapMixNormals;
    float conversionBaseMapPreSmoothRadius;
    float conversionBaseMapBlending;
    float conversionBaseMapWeight;

    BaseMapConvLevelProperties(){
        conversionBaseMapAmplitude      = 0;
        conversionBaseMapFlatness       = 0.5;
        conversionBaseMapNoIters        = 0;
        conversionBaseMapFilterRadius   = 3;
        conversionBaseMapMixNormals     = 1.0;
        conversionBaseMapPreSmoothRadius= 0;
        conversionBaseMapBlending       = 1.0;
        conversionBaseMapWeight         = 0.0;
    }

};

// Main object. Contains information about Image and the post process parameters
class FBOImageProporties{
public:
    bool bSkipProcessing;
    QGLFramebufferObject *fbo     ; // output image

    GLuint scr_tex_id;       // Id of texture loaded from image, from loaded file
    GLuint normalMixerInputTexId; // Used only by normal texture
    int scr_tex_width;       // width of the image loaded from file.
    int scr_tex_height;      // height ...
    QGLWidget* glWidget_ptr; // pointer to GL context
    TextureTypes imageType;  // This will define what kind of preprocessing will be applied to image



    // Variables used  to control the image processing (most of them are controlled from GUI)
    bool bFirstDraw;
    bool bGrayScale;
    GrayScalePreset grayScalePreset;

    bool bInvertR,bInvertG,bInvertB;
    bool bRemoveShading;

    // Specular settings
    int  noRemoveShadingGaussIter;
    float aoCancellation;
    float removeShadingLFBlending;
    float removeShadingLFRadius;

    int  noBlurPasses;
    bool bSpeclarControl;
    int  specularRadius;
    float specularW1,specularW2,specularContrast,specularAmplifier;
    float specularBrightness;
    // General processing
    float colorHue;
    float smallDetails;
    float mediumDetails;
    float detailDepth;
    int sharpenBlurAmount;
    float normalsStep;
    // Conversion settings
    float conversionHNDepth;
    bool bConversionHN;
    // Normal to Height conversion settings
    bool bConversionNH;
    int  conversionNHItersHuge;
    int  conversionNHItersVeryLarge;
    int  conversionNHItersLarge;
    int  conversionNHItersMedium;
    int  conversionNHItersSmall;
    int  conversionNHItersVerySmall;
    // Base to others settings
    static bool bConversionBaseMap;
    static bool bConversionBaseMapShowHeightTexture;
    float baseMapAngleCorrection;
    float baseMapAngleWeight;
    BaseMapConvLevelProperties baseMapConvLevels[4];
    // Base map height levels variables
    QVector3D conversionBaseMapheightMin; // User defined color of height=0
    QVector3D conversionBaseMapheightMax; // User defined color of height=1 if one compoment is < 0 there will be no action applied
    float conversionBaseMapHeightMinMaxTolerance; // defines the smoothnes of the filter

    // ambient occlusion settings
    int ssaoNoIters;
    float ssaoIntensity;
    float ssaoBias;
    float ssaoDepth;

    // height settings
    float heightMinValue;
    float heightMaxValue;
    int   heightAveragingRadius;
    float heightOffsetValue;
    bool  bHeightEnableNormalization;


    // selective blur variables
    SelectiveBlurType selectiveBlurType;
    bool bSelectiveBlurPreviewMask;
    bool bSelectiveBlurInvertMask;
    bool bSelectiveBlurEnable;

    float selectiveBlurBlending;
    int   selectiveBlurMaskRadius;
    int selectiveBlurDOGRadius;
    float selectiveBlurDOGConstrast;
    float selectiveBlurDOGAmplifier;
    float selectiveBlurDOGOffset;

    float selectiveBlurMinValue;
    float selectiveBlurMaxValue;
    int   selectiveBlurDetails;
    float selectiveBlurOffsetValue;

    // Input image type
    SourceImageType inputImageType;

    // roughness settings
    float roughnessDepth;
    float roughnessTreshold;
    float roughnessAmplifier;
    bool bRoughnessSurfaceEnable;

    bool bRoughnessEnableColorPicking;
    bool bRoughnessColorPickingToggled;
    QVector3D pickedColor;
    ColorPickerMethod colorPickerMethod;
    bool bRoughnessInvertColorMask;
    float roughnessColorOffset;
    float roughnessColorAmplifier;
    SourceImageType selectiveBlurMaskInputImageType;

    int selectiveBlurNoIters;
    float roughnessColorGlobalOffset;

    // normal map mixer
    bool bNormalMixerEnabled;
    bool bNormalMixerApplyNormals;
    float normalMixerDepth;
    float normalMixerScale;
    float normalMixerAngle;
    float normalMixerPosX;
    float normalMixerPosY;

    // grunge settings
    static float grungeOverallWeight; // general weight for all images
    static int   grungeSeed;          // grunge randomization seed (if = 0 no randomization)
    static float grungeRadius;        // random radius
    static bool bGrungeReplotAllWhenChanged;
    static bool bGrungeEnableRandomTranslations;
    static float grungeNormalWarp; // warp grunge image according to normal texture
    float grungeImageWeight;   // per image additional weight
    float grungeMainImageWeight; // used for normal blending only
    unsigned int grungeBlendingMode;
    // global settings seamless parameters

    static SeamlessMode seamlessMode;
    static float seamlessSimpleModeRadius;
    static int seamlessMirroModeType; // values: 2 - x repear, 1 - y  repeat, 0 - xy  repeat
    static RandomTilingMode seamlessRandomTiling;
    static float seamlessContrastStrenght;
    static float seamlessContrastPower;
    static int seamlessSimpleModeDirection;
    static SourceImageType seamlessContrastInputType;
    static bool bSeamlessTranslationsFirst;

    static map<QString,int> materialIndices;
    static int currentMaterialIndeks;



     FBOImageProporties(){
        bSkipProcessing = false;
        //ref_fbo      = NULL;
        fbo          = NULL;
        //aux_fbo      = NULL;
        //aux2_fbo     = NULL;
        normalMixerInputTexId = 0;
        glWidget_ptr = NULL;
        bFirstDraw   = true;
        bGrayScale   = false;
        grayScalePreset.mode2();
        bInvertR = bInvertG = bInvertB = false;

        scr_tex_id     = 0;
        bRemoveShading = false;
        noRemoveShadingGaussIter = 10;
        aoCancellation   = 0.0;

        removeShadingLFBlending = 0.0;
        removeShadingLFRadius   = 0.0;

        bSpeclarControl    = false;
        colorHue           = 0;
        specularRadius     = 10;
        specularW1         = 0.1;
        specularW2         = 10.0;
        specularContrast   = 0.05;
        specularAmplifier  = 3.0;
        specularBrightness = 0.0;
        noBlurPasses       = 0;
        smallDetails       = 0;
        mediumDetails      = 0;
        detailDepth        = 2.0;
        sharpenBlurAmount  = 0;
        normalsStep        = 0.0;

        conversionHNDepth  = 2.0;
        bConversionHN      = false;
        bConversionNH      = false;

        conversionNHItersHuge       = 10;
        conversionNHItersVeryLarge  = 10;
        conversionNHItersLarge      = 10;
        conversionNHItersMedium     = 10;
        conversionNHItersSmall      = 10;
        conversionNHItersVerySmall  = 10;

        bConversionBaseMap          = false;
        baseMapAngleCorrection      = 0.0;
        baseMapAngleWeight          = 0.0;

        conversionBaseMapheightMin             = QVector3D(-1,0,0);
        conversionBaseMapheightMax             = QVector3D(-1,0,0);
        conversionBaseMapHeightMinMaxTolerance =  0;

        ssaoNoIters   = 4;
        ssaoIntensity = 1.0;
        ssaoBias      = 0.0;
        ssaoDepth     = 0.1;


        heightMinValue        = 0.0;
        heightMaxValue        = 1.0;
        heightAveragingRadius = 1;
        heightOffsetValue     = 0.0;
        bHeightEnableNormalization = true;

        // selective blur variables
        selectiveBlurType = SELECTIVE_BLUR_DIFFERENCE_OF_GAUSSIANS;
        bSelectiveBlurPreviewMask = false;
        bSelectiveBlurInvertMask  = false;
        bSelectiveBlurEnable      = false;
        selectiveBlurBlending     = 0.0;
        selectiveBlurMaskRadius   = 5 ;
        selectiveBlurDOGRadius    = 5;
        selectiveBlurDOGConstrast = 0.05;
        selectiveBlurDOGAmplifier = 3.0;
        selectiveBlurDOGOffset    = 0.0;

        selectiveBlurMinValue        = 0.0;
        selectiveBlurMaxValue        = 1.0;
        selectiveBlurDetails         = 1.0;
        selectiveBlurOffsetValue     = 0.0;


        inputImageType = INPUT_NONE;

        roughnessDepth = 0;
        roughnessTreshold = 0.0;
        roughnessAmplifier = 0.0;
        bRoughnessSurfaceEnable = false;


        bRoughnessEnableColorPicking  = false;
        bRoughnessColorPickingToggled = false;
        pickedColor                   = QVector3D(1.0,1.0,1.0);
        colorPickerMethod             = COLOR_PICKER_METHOD_A;

        bRoughnessInvertColorMask     = false;
        roughnessColorOffset          = 0.0;
        roughnessColorGlobalOffset    = 0.0;
        roughnessColorAmplifier       = 0.0;
        selectiveBlurMaskInputImageType = INPUT_FROM_HEIGHT_OUTPUT;
        selectiveBlurNoIters            = 1;

        seamlessMode   = SEAMLESS_NONE;



        // normal map mixer
        bNormalMixerEnabled      = false;
        bNormalMixerApplyNormals = false;
        normalMixerDepth         = 0.0;

        normalMixerScale = 1.0;
        normalMixerAngle = 0.0;
        normalMixerPosX  = 0.0;
        normalMixerPosY  = 0.0;

        // grunge settings
        grungeImageWeight   = 0.0;
        grungeBlendingMode  = 0;
        grungeMainImageWeight = 0.0;


     }

     void copySettings(FBOImageProporties &src){

        bFirstDraw   = src.bFirstDraw;
        bGrayScale   = src.bGrayScale;
        grayScalePreset = src.grayScalePreset;
        bInvertR = src.bInvertR;
        bInvertG = src.bInvertG;
        bInvertB = src.bInvertB;


        bRemoveShading = src.bRemoveShading;
        noRemoveShadingGaussIter = src.noRemoveShadingGaussIter;
        aoCancellation   = src.aoCancellation;

        removeShadingLFBlending = src.removeShadingLFBlending;
        removeShadingLFRadius   = src.removeShadingLFRadius;

        bSpeclarControl    = src. bSpeclarControl;
        colorHue           = src. colorHue;
        specularRadius     = src. specularRadius;
        specularW1         = src. specularW1;
        specularW2         = src. specularW2;
        specularContrast   = src. specularContrast;
        specularAmplifier  = src. specularAmplifier;
        specularBrightness = src. specularBrightness;
        noBlurPasses       = src. noBlurPasses;
        smallDetails       = src. smallDetails;
        mediumDetails      = src. mediumDetails;
        detailDepth        = src. detailDepth;
        sharpenBlurAmount  = src. sharpenBlurAmount;
        normalsStep        = src. normalsStep;

        conversionHNDepth  = src. conversionHNDepth;
        bConversionHN      = src. bConversionHN;
        bConversionNH      = src. bConversionNH;

        conversionNHItersHuge       = src.conversionNHItersHuge;
        conversionNHItersVeryLarge  = src.conversionNHItersVeryLarge;
        conversionNHItersLarge      = src.conversionNHItersLarge;
        conversionNHItersMedium     = src.conversionNHItersMedium;
        conversionNHItersSmall      = src.conversionNHItersSmall;
        conversionNHItersVerySmall  = src.conversionNHItersVerySmall;
        bConversionBaseMap          = src.bConversionBaseMap;
        baseMapAngleCorrection      = src.baseMapAngleCorrection;
        baseMapAngleWeight          = src.baseMapAngleWeight;


        conversionBaseMapheightMin             = src.conversionBaseMapheightMin;
        conversionBaseMapheightMax             = src.conversionBaseMapheightMax;
        conversionBaseMapHeightMinMaxTolerance = src.conversionBaseMapHeightMinMaxTolerance;

        ssaoNoIters   = src.ssaoNoIters;
        ssaoIntensity = src.ssaoIntensity;
        ssaoBias      = src.ssaoBias;
        ssaoDepth     = src.ssaoDepth;


        heightMinValue        = src.heightMinValue;
        heightMaxValue        = src.heightMaxValue;
        heightAveragingRadius = src.heightAveragingRadius;
        heightOffsetValue     = src.heightOffsetValue;

        // selective blur variables
        selectiveBlurType = src.selectiveBlurType;
        bSelectiveBlurPreviewMask = src.bSelectiveBlurPreviewMask;
        bSelectiveBlurInvertMask  = src.bSelectiveBlurInvertMask;
        bSelectiveBlurEnable      = src.bSelectiveBlurEnable;
        selectiveBlurBlending     = src.selectiveBlurBlending;
        selectiveBlurMaskRadius   = src.selectiveBlurMaskRadius ;
        selectiveBlurDOGRadius    = src.selectiveBlurDOGRadius;
        selectiveBlurDOGConstrast = src.selectiveBlurDOGConstrast;
        selectiveBlurDOGAmplifier = src.selectiveBlurDOGAmplifier;
        selectiveBlurDOGOffset    = src.selectiveBlurDOGOffset;

        selectiveBlurMinValue        = src.selectiveBlurMinValue;
        selectiveBlurMaxValue        = src.selectiveBlurMaxValue;
        selectiveBlurDetails         = src.selectiveBlurDetails;
        selectiveBlurOffsetValue     = src.selectiveBlurOffsetValue;


        inputImageType = src.inputImageType;

        roughnessDepth          = src.roughnessDepth;
        roughnessTreshold       = src.roughnessTreshold;
        roughnessAmplifier      = src.roughnessAmplifier;
        bRoughnessSurfaceEnable = src.bRoughnessSurfaceEnable;


        bRoughnessEnableColorPicking    = src.bRoughnessEnableColorPicking;
        bRoughnessColorPickingToggled   = src.bRoughnessColorPickingToggled;
        pickedColor                     = src.pickedColor;
        colorPickerMethod               = src.colorPickerMethod;

        bRoughnessInvertColorMask       = src.bRoughnessInvertColorMask;
        roughnessColorOffset            = src.roughnessColorOffset;
        roughnessColorGlobalOffset      = src.roughnessColorGlobalOffset;
        roughnessColorAmplifier         = src.roughnessColorAmplifier;
        selectiveBlurMaskInputImageType = src.selectiveBlurMaskInputImageType;
        selectiveBlurNoIters            = src.selectiveBlurNoIters;

        // normal map mixer
        bNormalMixerEnabled      = src.bNormalMixerEnabled;
        bNormalMixerApplyNormals = src.bNormalMixerApplyNormals;
        normalMixerDepth         = src.normalMixerDepth;

        normalMixerScale = src.normalMixerScale;
        normalMixerAngle = src.normalMixerAngle;
        normalMixerPosX  = src.normalMixerPosX;
        normalMixerPosY  = src.normalMixerPosY;

        // grunge
        grungeOverallWeight             = src.grungeOverallWeight;      // general weight for all images
        grungeSeed                      = src.grungeSeed;               // grunge randomization seed (if = 0 no randomization)
        grungeRadius                    = src.grungeRadius;             // random radius
        bGrungeReplotAllWhenChanged     = src.bGrungeReplotAllWhenChanged;
        bGrungeEnableRandomTranslations = src.bGrungeEnableRandomTranslations;
        grungeNormalWarp                = src.grungeNormalWarp;         // warp grunge image according to normal texture
        grungeImageWeight               = src.grungeImageWeight;        // per image additional weight
        grungeMainImageWeight           = src.grungeMainImageWeight;    // used for normal blending only
        grungeBlendingMode              = src.grungeBlendingMode;

     }

    void init(QImage& image){
        qDebug() << Q_FUNC_INFO;

        glWidget_ptr->makeCurrent();
        if(glIsTexture(scr_tex_id)) glWidget_ptr->deleteTexture(scr_tex_id);
        scr_tex_id = glWidget_ptr->bindTexture(image,GL_TEXTURE_2D);
        scr_tex_width  = image.width();
        scr_tex_height = image.height();
        bFirstDraw    = true;

        /*
        switch(imageType){
           case(HEIGHT_TEXTURE):
           case(OCCLUSION_TEXTURE):
                GLCHK(FBOImages::create(fbo     ,image.width(),image.height(),GL_R16F));
                break;
           default:
                GLCHK(FBOImages::create(fbo     ,image.width(),image.height()));
                break;
        }
        */
        GLCHK(FBOImages::create(fbo , image.width(), image.height()));

    }

    void updateSrcTexId(QGLFramebufferObject* in_ref_fbo){
        glWidget_ptr->makeCurrent();
        if(glIsTexture(scr_tex_id)) glWidget_ptr->deleteTexture(scr_tex_id);
        QImage image = in_ref_fbo->toImage();
        scr_tex_id   = glWidget_ptr->bindTexture(image,GL_TEXTURE_2D);

    }

    void resizeFBO(int width, int height){

        GLCHK(FBOImages::resize(fbo     ,width,height));
       // double memUsage = width * height * 4 * 16 / (1024.0*1024.0*8);
       // qDebug() << "Resized image memory usage:" << memUsage << "[MB]";
        bFirstDraw = true;
    }

    /**
     * @brief getImage convert FBO image to QImage
     * @return QImage
     */
     QImage getImage(){
        glWidget_ptr->makeCurrent();
        return fbo->toImage();
    }

    ~FBOImageProporties(){

        if(glWidget_ptr != NULL){
            qDebug() << Q_FUNC_INFO;
            glWidget_ptr->makeCurrent();

            if(glIsTexture(normalMixerInputTexId)) glWidget_ptr->deleteTexture(normalMixerInputTexId);
            if(glIsTexture(scr_tex_id)) GLCHK(glWidget_ptr->deleteTexture(scr_tex_id));
            normalMixerInputTexId = 0;
            scr_tex_id = 0;
            glWidget_ptr = NULL;
            if(fbo        != NULL ) delete fbo;
        }
    }
};



#endif // COMMONOBJECTS_H

