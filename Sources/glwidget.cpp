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
QDir* GLWidget::recentMeshDir = NULL;

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
    bToggleHeightView       = true;
    bToggleNormalView       = true;
    shadingType             = SHADING_RELIEF_MAPPING;
    specularIntensity       = 0.5;
    diffuseIntensity        = 1.0;
    m_env_map               = NULL;

    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    lightCursor = QCursor(QPixmap(":/resources/lightCursor.png"));

}

GLWidget::~GLWidget()
{
  cleanup();
}

void GLWidget::cleanup()
{   
    makeCurrent();


    delete program;
    delete skybox_program;
    delete env_program;
    delete mesh;
    delete skybox_mesh;
    delete env_mesh;
    delete m_env_map;
    delete m_prefiltered_env_map;

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

void GLWidget::toggleNormalView(bool enable){
    bToggleNormalView = enable;
    updateGL();
}

void GLWidget::toggleHeightView(bool enable){
    bToggleHeightView = enable;
    updateGL();
}

void GLWidget::selectShadingType(int indeks){
    shadingType = (ShadingType)indeks;
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
    makeCurrent();
    qglClearColor(QColor::fromCmykF(0.79, 0.79, 0.79, 0.0).dark());


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);


    qDebug() << "Loading quad (vertex shader)";
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(":/resources/plane.vert");
    if (!vshader->log().isEmpty()) qDebug() << vshader->log();
    else qDebug() << "done";

    qDebug() << "Loading quad (fragment shader)";
    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile(":/resources/plane.frag");
    if (!fshader->log().isEmpty()) qDebug() << fshader->log();
    else qDebug() << "done";

    qDebug() << "Loading quad (tessellation control shader)";
    QOpenGLShader *tcshader = new QOpenGLShader(QOpenGLShader::TessellationControl, this);
    tcshader->compileSourceFile(":/resources/plane.tcs.vert");
    if (!tcshader->log().isEmpty()) qDebug() << tcshader->log();
    else qDebug() << "done";

    qDebug() << "Loading quad (tessellation evaluation shader)";
    QOpenGLShader *teshader = new QOpenGLShader(QOpenGLShader::TessellationEvaluation, this);
    teshader->compileSourceFile(":/resources/plane.tes.vert");
    if (!teshader->log().isEmpty()) qDebug() << teshader->log();
    else qDebug() << "done";

    qDebug() << "Loading quad (geometry shader)";
    QOpenGLShader *gshader = new QOpenGLShader(QOpenGLShader::Geometry, this);
    gshader->compileSourceFile(":/resources/plane.geom");
    if (!gshader->log().isEmpty()) qDebug() << gshader->log();
    else qDebug() << "done";

    program = new QOpenGLShaderProgram(this);
    program->addShader(vshader);
    program->addShader(fshader);
    program->addShader(tcshader);
    program->addShader(teshader);
    program->addShader(gshader);
    GLCHK(program->link());

    GLCHK(program->bind());
    program->setUniformValue("texDiffuse" , 0);
    program->setUniformValue("texNormal"  , 1);
    program->setUniformValue("texSpecular", 2);
    program->setUniformValue("texHeight"  , 3);
    program->setUniformValue("texSSAO"    , 4);
    program->setUniformValue("texDiffuseEnvMap", 5);
    program->setUniformValue("texEnvMap"       , 6);


    delete vshader;
    delete fshader;
    delete tcshader;
    delete teshader;
    delete gshader;


    // loading sky box shader
    qDebug() << "Loading skybox shader (vertex shader)";
    vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(":/resources/skybox.vert.glsl");
    if (!vshader->log().isEmpty()) qDebug() << vshader->log();
    else qDebug() << "done";

    qDebug() << "Loading skybox shader (fragment shader)";
    fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile(":/resources/skybox.frag.glsl");
    if (!fshader->log().isEmpty()) qDebug() << fshader->log();
    else qDebug() << "done";

    skybox_program = new QOpenGLShaderProgram(this);
    skybox_program->addShader(vshader);
    skybox_program->addShader(fshader);

    GLCHK(skybox_program->link());
    GLCHK(skybox_program->bind());
    skybox_program->setUniformValue("texEnv" , 0);


    // loading enviromental shader
    qDebug() << "Loading enviromental shader (vertex shader)";
    vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(":/resources/env.vert");
    if (!vshader->log().isEmpty()) qDebug() << vshader->log();
    else qDebug() << "done";

    qDebug() << "Loading enviromental shader (geometry shader)";
    gshader = new QOpenGLShader(QOpenGLShader::Geometry, this);
    gshader->compileSourceFile(":/resources/env.geom");
    if (!gshader->log().isEmpty()) qDebug() << gshader->log();
    else qDebug() << "done";

    qDebug() << "Loading enviromental shader (fragment shader)";
    fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile(":/resources/env.frag");
    if (!fshader->log().isEmpty()) qDebug() << fshader->log();
    else qDebug() << "done";

    env_program = new QOpenGLShaderProgram(this);
    env_program->addShader(vshader);
    env_program->addShader(gshader);
    env_program->addShader(fshader);

    GLCHK(env_program->link());
    GLCHK(env_program->bind());
    env_program->setUniformValue("texEnv" , 0);

    delete vshader;
    delete fshader;
    delete gshader;


    camera.position.setZ( -0 );
    camera.toggleFreeCamera(false);

    lightDirection.position.setZ(0);
    lightDirection.toggleFreeCamera(false);
    lightDirection.radius = 1;

    mesh        = new Mesh("Core/3D/","Cube.obj");
    skybox_mesh = new Mesh("Core/3D/","sky_cube.obj");
    env_mesh    = new Mesh("Core/3D/","sky_cube_env.obj");


    chooseSkyBox("SaintLazarusChurch");

    m_prefiltered_env_map = new GLTextureCube(256);
    emit readyGL();
}

void GLWidget::paintGL()
{
    // setting the camera viewpoint
    viewMatrix = camera.updateCamera();


    GLCHK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    GLCHK( glDisable(GL_CULL_FACE) );
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(zoom,ratio,0.1,350.0);

    // ---------------------------------------------------------
    // Drawing skybox
    // ---------------------------------------------------------
    skybox_program->bind();

    objectMatrix.setToIdentity();
    if(skybox_mesh->isLoaded()){
        objectMatrix.translate(camera.position);
        objectMatrix.scale(150.0);
    }
    modelViewMatrix         = viewMatrix * objectMatrix;
    NormalMatrix            = modelViewMatrix.normalMatrix();



    glDisable(GL_DEPTH_TEST); // disable depth

    GLCHK( skybox_program->setUniformValue("ModelViewMatrix"       , modelViewMatrix) );
    GLCHK( skybox_program->setUniformValue("NormalMatrix"          , NormalMatrix) );
    GLCHK( skybox_program->setUniformValue("ModelMatrix"           , objectMatrix) );
    GLCHK( skybox_program->setUniformValue("ProjectionMatrix"      , projectionMatrix) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( m_env_map->bind());
    GLCHK( skybox_mesh->drawMesh(true) );


    // ---------------------------------------------------------
    // Drawing env
    // ---------------------------------------------------------
    bakeEnviromentalMaps();

    // ---------------------------------------------------------
    // Drawing model
    // ---------------------------------------------------------
    GLCHK( glEnable(GL_CULL_FACE) );
    GLCHK( glEnable(GL_DEPTH_TEST) );
    GLCHK( glCullFace(GL_FRONT) );
    GLCHK( program->bind() );

    GLCHK( program->setUniformValue("ProjectionMatrix", projectionMatrix) );

    objectMatrix.setToIdentity();
    if( fboIdPtrs[0] != NULL){
        float fboRatio = float((*(fboIdPtrs[0]))->width())/(*(fboIdPtrs[0]))->height();
        objectMatrix.scale(fboRatio,1,fboRatio);
    }
    if(mesh->isLoaded()){

        objectMatrix.scale(0.5/mesh->radius);
        objectMatrix.translate(-mesh->centre_of_mass);
    }
    modelViewMatrix = camera.updateCamera()*objectMatrix;
    NormalMatrix = modelViewMatrix.normalMatrix();
    float mesh_scale = 0.5/mesh->radius;

    GLCHK( program->setUniformValue("ModelViewMatrix"       , modelViewMatrix) );    
    GLCHK( program->setUniformValue("NormalMatrix"          , NormalMatrix) );
    GLCHK( program->setUniformValue("ModelMatrix"           , objectMatrix) );
    GLCHK( program->setUniformValue("meshScale"             , mesh_scale) );
    GLCHK( program->setUniformValue("lightPos"              , lightPosition) );

    GLCHK( program->setUniformValue("lightDirection"        , lightDirection.direction) );
    GLCHK( program->setUniformValue("cameraPos"             , camera.get_position()) );
    GLCHK( program->setUniformValue("gui_depthScale"        , depthScale) );
    GLCHK( program->setUniformValue("gui_uvScale"           , uvScale) );
    GLCHK( program->setUniformValue("gui_uvScaleOffset"     , uvOffset) );
    GLCHK( program->setUniformValue("gui_bSpecular"         , bToggleSpecularView) );
    GLCHK( program->setUniformValue("gui_bDiffuse"          , bToggleDiffuseView) );
    GLCHK( program->setUniformValue("gui_bOcclusion"        , bToggleOcclusionView) );
    GLCHK( program->setUniformValue("gui_bHeight"           , bToggleHeightView) );
    GLCHK( program->setUniformValue("gui_bNormal"           , bToggleNormalView) );
    GLCHK( program->setUniformValue("gui_shading_type"      , shadingType) );
    GLCHK( program->setUniformValue("gui_SpecularIntensity" , specularIntensity) );
    GLCHK( program->setUniformValue("gui_DiffuseIntensity"  , diffuseIntensity) );

    if( fboIdPtrs[0] != NULL){


        GLCHK( glActiveTexture(GL_TEXTURE0) );

        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[0]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE1) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[1]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE2) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[2]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE3) );        
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[3]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE4) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[4]))->texture()) );

        GLCHK( glActiveTexture(GL_TEXTURE5) );
        GLCHK(m_prefiltered_env_map->bind());
        GLCHK( glActiveTexture(GL_TEXTURE6) );
        GLCHK(m_env_map->bind());
        GLCHK( mesh->drawMesh() );

        // set default active texture
        glActiveTexture(GL_TEXTURE0);
    }



    GLCHK( glDisable(GL_CULL_FACE) );
    emit renderGL();

}

void GLWidget::bakeEnviromentalMaps(){
    if(bDiffuseMapBaked) return;
    bDiffuseMapBaked = true;
    // ---------------------------------------------------------
    // Drawing env - one pass method
    // ---------------------------------------------------------
    env_program->bind();
    m_prefiltered_env_map->bindFBO();
    glViewport(0,0,256,256);

    objectMatrix.setToIdentity();
    objectMatrix.scale(1.0);
    objectMatrix.rotate(90.0,1,0,0);

    modelViewMatrix         = viewMatrix * objectMatrix;
    NormalMatrix            = modelViewMatrix.normalMatrix();


    GLCHK( env_program->setUniformValue("ModelViewMatrix"       , modelViewMatrix) );
    GLCHK( env_program->setUniformValue("NormalMatrix"          , NormalMatrix) );
    GLCHK( env_program->setUniformValue("ModelMatrix"           , objectMatrix) );
    GLCHK( env_program->setUniformValue("ProjectionMatrix"      , projectionMatrix) );

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( m_env_map->bind());
    GLCHK( env_mesh->drawMesh(true) );

    glBindFramebuffer   (GL_FRAMEBUFFER, 0);

    glViewport(0, 0, width(), height()) ;
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
    event->accept();
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

    if ((event->buttons() & Qt::LeftButton) && (event->buttons() & Qt::RightButton)) {

    }else if (event->buttons() & Qt::LeftButton) {
        camera.rotateView(dx/1.0,dy/1.0);
    } else if (event->buttons() & Qt::RightButton) {
        camera.position +=QVector3D(dx/500.0,dy/500.0,0)*camera.radius;
        //camera.position += camera.side_direction*0.001*dx;//
        //camera.position += QVector3D(0,dy/500.0,0)*camera.radius;
    } else if (event->buttons() & Qt::MiddleButton) {
        lightPosition += QVector4D(0.05*dx,-0.05*dy,-0,0);
        lightDirection.rotateView(dx/1.0,dy/1.0);
    }
    lastPos = event->pos();
    updateGL();
}
//! [10]

void GLWidget::wheelEvent(QWheelEvent *event){
    int numDegrees = -event->delta();
    camera.mouseWheelMove((numDegrees));

    updateGL();
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


void GLWidget::loadMeshFromFile()
{
    QStringList picturesLocations;
    if(recentMeshDir == NULL ) picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    else  picturesLocations << recentMeshDir->absolutePath();


    QFileDialog dialog(this,
                       tr("Open Mesh File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first(),
                       tr("OBJ file format (*.obj *.OBJ );;"));
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadMeshFile(dialog.selectedFiles().first())) {}
}

bool GLWidget::loadMeshFile(const QString &fileName, bool bAddExtension)
{

    // loading new mesh
    Mesh* new_mesh;
    if(bAddExtension){
        new_mesh = new Mesh(QString("Core/3D/"),fileName+QString(".obj"));
    }else{
        new_mesh = new Mesh(QString(""),fileName);
    }

    if(new_mesh->isLoaded()){
         if(mesh != NULL) delete mesh;
         mesh = new_mesh;
         recentMeshDir->setPath(fileName);
         if( new_mesh->getMeshLog() != QString("")  ){
             QMessageBox msgBox;
             msgBox.setText("Warning! There were some problems during model loading.");
             msgBox.setInformativeText("Loader message:\n"+new_mesh->getMeshLog());
             msgBox.setStandardButtons(QMessageBox::Cancel);
             msgBox.exec();
         }
    }else{
        QMessageBox msgBox;
        msgBox.setText("Error! Cannot load given model.");
        msgBox.setInformativeText("Sorry, but the loaded mesh is incorrect.\nLoader message:\n"+new_mesh->getMeshLog());

        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        delete new_mesh;
    }

    updateGL();
    return true;
}

void GLWidget::chooseMeshFile(const QString &fileName){
    loadMeshFile(fileName,true);
}


void GLWidget::chooseSkyBox(QString cubeMapName){
    QStringList list;
    makeCurrent();
    list << "Core/2D/skyboxes/" + cubeMapName + "/posx.jpg" << "Core/2D/skyboxes/" + cubeMapName  + "/negx.jpg" << "Core/2D/skyboxes/" + cubeMapName + "/posy.jpg"
         << "Core/2D/skyboxes/" + cubeMapName + "/negy.jpg" << "Core/2D/skyboxes/" + cubeMapName  + "/posz.jpg" << "Core/2D/skyboxes/" + cubeMapName + "/negz.jpg";


    qDebug() << "Reading new cube map:" << list;
    bDiffuseMapBaked     = false;

    if(m_env_map != NULL) delete m_env_map;
    m_env_map     = new GLTextureCube(list);

    if(m_env_map->failed()){
        qWarning() << "Cannot load cube map: check if images listed above exist.";
    }

}

