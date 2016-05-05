#include "CommonObjects.h"


SeamlessMode FBOImageProporties::seamlessMode             = SEAMLESS_NONE;
float FBOImageProporties::seamlessSimpleModeRadius        = 0.5;
float FBOImageProporties::seamlessContrastPower           = 0.0;
float FBOImageProporties::seamlessContrastStrenght        = 0.0;
int FBOImageProporties::seamlessSimpleModeDirection       = 0; // xy
SourceImageType FBOImageProporties::seamlessContrastInputType = INPUT_FROM_HEIGHT_INPUT;
bool FBOImageProporties::bSeamlessTranslationsFirst       = true;
int FBOImageProporties::seamlessMirroModeType             = 0;
bool FBOImageProporties::bConversionBaseMap = false;
bool FBOImageProporties::bConversionBaseMapShowHeightTexture = false;

int FBOImageProporties::currentMaterialIndeks = MATERIALS_DISABLED;
RandomTilingMode FBOImageProporties::seamlessRandomTiling = RandomTilingMode();

float Display3DSettings::openGLVersion = 3.3;

QString  PostfixNames::diffuseName    = "_d";
QString  PostfixNames::normalName     = "_n";
QString  PostfixNames::specularName   = "_s";
QString  PostfixNames::heightName     = "_h";
QString  PostfixNames::occlusionName  = "_o";
QString  PostfixNames::roughnessName  = "_r";
QString  PostfixNames::metallicName   = "_m";

QString  PostfixNames::outputFormat  = ".png";

bool FBOImages::bUseLinearInterpolation = true;

void TargaImage::write(QImage image, QString fileName){


    TargaColorFormat format = TARGA_BGRA;
    unsigned char   *pixels;
    int              width,height;

    width  = image.width();
    height = image.height();
    pixels = new unsigned char [width * height * 4];

    unsigned char* imageBuffer  = image.bits();
    int w = width;
    int h = height;

    for(int i = 0; i < height; i++)
       for(int j = 0; j < width; j++)
        {
          pixels[4 * ((h-i-1) * w + j) + 0] = imageBuffer[4 * (i * w + j)+0  ];
          pixels[4 * ((h-i-1) * w + j) + 1] = imageBuffer[4 * (i * w + j)+1  ];
          pixels[4 * ((h-i-1) * w + j) + 2] = imageBuffer[4 * (i * w + j)+2  ];
          pixels[4 * ((h-i-1) * w + j) + 3] = imageBuffer[4 * (i * w + j)+3  ];
        }
    bool test = save_targa(fileName.toStdString().c_str(),width,height,format,pixels);
    if(test == false) qWarning() << "Cannot save image to targa file:" << fileName ;
    delete[] pixels;
}



bool TargaImage::save_targa (const char *filename, int width, int height,
                      TargaColorFormat format, unsigned char *pixels)

{
    // check format of the file
    if (format != TARGA_BGR && format != TARGA_BGRA && format != TARGA_LUMINANCE)
        return false;


    // open file
    FILE *tga = fopen (filename,"wb");

    // check if file is good
    if (tga == NULL)
        return false;

    // create TGA header
    unsigned char header [TARGA_HEADER_SIZE];

    // clear header
    memset (header,0,TARGA_HEADER_SIZE);

    // set proper format for image
    if (format == TARGA_BGR || format == TARGA_BGRA)
        header [2] = TARGA_UNCOMP_RGB_IMG;
    else
        if (format == TARGA_LUMINANCE)
            header [2] = TARGA_UNCOMP_BW_IMG;

    // set width
    header [12] = (unsigned char)width;
    header [13] = (unsigned char)(width >> 8);

    // set height
    header [14] = (unsigned char)height;
    header [15] = (unsigned char)(height >> 8);

    // set bitrate
    if (format == TARGA_BGRA)
        header [16] = 32;
    else
        if (format == TARGA_BGR)
            header [16] = 24;
        else
            if (format == TARGA_LUMINANCE)
                header [16] = 8;

    // write header
    fwrite (header,TARGA_HEADER_SIZE,1,tga);

    // write data
    if (format == TARGA_BGRA)
        fwrite (pixels,width*height*4,1,tga);
    else
        if (format == TARGA_BGR)
            fwrite (pixels,width*height*3,1,tga);
        else
            if (format == TARGA_LUMINANCE)
                fwrite (pixels,width*height,1,tga);

    // close image
    fclose (tga);

    // done
    return true;
}


QImage TargaImage::read(QString fileName){
    int              width,height;
    TargaColorFormat format;
    unsigned char   *pixels;

    // read TGA from file
    bool test = load_targa(fileName.toStdString().c_str(),width,height,format,pixels);

    if(!test) return QImage(0,0);// return null image
    // allocate image
    QImage image(width,height,QImage::Format_ARGB32);

    unsigned char* imageBuffer  = image.bits();//get pointer do QImage pixels
    int w = width;
    int h = height;

    // write data to QImage according to image format
    if(format == TARGA_BGR){

    for(int i = 0; i < height; i++)
       for(int j = 0; j < width; j++)
        {
          imageBuffer[4 * (i * w + j) + 0] = pixels[3 * ((h-i-1) * w + j)+0  ] ;
          imageBuffer[4 * (i * w + j) + 1] = pixels[3 * ((h-i-1) * w + j)+1  ] ;
          imageBuffer[4 * (i * w + j) + 2] = pixels[3 * ((h-i-1) * w + j)+2  ] ;
          imageBuffer[4 * (i * w + j) + 3] = 255;
        }
    }// end of if RGB
    else if(format == TARGA_BGRA){

    for(int i = 0; i < height; i++)
       for(int j = 0; j < width; j++)
        {
          imageBuffer[4 * (i * w + j) + 0] = pixels[4 * ((h-i-1) * w + j)+0  ] ;
          imageBuffer[4 * (i * w + j) + 1] = pixels[4 * ((h-i-1) * w + j)+1  ] ;
          imageBuffer[4 * (i * w + j) + 2] = pixels[4 * ((h-i-1) * w + j)+2  ] ;
          imageBuffer[4 * (i * w + j) + 3] = pixels[4 * ((h-i-1) * w + j)+3  ] ;
        }
    }else {
    for(int i = 0; i < height; i++)
       for(int j = 0; j < width; j++)
        {
          imageBuffer[4 * (i * w + j) + 0] = pixels[1 * ((h-i-1) * w + j)+0  ] ;
          imageBuffer[4 * (i * w + j) + 1] = pixels[1 * ((h-i-1) * w + j)+0  ] ;
          imageBuffer[4 * (i * w + j) + 2] = pixels[1 * ((h-i-1) * w + j)+0  ] ;
          imageBuffer[4 * (i * w + j) + 3] = 1.0;
        }
    }
    delete[] pixels;
    return image;
}

bool TargaImage::load_targa (const char *filename, int &width, int &height,
                      TargaColorFormat &format, unsigned char *&pixels)
{
    // set default values
    pixels = NULL;
    width  = 0;
    height = 0;

    //open tga image file
    FILE *tga = fopen (filename,"rb");

    // check image file
    if (!tga)
        return false;

    // allocate array for header information
    unsigned char header [TARGA_HEADER_SIZE];

    // read header
    fread (header,TARGA_HEADER_SIZE,1,tga);

    // skeep some fields
    fseek (tga,header [0],SEEK_CUR);

    //  read image width
    width = header [12] + (header [13] << 8);

    // read image height
    height = header [14] + (header [15] << 8);

    // read file with 24 bbm
    if (header [2] == TARGA_UNCOMP_RGB_IMG && header [16] == 24)
    {
        pixels = new unsigned char [width * height * 3];
        fread ((void*)pixels,width * height * 3,1,tga);
        format = TARGA_BGR;
    }
    else

        // read RGBA image (32 bit)
        if (header [2] == TARGA_UNCOMP_RGB_IMG && header [16] == 32)
        {
            pixels = new unsigned char [width * height * 4];
            fread ((void*)pixels,width * height * 4,1,tga);
            format = TARGA_BGRA;
        }
        else

            // read monocolour image
            if (header [2] == TARGA_UNCOMP_BW_IMG && header [16] == 8)
            {
                pixels = new unsigned char [width * height];
                fread ((void*)pixels,width * height,1,tga);
                format = TARGA_LUMINANCE;
            }
            else
                return false;

    // close image
    fclose (tga);

    // done
    return true;
}
