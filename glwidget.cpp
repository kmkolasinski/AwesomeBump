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


GLWidget::GLWidget(QWidget *parent, QGLWidget * shareWidget)
    : QGLWidget(QGLFormat::defaultFormat(), parent, shareWidget)
{
    zoom                    = 60;
    lightPosition           = QVector4D(0,0,5.0,1);
    depthScale              = 1;
    uvScale                 = 1.0;
    uvOffset                = QVector2D(0,0);
    bToggleDiffuseView      = true;
    bToggleSpecularView     = true;
    bToggleOcclusionView    = true;
    specularIntensity       = 1.0;
    diffuseIntensity        = 1.0;

    RotatePlaneMatrix.rotate(180.0f,QVector3D(0.0,1.0,0.0));
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    lightCursor = QCursor(QPixmap(":/content/lightCursor.png"));
}

GLWidget::~GLWidget()
{
  cleanup();
}

void GLWidget::cleanup()
{   
    makeCurrent();

    glDeleteBuffers(sizeof(vbos)/sizeof(GLuint), &vbos[0]);
    delete program;
    
    doneCurrent();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(360, 360);
}
QSize GLWidget::sizeHint() const
{
    return QSize(500, 400);
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


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    qglClearColor(QColor::fromCmykF(0.79, 0.79, 0.79, 0.0).dark());


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_TEXTURE_2D); // non-core


    qDebug() << "Loading quad (fragment shader)";
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(":/content/plane.vert");
    if (!vshader->log().isEmpty()) qDebug() << vshader->log();
    else qDebug() << "done";

    qDebug() << "Loading quad (vertex shader)";
    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile(":/content/plane.frag");
    if (!fshader->log().isEmpty()) qDebug() << fshader->log();
    else qDebug() << "done";


    program = new QOpenGLShaderProgram(this);
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

    camera.position.setZ( -0 );
    camera.toggleFreeCamera(false);
    emit readyGL();
}

void GLWidget::paintGL()
{
    GLCHK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    GLCHK( glDisable(GL_CULL_FACE) );
    GLCHK( glEnable(GL_DEPTH_TEST) );
    
    GLCHK( program->bind() );
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(zoom,ratio,0.1,100.0);
    GLCHK( program->setUniformValue("ProjectionMatrix", projectionMatrix) );

    objectMatrix.setToIdentity();
    if( fboIdPtrs[0] != NULL){
        float fboRatio = float((*(fboIdPtrs[0]))->width())/(*(fboIdPtrs[0]))->height();
        objectMatrix.scale(fboRatio,1,1);
    }

    modelViewMatrix = camera.updateCamera()*RotatePlaneMatrix*objectMatrix;
    QMatrix3x3 NormalMatrix = modelViewMatrix.normalMatrix();

    GLCHK( program->setUniformValue("ModelViewMatrix"       , modelViewMatrix) );    
    GLCHK( program->setUniformValue("NormalMatrix"          , NormalMatrix) );
    GLCHK( program->setUniformValue("lightPos"              , lightPosition) );
    GLCHK( program->setUniformValue("cameraPos"             , cursorPositionOnPlane) );
    GLCHK( program->setUniformValue("gui_depthScale"        , depthScale) );
    GLCHK( program->setUniformValue("gui_uvScale"           , uvScale) );
    GLCHK( program->setUniformValue("gui_uvScaleOffset"     , uvOffset) );
    GLCHK( program->setUniformValue("gui_bSpecular"         , bToggleSpecularView) );
    GLCHK( program->setUniformValue("gui_bDiffuse"          , bToggleDiffuseView) );
    GLCHK( program->setUniformValue("gui_bOcclusion"        , bToggleOcclusionView) );
    GLCHK( program->setUniformValue("gui_SpecularIntensity" , specularIntensity) );
    GLCHK( program->setUniformValue("gui_DiffuseIntensity"  , diffuseIntensity) );

    if( fboIdPtrs[0] != NULL){
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glVertexAttribPointer(PROGRAM_VERTEX_ATTRIBUTE,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glVertexAttribPointer(PROGRAM_TEXCOORD_ATTRIBUTE,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,(void*)0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);

        GLCHK( glActiveTexture(GL_TEXTURE0) );
     //   if(bToggleDiffuseView)
	       GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[0]))->texture()) );
     //   else
       //    GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[1]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE1) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[1]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE2) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[2]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE3) );        
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[3]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE4) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[4]))->texture()) );

        GLCHK( glDrawElements(GL_TRIANGLES, 3*no_triangles, GL_UNSIGNED_INT, 0) );

        // restore filtering
        glActiveTexture(GL_TEXTURE0);
    }

    emit renderGL();

}

void GLWidget::resizeGL(int width, int height)
{
    ratio = float(width)/height;
    GLCHK( glViewport(0, 0, width, height) );
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    setCursor(Qt::ClosedHandCursor);
    if (event->buttons() & Qt::RightButton) {
        setCursor(Qt::SizeAllCursor);
    }else if(event->buttons() & Qt::MiddleButton){
        setCursor(lightCursor);
    }

    updateGL();
}
void GLWidget::mouseReleaseEvent(QMouseEvent *event){
    setCursor(Qt::PointingHandCursor);
}


int GLWidget::glhUnProjectf(float& winx, float& winy, float& winz,
                            QMatrix4x4& modelview, QMatrix4x4& projection,
                            QVector4D& objectCoordinate)
  {
      //Transformation matrices
      QVector4D in,out;
      //Calculation for inverting a matrix, compute projection x modelview
      //and store in A[16]
      QMatrix4x4  A = projection * modelview;
      //Now compute the inverse of matrix A
      QMatrix4x4  m = A.inverted();

      //Transformation of normalized coordinates between -1 and 1
      in[0]=(winx)/(float)width()*2.0-1.0;
      in[1]=(1-(winy)/(float)height())*2.0-1.0;
      in[2]=2.0*winz-1.0;
      in[3]=1.0;
      //Objects coordinates
      out = m * in;

      if(out[3]==0.0)
         return 0;
      out[3]=1.0/out[3];
      objectCoordinate[0]=out[0]*out[3];
      objectCoordinate[1]=out[1]*out[3];
      objectCoordinate[2]=out[2]*out[3];
      return 1;
  }

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{

    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();
    /*
    float zPos, xPos , yPos;
    xPos = event->x();
    yPos = event->y();
    glReadPixels(event->x(),height()-event->y(),1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&zPos);
    QVector4D oPos;
    if(glhUnProjectf(xPos,yPos,zPos,modelViewMatrix,projectionMatrix,oPos)){
        cursorPositionOnPlane = oPos;
    }
    */
    if ((event->buttons() & Qt::LeftButton) && (event->buttons() & Qt::RightButton)) {
        /*
        QMatrix4x4 mvp = modelViewMatrix;
        QVector4D corner1(-0.5,-0.5,0,1);
        QVector4D corner2(+0.5,-0.5,0,1);
        QVector4D corner3(-0.5,+0.5,0,1);
        corner1 = mvp * corner1;
        corner2 = mvp * corner2;
        corner3 = mvp * corner3;
        QVector3D r21 =QVector3D(corner2-corner1);
        QVector3D r31 =QVector3D(corner3-corner1);
        QVector3D n =  QVector3D::crossProduct(r21,r31);

        QPoint p = mapFromGlobal(QCursor::pos());//getting the global position of cursor
        QVector3D l0(double(p.x())/width()-0.5,(1.0-double(p.y()))/height()+0.5,1);
        QVector3D l(0.0,0,-1);
        QVector3D ll = -l0*QVector3D::dotProduct(l0,n)/QVector3D::dotProduct(l,n);
        */
    }else if (event->buttons() & Qt::LeftButton) {
        camera.rotateView(dx/1.0,dy/1.0);
    } else if (event->buttons() & Qt::RightButton) {
        camera.position += QVector3D(dx/500.0,dy/500.0,0)*camera.radius;
    } else if (event->buttons() & Qt::MiddleButton) {
        lightPosition += QVector4D(0.05*dx,-0.05*dy,-0,0);
    }
    lastPos = event->pos();
    updateGL();
}
//! [10]

void GLWidget::wheelEvent(QWheelEvent *event){
    int numDegrees = event->delta();
    camera.mouseWheelMove((numDegrees));
    updateGL();
}

void GLWidget::makeObject()
{

    int size = 1024;
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
