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
    : GLWidgetBase(QGLFormat::defaultFormat(), parent, shareWidget)
{
    setAcceptDrops(true);
    zoom                    = 60;
    lightPosition           = QVector4D(0,0.0,5.0,1);

    bToggleDiffuseView      = true;
    bToggleSpecularView     = true;
    bToggleOcclusionView    = true;
    bToggleHeightView       = true;
    bToggleNormalView       = true;
    bToggleRoughnessView    = true;
    bToggleMetallicView     = true;

    m_env_map               = NULL;

    setCursor(Qt::PointingHandCursor);
    lightCursor = QCursor(QPixmap(":/resources/lightCursor.png"));


    glImagePtr = (GLImage*)shareWidget;
    // Post processing variables:
    colorFBO = NULL;
    outputFBO= NULL;
    auxFBO   = NULL;
    for(int i = 0; i < 4; i++){
       glowInputColor[i] = NULL;
       glowOutputColor[i] = NULL;
    }

    cameraInterpolation = 1.0;

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
}

GLWidget::~GLWidget()
{
  cleanup();
}

void GLWidget::cleanup()
{   
    makeCurrent();
    deleteFBOs();

    typedef std::map<std::string,QOpenGLShaderProgram*>::iterator it_type;
    qDebug() << "Removing filters:";
    for(it_type iterator = post_processing_programs.begin(); iterator != post_processing_programs.end(); iterator++) {
        qDebug() << "Removing program:" << QString(iterator->first.c_str());
        delete iterator->second;
    }
    deleteTexture(lensFlareColorsTexture);
    deleteTexture(lensDirtTexture);
    deleteTexture(lensStarTexture);


    delete program;
    delete line_program;
    delete skybox_program;
    delete env_program;

    delete mesh;
    delete skybox_mesh;
    delete env_mesh;
    delete quad_mesh;
    delete m_env_map;
    delete m_prefiltered_env_map;

    doneCurrent();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}
QSize GLWidget::sizeHint() const
{
    return QSize(500, 400);
}



void GLWidget::setCameraMouseSensitivity(int value){
    camera.setMouseSensitivity(value);
}

void GLWidget::resetCameraPosition(){

    camera.reset();
    newCamera.reset();
    cameraInterpolation = 1.0;
    emit changeCamPositionApplied(false);
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

void GLWidget::toggleRoughnessView(bool enable){
    bToggleRoughnessView = enable;
    updateGL();

}
void GLWidget::toggleMetallicView(bool enable){
    bToggleMetallicView = enable;
    updateGL();
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    makeCurrent();
    qglClearColor(QColor::fromCmykF(0.79, 0.79, 0.79, 0.0).dark());


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    qDebug() << "Initializing 3D widget: detected openGL version:" << QString::number(Display3DSettings::openGLVersion);

    QOpenGLShader *vshader  = NULL;
    QOpenGLShader *fshader  = NULL;
    QOpenGLShader *tcshader = NULL;
    QOpenGLShader *teshader = NULL;
    QOpenGLShader *gshader  = NULL;



    qDebug() << "Loading quad (fragment shader)";
    fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceFile(":/resources/plane.frag");
    if (!fshader->log().isEmpty()) qDebug() << fshader->log();
    else qDebug() << "done";

    qDebug() << "Loading quad (geometry shader)";
    gshader = new QOpenGLShader(QOpenGLShader::Geometry, this);
    QFile gFile(":/resources/plane.geom");
    gFile.open(QFile::ReadOnly);
    QTextStream in(&gFile);
    QString shaderCode = in.readAll();
    QString preambule = "#version 330 core\n"
                        "layout(triangle_strip, max_vertices = 3) out;\n" ;
    gshader->compileSourceCode(preambule+shaderCode);
    if (!gshader->log().isEmpty()) qDebug() << gshader->log();
    else qDebug() << "done";

    program = new QOpenGLShaderProgram(this);


    #ifndef USE_OPENGL_330
        qDebug() << "Loading quad (vertex shader)";
        vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
        vshader->compileSourceFile(":/resources/plane.vert");
        if (!vshader->log().isEmpty()) qDebug() << vshader->log();
        else qDebug() << "done";

        qDebug() << "Loading quad (tessellation control shader)";
        tcshader = new QOpenGLShader(QOpenGLShader::TessellationControl, this);
        tcshader->compileSourceFile(":/resources/plane.tcs.vert");
        if (!tcshader->log().isEmpty()) qDebug() << tcshader->log();
        else qDebug() << "done";

        qDebug() << "Loading quad (tessellation evaluation shader)";
        teshader = new QOpenGLShader(QOpenGLShader::TessellationEvaluation, this);
        teshader->compileSourceFile(":/resources/plane.tes.vert");
        if (!teshader->log().isEmpty()) qDebug() << teshader->log();
        else qDebug() << "done";

        program->addShader(tcshader);
        program->addShader(teshader);

    // setting shaders for 3.30 version of openGL
    #else
        qDebug() << "Loading quad (vertex shader) for openGL 3.30";
        vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
        vshader->compileSourceFile(":/resources/plane_330.vert");
        if (!vshader->log().isEmpty()) qDebug() << vshader->log();
        else qDebug() << "done";
    #endif

    program->addShader(vshader);
    program->addShader(fshader);
    program->addShader(gshader);
    program->bindAttributeLocation("FragColor",0);
    program->bindAttributeLocation("FragNormal",1);
    program->bindAttributeLocation("FragGlowColor",2);
    program->bindAttributeLocation("FragPosition",3);
    GLCHK(program->link());


    GLCHK(program->bind());
    program->setUniformValue("texDiffuse"  , 0);
    program->setUniformValue("texNormal"   , 1);
    program->setUniformValue("texSpecular" , 2);
    program->setUniformValue("texHeight"   , 3);
    program->setUniformValue("texSSAO"     , 4);
    program->setUniformValue("texRoughness", 5);
    program->setUniformValue("texMetallic",  6);
    program->setUniformValue("texMaterial",  7);

    program->setUniformValue("texDiffuseEnvMap", 8);
    program->setUniformValue("texEnvMap"       , 9);



    // lines shader
    qDebug() << "Compiling lines program...";
     preambule = QString("#version 330 core\n")+
                        "layout(line_strip, max_vertices = 3) out;\n" ;
    gshader->compileSourceCode(preambule+shaderCode);
    if (!gshader->log().isEmpty()) qDebug() << gshader->log();
    else qDebug() << "done";


    line_program = new QOpenGLShaderProgram(this);
    line_program->addShader(vshader);
    line_program->addShader(fshader);

    #ifndef USE_OPENGL_330
        line_program->addShader(tcshader);
        line_program->addShader(teshader);
    #endif
    line_program->addShader(gshader);
    line_program->bindAttributeLocation("FragColor",0);
    line_program->bindAttributeLocation("FragNormal",1);
    line_program->bindAttributeLocation("FragGlowColor",2);
    line_program->bindAttributeLocation("FragPosition",3);
    GLCHK(line_program->link());

    GLCHK(line_program->bind());
    line_program->setUniformValue("texDiffuse"  , 0);
    line_program->setUniformValue("texNormal"   , 1);
    line_program->setUniformValue("texSpecular" , 2);
    line_program->setUniformValue("texHeight"   , 3);
    line_program->setUniformValue("texSSAO"     , 4);
    line_program->setUniformValue("texRoughness", 5);
    line_program->setUniformValue("texMetallic",  6);
    line_program->setUniformValue("texMaterial",  7);

    line_program->setUniformValue("texDiffuseEnvMap", 8);
    line_program->setUniformValue("texEnvMap"       , 9);


    if(vshader  != NULL) delete vshader;
    if(fshader  != NULL) delete fshader;
    if(tcshader != NULL) delete tcshader;
    if(teshader != NULL) delete teshader;
    if(gshader  != NULL) delete gshader;




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
    skybox_program->bindAttributeLocation("FragColor",0);
    skybox_program->bindAttributeLocation("FragNormal",1);
    skybox_program->bindAttributeLocation("FragGlowColor",2);
    skybox_program->bindAttributeLocation("FragPosition",3);
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


    if(vshader  != NULL) delete vshader;
    if(fshader  != NULL) delete fshader;
    if(gshader  != NULL) delete gshader;

    // -------------------------------------------------
    // Loading post processing filters
    // -------------------------------------------------

    qDebug() << "Loading post-processing shader (vertex shader)";
    vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(":/resources/filters_3d.vert");
    if (!vshader->log().isEmpty()) qDebug() << vshader->log();
    else qDebug() << "done";


    qDebug() << "Loading post-processing shaders (fragment shader)";


    QFile fFile(":/resources/filters_3d.frag");
    fFile.open(QFile::ReadOnly);
    QTextStream inf(&fFile);
    shaderCode = inf.readAll();


    QVector<QString> filters_list;
    filters_list.push_back("NORMAL_FILTER");
    filters_list.push_back("GAUSSIAN_BLUR_FILTER");
    filters_list.push_back("BLOOM_FILTER");
    filters_list.push_back("DOF_FILTER");
    filters_list.push_back("TONE_MAPPING_FILTER");
    filters_list.push_back("LENS_FLARES_FILTER");


    for(int filter = 0 ; filter < filters_list.size() ; filter++ ){

        qDebug() << "Compiling filter:" << filters_list[filter];
        QString preambule = "#version 330 core\n"
                            "#define "+filters_list[filter]+";\n" ;

        fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
        fshader->compileSourceCode(preambule + shaderCode);
        if (!fshader->log().isEmpty()) qDebug() << fshader->log();


        filter_program = new QOpenGLShaderProgram(this);
        filter_program->addShader(vshader);
        filter_program->addShader(fshader);
        filter_program->bindAttributeLocation("positionIn", 0);
        GLCHK( filter_program->link() );

        GLCHK( filter_program->bind() );
        GLCHK( filter_program->setUniformValue("layerA" , 0) );
        GLCHK( filter_program->setUniformValue("layerB" , 1) );
        GLCHK( filter_program->setUniformValue("layerC" , 2) );
        GLCHK( filter_program->setUniformValue("layerD" , 3) );
        GLCHK( filter_program->setUniformValue("layerE" , 4) );
        GLCHK( filter_program->setUniformValue("layerF" , 5) );
        GLCHK( filter_program->setUniformValue("layerG" , 6) );

        post_processing_programs[filters_list[filter].toStdString()] = filter_program;
        GLCHK( filter_program->release());
        delete fshader;
    }
    if(vshader  != NULL) delete vshader;


    GLCHK( lensFlareColorsTexture = bindTexture(QImage(":/resources/lenscolor.png"),GL_TEXTURE_2D) );
    qDebug() << "Loading lensColors texture: (id=" << lensFlareColorsTexture << ")";
    GLCHK( lensDirtTexture = bindTexture(QImage(":/resources/lensdirt.png"),GL_TEXTURE_2D) );
    qDebug() << "Loading lensDirt texture: (id=" << lensDirtTexture << ")";
    GLCHK( lensStarTexture = bindTexture(QImage(":/resources/lensstar.png"),GL_TEXTURE_2D) );
    qDebug() << "Loading lensDirt texture: (id=" << lensStarTexture << ")";



    camera.position.setZ( -0 );
    camera.toggleFreeCamera(false);
    newCamera.toggleFreeCamera(false);

    lightDirection.position  = QVector3D(0.0,0.0,0.0);
    lightDirection.direction = QVector3D(0.0,0.0,-1.0);// set the light infront of the camera
    lightDirection.toggleFreeCamera(false);
    lightDirection.radius = 1;

    mesh        = new Mesh(QString(RESOURCE_BASE) + "Core/3D/","Cube.obj");
    skybox_mesh = new Mesh(QString(RESOURCE_BASE) + "Core/3D/","sky_cube.obj");
    env_mesh    = new Mesh(QString(RESOURCE_BASE) + "Core/3D/","sky_cube_env.obj");
    quad_mesh   = new Mesh(QString(RESOURCE_BASE) + "Core/3D/","quad.obj");

    m_prefiltered_env_map = new GLTextureCube(512);

    resizeFBOs();
    emit readyGL();
}

void GLWidget::paintGL()
{


     glReadBuffer(GL_BACK);
    // ---------------------------------------------------------
    // Drawing env
    // ---------------------------------------------------------
    bakeEnviromentalMaps();
    colorFBO->bindDefault();
    GLCHK( glViewport(0, 0, width(), height()) );

    if(cameraInterpolation < 1.0){
        double w = cameraInterpolation;
        camera.position = camera.position*(1-w) + newCamera.position * w;
        cameraInterpolation += 0.01;

    }



    // setting the camera viewpoint
    viewMatrix = camera.updateCamera();

    colorFBO->bind();

    GLCHK( glDisable(GL_CULL_FACE) );
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(zoom,ratio,0.1,350.0);




    // set to which FBO result will be drawn
    GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 , GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4,  attachments);
    GLCHK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

    GLCHK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

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
    // Drawing model
    // ---------------------------------------------------------
    QOpenGLShaderProgram* program_ptrs[2] = {program,line_program};


    GLCHK( glEnable(GL_CULL_FACE) );
    GLCHK( glEnable(GL_DEPTH_TEST) );            
    GLCHK( glCullFace(GL_BACK) );
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for(int pindex = 0 ; pindex < 2 ; pindex ++){

    QOpenGLShaderProgram* program_ptr = program_ptrs[pindex];
    GLCHK( program_ptr->bind() );

    GLCHK( program_ptr->setUniformValue("ProjectionMatrix", projectionMatrix) );

    objectMatrix.setToIdentity();
    if( fboIdPtrs[0] != NULL){
        float fboRatio = float((*(fboIdPtrs[0]))->width())/(*(fboIdPtrs[0]))->height();
        objectMatrix.scale(fboRatio,1,fboRatio);
    }
    if(mesh->isLoaded()){

        objectMatrix.scale(0.5/mesh->radius);
        objectMatrix.translate(-mesh->centre_of_mass);
    }
    modelViewMatrix = viewMatrix*objectMatrix;
    NormalMatrix = modelViewMatrix.normalMatrix();
    float mesh_scale = 0.5/mesh->radius;

    GLCHK( program_ptr->setUniformValue("ModelViewMatrix"       , modelViewMatrix) );
    GLCHK( program_ptr->setUniformValue("NormalMatrix"          , NormalMatrix) );
    GLCHK( program_ptr->setUniformValue("ModelMatrix"           , objectMatrix) );
    GLCHK( program_ptr->setUniformValue("meshScale"             , mesh_scale) );

    GLCHK( program_ptr->setUniformValue("lightPos"              , lightPosition) );

    GLCHK( program_ptr->setUniformValue("lightDirection"        , lightDirection.direction) );

    GLCHK( program_ptr->setUniformValue("cameraPos"             , camera.get_position()) );
    GLCHK( program_ptr->setUniformValue("gui_depthScale"        , display3Dparameters.depthScale) );
    GLCHK( program_ptr->setUniformValue("gui_uvScale"           , display3Dparameters.uvScale) );
    GLCHK( program_ptr->setUniformValue("gui_uvScaleOffset"     , display3Dparameters.uvOffset) );
    GLCHK( program_ptr->setUniformValue("gui_bSpecular"         , bToggleSpecularView) );
    if(FBOImageProporties::bConversionBaseMap){
        GLCHK( program_ptr->setUniformValue("gui_bDiffuse"          , false) );
    }else{
        GLCHK( program_ptr->setUniformValue("gui_bDiffuse"          , bToggleDiffuseView) );
    }

    GLCHK( program_ptr->setUniformValue("gui_bOcclusion"        , bToggleOcclusionView) );
    GLCHK( program_ptr->setUniformValue("gui_bHeight"           , bToggleHeightView) );
    GLCHK( program_ptr->setUniformValue("gui_bNormal"           , bToggleNormalView) );
    GLCHK( program_ptr->setUniformValue("gui_bRoughness"        , bToggleRoughnessView) );
    GLCHK( program_ptr->setUniformValue("gui_bMetallic"         , bToggleMetallicView) );
    GLCHK( program_ptr->setUniformValue("gui_shading_type"      , display3Dparameters.shadingType) );
    GLCHK( program_ptr->setUniformValue("gui_shading_model"     , display3Dparameters.shadingModel) );
    GLCHK( program_ptr->setUniformValue("gui_SpecularIntensity" , display3Dparameters.specularIntensity) );
    GLCHK( program_ptr->setUniformValue("gui_DiffuseIntensity"  , display3Dparameters.diffuseIntensity) );
    GLCHK( program_ptr->setUniformValue("gui_LightPower"        , display3Dparameters.lightPower) );
    GLCHK( program_ptr->setUniformValue("gui_LightRadius"       , display3Dparameters.lightRadius) );

    // number of mipmaps
    GLCHK( program_ptr->setUniformValue("num_mipmaps"   , m_env_map->numMipmaps ) );
    // 3D settings
    GLCHK( program_ptr->setUniformValue("gui_bUseCullFace"   , display3Dparameters.bUseCullFace) );
    GLCHK( program_ptr->setUniformValue("gui_bUseSimplePBR"  , display3Dparameters.bUseSimplePBR) );
    GLCHK( program_ptr->setUniformValue("gui_noTessSub"      , display3Dparameters.noTessSubdivision) );
    GLCHK( program_ptr->setUniformValue("gui_noPBRRays"      , display3Dparameters.noPBRRays) );

    if(display3Dparameters.bShowTriangleEdges && pindex == 0){
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glEnable( GL_POLYGON_OFFSET_FILL );
        glPolygonOffset( 1.0f, 1.0f );
        GLCHK( program_ptr->setUniformValue("gui_bShowTriangleEdges", true) );
        GLCHK( program_ptr->setUniformValue("gui_bMaterialsPreviewEnabled"      , true) );
    }else{
        if(display3Dparameters.bShowTriangleEdges){
            glDisable( GL_POLYGON_OFFSET_FILL );
            glEnable( GL_POLYGON_OFFSET_LINE );
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glPolygonOffset( -2.0f, -2.0f );
            glLineWidth(1.0f);

        }

        GLCHK( program_ptr->setUniformValue("gui_bShowTriangleEdges", display3Dparameters.bShowTriangleEdges) );

        // Material preview: M key : when triangles are disabled
        if(!display3Dparameters.bShowTriangleEdges)
            GLCHK( program_ptr->setUniformValue("gui_bMaterialsPreviewEnabled"      , bool(keyPressed == KEY_SHOW_MATERIALS)) );
    }



    if( fboIdPtrs[0] != NULL){

        int tindeks = 0;

        for(tindeks = 0 ; tindeks <= MATERIAL_TEXTURE ; tindeks++){ // skip grunge texture (not used in 3D view)
            GLCHK( glActiveTexture(GL_TEXTURE0+tindeks) );
            GLCHK( glBindTexture(GL_TEXTURE_2D, (*(fboIdPtrs[tindeks]))->texture()) );
        }

        GLCHK( glActiveTexture(GL_TEXTURE0 + tindeks ) );
        GLCHK(m_prefiltered_env_map->bind());

        tindeks++;
        GLCHK( glActiveTexture(GL_TEXTURE0 + tindeks) );
        GLCHK( m_env_map->bind());    
        GLCHK( mesh->drawMesh() );
        // set default active texture
        glActiveTexture(GL_TEXTURE0);
    }

    if(!display3Dparameters.bShowTriangleEdges) break;
    }// end of loop over triangles
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable( GL_POLYGON_OFFSET_LINE );
    // return to standard settings
    GLCHK( glDisable(GL_CULL_FACE) );  
    GLCHK( glDisable(GL_DEPTH_TEST) );


    // set to which FBO result will be drawn
    GLuint attachments2[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1,  attachments2);


    colorFBO->bindDefault();

    GLCHK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );



    // do post processing if materials are not shown
    if( keyPressed != KEY_SHOW_MATERIALS ){
        // -----------------------------------------------------------
        // Post processing:
        // 1. Bloom (can be disabled/enabled by gui)
        // -----------------------------------------------------------

        // enable of disable bloom effect
        if(display3Dparameters.bBloomEffect){
             applyGlowFilter(outputFBO->fbo);
             copyTexToFBO(outputFBO->fbo->texture(),colorFBO->fbo);

        }// end of if bloom effect

        // -----------------------------------------------------------
        // Post processing:
        // 2. DOF (can be disabled/enabled by gui)
        // -----------------------------------------------------------
        if(display3Dparameters.bDofEffect){
            applyDofFilter(colorFBO->fbo->texture(),outputFBO->fbo);
            copyTexToFBO(outputFBO->fbo->texture(),colorFBO->fbo);
        }

        // -----------------------------------------------------------
        // Post processing:
        // 3. Lens Flares (can be disabled/enabled by gui)
        // -----------------------------------------------------------
        if(display3Dparameters.bLensFlares){
            applyLensFlaresFilter(colorFBO->fbo->texture(),outputFBO->fbo);
            copyTexToFBO(outputFBO->fbo->texture(),colorFBO->fbo);
        }

        applyToneFilter(colorFBO->fbo->texture(),outputFBO->fbo);

        //copyTexToFBO(outputFBO->fbo->texture(),colorFBO->fbo);
        applyNormalFilter(outputFBO->fbo->texture());

    }else{ // end of if SHOW MATERIALS TEXTURE DISABLED
        GLCHK( applyNormalFilter(colorFBO->fbo->texture()));
    }

    GLCHK( filter_program->release() );

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
    glViewport(0,0,512,512);

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
    resizeFBOs();

    GLCHK( glViewport(0, 0, width, height) );
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    GLWidgetBase::mousePressEvent(event);

    setCursor(Qt::ClosedHandCursor);
    if (event->buttons() & Qt::RightButton) {
        setCursor(Qt::SizeAllCursor);
    }else if(event->buttons() & Qt::MiddleButton){
        setCursor(lightCursor);


    }else if((event->buttons() & Qt::LeftButton) && (keyPressed == Qt::Key_Shift) ){

        colorFBO->bind();
        glReadBuffer(GL_COLOR_ATTACHMENT1); // NormalFBO actually it contains World Space position
        vector< float > pixels( 1 * 1 * 4 );
        glReadPixels(event->pos().x(), height()-event->pos().y(), 1, 1,GL_RGBA, GL_FLOAT, &pixels[0]);
        QVector3D position = QVector3D(pixels[0],pixels[1],pixels[2]);

        // when clicked on mesh other wise it has to be skybox
        if(position.length() < 50.0){
            qDebug() << "Picked position pixel (" << event->pos().x() << " , " << height()-event->pos().y() << ") with position:" << position;
            colorFBO->bindDefault();
            QVector3D curr_pos = camera.position - camera.radius * camera.direction;
            QVector3D new_dir = (position - curr_pos);
            new_dir.normalize();
            // update new Camera position
            cameraInterpolation = 0 ; // reset camera interpolation 'clock'
            newCamera.position  = position;
            newCamera.direction = new_dir;
            newCamera.radius    =-QVector3D::dotProduct(curr_pos - position,new_dir);
            glReadBuffer(GL_BACK);
            keyPressed = (Qt::Key)0;
            newCamera.side_direction = QVector3D(newCamera.direction.z(),0,-newCamera.direction.x());
            newCamera.rotateView(0,0);
        }
        emit changeCamPositionApplied(false);

    }


    updateGL();
    // capture the pixel color if material preview is enabled
    if((event->buttons() & Qt::LeftButton) && keyPressed == KEY_SHOW_MATERIALS){

        vector< unsigned char > pixels( 1 * 1 * 4 );

        glReadPixels(event->pos().x(), height()-event->pos().y(), 1, 1,GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
        QColor color = QColor(pixels[0],pixels[1],pixels[2],pixels[3]);

        qDebug() << "Picked material pixel (" << event->pos().x() << " , " << height()-event->pos().y() << ") with color:" << color;
        emit materialColorPicked(color);

    }


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

void GLWidget::relativeMouseMoveEvent(int dx, int dy, bool* wrapMouse, Qt::MouseButtons buttons)
{
    if ((buttons & Qt::LeftButton) && (buttons & Qt::RightButton)) {

    }else if (buttons & Qt::LeftButton) {
        camera.rotateView(dx/1.0,dy/1.0);
    } else if (buttons & Qt::RightButton) {
        camera.position +=QVector3D(dx/500.0,dy/500.0,0)*camera.radius;
    } else if (buttons & Qt::MiddleButton) {

        lightPosition += QVector4D(0.05*dx,-0.05*dy,-0,0);
        if(lightPosition.x() > +10.0) lightPosition.setX(+10.0);
        if(lightPosition.x() < -10.0) lightPosition.setX(-10.0);
        if(lightPosition.y() > +10.0) lightPosition.setY(+10.0);
        if(lightPosition.y() < -10.0) lightPosition.setY(-10.0);
        lightDirection.rotateView(-2*dx/1.0,2*dy/1.0);
    }else{
        *wrapMouse = false;
    }
}
//! [10]

void GLWidget::wheelEvent(QWheelEvent *event){
    int numDegrees = -event->delta();
    camera.mouseWheelMove((numDegrees));

    updateGL();
}

void GLWidget::dropEvent(QDropEvent *event)
{

    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int i = 0;
    QString localPath = droppedUrls[i].toLocalFile();
    QFileInfo fileInfo(localPath);

    loadMeshFile(fileInfo.absoluteFilePath(),false);


    event->acceptProposedAction();

}

void GLWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
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
        case(ROUGHNESS_TEXTURE ):
            fboIdPtrs[5] = pointer;
            break;
        case(METALLIC_TEXTURE ):
            fboIdPtrs[6] = pointer;
            break;
        case(MATERIAL_TEXTURE ):
            fboIdPtrs[7] = pointer;
            break;
        default:
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
      new_mesh = new Mesh(QString(RESOURCE_BASE) + "Core/3D/",fileName+QString(".obj"));
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


void GLWidget::chooseSkyBox(QString cubeMapName,bool bFirstTime){
    QStringList list;
    makeCurrent();
    list << QString(RESOURCE_BASE) + "Core/2D/skyboxes/" + cubeMapName + "/posx.jpg" << QString(RESOURCE_BASE) + "Core/2D/skyboxes/" + cubeMapName  + "/negx.jpg" << QString(RESOURCE_BASE) + "Core/2D/skyboxes/" + cubeMapName + "/posy.jpg"
         << QString(RESOURCE_BASE) + "Core/2D/skyboxes/" + cubeMapName + "/negy.jpg" << QString(RESOURCE_BASE) + "Core/2D/skyboxes/" + cubeMapName  + "/posz.jpg" << QString(RESOURCE_BASE) + "Core/2D/skyboxes/" + cubeMapName + "/negz.jpg";

    qDebug() << "Reading new cube map:" << list;
    bDiffuseMapBaked     = false;

    if(m_env_map != NULL) delete m_env_map;
    m_env_map     = new GLTextureCube(list);

    if(m_env_map->failed()){
        qWarning() << "Cannot load cube map: check if images listed above exist.";
    }
    // skip this when loading first cube map
    if(!bFirstTime)updateGL();
    else qDebug() << "Skipping glWidget repainting during first Env. maps. load.";
}

void GLWidget::updatePerformanceSettings(Display3DSettings settings){
    qDebug() << "Changing 3D settings";
    display3Dparameters = settings;
    updateGL();
}



// ------------------------------------------------------------------------------- //
//                          POST PROCESSING TOOLS
// ------------------------------------------------------------------------------- //
void GLWidget::resizeFBOs(){

    if(colorFBO != NULL) delete colorFBO;
    colorFBO = new GLFrameBufferObject(width(),height());
    colorFBO->addTexture(GL_COLOR_ATTACHMENT1);
    colorFBO->addTexture(GL_COLOR_ATTACHMENT2);
    colorFBO->addTexture(GL_COLOR_ATTACHMENT3);

    if(outputFBO != NULL) delete outputFBO;
    outputFBO = new GLFrameBufferObject(width(),height());

    if(auxFBO != NULL) delete auxFBO;
    auxFBO = new GLFrameBufferObject(width(),height());

    for(int i = 0; i < 4 ; i++){

        if(glowInputColor[i]  != NULL) delete glowInputColor[i];
        if(glowOutputColor[i] != NULL) delete glowOutputColor[i];
        glowInputColor[i]  = new GLFrameBufferObject(width()/pow(2.0,i+1),height()/pow(2.0,i+1));
        glowOutputColor[i] = new GLFrameBufferObject(width()/pow(2.0,i+1),height()/pow(2.0,i+1));
    }

}
void GLWidget::deleteFBOs(){
    colorFBO->bindDefault();
    delete colorFBO;
    delete outputFBO;
    delete auxFBO;
    for(int i = 0; i < 4 ; i++){
        if(glowInputColor[i]  != NULL) delete glowInputColor[i];
        if(glowOutputColor[i] != NULL) delete glowOutputColor[i];
    }
}

void GLWidget::applyNormalFilter(GLuint input_tex){

    filter_program = post_processing_programs["NORMAL_FILTER"];
    filter_program->bind();
    GLCHK( glViewport(0,0,width(),height()) );
    //GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_filter"]) );

    GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, input_tex) );

    GLCHK( quad_mesh->drawMesh(true) );


}

void GLWidget::copyTexToFBO(GLuint input_tex,QGLFramebufferObject* dst){

    filter_program = post_processing_programs["NORMAL_FILTER"];
    filter_program->bind();
    dst->bind();
    GLCHK( glViewport(0,0,dst->width(),dst->height()) );
  //  GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_filter"]) );
    GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, input_tex) );
    quad_mesh->drawMesh(true);
    dst->bindDefault();

}

void GLWidget::applyGaussFilter(  GLuint input_tex,
                                  QGLFramebufferObject* auxFBO,
                                  QGLFramebufferObject* outputFBO, float radius){



    filter_program = post_processing_programs["GAUSSIAN_BLUR_FILTER"];
    filter_program->bind();

    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( filter_program->setUniformValue("gui_gauss_w"       , (float)radius ));
    GLCHK( filter_program->setUniformValue("gui_gauss_radius"  , (float)radius ));
    GLCHK( filter_program->setUniformValue("gui_gauss_mode"    , 0 ));
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, input_tex) );

    auxFBO->bind();
    quad_mesh->drawMesh(true);
    auxFBO->bindDefault();

    outputFBO->bind();
        GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
        GLCHK( filter_program->setUniformValue("gui_gauss_mode"    , 1 ));
        GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
        quad_mesh->drawMesh(true);
    outputFBO->bindDefault();


}

void GLWidget::applyDofFilter(GLuint input_tex,
                QGLFramebufferObject* outputFBO){

    //applyGaussFilter(input_tex,auxFBO,outputFBO,15.0);

    filter_program = post_processing_programs["DOF_FILTER"];
    filter_program->bind();

    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
   // GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_dof_filter"]) );
    GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, input_tex) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, colorFBO->getAttachedTexture(2)) );


    outputFBO->bind();
        quad_mesh->drawMesh(true);
    outputFBO->bindDefault();
    GLCHK( glActiveTexture(GL_TEXTURE0) );


}



void GLWidget::applyGlowFilter(QGLFramebufferObject* outputFBO){




    applyGaussFilter(colorFBO->getAttachedTexture(1),glowInputColor[0]->fbo,glowOutputColor[0]->fbo);
    applyGaussFilter(glowOutputColor[0]->fbo->texture(),glowInputColor[0]->fbo,glowOutputColor[0]->fbo);


    applyGaussFilter(glowOutputColor[0]->fbo->texture(),glowInputColor[1]->fbo,glowOutputColor[1]->fbo);
    applyGaussFilter(glowOutputColor[1]->fbo->texture(),glowInputColor[1]->fbo,glowOutputColor[1]->fbo);

    applyGaussFilter(glowOutputColor[1]->fbo->texture(),glowInputColor[2]->fbo,glowOutputColor[2]->fbo);
    applyGaussFilter(glowOutputColor[2]->fbo->texture(),glowInputColor[2]->fbo,glowOutputColor[2]->fbo);

    applyGaussFilter(glowOutputColor[2]->fbo->texture(),glowInputColor[3]->fbo,glowOutputColor[3]->fbo);


    filter_program = post_processing_programs["BLOOM_FILTER"];
    filter_program->bind();

    outputFBO->bind();
        GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
     //   GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_bloom_filter"]) );
        GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
        GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, colorFBO->fbo->texture()) );
        GLCHK( glActiveTexture(GL_TEXTURE1) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[0]->fbo->texture()) );
        GLCHK( glActiveTexture(GL_TEXTURE2) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[1]->fbo->texture()) );
        GLCHK( glActiveTexture(GL_TEXTURE3) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[2]->fbo->texture()) );
        GLCHK( glActiveTexture(GL_TEXTURE4) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[3]->fbo->texture()) );
        quad_mesh->drawMesh(true);

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    outputFBO->bindDefault();
}


void GLWidget::applyToneFilter(GLuint input_tex,QGLFramebufferObject* outputFBO){


    filter_program = post_processing_programs["TONE_MAPPING_FILTER"];
    filter_program->bind();
    outputFBO->bind();
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, input_tex) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[3]->fbo->texture()) );
    quad_mesh->drawMesh(true);
    outputFBO->bindDefault();
    GLCHK( glActiveTexture(GL_TEXTURE0) );
}

void GLWidget::applyLensFlaresFilter(GLuint input_tex,QGLFramebufferObject* outputFBO){
    // Based on: http://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html
    // prepare mask image
    if(!display3Dparameters.bBloomEffect){
        applyGaussFilter(colorFBO->getAttachedTexture(1),glowInputColor[0]->fbo,glowOutputColor[0]->fbo);
        applyGaussFilter(glowOutputColor[0]->fbo->texture(),glowInputColor[0]->fbo,glowOutputColor[0]->fbo);
    }

    filter_program = post_processing_programs["LENS_FLARES_FILTER"];
    filter_program->bind();

    // First step -- prepare treshold image

    glowInputColor[0]->fbo->bind();
    GLCHK( glViewport(0,0,glowInputColor[0]->fbo->width(),glowInputColor[0]->fbo->height()) );
    GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( filter_program->setUniformValue("lf_step"  , int(0)) );// treshold step

    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[0]->fbo->texture()) );
    quad_mesh->drawMesh(true);

    // Second step -- create ghosts and halos

    glowOutputColor[0]->fbo->bind();
    GLCHK( glViewport(0,0,glowOutputColor[0]->fbo->width(),glowOutputColor[0]->fbo->height()) );

    GLCHK( filter_program->setUniformValue("lf_step"  , int(1)) );//II step

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, input_tex) );
    GLCHK( glActiveTexture(GL_TEXTURE2) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, lensFlareColorsTexture) );

    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, glowInputColor[0]->fbo->texture()) );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    quad_mesh->drawMesh(true);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    outputFBO->bindDefault();

    float camrot = QVector3D::dotProduct(camera.side_direction,QVector3D(1,0,0)) +
                   QVector3D::dotProduct(camera.updown_direction,QVector3D(0,1,0));


    QMatrix4x4 scaleBias1 = QMatrix4x4(
       2.0f,   0.0f,  -1.0f, 0.0f,
       0.0f,   2.0f,  -1.0f, 0.0f,
       0.0f,   0.0f,   1.0f, 0.0f,
       0.0f,   0.0f,   0.0f, 1.0f
    );
    QMatrix4x4 rotation = QMatrix4x4(
       cos(camrot), -sin(camrot), 0.0f, 0.0f,
       sin(camrot), cos(camrot),  0.0f, 0.0f,
       0.0f,        0.0f,         1.0f, 0.0f,
       0.0f,   0.0f,   0.0f, 1.0f
    );
    QMatrix4x4 scaleBias2 = QMatrix4x4(
       0.5f,   0.0f,   0.5f, 0.0f,
       0.0f,   0.5f,   0.5f, 0.0f,
       0.0f,   0.0f,   1.0f, 0.0f,
       0.0f,   0.0f,   0.0f, 1.0f
    );

    QMatrix4x4 uLensStarMatrix = scaleBias2 * rotation * scaleBias1;

     // Third step -- blend images (orginal,halos,dirt texture, and star texture)


    outputFBO->bind();
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( filter_program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( filter_program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( filter_program->setUniformValue("lf_step"  , int(2)) );// 3
    GLCHK( filter_program->setUniformValue("lf_starMatrix"  , uLensStarMatrix) );// 3

    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[0]->fbo->texture()) );// ghost texture
    GLCHK( glActiveTexture(GL_TEXTURE2) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, lensDirtTexture) ); // dirt texture
    GLCHK( glActiveTexture(GL_TEXTURE3) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, lensStarTexture) ); // star texture
    GLCHK( glActiveTexture(GL_TEXTURE4) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, glowOutputColor[3]->fbo->texture()) ); // exposure reference
    quad_mesh->drawMesh(true);

    GLCHK( glActiveTexture(GL_TEXTURE0) );
}
