
#ifndef GLPREVIEW_H
#define GLPREVIEW_H

#include <qmath.h>
#include <qopengl.h>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

#include "CommonObjects.h"

#ifdef USE_OPENGL_330
    #include <QOpenGLFunctions_3_3_Core>
    #define OpenGLFunctionsBase QOpenGLFunctions_3_3_Core
#else
    #include <QOpenGLFunctions_4_0_Core>
    #define OpenGLFunctionsBase QOpenGLFunctions_4_0_Core
#endif

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);

enum TextureAlign {
  TextureAll,
  TextureFit,
  TextureFull,
  TextureAlignMaxType
};

class GLPreview : public QOpenGLWidget, protected OpenGLFunctionsBase
{
    Q_OBJECT

public:
    GLPreview(QWidget *parent = 0);
    ~GLPreview();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void setTextureAlign(TextureAlign align);
    void nextTextureAlign();

public slots:
    void cleanup();
    void textureChanged(TextureTypes imageType, GLuint texture);

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    void makeScreenQuad();
    GLuint textures[MAX_TEXTURES_TYPE];
    QOpenGLShaderProgram *program = 0;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbos[3];
    float ratio;
    QSize viewport;
    TextureAlign alignType;
};

#endif
