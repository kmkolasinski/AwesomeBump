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
#include "glimageeditor.h"



GLImage::GLImage(QWidget *parent)
    : GLWidgetBase(QGLFormat::defaultFormat(), parent)

{
    bShadowRender         = false;
    bSkipProcessing       = false;
    bRendering            = false;
    bToggleColorPicking   = false;
    conversionType        = CONVERT_NONE;
    uvManilupationMethod  = UV_TRANSLATE;
    cornerWeights         = QVector4D(0,0,0,0);
    fboRatio = 1;
    renderFBO		  = NULL;
    paintFBO		  = NULL;

    // initialize position of the corners
    cornerPositions[0] = QVector2D(-0.0,-0);
    cornerPositions[1] = QVector2D( 1,-0);
    cornerPositions[2] = QVector2D( 1, 1);
    cornerPositions[3] = QVector2D(-0, 1);
    for(int i = 0 ; i < 4 ; i++){
        grungeCornerPositions[i] = cornerPositions[i];
    }
    draggingCorner       = -1;
    gui_perspective_mode =  0;
    gui_seamless_mode    =  0;
    setCursor(Qt::OpenHandCursor);
    cornerCursors[0] = QCursor(QPixmap(":/resources/cursors/corner1.png"));
    cornerCursors[1] = QCursor(QPixmap(":/resources/cursors/corner2.png"));
    cornerCursors[2] = QCursor(QPixmap(":/resources/cursors/corner3.png"));
    cornerCursors[3] = QCursor(QPixmap(":/resources/cursors/corner4.png"));
    activeImage = NULL;
    connect(this,SIGNAL(rendered()),this,SLOT(copyRenderToPaintFBO()));
}

GLImage::~GLImage()
{
  cleanup();
}

void GLImage::cleanup()
{
  makeCurrent();
  averageColorFBO->bindDefault();
  typedef std::map<std::string,QOpenGLShaderProgram*>::iterator it_type;
  qDebug() << "Removing GLImage filters:";
  for(it_type iterator = filter_programs.begin(); iterator != filter_programs.end(); iterator++) {
      qDebug() << "Removing filter:" << QString(iterator->first.c_str());
      delete iterator->second;
  }

  delete averageColorFBO;
  delete samplerFBO1;
  delete samplerFBO2;
  delete auxFBO1;
  delete auxFBO2;
  delete auxFBO3;
  delete auxFBO4;

  for(int i = 0; i < 3 ; i++){
      delete auxFBO0BMLevels[i] ;
      delete auxFBO1BMLevels[i] ;
      delete auxFBO2BMLevels[i] ;
  }
  delete  paintFBO;
  delete renderFBO;


  GLCHK(glDeleteBuffers(sizeof(vbos)/sizeof(GLuint), &vbos[0]));
  GLCHK(glDeleteVertexArrays(1, &screen_vao));
  doneCurrent();
}

QSize GLImage::minimumSizeHint() const
{
    return QSize(100, 100);

}

QSize GLImage::sizeHint() const
{
    return QSize(500, 400);
}

void GLImage::initializeGL()
{

    initializeOpenGLFunctions();

    qDebug() << "calling " << Q_FUNC_INFO;
    
    QColor clearColor = QColor::fromCmykF(0.79, 0.79, 0.79, 0.0).dark();
    GLCHK( glClearColor((GLfloat)clearColor.red() / 255.0, (GLfloat)clearColor.green() / 255.0,
			(GLfloat)clearColor.blue() / 255.0, (GLfloat)clearColor.alpha() / 255.0) );
    GLCHK( glEnable(GL_MULTISAMPLE) );
    GLCHK( glEnable(GL_DEPTH_TEST) );



    QVector<QString> filters_list;
    filters_list.push_back("mode_normal_filter");
    filters_list.push_back("mode_height_to_normal");
    filters_list.push_back("mode_perspective_transform_filter");
    filters_list.push_back("mode_seamless_linear_filter");
    filters_list.push_back("mode_seamless_filter");
    filters_list.push_back("mode_occlusion_filter");
    filters_list.push_back("mode_normal_to_height");
    filters_list.push_back("mode_normalize_filter");
    filters_list.push_back("mode_gauss_filter");
    filters_list.push_back("mode_gray_scale_filter");
    filters_list.push_back("mode_invert_components_filter");
    filters_list.push_back("mode_color_hue_filter");
    filters_list.push_back("mode_roughness_filter");
    filters_list.push_back("mode_dgaussians_filter");
    filters_list.push_back("mode_constrast_filter");
    filters_list.push_back("mode_height_processing_filter");
    filters_list.push_back("mode_remove_low_freq_filter");
    filters_list.push_back("mode_invert_filter");
    filters_list.push_back("mode_overlay_filter");
    filters_list.push_back("mode_ao_cancellation_filter");
    filters_list.push_back("mode_small_details_filter");
    filters_list.push_back("mode_medium_details_filter");
    filters_list.push_back("mode_sharpen_blur");
    filters_list.push_back("mode_normals_step_filter");
    filters_list.push_back("mode_normal_mixer_filter");
    filters_list.push_back("mode_sobel_filter");
    filters_list.push_back("mode_normal_expansion_filter");
    filters_list.push_back("mode_mix_normal_levels_filter");
    filters_list.push_back("mode_combine_normal_height_filter");
    filters_list.push_back("mode_roughness_color_filter");
    filters_list.push_back("mode_grunge_filter");
    filters_list.push_back("mode_grunge_randomization_filter");
    filters_list.push_back("mode_grunge_normal_warp_filter");
    filters_list.push_back("mode_normal_angle_correction_filter");
    filters_list.push_back("mode_add_noise_filter");



    qDebug() << "Loading filters (fragment shader)";
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    vshader->compileSourceFile(":/resources/shaders/filters.vert");
    if (!vshader->log().isEmpty()) qDebug() << vshader->log();
    else qDebug() << "done";

    QFile fFile(":/resources/shaders/filters.frag");
    fFile.open(QFile::ReadOnly);
    QTextStream inf(&fFile);
    QString shaderCode = inf.readAll();

#ifdef USE_OPENGL_330

    for(int filter = 0 ; filter < filters_list.size() ; filter++ ){

        qDebug() << "Compiling filter:" << filters_list[filter];
        QString preambule = "#version 330 core\n"
                            "#define USE_OPENGL_330\n"
                            "#define "+filters_list[filter]+"_330\n" ;

        QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
        fshader->compileSourceCode(preambule + shaderCode);
        if (!fshader->log().isEmpty()) qDebug() << fshader->log();

        program = new QOpenGLShaderProgram(this);
        program->addShader(vshader);
        program->addShader(fshader);
        program->bindAttributeLocation("positionIn", 0);
        GLCHK( program->link() );

        GLCHK( program->bind() );
        GLCHK( program->setUniformValue("layerA" , 0) );
        GLCHK( program->setUniformValue("layerB" , 1) );
        GLCHK( program->setUniformValue("layerC" , 2) );
        GLCHK( program->setUniformValue("layerD" , 3) );
        GLCHK( program->setUniformValue("materialTexture" ,10) );

        filter_programs[filters_list[filter].toStdString()] = program;
        GLCHK( program->release());
        delete fshader;

    }

    delete vshader;

#else



    qDebug() << "Loading filters (vertex shader)";
    QString preambule = "#version 400 core\n";

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    fshader->compileSourceCode(preambule + shaderCode);
    if (!fshader->log().isEmpty()) qDebug() << fshader->log();
    else qDebug() << "done";



    program = new QOpenGLShaderProgram(this);
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("positionIn", 0);
    GLCHK( program->link() );

    GLCHK( program->bind() );
    GLCHK( program->setUniformValue("layerA" , 0) );
    GLCHK( program->setUniformValue("layerB" , 1) );
    GLCHK( program->setUniformValue("layerC" , 2) );
    GLCHK( program->setUniformValue("layerD" , 3) );
    GLCHK( program->setUniformValue("materialTexture" ,10) );

    delete vshader;
    delete fshader;


    GLCHK( subroutines["mode_normal_filter"]               = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_normal_filter") );
    GLCHK( subroutines["mode_color_hue_filter"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_color_hue_filter") );
    GLCHK( subroutines["mode_overlay_filter"]              = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_overlay_filter") );
    GLCHK( subroutines["mode_ao_cancellation_filter"]      = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_ao_cancellation_filter") );
    GLCHK( subroutines["mode_invert_filter"]               = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_invert_filter") );
    GLCHK( subroutines["mode_gauss_filter"]                = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_gauss_filter") );
    GLCHK( subroutines["mode_seamless_filter"]             = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_seamless_filter") );
    GLCHK( subroutines["mode_seamless_linear_filter"]      = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_seamless_linear_filter") );
    GLCHK( subroutines["mode_dgaussians_filter"]           = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_dgaussians_filter") );
    GLCHK( subroutines["mode_constrast_filter"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_constrast_filter") );
    GLCHK( subroutines["mode_small_details_filter"]        = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_small_details_filter") );
    GLCHK( subroutines["mode_gray_scale_filter"]           = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_gray_scale_filter") );
    GLCHK( subroutines["mode_medium_details_filter"]       = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_medium_details_filter") );
    GLCHK( subroutines["mode_height_to_normal"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_height_to_normal") );
    GLCHK( subroutines["mode_sharpen_blur"]                = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_sharpen_blur") );
    GLCHK( subroutines["mode_normals_step_filter"]         = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_normals_step_filter") );
    GLCHK( subroutines["mode_normal_mixer_filter"]         = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_normal_mixer_filter") );
    GLCHK( subroutines["mode_invert_components_filter"]    = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_invert_components_filter") );
    GLCHK( subroutines["mode_normal_to_height"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_normal_to_height") );
    GLCHK( subroutines["mode_sobel_filter"]                = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_sobel_filter") );
    GLCHK( subroutines["mode_normal_expansion_filter"]     = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_normal_expansion_filter") );
    GLCHK( subroutines["mode_mix_normal_levels_filter"]    = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_mix_normal_levels_filter") );
    GLCHK( subroutines["mode_normalize_filter"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_normalize_filter") );    
    GLCHK( subroutines["mode_occlusion_filter"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_occlusion_filter") );
    GLCHK( subroutines["mode_combine_normal_height_filter"]= glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_combine_normal_height_filter") );
    GLCHK( subroutines["mode_perspective_transform_filter"]= glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_perspective_transform_filter") );
    GLCHK( subroutines["mode_height_processing_filter"]    = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_height_processing_filter" ) );
    GLCHK( subroutines["mode_roughness_filter"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_roughness_filter" ) );
    GLCHK( subroutines["mode_roughness_color_filter"]      = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_roughness_color_filter" ) );
    GLCHK( subroutines["mode_remove_low_freq_filter"]      = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_remove_low_freq_filter" ) );
    GLCHK( subroutines["mode_grunge_filter"]               = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_grunge_filter" ) );
    GLCHK( subroutines["mode_grunge_randomization_filter"] = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_grunge_randomization_filter" ) );
    GLCHK( subroutines["mode_grunge_normal_warp_filter"]   = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_grunge_normal_warp_filter" ) );
    GLCHK( subroutines["mode_normal_angle_correction_filter"]   = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_normal_angle_correction_filter" ) );
    GLCHK( subroutines["mode_add_noise_filter"]            = glGetSubroutineIndex(program->programId(),GL_FRAGMENT_SHADER,"mode_add_noise_filter" ) );


#endif

    makeScreenQuad();

    averageColorFBO = NULL;
    samplerFBO1     = NULL;
    samplerFBO2     = NULL;
    FBOImages::create(averageColorFBO,256,256);
    FBOImages::create(samplerFBO1,1024,1024);
    FBOImages::create(samplerFBO2,1024,1024);

    auxFBO1 = NULL;
    auxFBO2 = NULL;
    auxFBO3 = NULL;
    auxFBO4 = NULL;
    for(int i = 0; i < 3 ; i++){
        auxFBO0BMLevels[i] = NULL;
        auxFBO1BMLevels[i] = NULL;
        auxFBO2BMLevels[i] = NULL;
    }
    paintFBO   = NULL;
    emit readyGL();
}

void GLImage::paintGL()
{

    // Perform filters on images and render the final result to renderFBO
    // avoid rendering function if there is rendered something already
    if(!bSkipProcessing && !bRendering){        
        bRendering = true;
        render();
    }


    bSkipProcessing = false;
    conversionType  = CONVERT_NONE;

    // Draw current FBO using current image after rendering the paintFBO will be replaced
    if(!bShadowRender){

        //if (!activeImage) return;
        if (paintFBO != NULL){ // since grunge map can be different we need to calculate ratio each time
          fboRatio = float(paintFBO->width())/paintFBO->height();
          orthographicProjHeight = (1+zoom)/windowRatio;
          orthographicProjWidth  = (1+zoom)/fboRatio;
        }
        GLCHK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
        GLCHK( glDisable(GL_CULL_FACE) );
        GLCHK( glDisable(GL_DEPTH_TEST) );

        GLCHK(glBindVertexArray(screen_vao));
        // positions
        GLCHK(glBindBuffer(GL_ARRAY_BUFFER, vbos[0]));
        GLCHK(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)0));
        // indices
        GLCHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]));

        QGLFramebufferObject* activeFBO     = paintFBO;

        #ifdef USE_OPENGL_330
            program = filter_programs["mode_normal_filter"];
            program->bind();
        #else
            GLCHK(glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_filter"]));
        #endif

        // Displaying new image
        activeFBO->bindDefault();
        program->setUniformValue("quad_draw_mode", 1);

        GLCHK( glViewport(0,0,width(),height()) );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, activeFBO->texture()) );

        QMatrix4x4 m;
        m.ortho(0,orthographicProjWidth,0,orthographicProjHeight,-1,1);
        GLCHK( program->setUniformValue("ProjectionMatrix", m) );
        m.setToIdentity();
        m.translate(xTranslation,yTranslation,0);
        GLCHK( program->setUniformValue("ModelViewMatrix", m) );
        GLCHK( program->setUniformValue("material_id", int(-1)) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
        GLCHK( program->setUniformValue("quad_draw_mode", int(0)) );        
        GLCHK(glBindVertexArray(0));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}



void GLImage::render(){

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (!activeImage) return;
    if ( activeImage->fbo){ // since grunge map can be different we need to calculate ratio each time
      fboRatio = float(activeImage->fbo->width())/activeImage->fbo->height();
      orthographicProjHeight = (1+zoom)/windowRatio;
      orthographicProjWidth  = (1+zoom)/fboRatio;
    }
    // do not clear the background during rendering process
    if(!bShadowRender){
        GLCHK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    }

    GLCHK( glDisable(GL_CULL_FACE) );
    GLCHK( glDisable(GL_DEPTH_TEST) );
    GLCHK(glBindVertexArray(screen_vao));

    // positions
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)0);
    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);

    QGLFramebufferObject* activeFBO = activeImage->fbo;


    bool bTransformUVs = true; // images which depend on others will not be affected by UV changes again
    bool bSkipStandardProcessing = false;


    if((activeImage->bSkipProcessing) && (activeImage->imageType != MATERIAL_TEXTURE)) bSkipProcessing = true;// do not process images when is disabled
    if(bToggleColorPicking) bSkipStandardProcessing = true;


    if(!bSkipProcessing == true){
    // resizing the FBOs in case of convertion procedure
    switch(conversionType){
        case(CONVERT_FROM_H_TO_N):

        break;
        case(CONVERT_FROM_N_TO_H):

        break;
        case(CONVERT_FROM_D_TO_O):

        break;
        case(CONVERT_RESIZE): // apply resize textures
            activeImage->resizeFBO(resize_width,resize_height);
            // pointers were changed in resize function
            activeFBO  = activeImage->fbo;

            bSkipStandardProcessing = true;
        break;
        default:
        break;
    }

    // create or resize when image was changed
    FBOImages::resize(auxFBO1,activeFBO->width(),activeFBO->height());
    FBOImages::resize(auxFBO2,activeFBO->width(),activeFBO->height());
    FBOImages::resize(auxFBO3,activeFBO->width(),activeFBO->height());
    FBOImages::resize(auxFBO4,activeFBO->width(),activeFBO->height());
    // allocate additional FBOs when conversion from BaseMap is enabled
    if(activeImage->imageType == DIFFUSE_TEXTURE && activeImage->bConversionBaseMap){
        for(int i = 0; i < 3 ; i++){
            FBOImages::resize(auxFBO0BMLevels[i],activeFBO->width()/pow(2,i+1),activeFBO->height()/pow(2,i+1));
            FBOImages::resize(auxFBO1BMLevels[i],activeFBO->width()/pow(2,i+1),activeFBO->height()/pow(2,i+1));
            FBOImages::resize(auxFBO2BMLevels[i],activeFBO->width()/pow(2,i+1),activeFBO->height()/pow(2,i+1));
        }
    }else{// other wise "delete" unnecessary FBOs (I know this is stupid...)
        int small_w_h = 1;
        for(int i = 0; i < 3 ; i++){
            FBOImages::resize(auxFBO0BMLevels[i],small_w_h,small_w_h);
            FBOImages::resize(auxFBO1BMLevels[i],small_w_h,small_w_h);
            FBOImages::resize(auxFBO2BMLevels[i],small_w_h,small_w_h);
        }
    }

    GLCHK( program->bind() );
    GLCHK( program->setUniformValue("gui_image_type", activeImage->imageType) );    
    GLCHK( program->setUniformValue("gui_depth", float(1.0)) );
    GLCHK( program->setUniformValue("gui_mode_dgaussian", 1) );
    GLCHK( program->setUniformValue("material_id", int(activeImage->currentMaterialIndeks) ) );
    openGL330ForceTexType = activeImage->imageType;


    if(activeImage->bFirstDraw){
        resetView();
        qDebug() << "Doing first draw of" << PostfixNames::getTextureName(activeImage->imageType) << " texture.";
        activeImage->bFirstDraw = false;
    }

    // skip all precessing when material tab is selected
    if(activeImage->imageType == MATERIAL_TEXTURE){
        bSkipStandardProcessing = true;
        GLCHK( program->setUniformValue("material_id", int(-1) ) );
    }
    if(activeImage->imageType == GRUNGE_TEXTURE){
        bTransformUVs = false;
        GLCHK( program->setUniformValue("material_id", int(MATERIALS_DISABLED) ) );
    }

    GLCHK( glActiveTexture(GL_TEXTURE10) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, targetImageMaterial->scr_tex_id) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );

//    if(int(activeImage->currentMaterialIndeks) < 0){
        copyTex2FBO(activeImage->scr_tex_id,activeImage->fbo);
//    }

    // in some cases the output image will be taken from other sources
    switch(activeImage->imageType){
        // ----------------------------------------------------
        //
        // ----------------------------------------------------
        case(NORMAL_TEXTURE):{
        // Choosing proper action

        switch(activeImage->inputImageType){
            case(INPUT_FROM_NORMAL_INPUT):
                if(conversionType == CONVERT_FROM_H_TO_N){
                    applyHeightToNormal(targetImageHeight->fbo,activeFBO);
                    bTransformUVs = false;
                }
                break;
            case(INPUT_FROM_HEIGHT_INPUT):
                // transform height before  normal calculation

                if(conversionType == CONVERT_NONE){
                    // perspective transformation treats differently normal texture
                    // change for a moment the texture type to performe transformations

                    openGL330ForceTexType = HEIGHT_TEXTURE;// used for GL3.30 version
                    GLCHK( program->setUniformValue("gui_image_type", HEIGHT_TEXTURE) );

                    copyTex2FBO(targetImageHeight->scr_tex_id,activeFBO);
                    applyAllUVsTransforms(activeFBO);

                    copyFBO(activeFBO,auxFBO1);
                    applyHeightToNormal(auxFBO1,activeFBO);
                    GLCHK( program->setUniformValue("gui_image_type", activeImage->imageType) );
                    openGL330ForceTexType = activeImage->imageType;
                    bTransformUVs = false;
                }else{                    
                    copyTex2FBO(targetImageHeight->scr_tex_id,activeFBO);
                }

                break;
            case(INPUT_FROM_HEIGHT_OUTPUT):
                applyHeightToNormal(targetImageHeight->fbo,activeFBO);

                if(!targetImageHeight->bSkipProcessing)bTransformUVs = false;
                break;
            default: break;
        }
        break;}// end of case Normal
        // ----------------------------------------------------
        //
        // ----------------------------------------------------
        case(SPECULAR_TEXTURE):{
        // Choosing proper action

        switch(activeImage->inputImageType){
            case(INPUT_FROM_SPECULAR_INPUT):
                // do nothing
                break;
            case(INPUT_FROM_HEIGHT_INPUT):                
                copyTex2FBO(targetImageHeight->scr_tex_id,activeFBO);                
                break;
            case(INPUT_FROM_HEIGHT_OUTPUT):
                copyFBO(targetImageHeight->fbo,activeFBO);
                if(!targetImageHeight->bSkipProcessing) bTransformUVs = false;
                break;
            case(INPUT_FROM_DIFFUSE_INPUT):                
                copyTex2FBO(targetImageDiffuse->scr_tex_id,activeFBO);                
                break;
            case(INPUT_FROM_DIFFUSE_OUTPUT):
                copyFBO(targetImageDiffuse->fbo,activeFBO);
                if(!targetImageDiffuse->bSkipProcessing) bTransformUVs = false;
                break;
            default: break;
        }
        break;}// end of case Specular
        // ----------------------------------------------------
        //
        // ----------------------------------------------------
        case(OCCLUSION_TEXTURE):{
        // Choosing proper action

        switch(activeImage->inputImageType){
            case(INPUT_FROM_OCCLUSION_INPUT):

                if(conversionType == CONVERT_FROM_HN_TO_OC){
                    // Ambient occlusion is calculated from normal and height map, so
                    // some part of processing is skiped                    
                    applyOcclusionFilter(targetImageHeight->fbo->texture(),targetImageNormal->fbo->texture(),activeFBO);
                    bSkipStandardProcessing =  true;
                    if(!targetImageHeight->bSkipProcessing && !targetImageNormal->bSkipProcessing) bTransformUVs = false;
                    qDebug() << "Calculation AO from Normal and Height";
                }

                break;
            case(INPUT_FROM_HI_NI):
                // Ambient occlusion is calculated from normal and height map, so
                // some part of processing is skiped
                applyOcclusionFilter(targetImageHeight->scr_tex_id,targetImageNormal->scr_tex_id,activeFBO);


                break;     
            case(INPUT_FROM_HO_NO):
                applyOcclusionFilter(targetImageHeight->fbo->texture(),targetImageNormal->fbo->texture(),activeFBO);
                if(!targetImageHeight->bSkipProcessing && !targetImageNormal->bSkipProcessing) bTransformUVs = false;
                break;
            default: break;
        }
        break;}// end of case Occlusion
        // ----------------------------------------------------
        //
        // ----------------------------------------------------
        case(HEIGHT_TEXTURE):{
        if(conversionType == CONVERT_FROM_N_TO_H){
            applyNormalToHeight(activeImage,targetImageNormal->fbo,activeFBO,auxFBO1);
            applyCPUNormalizationFilter(auxFBO1,activeFBO);
            applyAddNoiseFilter(activeFBO,auxFBO1);
            copyFBO(auxFBO1,activeFBO);

            targetImageHeight->updateSrcTexId(activeFBO);
            if(!targetImageNormal->bSkipProcessing)  bTransformUVs = false;
        }
        // ----------------------------------------------------
        //
        // ----------------------------------------------------
        break;}// end of case Height
        case(ROUGHNESS_TEXTURE):{
        // Choosing proper action

        switch(activeImage->inputImageType){
            case(INPUT_FROM_ROUGHNESS_INPUT):
                // do nothing
                break;
            case(INPUT_FROM_HEIGHT_INPUT):                
                copyTex2FBO(targetImageHeight->scr_tex_id,activeFBO);
                break;
            case(INPUT_FROM_HEIGHT_OUTPUT):
                copyFBO(targetImageHeight->fbo,activeFBO);
                if(!targetImageHeight->bSkipProcessing)  bTransformUVs = false;
                break;
            case(INPUT_FROM_DIFFUSE_INPUT):                
                copyTex2FBO(targetImageDiffuse->scr_tex_id,activeFBO);
                break;
            case(INPUT_FROM_DIFFUSE_OUTPUT):
                copyFBO(targetImageDiffuse->fbo,activeFBO);
                if(!targetImageDiffuse->bSkipProcessing)  bTransformUVs = false;
                break;
            default: break;
        }

        break;
        }// end of case Roughness
        case(METALLIC_TEXTURE):{
        // Choosing proper action

        switch(activeImage->inputImageType){
            case(INPUT_FROM_METALLIC_INPUT):
                // do nothing
                break;
            case(INPUT_FROM_HEIGHT_INPUT):                
                copyTex2FBO(targetImageHeight->scr_tex_id,activeFBO);
                break;
            case(INPUT_FROM_HEIGHT_OUTPUT):
                copyFBO(targetImageHeight->fbo,activeFBO);
                if(!targetImageHeight->bSkipProcessing)  bTransformUVs = false;
                break;
            case(INPUT_FROM_DIFFUSE_INPUT):                
                copyTex2FBO(targetImageDiffuse->scr_tex_id,activeFBO);
                break;
            case(INPUT_FROM_DIFFUSE_OUTPUT):
                copyFBO(targetImageDiffuse->fbo,activeFBO);
                if(!targetImageDiffuse->bSkipProcessing)  bTransformUVs = false;
                break;
            default: break;
        }
        break;}// end of case Roughness        
        case(GRUNGE_TEXTURE):{
            applyGrungeWarpNormalFilter(activeFBO,auxFBO2);
            applyGrungeRandomizationFilter(auxFBO2,activeFBO);
        break;
        }// end of case Grunge
        default:break;
    };

    // apply grunge filter when enabled
    if(conversionType == CONVERT_NONE && GrungeProp.OverallWeight != 0.0f ){
        if(activeImage->imageType < MATERIAL_TEXTURE){

            copyTex2FBO(targetImageGrunge->fbo->texture(),auxFBO2);
            // when user choose source image "output" type one must
            // transform grunge map additionally
            if(bTransformUVs == false) applyAllUVsTransforms(auxFBO2); // auxFBO1 is used inside

            applyGrungeImageFilter(activeFBO,auxFBO1,auxFBO2);// auxFBO3 is used inside
            copyTex2FBO(auxFBO1->texture(),activeFBO);
        }// end of image type > MATERIAL
    }// end of if not conversion process

    // Transform UVs in some cases

    if(conversionType == CONVERT_NONE && bTransformUVs){

        applyAllUVsTransforms(activeFBO);
    }

    // skip all processing and when mouse is dragged
    if(!bSkipStandardProcessing){

    // begin standart pipe-line (for each image)
    applyInvertComponentsFilter(activeFBO,auxFBO1);
    copyFBO(auxFBO1,activeFBO);



    if(activeImage->imageType != HEIGHT_TEXTURE &&
       activeImage->imageType != NORMAL_TEXTURE &&
       activeImage->imageType != OCCLUSION_TEXTURE &&
       activeImage->imageType != ROUGHNESS_TEXTURE){

        // hue manipulation
        applyColorHueFilter(activeFBO,auxFBO1);
        copyFBO(auxFBO1,activeFBO);
    }



    if(BasicProp.GrayScale.EnableGrayScale ||
            activeImage->imageType == ROUGHNESS_TEXTURE ||
            activeImage->imageType == OCCLUSION_TEXTURE ||
            activeImage->imageType == HEIGHT_TEXTURE ){
        applyGrayScaleFilter(auxFBO1,activeFBO);
    }else{
        copyFBO(auxFBO1,activeFBO);
    }



    // specular manipulation
    if(SurfaceDetailsProp.EnableSurfaceDetails && activeImage->imageType != HEIGHT_TEXTURE){
        applyDGaussiansFilter(activeFBO,auxFBO2,auxFBO1);
        //copyFBO(auxFBO1,activeFBO);
        applyContrastFilter(auxFBO1,activeFBO);
    }


    // Removing shading...
    if(activeImage->properties->EnableRemoveShading){


        applyRemoveLowFreqFilter(activeFBO,auxFBO1,auxFBO2);
        copyFBO(auxFBO2,activeFBO);


        applyGaussFilter(activeFBO,auxFBO2,auxFBO1,1);
        applyInverseColorFilter(auxFBO1,auxFBO2);
        copyFBO(auxFBO2,auxFBO1);
        applyOverlayFilter(activeFBO,auxFBO1,auxFBO2);


        applyRemoveShadingFilter(auxFBO2,
                                targetImageOcclusion->fbo,
                                activeFBO,
                                auxFBO1);
        copyFBO(auxFBO1,activeFBO);
    }





    if(BasicProp.EnhanceDetails > 0){
        for(int i = 0 ; i < BasicProp.EnhanceDetails ; i++ ){
            applyGaussFilter(activeFBO,auxFBO2,auxFBO1,1);
            applyOverlayFilter(activeFBO,auxFBO1,auxFBO2);
            copyFBO(auxFBO2,activeFBO);
        }
    }

    if(BasicProp.SmallDetails  > 0.0f){
        applySmallDetailsFilter(activeFBO,auxFBO2,auxFBO1);
        copyFBO(auxFBO1,activeFBO);
    }


    if(BasicProp.MediumDetails > 0.0f){
        applyMediumDetailsFilter(activeFBO,auxFBO2,auxFBO1);
        copyFBO(auxFBO1,activeFBO);
    }

    if(BasicProp.SharpenBlur != 0){
        applySharpenBlurFilter(activeFBO,auxFBO2,auxFBO1);
        copyFBO(auxFBO1,activeFBO);
    }

    if(activeImage->imageType != NORMAL_TEXTURE){
        applyHeightProcessingFilter(activeFBO,auxFBO1);
        copyFBO(auxFBO1,activeFBO);
    }


    // -------------------------------------------------------- //
    // roughness color mapping
    // -------------------------------------------------------- //

    // both metallic and roughness are almost the same
    // so use same filters for them
    if( (activeImage->imageType == ROUGHNESS_TEXTURE ||
         activeImage->imageType == METALLIC_TEXTURE )
        && RMFilterProp.Filter == COLOR_FILTER::Noise ){
        // processing surface
        applyRoughnessFilter(activeFBO,auxFBO2,auxFBO1);
        copyFBO(auxFBO1,activeFBO);
    }


    if(activeImage->imageType == ROUGHNESS_TEXTURE ||
       activeImage->imageType == METALLIC_TEXTURE){
        if(RMFilterProp.Filter == COLOR_FILTER::Color){
            applyRoughnessColorFilter(activeFBO,auxFBO1);            
            copyFBO(auxFBO1,activeFBO);
        }
    }
    // -------------------------------------------------------- //
    // height processing pipeline
    // -------------------------------------------------------- //

    // -------------------------------------------------------- //
    // normal processing pipeline
    // -------------------------------------------------------- //
    if(activeImage->imageType == NORMAL_TEXTURE){

        applyNormalsStepFilter(activeFBO,auxFBO1);

        // apply normal mixer filter
        if(NormalMixerProp.EnableMixer && activeImage->normalMixerInputTexId != 0){
            applyNormalMixerFilter(auxFBO1,activeFBO);
        }else{// otherwise skip
            copyFBO(auxFBO1,activeFBO);
        }

    }
    // -------------------------------------------------------- //
    // diffuse processing pipeline
    // -------------------------------------------------------- //

    if(!bToggleColorPicking) // skip this step if the Color picking is enabled
    if(activeImage->imageType == DIFFUSE_TEXTURE &&
            (activeImage->bConversionBaseMap || conversionType == CONVERT_FROM_D_TO_O )){

        // create mipmaps
        copyTex2FBO(activeFBO->texture(),auxFBO0BMLevels[0]);
        copyTex2FBO(activeFBO->texture(),auxFBO0BMLevels[1]);
        copyTex2FBO(activeFBO->texture(),auxFBO0BMLevels[2]);
        // calculate normal for orginal image


        activeImage->baseMapConvLevels[0].fromProperty(BaseMapToOthersProp.LevelSmall);
        activeImage->baseMapConvLevels[1].fromProperty(BaseMapToOthersProp.LevelMedium);
        activeImage->baseMapConvLevels[2].fromProperty(BaseMapToOthersProp.LevelBig);
        activeImage->baseMapConvLevels[3].fromProperty(BaseMapToOthersProp.LevelHuge);

        applyBaseMapConversion(activeFBO,auxFBO2,auxFBO1,activeImage->baseMapConvLevels[0]);

        // calulcate normal for mipmaps
        for(int i = 0 ; i < 3 ; i++){
             applyBaseMapConversion(auxFBO0BMLevels[i],auxFBO1BMLevels[i],auxFBO2BMLevels[i],activeImage->baseMapConvLevels[i+1]);
        }

        // mix normals toghether
        applyMixNormalLevels(auxFBO1->texture(),
                           auxFBO2BMLevels[0]->texture(),
                           auxFBO2BMLevels[1]->texture(),
                           auxFBO2BMLevels[2]->texture(),
                           activeFBO);

        //  apply angle correction
        applyNormalAngleCorrectionFilter(activeFBO,auxFBO1);
        copyTex2FBO(auxFBO1->texture(),activeFBO);

        if(conversionType == CONVERT_FROM_D_TO_O){
            applyNormalToHeight(targetImageHeight,activeFBO,auxFBO1,auxFBO2);
            applyCPUNormalizationFilter(auxFBO2,auxFBO1);
            applyAddNoiseFilter(auxFBO1,auxFBO2);
            copyFBO(auxFBO2,auxFBO1);

        }else if(activeImage->bConversionBaseMapShowHeightTexture){
            applyNormalToHeight(targetImageHeight,activeFBO,auxFBO1,auxFBO2);
            applyCPUNormalizationFilter(auxFBO2,activeFBO);
        }
    } // end of base map conversion
    }// end of skip standard processing


    // copying the conversion results to proper textures
    switch(conversionType){
        case(CONVERT_FROM_H_TO_N):
        if(activeImage->imageType == NORMAL_TEXTURE){

            copyFBO(activeFBO,targetImageNormal->fbo);
            targetImageNormal->updateSrcTexId(targetImageNormal->fbo);
        }

        break;
        case(CONVERT_FROM_N_TO_H):
            if(activeImage->imageType == HEIGHT_TEXTURE){
                qDebug() << "Changing reference image of height";                
            }
        break;
        case(CONVERT_FROM_D_TO_O):        
            copyFBO(activeFBO,targetImageNormal->fbo);
            targetImageNormal->updateSrcTexId(targetImageNormal->fbo);


            copyFBO(auxFBO1,targetImageHeight->fbo);
            targetImageHeight->updateSrcTexId(targetImageHeight->fbo);


            applyOcclusionFilter(targetImageHeight->scr_tex_id,targetImageNormal->scr_tex_id,targetImageOcclusion->fbo);

            targetImageOcclusion->updateSrcTexId(targetImageOcclusion->fbo);

            copyTex2FBO(activeImage->scr_tex_id,targetImageSpecular->fbo);
            targetImageSpecular->updateSrcTexId(targetImageSpecular->fbo);


            copyTex2FBO(activeImage->scr_tex_id,targetImageRoughness->fbo);
            targetImageRoughness->updateSrcTexId(targetImageRoughness->fbo);


            copyTex2FBO(activeImage->scr_tex_id,targetImageMetallic->fbo);
            targetImageMetallic->updateSrcTexId(targetImageMetallic->fbo);

        break;
        case(CONVERT_FROM_HN_TO_OC):
            //copyFBO(activeFBO,targetImageOcclusion->ref_fbo);
            copyFBO(activeFBO,targetImageOcclusion->fbo);
            targetImageOcclusion->updateSrcTexId(activeFBO);

        break;

        default:
        break;
    }


    activeFBO = activeImage->fbo;

    }// end of skip processing



    bSkipProcessing = false;
    conversionType  = CONVERT_NONE;

    if(!bShadowRender){
        GLCHK(FBOImages::resize(renderFBO,activeFBO->width(),activeFBO->height()));
        GLCHK( program->setUniformValue("material_id", int(-1)) );
        GLCHK(applyNormalFilter(activeFBO,renderFBO));
    }
    emit rendered();
    GLCHK(glBindVertexArray(0));
    GLCHK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void GLImage::showEvent(QShowEvent* event){
    QWidget::showEvent( event );
    resetView();
}


void GLImage::resizeFBO(int width, int height){
     conversionType = CONVERT_RESIZE;
     resize_width   = width;
     resize_height  = height;
}

void GLImage::resetView(){

    if (!activeImage) return;

    makeCurrent();

    zoom = 0;
    windowRatio = float(width())/height();
    fboRatio    = float(activeImage->fbo->width())/activeImage->fbo->height();
    // openGL window dimensions
    orthographicProjHeight = (1+zoom)/windowRatio;
    orthographicProjWidth = (1+zoom)/fboRatio;

    if(orthographicProjWidth < 1.0) { // fitting x direction
        zoom = fboRatio - 1;
        orthographicProjHeight = (1+zoom)/windowRatio;
        orthographicProjWidth = (1+zoom)/fboRatio;
    }
    if(orthographicProjHeight < 1.0) { // fitting y direction
        zoom = windowRatio - 1;
        orthographicProjHeight = (1+zoom)/windowRatio;
        orthographicProjWidth = (1+zoom)/fboRatio;
    }
    // setting the image in the center
    xTranslation = orthographicProjWidth /2;
    yTranslation = orthographicProjHeight/2;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLImage::resizeGL(int width, int height)
{
  qDebug() << "Resizing GL image to (" << width << ", " << height << ")";
  windowRatio = float(width)/height;
  if (isValid()) {
    GLCHK( glViewport(0, 0, width, height) );

    if (activeImage && activeImage->fbo){
      fboRatio = float(activeImage->fbo->width())/activeImage->fbo->height();
      orthographicProjHeight = (1+zoom)/windowRatio;
      orthographicProjWidth = (1+zoom)/fboRatio;
    } else {
      qWarning() << Q_FUNC_INFO;
      if (!activeImage) qWarning() << "  activeImage is null";
      else
	if (!activeImage->fbo) qWarning() << "  activeImage->fbo is null";
    }
  } else
    qDebug() << Q_FUNC_INFO << "invalid context.";

  resetView();
}


void GLImage::setActiveImage(FBOImageProporties* ptr){
        activeImage = ptr;        
        updateGLNow();
}
void GLImage::enableShadowRender(bool enable){
        bShadowRender = enable;
}
void GLImage::setConversionType(ConversionType type){
    conversionType = type ;
}
ConversionType GLImage::getConversionType(){
    return conversionType;
}

void GLImage::updateCornersPosition(QVector2D dc1,QVector2D dc2,QVector2D dc3,QVector2D dc4){

    cornerPositions[0] = QVector2D(0,0) + dc1;
    cornerPositions[1] = QVector2D(1,0) + dc2;
    cornerPositions[2] = QVector2D(1,1) + dc3;
    cornerPositions[3] = QVector2D(0,1) + dc4;

    for(int i = 0 ; i < 4 ; i++){
        grungeCornerPositions[i] = cornerPositions[i];
    }
    updateGL();
}
void GLImage::selectPerspectiveTransformMethod(int method){
    gui_perspective_mode = method;
    updateGL();
}

void GLImage::selectUVManipulationMethod(UVManipulationMethods method){
    uvManilupationMethod = method;
    updateGL();
}

void GLImage::updateCornersWeights(float w1,float w2,float w3,float w4){
    cornerWeights.setX( w1);
    cornerWeights.setY( w2);
    cornerWeights.setZ( w3);
    cornerWeights.setW( w4);
    updateGL();
}

void GLImage::selectSeamlessMode(SeamlessMode mode){
    FBOImageProporties::seamlessMode = mode;
    updateGL();
}

void GLImage::imageChanged(){
    bToggleColorPicking = false; // restore picking state when another property was changed
}


void GLImage::applyNormalFilter(QGLFramebufferObject* inputFBO,
                         QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_filter"]) );
#endif

    GLCHK( outputFBO->bind() );
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    outputFBO->bindDefault();
}

void GLImage::applyHeightToNormal(QGLFramebufferObject* inputFBO,
                         QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_height_to_normal"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_height_to_normal"]) );
#endif
    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("gui_hn_conversion_depth", activeImage->conversionHNDepth) );
    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );
}


void GLImage::applyColorHueFilter(  QGLFramebufferObject* inputFBO,
                           QGLFramebufferObject* outputFBO){

    GLCHK( outputFBO->bind() );
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
#ifdef USE_OPENGL_330
    program = filter_programs["mode_color_hue_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_color_hue_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );   
    GLCHK( program->setUniformValue("gui_hue"   , float(activeImage->properties->Basic.ColorHue)) );


    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    outputFBO->bindDefault();
}

void GLImage::applyPerspectiveTransformFilter(  QGLFramebufferObject* inputFBO,
                                                QGLFramebufferObject* outputFBO){

    // when materials texture is enabled UV transformation are disabled
    if(FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED){
        copyFBO(inputFBO,outputFBO);
        return;
    }
    #ifdef USE_OPENGL_330
        program = filter_programs["mode_perspective_transform_filter"];
        program->bind();
        updateProgramUniforms(0);
    #else
        GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_perspective_transform_filter"]) );
    #endif

    GLCHK( outputFBO->bind() );
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    if(activeImage->imageType == GRUNGE_TEXTURE){
        GLCHK( program->setUniformValue("corner1"  , grungeCornerPositions[0]) );
        GLCHK( program->setUniformValue("corner2"  , grungeCornerPositions[1]) );
        GLCHK( program->setUniformValue("corner3"  , grungeCornerPositions[2]) );
        GLCHK( program->setUniformValue("corner4"  , grungeCornerPositions[3]) );
    }else{
        GLCHK( program->setUniformValue("corner1"  , cornerPositions[0]) );
        GLCHK( program->setUniformValue("corner2"  , cornerPositions[1]) );
        GLCHK( program->setUniformValue("corner3"  , cornerPositions[2]) );
        GLCHK( program->setUniformValue("corner4"  , cornerPositions[3]) );
    }
    GLCHK( program->setUniformValue("corners_weights"  , cornerWeights) );
    GLCHK( program->setUniformValue("uv_scaling_mode", 0) );
    GLCHK( program->setUniformValue("gui_perspective_mode"  , gui_perspective_mode) );

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    GLCHK( inputFBO->bind() );
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( program->setUniformValue("uv_scaling_mode", 1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    inputFBO->bindDefault();
}


void GLImage::applyGaussFilter(QGLFramebufferObject* sourceFBO,
                               QGLFramebufferObject* auxFBO,
                               QGLFramebufferObject* outputFBO,int no_iter,float w ){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_gauss_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_gauss_filter"]) );
#endif

    GLCHK( program->setUniformValue("gui_gauss_radius", no_iter) );
    if( w == 0){
        GLCHK( program->setUniformValue("gui_gauss_w", float(no_iter)) );
    }else
        GLCHK( program->setUniformValue("gui_gauss_w", float(w)) );

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( program->setUniformValue("gauss_mode",1) );

    GLCHK( auxFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, sourceFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( program->setUniformValue("gauss_mode",2) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( program->setUniformValue("gauss_mode",0) );
}


void GLImage::applyInverseColorFilter(QGLFramebufferObject* inputFBO,
                                      QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_invert_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_invert_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );
}

void GLImage::applyRemoveShadingFilter(QGLFramebufferObject* inputFBO,
                               QGLFramebufferObject* aoMaskFBO,
                               QGLFramebufferObject* refFBO,
                               QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_ao_cancellation_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_ao_cancellation_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_remove_shading",RemoveShadingProp.RemoveShadingByGaussian));
    GLCHK( program->setUniformValue("gui_ao_cancellation",RemoveShadingProp.AOCancellation ));

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, aoMaskFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE2) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, refFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
}

void GLImage::applyRemoveLowFreqFilter(QGLFramebufferObject* inputFBO,
                                       QGLFramebufferObject* auxFBO,
                                       QGLFramebufferObject* outputFBO){

    applyGaussFilter(inputFBO,samplerFBO1,samplerFBO2,RemoveShadingProp.LowFrequencyFilterRadius*50);

    // calculating the average color on CPU
    applyNormalFilter(inputFBO,averageColorFBO); // copy large file to smaller FBO (save time!)


    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, averageColorFBO->texture()) );

    GLint textureWidth, textureHeight;
    GLCHK( glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH , &textureWidth ) );
    GLCHK( glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight) );

    float* img = new float[textureWidth*textureHeight*3];
    float ave_color[3] = {0,0,0};

    GLCHK( glGetTexImage(	GL_TEXTURE_2D,0,GL_RGB,GL_FLOAT,img) );


    for(int i = 0 ; i < textureWidth*textureHeight ; i++){
        for(int c = 0 ; c < 3 ; c++){
             ave_color[c] += img[3*i+c];
        }
    }

    // normalization sum
    ave_color[0] /= (textureWidth*textureHeight);
    ave_color[1] /= (textureWidth*textureHeight);
    ave_color[2] /= (textureWidth*textureHeight);
    delete[] img;

    QVector3D aveColor = QVector3D(ave_color[0],ave_color[1],ave_color[2]);

#ifdef USE_OPENGL_330
    program = filter_programs["mode_remove_low_freq_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_remove_low_freq_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("average_color"  , aveColor ) );
    GLCHK( program->setUniformValue("gui_remove_shading_lf_blending"  , RemoveShadingProp.LowFrequencyFilterBlending ) );

    GLCHK( outputFBO->bind() );
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, samplerFBO2->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    outputFBO->bindDefault();

}

void GLImage::applyOverlayFilter(QGLFramebufferObject* layerAFBO,
                                 QGLFramebufferObject* layerBFBO,
                                 QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_overlay_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_overlay_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( outputFBO->bind() );
    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, layerAFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, layerBFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );

    outputFBO->bindDefault();
}



void GLImage::applySeamlessLinearFilter(QGLFramebufferObject* inputFBO,
                                       QGLFramebufferObject* outputFBO){

    // when materials texture is enabled UV transformation are disabled
    if(FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED){
        copyFBO(inputFBO,outputFBO);
        return;
    }
    switch(FBOImageProporties::seamlessContrastInputType){
        default:
        case(INPUT_FROM_HEIGHT_INPUT):
            //copyFBO(targetImageHeight->ref_fbo,activeImage->aux2_fbo);
            copyTex2FBO(targetImageHeight->scr_tex_id,auxFBO2);
            break;
        case(INPUT_FROM_DIFFUSE_INPUT):
            //copyFBO(targetImageDiffuse->ref_fbo,activeImage->aux2_fbo);
            copyTex2FBO(targetImageDiffuse->scr_tex_id,auxFBO2);
            break;
    };

    // when translations are applied first one has to translate
    // alse the contrast mask image
    if(FBOImageProporties::bSeamlessTranslationsFirst){
        applyPerspectiveTransformFilter(auxFBO2,outputFBO);// the output is save to auxFBO1
    }

    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO2->texture()) );

#ifdef USE_OPENGL_330
    program = filter_programs["mode_seamless_linear_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_seamless_linear_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("make_seamless_radius"           , FBOImageProporties::seamlessSimpleModeRadius) );
    GLCHK( program->setUniformValue("gui_seamless_contrast_strenght" , FBOImageProporties::seamlessContrastStrenght) );
    GLCHK( program->setUniformValue("gui_seamless_contrast_power"    , FBOImageProporties::seamlessContrastPower) );


    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    switch(FBOImageProporties::seamlessSimpleModeDirection){
        default:
        case(0)://XY
        GLCHK( program->setUniformValue("gui_seamless_mode"         , (int)0) ); // horizontal filtering
        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );

        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

        GLCHK( inputFBO->bind() );
        GLCHK( program->setUniformValue("gui_seamless_mode"         , (int)1) ); // vertical filtering
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
        break;
        case(1)://X
        GLCHK( program->setUniformValue("gui_seamless_mode"         , (int)0) ); // horizontal filtering

        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
        inputFBO->bindDefault();
        copyFBO(outputFBO,inputFBO);
        break;
        case(2)://Y
        GLCHK( outputFBO->bind() );
        GLCHK( program->setUniformValue("gui_seamless_mode"         , (int)1) ); // vertical filtering
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
        copyFBO(outputFBO,inputFBO);
        break;

    }

    inputFBO->bindDefault();
    GLCHK( glActiveTexture(GL_TEXTURE0) );
}



void GLImage::applySeamlessFilter(QGLFramebufferObject* inputFBO,
                                  QGLFramebufferObject* outputFBO){

    // when materials texture is enabled UV transformation are disabled
    if(FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED){
        copyFBO(inputFBO,outputFBO);
        return;
    }
    switch(FBOImageProporties::seamlessContrastInputType){
        default:
        case(INPUT_FROM_HEIGHT_INPUT):            
            copyTex2FBO(targetImageHeight->scr_tex_id,auxFBO1);
            break;
        case(INPUT_FROM_DIFFUSE_INPUT):            
            copyTex2FBO(targetImageDiffuse->scr_tex_id,auxFBO1);
            break;
    };

    // when translations are applied first one has to translate
    // alse the contrast mask image
    if(FBOImageProporties::bSeamlessTranslationsFirst){
      applyPerspectiveTransformFilter(auxFBO1,outputFBO);// the output is save to auxFBO2
    }

#ifdef USE_OPENGL_330
    program = filter_programs["mode_seamless_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_seamless_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("make_seamless_radius"      , FBOImageProporties::seamlessSimpleModeRadius) );
    GLCHK( program->setUniformValue("gui_seamless_contrast_strenght" , FBOImageProporties::seamlessContrastStrenght) );
    GLCHK( program->setUniformValue("gui_seamless_contrast_power"    , FBOImageProporties::seamlessContrastPower) );
    GLCHK( program->setUniformValue("gui_seamless_mode"         , (int)FBOImageProporties::seamlessMode) );
    GLCHK( program->setUniformValue("gui_seamless_mirror_type"  , FBOImageProporties::seamlessMirroModeType) );

    // sending the random angles
    QMatrix3x3 random_angles;
    for(int i = 0; i < 9; i++)random_angles.data()[i] = FBOImageProporties::seamlessRandomTiling.angles[i];
    GLCHK( program->setUniformValue("gui_seamless_random_angles" , random_angles) );
    GLCHK( program->setUniformValue("gui_seamless_random_phase" , FBOImageProporties::seamlessRandomTiling.common_phase) );
    GLCHK( program->setUniformValue("gui_seamless_random_inner_radius" , FBOImageProporties::seamlessRandomTiling.inner_radius) );
    GLCHK( program->setUniformValue("gui_seamless_random_outer_radius" , FBOImageProporties::seamlessRandomTiling.outer_radius) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );


    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO1->texture()) );


    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    outputFBO->bindDefault();
    GLCHK( glActiveTexture(GL_TEXTURE0) );
}


void GLImage::applyDGaussiansFilter(QGLFramebufferObject* inputFBO,
                                    QGLFramebufferObject* auxFBO,
                                    QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_gauss_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_gauss_filter"]) );
#endif

    GLCHK( program->setUniformValue("gui_gauss_radius", int(SurfaceDetailsProp.Radius)) );
    GLCHK( program->setUniformValue("gui_gauss_w", SurfaceDetailsProp.WeightA) );


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );

    GLCHK( program->setUniformValue("gauss_mode",1) );


    GLCHK( auxFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    GLCHK( program->setUniformValue("gauss_mode",2) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    GLCHK( program->setUniformValue("gui_gauss_w", SurfaceDetailsProp.WeightB) );


    GLCHK( auxFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( program->setUniformValue("gauss_mode",1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    GLCHK( program->setUniformValue("gauss_mode",2) );
    GLCHK( inputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

#ifdef USE_OPENGL_330
    program = filter_programs["mode_dgaussians_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_dgaussians_filter"]) );
#endif

    GLCHK( program->setUniformValue("gui_mode_dgaussian", 1) );
    GLCHK( program->setUniformValue("gui_specular_amplifier", SurfaceDetailsProp.Amplifier) );

    GLCHK( auxFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( program->setUniformValue("gauss_mode",0) );

    copyFBO(auxFBO,outputFBO);
    outputFBO->bindDefault();

}

void GLImage::applyContrastFilter(QGLFramebufferObject* inputFBO,
                                  QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_constrast_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_constrast_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_specular_contrast", SurfaceDetailsProp.Contrast) );
    GLCHK( program->setUniformValue("gui_specular_brightness", 0.0f) );//not used since offset does the same

    
    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault());
}

void GLImage::applySmallDetailsFilter(QGLFramebufferObject* inputFBO,
                                      QGLFramebufferObject* auxFBO,
                                    QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_gauss_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_gauss_filter"]) );
#endif


    GLCHK( program->setUniformValue("gui_depth", BasicProp.DetailDepth) );
    GLCHK( program->setUniformValue("gui_gauss_radius", int(3.0)) );
    GLCHK( program->setUniformValue("gui_gauss_w", float(3.0)) );

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( program->setUniformValue("gauss_mode",1) );

    GLCHK( auxFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( program->setUniformValue("gauss_mode",2) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

#ifdef USE_OPENGL_330
    program = filter_programs["mode_dgaussians_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_dgaussians_filter"]) );
#endif

    GLCHK( program->setUniformValue("gui_mode_dgaussian", 0) );

    GLCHK( auxFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( program->setUniformValue("gauss_mode",0) );
    GLCHK( program->setUniformValue("gui_mode_dgaussian", 1) );



#ifdef USE_OPENGL_330
    program = filter_programs["mode_small_details_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_small_details_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_small_details", BasicProp.SmallDetails) );
    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( outputFBO->bindDefault() );
    GLCHK( program->setUniformValue("gui_depth", float(1.0)) );

}

void GLImage::applyMediumDetailsFilter(QGLFramebufferObject* inputFBO,
                                       QGLFramebufferObject* auxFBO,
                                       QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_gauss_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_gauss_filter"]) );
#endif

    GLCHK( program->setUniformValue("gui_depth", BasicProp.DetailDepth) );
    GLCHK( program->setUniformValue("gui_gauss_radius", int(15.0)) );
    GLCHK( program->setUniformValue("gui_gauss_w", float(15.0)) );

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( program->setUniformValue("gauss_mode",1) );

    GLCHK( auxFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( program->setUniformValue("gauss_mode",2) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );


    GLCHK( program->setUniformValue("gauss_mode",1) );
    GLCHK( program->setUniformValue("gui_gauss_radius", int(20.0)) );
    GLCHK( program->setUniformValue("gui_gauss_w"     , float(20.0)) );

#ifdef USE_OPENGL_330
    program = filter_programs["mode_medium_details_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_medium_details_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    program->setUniformValue("gui_small_details", BasicProp.MediumDetails);
    glViewport(0,0,inputFBO->width(),inputFBO->height());
    GLCHK( auxFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );

    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( outputFBO->bindDefault() );
    GLCHK( program->setUniformValue("gui_depth", float(1.0)) );
    copyFBO(auxFBO,outputFBO);


}


void GLImage::applyGrayScaleFilter(QGLFramebufferObject* inputFBO,
                                   QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_gray_scale_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_gray_scale_filter"]) );
#endif

    // There is a change if gray scale filter is used for convertion from diffuse
    // texture to others in any other case this filter works just a simple gray scale filter.
    // Check if the baseMapToOthers is enabled, if yes check is min and max values are defined.

    GLCHK( program->setUniformValue("gui_gray_scale_max_color_defined",false) );
    GLCHK( program->setUniformValue("gui_gray_scale_min_color_defined",false) );

    if(activeImage->bConversionBaseMap){
        if(QColor(BaseMapToOthersProp.MaxColor.value()).red() >= 0){

            QColor color = QColor(BaseMapToOthersProp.MaxColor.value());
            QVector3D dcolor(color.redF(),color.greenF(),color.blueF());
            GLCHK( program->setUniformValue("gui_gray_scale_max_color_defined",true) );
            GLCHK( program->setUniformValue("gui_gray_scale_max_color",dcolor) );
        }
        if(QColor(BaseMapToOthersProp.MinColor.value()).red() >= 0){
            QColor color = QColor(BaseMapToOthersProp.MinColor.value());
            QVector3D dcolor(color.redF(),color.greenF(),color.blueF());
            GLCHK( program->setUniformValue("gui_gray_scale_min_color_defined",true) );
            GLCHK( program->setUniformValue("gui_gray_scale_min_color",dcolor) );
        }
        GLCHK( program->setUniformValue("gui_gray_scale_range_tol",float(BaseMapToOthersProp.ColorBalance*10)) );
    }

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_gray_scale_preset",QVector3D(BasicProp.GrayScale.GrayScaleR,
                                                                      BasicProp.GrayScale.GrayScaleG,
                                                                      BasicProp.GrayScale.GrayScaleB)) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );
}

void GLImage::applyInvertComponentsFilter(QGLFramebufferObject* inputFBO,
                             QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_invert_components_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_invert_components_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_inverted_components"  , QVector3D(BasicProp.ColorComponents.InvertRed,
                                                                           BasicProp.ColorComponents.InvertGreen,
                                                                           BasicProp.ColorComponents.InvertBlue)) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );

}

void GLImage::applySharpenBlurFilter(QGLFramebufferObject* inputFBO,
                                     QGLFramebufferObject* auxFBO,
                                     QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_sharpen_blur"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_sharpen_blur"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("gui_sharpen_blur", BasicProp.SharpenBlur) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );

    GLCHK( auxFBO->bind() );
    GLCHK( program->setUniformValue("gauss_mode",1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bind() );
    GLCHK( program->setUniformValue("gauss_mode",2) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    GLCHK( outputFBO->bindDefault() );
    GLCHK( program->setUniformValue("gauss_mode",0) );

}

void GLImage::applyNormalsStepFilter(QGLFramebufferObject* inputFBO,
                               QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_normals_step_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normals_step_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("gui_normals_step", BasicProp.NormalsStep) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );

}

void GLImage::applyNormalMixerFilter(QGLFramebufferObject* inputFBO,
                                     QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_mixer_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_mixer_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_normal_mixer_depth",NormalMixerProp.Depth*2 ) );
    GLCHK( program->setUniformValue("gui_normal_mixer_angle",NormalMixerProp.Angle/180.0f*3.1415926f) );
    GLCHK( program->setUniformValue("gui_normal_mixer_scale",NormalMixerProp.Scale) );
    GLCHK( program->setUniformValue("gui_normal_mixer_pos_x",NormalMixerProp.PosX) );
    GLCHK( program->setUniformValue("gui_normal_mixer_pos_y",NormalMixerProp.PosY) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );

    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, activeImage->normalMixerInputTexId) );


    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );

}


void GLImage::applyPreSmoothFilter(  QGLFramebufferObject* inputFBO,
                                     QGLFramebufferObject* auxFBO,
                                     QGLFramebufferObject* outputFBO,BaseMapConvLevelProperties& convProp){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_gauss_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_gauss_filter"]) );
#endif

    GLCHK( program->setUniformValue("gui_gauss_radius", int(convProp.conversionBaseMapPreSmoothRadius)) );
    GLCHK( program->setUniformValue("gui_gauss_w", float(convProp.conversionBaseMapPreSmoothRadius)) );


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( program->setUniformValue("gauss_mode",1) );

    GLCHK( auxFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( program->setUniformValue("gauss_mode",2) );
    GLCHK( outputFBO->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( program->setUniformValue("gauss_mode",0) );
    GLCHK( outputFBO->bindDefault() );


}

void GLImage::applySobelToNormalFilter(QGLFramebufferObject* inputFBO,
                                       QGLFramebufferObject* outputFBO,
                                       BaseMapConvLevelProperties& convProp){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_sobel_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_sobel_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("gui_basemap_amp", convProp.conversionBaseMapAmplitude) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );

}




void GLImage::applyNormalToHeight(FBOImageProporties* image,QGLFramebufferObject* normalFBO,
                                  QGLFramebufferObject* heightFBO,
                                  QGLFramebufferObject* outputFBO){



    applyGrayScaleFilter(normalFBO,heightFBO);

#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_to_height"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_to_height"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );



    GLCHK( outputFBO->bind() );
    GLCHK( glViewport(0,0,heightFBO->width(),heightFBO->height()) );
    GLCHK( program->setUniformValue("hn_min_max_scale",QVector3D(-0.0,1.0,1.0)) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, normalFBO->texture()) );

    // When conversion is enabled (diffuse image only) then
    // this texture (auxFBO4) keeps the orginal diffuse image
    // before the gray scale filter applied, this is then
    // used to force propper height levels in the bump map
    // since user wants to have defined min/max colors to be 0 or 1
    // in the height map. In case of other conversion this is no more used.
    if(activeImage->bConversionBaseMap){
        GLCHK( glActiveTexture(GL_TEXTURE2) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO4->texture()) );
    }

    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    for(int i = 0; i < image->properties->NormalHeightConv.Huge ; i++){
        GLCHK( program->setUniformValue("hn_min_max_scale",QVector3D(-0.0,1.0,pow(2.0,5))) );
        GLCHK( heightFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    }

    for(int i = 0; i < image->properties->NormalHeightConv.VeryLarge ; i++){
        GLCHK( program->setUniformValue("hn_min_max_scale",QVector3D(-0.0,1.0,pow(2.0,4))) );
        GLCHK( heightFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    }

    for(int i = 0; i < image->properties->NormalHeightConv.Large ; i++){
        GLCHK( program->setUniformValue("hn_min_max_scale",QVector3D(-0.0,1.0,pow(2.0,3))) );
        GLCHK( heightFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

    }

    for(int i = 0; i < image->properties->NormalHeightConv.Medium; i++){
        GLCHK( program->setUniformValue("hn_min_max_scale",QVector3D(-0.0,1.0,pow(2.0,2))) );
        GLCHK( heightFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    }
    for(int i = 0; i < image->properties->NormalHeightConv.Small; i++){
        GLCHK( program->setUniformValue("hn_min_max_scale",QVector3D(-0.0,1.0,pow(2.0,1))) );
        GLCHK( heightFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    }
    for(int i = 0; i < image->properties->NormalHeightConv.VerySmall; i++){
        GLCHK( program->setUniformValue("hn_min_max_scale",QVector3D(-0.0,1.0,pow(2.0,0))) );
        GLCHK( heightFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, outputFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );

        GLCHK( outputFBO->bind() );
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
        GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    }

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( outputFBO->bindDefault() );

}


void GLImage::applyNormalAngleCorrectionFilter(QGLFramebufferObject* inputFBO,
                                               QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_angle_correction_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_angle_correction_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("base_map_angle_correction"  , BaseMapToOthersProp.AngleCorrection/180.0f*3.1415926f) );
    GLCHK( program->setUniformValue("base_map_angle_weight"      , BaseMapToOthersProp.AngleWeight) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );

}

void GLImage::applyNormalExpansionFilter(QGLFramebufferObject* inputFBO,
                                         QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_expansion_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_expansion_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );

}


void GLImage::applyMixNormalLevels(GLuint level0,
                                   GLuint level1,
                                   GLuint level2,
                                   GLuint level3,
                                   QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_mix_normal_levels_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_mix_normal_levels_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_base_map_w0"  , activeImage->properties->BaseMapToOthers.WeightSmall ) );
    GLCHK( program->setUniformValue("gui_base_map_w1"  , activeImage->properties->BaseMapToOthers.WeightMedium ) );
    GLCHK( program->setUniformValue("gui_base_map_w2"  , activeImage->properties->BaseMapToOthers.WeightBig) );
    GLCHK( program->setUniformValue("gui_base_map_w3"  , activeImage->properties->BaseMapToOthers.WeightHuge ) );

    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( outputFBO->bind() );

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, level0) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, level1) );
    GLCHK( glActiveTexture(GL_TEXTURE2) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, level2) );
    GLCHK( glActiveTexture(GL_TEXTURE3) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, level3) );

    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( outputFBO->bindDefault() );
}

void GLImage::applyCPUNormalizationFilter(QGLFramebufferObject* inputFBO,
                                          QGLFramebufferObject* outputFBO){




    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLint textureWidth, textureHeight;
    GLCHK( glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureWidth) );
    GLCHK( glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureHeight) );

    float* img = new float[textureWidth*textureHeight*3];

    GLCHK( glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_FLOAT,img) );

    float min[3] = {img[0],img[1],img[2]};
    float max[3] = {img[0],img[1],img[2]};

    // if materials are enabled one must calulate height only in the
    // region of selected material color
    if(FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED){
        QImage maskImage = targetImageMaterial->getImage();
        int currentMaterialIndex = FBOImageProporties::currentMaterialIndeks;
        // number of components
        int nc = maskImage. byteCount () / (textureWidth*textureHeight) ;
        bool bFirstTimeChecked = true;
        unsigned char * data = maskImage.bits();
        for(int i = 0 ; i < textureWidth*textureHeight ; i++){
            int materialColor = data[nc*i+0]*255*255 + data[nc*i+1]*255 + data[nc*i+2];
            if(materialColor == currentMaterialIndex){
                if(bFirstTimeChecked){
                    for(int c = 0 ; c < 3 ; c++){
                        min[c] = img[3*i+c];
                        max[c] = max[c];
                    }
                    bFirstTimeChecked = false;
                }// end of if first time

                for(int c = 0 ; c < 3 ; c++){
                    if( max[c] < img[3*i+c] ) max[c] = img[3*i+c];
                    if( min[c] > img[3*i+c] ) min[c] = img[3*i+c];
                }
            }// end of if material colors are same
        }// end of loop over image

    }else{// if materials are disabled calculate
        for(int i = 0 ; i < textureWidth*textureHeight ; i++){
            for(int c = 0 ; c < 3 ; c++){
                if( max[c] < img[3*i+c] ) max[c] = img[3*i+c];
                if( min[c] > img[3*i+c] ) min[c] = img[3*i+c];
            }
        }
    }// end of if materials are enables

    // prevent from singularities
    for(int k = 0; k < 3 ; k ++)
    if(qAbs(min[k] - max[k]) < 0.0001) max[k] += 0.1;


    qDebug() << "Image normalization:";
    qDebug() << "Min color = (" << min[0] << "," << min[1] << "," << min[2] << ")"  ;
    qDebug() << "Max color = (" << max[0] << "," << max[1] << "," << max[2] << ")"  ;


    delete[] img;
#ifdef USE_OPENGL_330
    program = filter_programs["mode_normalize_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normalize_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( outputFBO->bind() );
    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( program->setUniformValue("min_color",QVector3D(min[0],min[1],min[2])) );
    GLCHK( program->setUniformValue("max_color",QVector3D(max[0],max[1],max[2])) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );


    GLCHK( outputFBO->bindDefault() );

}

void GLImage::applyAddNoiseFilter(QGLFramebufferObject* inputFBO,
                                  QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_add_noise_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_add_noise_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("gui_add_noise_amp"  , float(targetImageHeight->properties->NormalHeightConv.NoiseLevel/100.0) ));

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );

}


void GLImage::applyBaseMapConversion(QGLFramebufferObject* baseMapFBO,
                                     QGLFramebufferObject *auxFBO,
                                     QGLFramebufferObject* outputFBO,BaseMapConvLevelProperties& convProp){


        applyGrayScaleFilter(baseMapFBO,outputFBO);
        applySobelToNormalFilter(outputFBO,auxFBO,convProp);
        applyInvertComponentsFilter(auxFBO,baseMapFBO);
        applyPreSmoothFilter(baseMapFBO,auxFBO,outputFBO,convProp);

        #ifdef USE_OPENGL_330
            program = filter_programs["mode_normal_expansion_filter"];
            program->bind();
        #endif

        GLCHK( program->setUniformValue("gui_combine_normals" , 0) );
        GLCHK( program->setUniformValue("gui_filter_radius" , convProp.conversionBaseMapFilterRadius) );
        GLCHK( program->setUniformValue("gui_normal_flatting" , convProp.conversionBaseMapFlatness) );

        for(int i = 0; i < convProp.conversionBaseMapNoIters ; i ++){
            copyFBO(outputFBO,auxFBO);

            applyNormalExpansionFilter(auxFBO,outputFBO);
        }
        #ifdef USE_OPENGL_330
            program = filter_programs["mode_normal_expansion_filter"];
            program->bind();
        #endif
        GLCHK( program->setUniformValue("gui_combine_normals" , 1) );
        GLCHK( program->setUniformValue("gui_mix_normals"   , convProp.conversionBaseMapMixNormals) );
        GLCHK( program->setUniformValue("gui_blend_normals" , convProp.conversionBaseMapBlending) );
        copyFBO(outputFBO,auxFBO);
        GLCHK( glActiveTexture(GL_TEXTURE1) );
        GLCHK( glBindTexture(GL_TEXTURE_2D, baseMapFBO->texture()) );
        applyNormalExpansionFilter(auxFBO,outputFBO);
        GLCHK( glActiveTexture(GL_TEXTURE0) );
        #ifdef USE_OPENGL_330
            program = filter_programs["mode_normal_expansion_filter"];
            program->bind();
        #endif
        GLCHK( program->setUniformValue("gui_combine_normals" , 0 ) );


}


void GLImage::applyOcclusionFilter(GLuint height_tex,GLuint normal_tex,
                          QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_occlusion_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_occlusion_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_ssao_no_iters"   ,AOProp.NumIters) );
    GLCHK( program->setUniformValue("gui_ssao_depth"      ,AOProp.Depth) );
    GLCHK( program->setUniformValue("gui_ssao_bias"       ,AOProp.Bias) );
    GLCHK( program->setUniformValue("gui_ssao_intensity"  ,AOProp.Intensity) );

    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( outputFBO->bind() );

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, height_tex) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, normal_tex) );

    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( outputFBO->bindDefault() );

}

void GLImage::applyHeightProcessingFilter(QGLFramebufferObject* inputFBO,
                                           QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_height_processing_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_height_processing_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    GLCHK( program->setUniformValue("gui_height_proc_min_value"   ,ColorLevelsProp.MinValue) );
    GLCHK( program->setUniformValue("gui_height_proc_max_value"   ,ColorLevelsProp.MaxValue) );
    GLCHK( program->setUniformValue("gui_height_proc_ave_radius"  ,int(ColorLevelsProp.DetailsRadius*100.0) ));
    GLCHK( program->setUniformValue("gui_height_proc_offset_value",ColorLevelsProp.Offset) );
    GLCHK( program->setUniformValue("gui_height_proc_normalization",ColorLevelsProp.EnableNormalization) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( outputFBO->bindDefault());
}

void GLImage::applyCombineNormalHeightFilter(QGLFramebufferObject* normalFBO,
                                             QGLFramebufferObject* heightFBO,
                                             QGLFramebufferObject* outputFBO){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_combine_normal_height_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_combine_normal_height_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glViewport(0,0,normalFBO->width(),normalFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, normalFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, heightFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    outputFBO->bindDefault();
}

void GLImage::applyRoughnessFilter(QGLFramebufferObject* inputFBO,
                                   QGLFramebufferObject* auxFBO,
                                    QGLFramebufferObject* outputFBO){

    // do the gaussian filter
    applyGaussFilter(inputFBO,auxFBO,outputFBO,int(RMFilterProp.NoiseFilter.Depth));

    copyFBO(outputFBO,auxFBO);

#ifdef USE_OPENGL_330
    program = filter_programs["mode_roughness_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_roughness_filter"]) );
#endif


    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("gui_roughness_depth"     ,  RMFilterProp.NoiseFilter.Depth ) );
    GLCHK( program->setUniformValue("gui_roughness_treshold"  ,  RMFilterProp.NoiseFilter.Treshold) );
    GLCHK( program->setUniformValue("gui_roughness_amplifier"  , RMFilterProp.NoiseFilter.Amplifier) );


    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( outputFBO->bindDefault() );

}

void GLImage::applyRoughnessColorFilter(QGLFramebufferObject* inputFBO,
                                        QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_roughness_color_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_roughness_color_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );

    QColor color = QColor(RMFilterProp.ColorFilter.PickColor);
    QVector3D dcolor(color.redF(),color.greenF(),color.blueF());

    GLCHK( program->setUniformValue("gui_roughness_picked_color"  , dcolor ) );
    GLCHK( program->setUniformValue("gui_roughness_color_method"  , (int)RMFilterProp.ColorFilter.Method) );
    GLCHK( program->setUniformValue("gui_roughness_color_offset"  , RMFilterProp.ColorFilter.Bias ) );
    GLCHK( program->setUniformValue("gui_roughness_color_global_offset"  , RMFilterProp.ColorFilter.Offset) );

    GLCHK( program->setUniformValue("gui_roughness_invert_mask"   , RMFilterProp.ColorFilter.InvertColors ) );
    GLCHK( program->setUniformValue("gui_roughness_color_amplifier", RMFilterProp.ColorFilter.Amplifier ) );

    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    outputFBO->bindDefault();

}

void GLImage::copyFBO(QGLFramebufferObject* src,QGLFramebufferObject* dst){
#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_filter"]) );
#endif
    GLCHK( dst->bind() );
    GLCHK( glViewport(0,0,dst->width(),dst->height()) );    
    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, src->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    src->bindDefault();
}

void GLImage::copyTex2FBO(GLuint src_tex_id,QGLFramebufferObject* dst){

#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_filter"]) );
#endif

    GLCHK( dst->bind() );
    GLCHK( glViewport(0,0,dst->width(),dst->height()) );

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, src_tex_id) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    dst->bindDefault();
}

void GLImage::applyAllUVsTransforms(QGLFramebufferObject* inoutFBO){
    if(FBOImageProporties::bSeamlessTranslationsFirst){
      applyPerspectiveTransformFilter(inoutFBO,auxFBO1);// the output is save to activeFBO
    }
    // Making seamless...
    switch(FBOImageProporties::seamlessMode){
        case(SEAMLESS_SIMPLE):
            applySeamlessLinearFilter(inoutFBO,auxFBO1); //  the output is save to activeFBO
            break;
        case(SEAMLESS_MIRROR):
        case(SEAMLESS_RANDOM):
            applySeamlessFilter(inoutFBO,auxFBO1);
            copyFBO(auxFBO1,inoutFBO);
        break;
        case(SEAMLESS_NONE):
        default: break;
    }
    if(!FBOImageProporties::bSeamlessTranslationsFirst){
      applyPerspectiveTransformFilter(inoutFBO,auxFBO1);// the output is save to activeFBO
    }
}

void GLImage::applyGrungeImageFilter (QGLFramebufferObject* inputFBO,
                                      QGLFramebufferObject* outputFBO,
                                      QGLFramebufferObject* grungeFBO){


    // in case of normal texture grunge is treated differently
    if(activeImage->imageType == NORMAL_TEXTURE){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_height_to_normal"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_height_to_normal"]) );
#endif
    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );


    GLCHK( program->setUniformValue("gui_hn_conversion_depth", 2*GrungeOnImageProp.GrungeWeight * GrungeProp.OverallWeight) );
    GLCHK( glViewport(0,0,auxFBO3->width(),auxFBO3->height()) );
    GLCHK( auxFBO3->bind() );
    GLCHK( glBindTexture(GL_TEXTURE_2D, grungeFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( auxFBO3->bindDefault() );

#ifdef USE_OPENGL_330
    program = filter_programs["mode_normal_mixer_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_normal_mixer_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    float weight = 2*GrungeOnImageProp.ImageWeight;

    GLCHK( program->setUniformValue("gui_normal_mixer_depth", weight) );
    GLCHK( program->setUniformValue("gui_normal_mixer_angle", 0.0f) );
    GLCHK( program->setUniformValue("gui_normal_mixer_scale", 1.0f) );
    GLCHK( program->setUniformValue("gui_normal_mixer_pos_x", 0.0f) );
    GLCHK( program->setUniformValue("gui_normal_mixer_pos_y", 0.0f) );

    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );

    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, auxFBO3->texture()) );


    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( outputFBO->bindDefault() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );

    }else{

#ifdef USE_OPENGL_330
    program = filter_programs["mode_grunge_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_grunge_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    float weight = GrungeOnImageProp.GrungeWeight * GrungeProp.OverallWeight;

    GLCHK( program->setUniformValue("gui_grunge_overall_weight"  , weight ) );
    GLCHK( program->setUniformValue("gui_grunge_blending_mode"  , (int)GrungeOnImageProp.BlendingMode) );



    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, grungeFBO->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    outputFBO->bindDefault();

    }
}

void GLImage::applyGrungeRandomizationFilter(QGLFramebufferObject* inputFBO,
                                             QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_grunge_randomization_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_grunge_randomization_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    QMatrix3x3 random_angles;
    qsrand(GrungeProp.Randomize);
    for(int i = 0; i < 9; i++)random_angles.data()[i] = 3.1415*qrand()/(RAND_MAX+0.0);
    GLCHK( program->setUniformValue("gui_seamless_random_angles" , random_angles) );
    float phase = 3.1415*qrand()/(RAND_MAX+0.0);
    GLCHK( program->setUniformValue("gui_seamless_random_phase" , phase) );
    GLCHK( program->setUniformValue("gui_grunge_radius"    , GrungeProp.Scale) );
    GLCHK( program->setUniformValue("gui_grunge_brandomize" , bool(GrungeProp.Randomize!=0)) );

    GLCHK( program->setUniformValue("gui_grunge_translations" , int(GrungeProp.RandomTranslations) ) );


    GLCHK( glViewport(0,0,inputFBO->width(),inputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, targetImageGrunge->fbo->texture()) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    outputFBO->bindDefault();
}


void GLImage::applyGrungeWarpNormalFilter(QGLFramebufferObject* inputFBO,
                                          QGLFramebufferObject* outputFBO){


#ifdef USE_OPENGL_330
    program = filter_programs["mode_grunge_normal_warp_filter"];
    program->bind();
    updateProgramUniforms(0);
#else
    GLCHK( glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &subroutines["mode_grunge_normal_warp_filter"]) );
#endif

    GLCHK( program->setUniformValue("quad_scale", QVector2D(1.0,1.0)) );
    GLCHK( program->setUniformValue("quad_pos"  , QVector2D(0.0,0.0)) );
    GLCHK( program->setUniformValue("grunge_normal_warp"  , GrungeProp.NormalWarp) );

    GLCHK( glViewport(0,0,outputFBO->width(),outputFBO->height()) );
    GLCHK( outputFBO->bind() );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, inputFBO->texture()) );
    GLCHK( glActiveTexture(GL_TEXTURE1) );
    GLCHK( glBindTexture(GL_TEXTURE_2D, targetImageNormal->scr_tex_id) );
    GLCHK( glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_INT, 0) );
    GLCHK( glActiveTexture(GL_TEXTURE0) );
    outputFBO->bindDefault();

}

void GLImage::updateProgramUniforms(int step){

    switch(step){
        case(0):
        GLCHK( program->setUniformValue("gui_image_type", openGL330ForceTexType) );
        GLCHK( program->setUniformValue("gui_depth", float(1.0)) );
        GLCHK( program->setUniformValue("gui_mode_dgaussian", 1) );
        GLCHK( program->setUniformValue("material_id", int(activeImage->currentMaterialIndeks) ) );

        if(activeImage->imageType == MATERIAL_TEXTURE){

            GLCHK( program->setUniformValue("material_id", int(-1) ) );
        }
        break;
        default: break;
    }; // end of switch


}

void GLImage::makeScreenQuad()
{

    int size = 2;
    QVector<QVector2D> texCoords = QVector<QVector2D>(size*size);
    QVector<QVector3D> vertices  = QVector<QVector3D>(size*size);
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

    GLCHK(glGenVertexArrays(1, &screen_vao));
    GLCHK(glBindVertexArray(screen_vao));

    // Filling buffers with data:
    GLCHK(glGenBuffers(3, &vbos[0]));
    qDebug() << "Buffers ids:" << vbos[0] << ", " << vbos[1] << ", " << vbos[2];
    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, vbos[0]));
    GLCHK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float)*3, vertices.constData(), GL_STATIC_DRAW));
    GLCHK(glEnableVertexAttribArray(0));
    GLCHK(glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(float)*3,(void*)0));

    GLCHK(glBindBuffer(GL_ARRAY_BUFFER, vbos[1]));
    GLCHK(glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float)*2, texCoords.constData(), GL_STATIC_DRAW));
    GLCHK(glEnableVertexAttribArray(1));
    GLCHK(glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,(void*)0));

    GLCHK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]));

    int no_triangles = 2*(size - 1)*(size - 1);
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
    GLCHK(glBufferData(
              GL_ELEMENT_ARRAY_BUFFER,
              sizeof(GLuint) * no_triangles * 3 ,
              indices.constData(),
              GL_STATIC_DRAW
              ));

    GLCHK(glBindVertexArray(0));
}


void GLImage::updateMousePosition(){
    QPoint p = mapFromGlobal(QCursor::pos());
    cursorPhysicalXPosition =       double(p.x())/width() * orthographicProjWidth  - xTranslation;
    cursorPhysicalYPosition = (1.0-double(p.y())/height())* orthographicProjHeight - yTranslation;
    bSkipProcessing = true;    
}

void GLImage::wheelEvent(QWheelEvent *event){

    if( event->delta() > 0) zoom-=0.1;
        else zoom+=0.1;
    if(zoom < -0.90) zoom = -0.90;

    updateMousePosition();

    //resizeGL(width(),height());
    windowRatio = float(width())/height();
    if (isValid()) {
      GLCHK( glViewport(0, 0, width(), height()) );

      if (activeImage && activeImage->fbo){
        fboRatio = float(activeImage->fbo->width())/activeImage->fbo->height();
        orthographicProjHeight = (1+zoom)/windowRatio;
        orthographicProjWidth = (1+zoom)/fboRatio;
      } else {
        qWarning() << Q_FUNC_INFO;
        if (!activeImage) qWarning() << "  activeImage is null";
        else
      if (!activeImage->fbo) qWarning() << "  activeImage->fbo is null";
      }
    } else
      qDebug() << Q_FUNC_INFO << "invalid context.";

    QPoint p = mapFromGlobal(QCursor::pos());//getting the global position of cursor
    // restoring the translation after zooming
    xTranslation =        double(p.x())/width() *orthographicProjWidth  - cursorPhysicalXPosition;
    yTranslation = ((1.0-double(p.y())/height())*orthographicProjHeight - cursorPhysicalYPosition );

    updateGL();
}

void GLImage::relativeMouseMoveEvent(int dx, int dy, bool* wrapMouse, Qt::MouseButtons buttons)
{

    if(activeImage->imageType != GRUNGE_TEXTURE)
    if(FBOImageProporties::currentMaterialIndeks != MATERIALS_DISABLED && buttons & Qt::LeftButton){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping when materials textures are enabled.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }

    if(activeImage->imageType == MATERIAL_TEXTURE && buttons & Qt::LeftButton){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping of materials texture. This texture is static.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }

    if(activeImage->imageType == GRUNGE_TEXTURE && buttons & Qt::LeftButton){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping of Grunge texture. Try Diffuse or height texture.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }

    if(activeImage->imageType == OCCLUSION_TEXTURE && buttons & Qt::LeftButton){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping of occlusion texture. Try Diffuse or height texture.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }
    if(activeImage->imageType == NORMAL_TEXTURE && (buttons & Qt::LeftButton)){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping of normal texture. Try Diffuse or height texture.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }
    if(activeImage->imageType == METALLIC_TEXTURE && (buttons & Qt::LeftButton)){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping of metallic texture. Try Diffuse or height texture.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }

    if(activeImage->imageType == ROUGHNESS_TEXTURE && (buttons & Qt::LeftButton)){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping of roughness texture. Try Diffuse or height texture.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }
    if(activeImage->imageType == SPECULAR_TEXTURE && (buttons & Qt::LeftButton)){
        QMessageBox msgBox;
        msgBox.setText("Warning!");
        msgBox.setInformativeText("Sorry, but you cannot modify UV's mapping of specular texture. Try Diffuse or height texture.");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.exec();
        return;
    }

    QVector2D defCorners[4];//default position of corners
    defCorners[0] = QVector2D(0,0) ;
    defCorners[1] = QVector2D(1,0) ;
    defCorners[2] = QVector2D(1,1) ;
    defCorners[3] = QVector2D(0,1) ;
    QVector2D mpos((cursorPhysicalXPosition+0.5),(cursorPhysicalYPosition+0.5));

    // manipulate UV coordinates based on chosen method
    switch(uvManilupationMethod){
        // translate UV coordinates
        case(UV_TRANSLATE):
            if(buttons & Qt::LeftButton){ // drag image
                setCursor(Qt::SizeAllCursor);
                // move all corners
                QVector2D averagePos(0.0,0.0);
                QVector2D dmouse = QVector2D(-dx*(float(orthographicProjWidth)/width()),dy*(float(orthographicProjHeight)/height()));
                for(int i = 0; i < 4 ; i++){
                    averagePos += cornerPositions[i]*0.25;
                    if(activeImage->imageType == GRUNGE_TEXTURE) grungeCornerPositions[i] += dmouse;
                    else cornerPositions[i] += dmouse;
                }
                repaint();
            }
        break;
        // grab corners in perspective correction tool
        case(UV_GRAB_CORNERS):
            if(activeImage->imageType == GRUNGE_TEXTURE) break;
            if(draggingCorner == -1){
            setCursor(Qt::OpenHandCursor);
            for(int i = 0; i < 4 ; i++){
                float dist = (mpos - defCorners[i]).length();
                if(dist < 0.2){
                    setCursor(cornerCursors[i]);
                }
            }
            }// end if dragging
            if(buttons & Qt::LeftButton){
            // calculate distance from corners
            if(draggingCorner == -1){
            for(int i = 0; i < 4 ; i++){
                float dist = (mpos - defCorners[i]).length();
                if(dist < 0.2){
                    draggingCorner = i;
                }
            }// end of for corners
            }// end of if
            if(draggingCorner >=0 && draggingCorner < 4)
                cornerPositions[draggingCorner] += QVector2D(-dx*(float(orthographicProjWidth)/width()),dy*(float(orthographicProjHeight)/height()));
            repaint();
            }
        break;
        case(UV_SCALE_XY):
            if(activeImage->imageType == GRUNGE_TEXTURE) break;
            setCursor(Qt::OpenHandCursor);
            if(buttons & Qt::LeftButton){ // drag image
                setCursor(Qt::SizeAllCursor);
                QVector2D dmouse = QVector2D(-dx*(float(orthographicProjWidth)/width()),dy*(float(orthographicProjHeight)/height()));
                cornerWeights.setX(cornerWeights.x()-dmouse.x());
                cornerWeights.setY(cornerWeights.y()-dmouse.y());
                repaint();
            }
        break;
        default:;//no actions
    }


    if (buttons & Qt::RightButton){
        xTranslation += dx*(float(orthographicProjWidth)/width());
        yTranslation -= dy*(float(orthographicProjHeight)/height());
        setCursor(Qt::ClosedHandCursor);
    }

    // mouse looping in 2D view window
    *wrapMouse = (buttons & Qt::RightButton || buttons & Qt::LeftButton );


    updateMousePosition();
    if(bToggleColorPicking){
        setCursor(Qt::UpArrowCursor);
    }

    updateGL();
}
void GLImage::mousePressEvent(QMouseEvent *event)
{
    GLWidgetBase::mousePressEvent(event);

    bSkipProcessing = true;    
    draggingCorner = -1;
    // change cursor
    if (event->buttons() & Qt::RightButton) {
        setCursor(Qt::ClosedHandCursor);
    }
    updateGL();


    // In case of color picking: emit and stop picking
    if(bToggleColorPicking){
        vector< unsigned char > pixels( 1 * 1 * 4 );
        glReadPixels(event->pos().x(), height()-event->pos().y(), 1, 1,GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
        QVector4D color(pixels[0],pixels[1],pixels[2],pixels[3]);
        qDebug() << "Picked pixel (" << event->pos().x() << " , " << height()-event->pos().y() << ") with color:" << color;

        QColor qcolor = QColor(pixels[0],pixels[1],pixels[2]);
        if(ptr_ABColor != NULL) ptr_ABColor->setValue(qcolor);
        toggleColorPicking(false);
    }

}
void GLImage::mouseReleaseEvent(QMouseEvent *event){
    setCursor(Qt::OpenHandCursor);
    draggingCorner = -1;
    event->accept();
    bSkipProcessing = true;
    repaint();
}

void GLImage::toggleColorPicking(bool toggle){
    bToggleColorPicking = toggle;
    ptr_ABColor = NULL;
    if(toggle){
        setCursor(Qt::UpArrowCursor);
    }else
        setCursor(Qt::PointingHandCursor);
}

void GLImage::pickImageColor( QtnPropertyABColor* property) {
    bool toggle = true;
    bToggleColorPicking = toggle;
    ptr_ABColor = property;
    if(toggle){
        setCursor(Qt::UpArrowCursor);
    }else
        setCursor(Qt::PointingHandCursor);
    repaint();
}

void GLImage::copyRenderToPaintFBO(){
     GLCHK(FBOImages::resize(paintFBO,renderFBO->width(),renderFBO->height()));
     GLCHK( program->setUniformValue("material_id", int(-1)) );
     copyFBO(renderFBO,paintFBO);
     bRendering      = false;
}
