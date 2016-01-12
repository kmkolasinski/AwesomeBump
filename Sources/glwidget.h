/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include <qmath.h>

#include "CommonObjects.h"
#include "camera.h"
#include "utils/Mesh.hpp"
#include "utils/qglbuffers.h"
#include "glwidgetbase.h"
#include "glimageeditor.h"
#include "properties/Dialog3DGeneralSettings.h"
#include "utils/glslshaderparser.h"

#define settings3D      Dialog3DGeneralSettings::settings3D
#define currentShader   Dialog3DGeneralSettings::currentRenderShader
#define glslShadersList Dialog3DGeneralSettings::glslParsedShaders


#ifdef USE_OPENGL_330
    #include <QOpenGLFunctions_3_3_Core>
    #define OPENGL_FUNCTIONS QOpenGLFunctions_3_3_Core
#else
    #include <QOpenGLFunctions_4_0_Core>
    #define OPENGL_FUNCTIONS QOpenGLFunctions_4_0_Core
#endif



class GLWidget : public GLWidgetBase , protected OPENGL_FUNCTIONS
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0 , QGLWidget * shareWidget  = 0);
    ~GLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;    
    void setPointerToTexture(QGLFramebufferObject **pointer, TextureTypes type);

public slots:


    void toggleDiffuseView(bool);
    void toggleSpecularView(bool);
    void toggleOcclusionView(bool);
    void toggleNormalView(bool);
    void toggleHeightView(bool);
    void toggleRoughnessView(bool);
    void toggleMetallicView(bool);
    void setCameraMouseSensitivity(int value);
    void resetCameraPosition();
    void cleanup();

    // mesh loading functions
    void loadMeshFromFile();//opens file dialog
    // mesh functions
    bool loadMeshFile(const QString &fileName,bool bAddExtension = false);
    void chooseMeshFile(const QString &fileName);

    // pbr functions
    void chooseSkyBox(QString cubeMapName, bool bFirstTime = false);
    void updatePerformanceSettings(Display3DSettings settings);
    void recompileRenderShader(); // read and compile custom fragment shader again, can be called from 3D settings GUI.

signals:
    void renderGL();
    void readyGL();
    void materialColorPicked(QColor); // emit material index color

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void relativeMouseMoveEvent(int dx, int dy, bool *wrapMouse, Qt::MouseButtons buttons);
    void wheelEvent(QWheelEvent *event);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

private:



    // other functions
    QPointF pixelPosToViewPos(const QPointF& p);
    int glhUnProjectf(float &winx, float &winy, float &winz,
                      QMatrix4x4 &modelview, QMatrix4x4 &projection,
                      QVector4D& objectCoordinate);

    void bakeEnviromentalMaps(); // calculate prefiltered enviromental map

    QOpenGLShaderProgram *line_program; // same as "program" but instead of triangles lines are used
    QOpenGLShaderProgram *skybox_program;
    QOpenGLShaderProgram *env_program;

    QGLFramebufferObject**  fboIdPtrs[8];



    bool bToggleDiffuseView;
    bool bToggleSpecularView;
    bool bToggleOcclusionView;
    bool bToggleNormalView;
    bool bToggleHeightView;
    bool bToggleRoughnessView;
    bool bToggleMetallicView;

    Display3DSettings display3Dparameters;

    // 3D view parameters
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;
    QMatrix3x3 NormalMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 objectMatrix;
    QVector4D lightPosition;

    QVector4D cursorPositionOnPlane;
    float ratio;
    float zoom;
    AwesomeCamera camera;   // light used for standard phong shading
    AwesomeCamera newCamera;// to make smooth linear interpolation between two views
    double cameraInterpolation;
    AwesomeCamera lightDirection;//second light - use camera class to rotate light
    QCursor lightCursor;



    Mesh* mesh; // displayed 3d mesh
    Mesh* skybox_mesh; // sky box cube
    Mesh* env_mesh;                       // one trinagle used for calculation of prefiltered env. map

    GLTextureCube* m_env_map;             // orginal cube map
    GLTextureCube* m_prefiltered_env_map; // filtered lambertian cube map
    bool bDiffuseMapBaked;                // prevent program from calculating diffuse env. map many times

    GLImage* glImagePtr;

    // Post-processing variables
    std::map<std::string,QOpenGLShaderProgram*> post_processing_programs; // all post processing functions
    Mesh* quad_mesh;                      // quad mesh used for post processing
    QOpenGLShaderProgram *filter_program; // holds pointer to current post-processing program
    GLFrameBufferObject* colorFBO;
    GLFrameBufferObject* outputFBO;
    GLFrameBufferObject* auxFBO;
    // glow FBOs
    GLFrameBufferObject* glowInputColor[4];
    GLFrameBufferObject* glowOutputColor[4];
    // tone mapping mipmaps FBOS
    GLFrameBufferObject* toneMipmaps[10];

    GLuint lensFlareColorsTexture;
    GLuint lensDirtTexture;
    GLuint lensStarTexture;

protected:
    void resizeFBOs();
    void deleteFBOs();
    void applyNormalFilter(GLuint input_tex);
    void copyTexToFBO(GLuint input_tex,QGLFramebufferObject* dst);
    void applyGaussFilter(GLuint input_tex,
                          QGLFramebufferObject* auxFBO,
                          QGLFramebufferObject* outputFBO, float radius = 10.0);
    void applyDofFilter(GLuint input_tex,
                        QGLFramebufferObject* outputFBO);
    void applyGlowFilter(QGLFramebufferObject* outputFBO);
    void applyToneFilter(GLuint input_tex,QGLFramebufferObject* outputFBO);
    void applyLensFlaresFilter(GLuint input_tex,QGLFramebufferObject* outputFBO);
public:
    static QDir* recentMeshDir;
};
//! [3]

#endif
