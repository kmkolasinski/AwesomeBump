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

#include <QtWidgets>
#include <QtOpenGL>
#include <math.h>

#include "glwidget.h"


#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

//! [0]
GLWidget::GLWidget(QWidget *parent, QGLWidget * shareWidget)
    : QGLWidget(QGLFormat::defaultFormat(), parent, shareWidget)
{

    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoom = 60;
    lightPosition = QVector4D(0,0,5.0,1);
    depthScale         = 1;
    uvScale            = 1.0;
    uvOffset           = QVector2D(0,0);
    bToggleDiffuseView = true;
    bToggleSpecularView = true;
    bToggleOcclusionView = true;
    specularIntensity = 1.0;
    diffuseIntensity  = 1.0;
}

GLWidget::~GLWidget()
{   

    glDeleteBuffers(sizeof(vbos)/sizeof(GLuint), &vbos[0]);

    delete program;
}
//! [1]

//! [2]
QSize GLWidget::minimumSizeHint() const
{
    return QSize(360, 360);
}
//! [2]

//! [3]
QSize GLWidget::sizeHint() const
//! [3] //! [4]
{
    return QSize(500, 400);
}
//! [4]

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

//! [5]
void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}
//! [5]

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setDepthScale(int scale){

    depthScale = scale/50.0;
    updateGL();

}

void GLWidget::setUVScale(int scale){
    uvScale = scale/10.0;
    updateGL();
}

void GLWidget::setUVScaleOffset(double x,double y){
    uvOffset = QVector2D(x,y);
    updateGL();
}

void GLWidget::toggleDiffuseView(bool enable){
    bToggleDiffuseView = enable;
    updateGL();
}

void GLWidget::toggleSpecularView(bool enable){
    bToggleSpecularView = enable;
    updateGL();
}

void GLWidget::toggleOcclusionView(bool enable){
    bToggleOcclusionView = enable;
    updateGL();
}

void GLWidget::setSpecularIntensity(double val){
    specularIntensity = val;
    updateGL();
}
void GLWidget::setDiffuseIntensity(double val){
    diffuseIntensity = val;
    updateGL();
}

//! [6]
void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qglClearColor(QColor::fromCmykF(0.79, 0.79, 0.79, 0.0).dark());


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);


#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
    qDebug() << "Loading quad (fragment shader)";
    QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
    vshader->compileSourceFile(":/content/plane.vert");
    if (!vshader->log().isEmpty()) qDebug() << vshader->log();
    qDebug() << "Loading quad (vertex shader)";
    QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);
    fshader->compileSourceFile(":/content/plane.frag");
    if (!fshader->log().isEmpty()) qDebug() << fshader->log();

    program = new QGLShaderProgram(this);
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex"  , PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texDiffuse" , 0);
    program->setUniformValue("texNormal"  , 1);
    program->setUniformValue("texSpecular", 2);
    program->setUniformValue("texHeight"  , 3);
    program->setUniformValue("texSSAO"    , 4);
    makeObject();
}
//! [6]

//! [7]
void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    QMatrix4x4 m;
    program->bind();

    m.perspective(zoom,ratio,0.1,100.0);
    program->setUniformValue("ProjectionMatrix", m);

    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    if( fboIdPtrs[0] != NULL){
        float fboRatio = float((*(fboIdPtrs[0]))->width())/(*(fboIdPtrs[0]))->height();
        modelMatrix.scale(fboRatio,1,1);
    }
    QMatrix4x4 rotMatrix;
    rotMatrix.setToIdentity();
    rotMatrix.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
    rotMatrix.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
    rotMatrix.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(0.0f, 0.0f, -1.5f);
    m =  viewMatrix*rotMatrix*modelMatrix;

    program->setUniformValue("ModelViewMatrix", m);
    QMatrix3x3 NormalMatrix = m.normalMatrix();
    program->setUniformValue("NormalMatrix", NormalMatrix);
    program->setUniformValue("lightPos", lightPosition);
    program->setUniformValue("gui_depthScale"     , depthScale);
    program->setUniformValue("gui_uvScale"        , uvScale);
    program->setUniformValue("gui_uvScaleOffset"  ,uvOffset);
    program->setUniformValue("gui_bSpecular"      , bToggleSpecularView);
    program->setUniformValue("gui_bOcclusion"     , bToggleOcclusionView);
    program->setUniformValue("gui_SpecularIntensity"      , specularIntensity);
    program->setUniformValue("gui_DiffuseIntensity"       , diffuseIntensity);

    if( fboIdPtrs[0] != NULL){
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glVertexAttribPointer(PROGRAM_VERTEX_ATTRIBUTE,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glVertexAttribPointer(PROGRAM_TEXCOORD_ATTRIBUTE,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,(void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);

        glActiveTexture(GL_TEXTURE0);
        if(bToggleDiffuseView) glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[0]))->texture());
        else glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[1]))->texture());
        //glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[1]))->texture());
        //glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[2]))->texture());
        //glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE3);        
        glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[3]))->texture());
        //glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[4]))->texture());
        //glGenerateMipmap(GL_TEXTURE_2D);

        glDrawElements(GL_TRIANGLES, 3*no_triangles, GL_UNSIGNED_INT, 0);
    }

}
//! [7]

//! [8]
void GLWidget::resizeGL(int width, int height)
{
    ratio = float(width)/height;
    glViewport(0, 0, width, height);
}
//! [8]

//! [9]
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();

    updateGL();
}
//! [9]
void GLWidget::mouseReleaseEvent(QMouseEvent *event){


   // updateGL();
}
//! [10]
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    } else if (event->buttons() & Qt::MiddleButton) {
        lightPosition += QVector4D(0.05*dx,-0.05*dy,-0,0);
        updateGL();
    }
    lastPos = event->pos();

}
//! [10]

void GLWidget::wheelEvent(QWheelEvent *event){
    int numDegrees = event->delta();

    if(numDegrees > 0) zoom+=2.0;
    else zoom-=2.0;
    if(zoom < 10)  zoom = 10;
    if(zoom > 120) zoom = 120;
    updateGL();
}

void GLWidget::makeObject()
{

    int size = 512;
    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    texCoords = QVector<QVector2D>(size*size);
    vertices  = QVector<QVector3D>(size*size);
    int iter = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            float offset = 0.5;
            float x = i/(size-1.0);
            float y = j/(size-1.0);
            vertices[iter]  = (QVector3D(x-offset,y-offset,0));
            texCoords[iter] = (QVector2D(x,y));
            iter++;
    }}



    glGenBuffers(3, &vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float)*3, vertices.constData(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(PROGRAM_VERTEX_ATTRIBUTE);
    glVertexAttribPointer(PROGRAM_VERTEX_ATTRIBUTE,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float)*2, texCoords.constData(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    glVertexAttribPointer(PROGRAM_TEXCOORD_ATTRIBUTE,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,(void*)0);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);


    no_triangles = 2*(size - 1)*(size - 1);
    QVector<GLuint> indices(no_triangles*3);
    iter = 0;
    for(int i = 0 ; i < size -1 ; i++){
    for(int j = 0 ; j < size -1 ; j++){
        GLuint i1 = i + j*size;
        GLuint i2 = i + (j+1)*size;
        GLuint i3 = i+1 + j*size;
        GLuint i4 = i+1 + (j+1)*size;
        indices[iter++] = (i1);
        indices[iter++] = (i3);
        indices[iter++] = (i2);
        indices[iter++] = (i2);
        indices[iter++] = (i3);
        indices[iter++] = (i4);
    }
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * no_triangles * 3 , indices.constData(), GL_STATIC_DRAW);
}

void GLWidget::setPointerToTexture(QGLFramebufferObject **pointer, TextureTypes tType){
    switch(tType){
        case(DIFFUSE_TEXTURE ):
            fboIdPtrs[0] = pointer;
            break;
        case(NORMAL_TEXTURE  ):
            fboIdPtrs[1] = pointer;
            break;
        case(SPECULAR_TEXTURE):
            fboIdPtrs[2] = pointer;
            break;
        case(HEIGHT_TEXTURE  ):
            fboIdPtrs[3] = pointer;
            break;
        case(OCCLUSION_TEXTURE ):
            fboIdPtrs[4] = pointer;
            break;
    }
}

QPointF GLWidget::pixelPosToViewPos(const QPointF& p)
{
    return QPointF(2.0 * float(p.x()) / width() - 1.0,
                   1.0 - 2.0 * float(p.y()) / height());
}
