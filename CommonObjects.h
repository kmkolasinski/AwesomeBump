#ifndef COMMONOBJECTS_H
#define COMMONOBJECTS_H
#include <QtOpenGL>
#include <QImage>
#include <cstdio>
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

enum TextureTypes{
    DIFFUSE_TEXTURE = 0,
    NORMAL_TEXTURE ,
    SPECULAR_TEXTURE,
    HEIGHT_TEXTURE,
    OCCLUSION_TEXTURE
};

enum ConversionType{
    CONVERT_NONE = 0,
    CONVERT_FROM_H_TO_N,
    CONVERT_FROM_N_TO_H,
    CONVERT_FROM_D_TO_O // diffuse to others
};

// Compressed texture type
enum CompressedFromTypes{
    H_TO_D_AND_S_TO_N = 0,
    S_TO_D_AND_H_TO_N = 1
};
enum TargaColorFormat{
    TARGA_BGR=0,
    TARGA_BGRA,
    TARGA_LUMINANCE
};
#define TARGA_HEADER_SIZE 0x12
#define TARGA_UNCOMP_RGB_IMG 0x02
#define TARGA_UNCOMP_BW_IMG 0x03
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
            default: return diffuseName;
        }
    }
};

// Wrapper for FBO initialization.
class FBOImages{
public:
    static void create(QGLFramebufferObject *&fbo,int width,int height){
        if(fbo !=NULL ){
            fbo->release();
            delete fbo;
        }
        QGLFramebufferObjectFormat format;
        format.setInternalTextureFormat(GL_RGBA16F);
        format.setTextureTarget(GL_TEXTURE_2D);
        format.setMipmap(true);
        fbo = new QGLFramebufferObject(width,height,format);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        float aniso = 0.0;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
        glBindTexture(GL_TEXTURE_2D, 0);
        qDebug() << "FBOImages::creatig new FBO(" << width << "," << height << ") with id=" << fbo->texture() ;
    }
    static void resize(QGLFramebufferObject *&src,QGLFramebufferObject *&ref){
        if( ref->width()  == src->width() &&
            ref->height() == src->height() ){}else{
            FBOImages::create(src ,ref->width(),ref->height());
        }
    }

};

// Main object. Contains information about Image and the post process parameters
class FBOImageProporties{
public:
    QGLFramebufferObject *ref_fbo ; // reference image
    QGLFramebufferObject *fbo     ; // output image
    QGLFramebufferObject *aux_fbo ; // aux image (used in post processing)
    QGLFramebufferObject *aux2_fbo; // the same

    GLuint scr_tex_id;       // Id of texture loaded from image, from loaded file
    QGLWidget* glWidget_ptr; // pointer to GL context
    TextureTypes imageType;  // This will define what kind of preprocessing will be applied to image

    // Variables used  to control the image processing (most of them are controlled from GUI)
    bool bFirstDraw;
    bool bGrayScale;
    bool bInvertR,bInvertG,bInvertB;
    bool bRemoveShading;

    // Specular settings
    int  noRemoveShadingGaussIter;
    int  noBlurPasses;
    bool bSpeclarControl;
    int  specularRadius;
    float specularW1,specularW2,specularContrast,specularAmplifier;
    // General processing
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
    bool bConversionBaseMap;
    float conversionBaseMapAmplitude;
    float conversionBaseMapFlatness;
    int   conversionBaseMapNoIters;
    int   conversionBaseMapFilterRadius;
    float conversionBaseMapMixNormals;
    float conversionBaseMapPreSmoothRadius;
    float conversionBaseMapBlending;

    // ambient occlusion settings
    int ssaoNoIters;
    float ssaoIntensity;
    float ssaoBias;
    float ssaoDepth;

    // global settings
    static bool bMakeSeamless;
    static float MakeSeamlessRadius;

     FBOImageProporties(){

        ref_fbo      = NULL;
        fbo          = NULL;
        aux_fbo      = NULL;
        aux2_fbo     = NULL;
        glWidget_ptr = NULL;
        bFirstDraw    = true;
        bGrayScale   = false;
        bInvertR = bInvertG = bInvertB = false;

        scr_tex_id   = 0;
        bRemoveShading = false;
        noRemoveShadingGaussIter = 10;

        bSpeclarControl = false;
        specularRadius = 10;
        specularW1     = 0.1;
        specularW2     = 10.0;
        specularContrast = 0.05;
        specularAmplifier = 3.0;
        noBlurPasses = 0;        
        smallDetails = 0;
        mediumDetails = 0;
        detailDepth  = 2.0;
        sharpenBlurAmount = 0;
        normalsStep = 0.0;

        conversionHNDepth  = 2.0;
        bConversionHN      = false;
        bConversionNH      = false;

        conversionNHItersHuge = 10;
        conversionNHItersVeryLarge = 10;
        conversionNHItersLarge = 10;
        conversionNHItersMedium = 10;
        conversionNHItersSmall = 10;
        conversionNHItersVerySmall = 10;

        bConversionBaseMap = false;
        conversionBaseMapAmplitude = 0;
        conversionBaseMapFlatness = 0.5;
        conversionBaseMapNoIters = 0;
        conversionBaseMapFilterRadius = 3;
        conversionBaseMapMixNormals = 1.0;
        conversionBaseMapPreSmoothRadius = 0;
        conversionBaseMapBlending  = 1.0;


        ssaoNoIters   = 4;

        ssaoIntensity = 1.0;
        ssaoBias      = 0.0;
        ssaoDepth     = 0.1;

     }
    void init(QImage& image){
        qDebug() << "<FBOImageProporties> init.";
        glWidget_ptr->makeCurrent();
        if(glIsTexture(scr_tex_id)) glWidget_ptr->deleteTexture(scr_tex_id);
        scr_tex_id = glWidget_ptr->bindTexture(image,GL_TEXTURE_2D);

        FBOImages::create(ref_fbo ,image.width(),image.height());
        bFirstDraw    = true;

        FBOImages::create(fbo     ,image.width(),image.height());
        FBOImages::create(aux_fbo ,image.width(),image.height());
        FBOImages::create(aux2_fbo,image.width(),image.height());

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
        qDebug() << "<FBOImageProporties> delete.";
        if(glIsTexture(scr_tex_id)) glWidget_ptr->deleteTexture(scr_tex_id);
        glWidget_ptr = NULL;
        if(ref_fbo    != NULL ) delete ref_fbo;
        if(fbo        != NULL ) delete fbo;
        if(aux_fbo    != NULL ) delete aux_fbo;
        if(aux2_fbo   != NULL ) delete aux2_fbo;
    }
};



#endif // COMMONOBJECTS_H

