#ifndef COMMONOBJECTS_H
#define COMMONOBJECTS_H

#include <QtOpenGL>
#include <QImage>
#include <cstdio>
#include <iostream>
#include "qopenglerrorcheck.h"
#include <QOpenGLFunctions_3_3_Core>
#include "properties/ImageProperties.peg.h"
#define TAB_SETTINGS 9
#define TAB_TILING   10

#ifdef Q_OS_MAC
# define AB_INI "AwesomeBump.ini"
# define AB_LOG "AwesomeBump.log"
#else
# define AB_INI "config.ini"
# define AB_LOG "log.txt"
#endif



//#define TEXTURE_FORMAT GL_RGB16F
#define TEXTURE_FORMAT GL_RGB16F
#define TEXTURE_3DRENDER_FORMAT GL_RGB16F

#define KEY_SHOW_MATERIALS Qt::Key_S

//#define USE_OPENGL_330

#ifdef USE_OPENGL_330
#define AWESOME_BUMP_VERSION "AwesomeBump " VERSION_STRING " (2019) (openGL 330 release)"
#else
#define AWESOME_BUMP_VERSION "AwesomeBump " VERSION_STRING " (2019)"
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
    MAX_TEXTURES_TYPE
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

      depthScale = 0.1;
      uvScale    = 1.0;
      uvOffset   = QVector2D(0.0,0.0);
      specularIntensity = 1.0;
      diffuseIntensity  = 1.0;
      lightPower        = 0.1;
      lightRadius       = 0.1;
      shadingType       = SHADING_RELIEF_MAPPING;
      shadingModel      = SHADING_MODEL_PBR;

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


        fbo = new QGLFramebufferObject(width, height, format);

        GLCHK(glBindTexture(GL_TEXTURE_2D, fbo->texture()));
        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

        if(FBOImages::bUseLinearInterpolation){
            GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        }else{
            GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
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
        }else if( width  == src->width() && height == src->height() ){
        }else{
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
    float conversionBaseMapFilterRadius;
    float conversionBaseMapMixNormals;
    float conversionBaseMapPreSmoothRadius;
    float conversionBaseMapBlending;


    BaseMapConvLevelProperties(){
        conversionBaseMapAmplitude      = 0;
        conversionBaseMapFlatness       = 0.5;
        conversionBaseMapNoIters        = 0;
        conversionBaseMapFilterRadius   = 3;
        conversionBaseMapMixNormals     = 1.0;
        conversionBaseMapPreSmoothRadius= 0;
        conversionBaseMapBlending       = 1.0;

    }
    void fromProperty(QtnPropertySetConvertsionBaseMapLevelProperty& level){
        conversionBaseMapAmplitude      = level.Amplitude;
        conversionBaseMapFlatness       = level.Flatness;
        conversionBaseMapNoIters        = level.NumIters;
        conversionBaseMapFilterRadius   = level.FilterRadius;
        conversionBaseMapMixNormals     = level.Edges;
        conversionBaseMapPreSmoothRadius= level.PreSmoothRadius;
        conversionBaseMapBlending       = level.Blending;

    }

};

// Main object. Contains information about Image and the post process parameters
class FBOImageProporties{
public:
    QtnPropertySetFormImageProp* properties;
    bool bSkipProcessing;
    QGLFramebufferObject *fbo     ; // output image

    GLuint scr_tex_id;       // Id of texture loaded from image, from loaded file
    GLuint normalMixerInputTexId; // Used only by normal texture
    int scr_tex_width;       // width of the image loaded from file.
    int scr_tex_height;      // height ...
    QGLWidget* glWidget_ptr; // pointer to GL context
    TextureTypes imageType;  // This will define what kind of preprocessing will be applied to image


    bool bFirstDraw;
    // Conversion settings
    float conversionHNDepth;
    // Base to others settings
    static bool bConversionBaseMap;
    static bool bConversionBaseMapShowHeightTexture;
    BaseMapConvLevelProperties baseMapConvLevels[4];
    // Input image type
    SourceImageType inputImageType;


    static SeamlessMode seamlessMode;
    static float seamlessSimpleModeRadius;
    static int seamlessMirroModeType; // values: 2 - x repear, 1 - y  repeat, 0 - xy  repeat
    static RandomTilingMode seamlessRandomTiling;
    static float seamlessContrastStrenght;
    static float seamlessContrastPower;
    static int seamlessSimpleModeDirection;
    static SourceImageType seamlessContrastInputType;
    static bool bSeamlessTranslationsFirst;
    static int currentMaterialIndeks;


     FBOImageProporties(){
        bSkipProcessing = false;
        properties      = NULL;
        fbo             = NULL;
        normalMixerInputTexId = 0;
        glWidget_ptr = NULL;
        bFirstDraw   = true;
        scr_tex_id   = 0;
        conversionHNDepth  = 2.0;
        bConversionBaseMap = false;
        inputImageType = INPUT_NONE;
        seamlessMode   = SEAMLESS_NONE;
        properties     = new QtnPropertySetFormImageProp;
     }

     void copySettings(FBOImageProporties &src){

        bFirstDraw         = src.bFirstDraw;
        conversionHNDepth  = src.conversionHNDepth;
        bConversionBaseMap = src.bConversionBaseMap;
        inputImageType     = src.inputImageType;

        if(properties != NULL && src.properties != NULL ) properties->copyValues(src.properties);
     }

    void init(QImage& image){
        qDebug() << Q_FUNC_INFO;

        if(!glWidget_ptr->isValid()){
            qDebug() << "Incorrect Widget pointer. Cannot initialize textures.";
        }

        GLCHK(glWidget_ptr->makeCurrent());
        if(glIsTexture(scr_tex_id))
            GLCHK(glWidget_ptr->deleteTexture(scr_tex_id));

        scr_tex_id = bindImageAsTexture(image);
        GLCHK(glBindTexture(GL_TEXTURE_2D, 0));

        scr_tex_width  = image.width();
        scr_tex_height = image.height();
        bFirstDraw = true;
        qDebug() << "Bind image texture with id: " << scr_tex_id << " w =" << scr_tex_width << " h = " << scr_tex_height;

        GLuint internal_format = TEXTURE_FORMAT;
        if(imageType == HEIGHT_TEXTURE) internal_format = TEXTURE_3DRENDER_FORMAT;
        GLCHK(FBOImages::create(fbo , image.width(), image.height(), internal_format));
    }

    void updateSrcTexId(QGLFramebufferObject* in_ref_fbo){
        glWidget_ptr->makeCurrent();
        if(glIsTexture(scr_tex_id)) glWidget_ptr->deleteTexture(scr_tex_id);
        QImage image = in_ref_fbo->toImage();
        scr_tex_id = bindImageAsTexture(image);
    }

    void resizeFBO(int width, int height){

        GLuint internal_format = TEXTURE_FORMAT;
        if(imageType == HEIGHT_TEXTURE) internal_format = TEXTURE_3DRENDER_FORMAT;
        GLCHK(FBOImages::resize(fbo,width,height,internal_format));
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

            if(glIsTexture(normalMixerInputTexId))
                GLCHK(glWidget_ptr->deleteTexture(normalMixerInputTexId));
            if(glIsTexture(scr_tex_id))
                GLCHK(glWidget_ptr->deleteTexture(scr_tex_id));

            normalMixerInputTexId = 0;
            scr_tex_id = 0;
            glWidget_ptr = NULL;            
            if(properties != NULL ) delete properties;
            if(fbo        != NULL ) delete fbo;
            properties = NULL;
            fbo        = NULL;
        }
    }

    static int bindImageAsTexture(QImage image){

        if (image.isNull()) {
            qDebug() << "bindTexture::Cannot create texture for empty image.";
            return NULL;
        }
        image = image.convertToFormat(QImage::Format_ARGB32);
//        QTransform flip_transform;
//        flip_transform.rotate(180);
//        flip_transform.transposed();
//        image = image.transformed(flip_transform);
        image = image.mirrored();

        GLuint texture_id; // get id of new texture
        GLCHK(glGenTextures(1, &texture_id));
        GLCHK(glBindTexture(GL_TEXTURE_2D, texture_id));

        GLCHK(glTexImage2D(
                  GL_TEXTURE_2D, 0,
                  GL_RGBA, image.width(), image.height(), 0,
                  GL_BGRA, GL_UNSIGNED_BYTE, image.bits())
              );

        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
        return texture_id;
    }

};



#endif // COMMONOBJECTS_H

