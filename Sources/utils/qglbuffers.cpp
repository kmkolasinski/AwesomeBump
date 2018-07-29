/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglbuffers.h"
#include <QtGui/qmatrix4x4.h>


void qgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    const GLdouble ymax = zNear * tan(fovy * M_PI / 360.0);
    const GLdouble ymin = -ymax;
    const GLdouble xmin = ymin * aspect;
    const GLdouble xmax = ymax * aspect;
    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

//============================================================================//
//                                  GLTexture                                 //
//============================================================================//

GLTexture::GLTexture() : m_texture(0), m_failed(false)
{
    initializeOpenGLFunctions();
    fbo = 0;    
    glGenTextures(1, &m_texture);
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_texture);
    if(fbo != 0 ) glDeleteFramebuffers(1, &fbo);
}

//============================================================================//
//                                 GLTexture2D                                //
//============================================================================//

GLTexture2D::GLTexture2D(int width, int height)
{
    GLCHK(glBindTexture(GL_TEXTURE_2D, m_texture));
    GLCHK(glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, 0));

    GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
}


GLTexture2D::GLTexture2D(const QString& fileName, int width, int height)
{
    // TODO: Add error handling.
    QImage image(fileName);

    if (image.isNull()) {
        m_failed = true;
        return;
    }

    image = image.convertToFormat(QImage::Format_ARGB32);

    //qDebug() << "Image size:" << image.width() << "x" << image.height();
    if (width <= 0)
        width = image.width();
    if (height <= 0)
        height = image.height();
    if (width != image.width() || height != image.height())
        image = image.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    glBindTexture(GL_TEXTURE_2D, m_texture);

    // Works on x86, so probably works on all little-endian systems.
    // Does it work on big-endian systems?
    glTexImage2D(GL_TEXTURE_2D, 0, 4, image.width(), image.height(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2D::load(int width, int height, QRgb *data)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2D::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glEnable(GL_TEXTURE_2D);
}

void GLTexture2D::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}


//============================================================================//
//                                 GLTexture3D                                //
//============================================================================//

GLTexture3D::GLTexture3D(int width, int height, int depth)
{

    glBindTexture(GL_TEXTURE_3D, m_texture);
    glTexImage3D(GL_TEXTURE_3D, 0, 4, width, height, depth, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void GLTexture3D::load(int width, int height, int depth, QRgb *data)
{

    glBindTexture(GL_TEXTURE_3D, m_texture);
    glTexImage3D(GL_TEXTURE_3D, 0, 4, width, height, depth, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_3D, 0);
}

void GLTexture3D::bind()
{
    glBindTexture(GL_TEXTURE_3D, m_texture);
    glEnable(GL_TEXTURE_3D);
}

void GLTexture3D::unbind()
{
    glBindTexture(GL_TEXTURE_3D, 0);
    glDisable(GL_TEXTURE_3D);
}

//============================================================================//
//                                GLTextureCube                               //
//============================================================================//

GLTextureCube::GLTextureCube(int size)
{
    GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));
    qDebug() << "m_texture:" << m_texture;
    for (int i = 0; i < 6; ++i)
        GLCHK(glTexImage2D(
                  GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size, size, 0,
                  GL_BGRA, GL_UNSIGNED_BYTE, 0)
              );

    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

    // from http://stackoverflow.com/questions/462721/rendering-to-cube-map
    // framebuffer object
    GLCHK(glGenFramebuffers   (1, &fbo));
    GLCHK(glBindFramebuffer   (GL_FRAMEBUFFER, fbo));
    GLCHK(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0));
    GLCHK(glDrawBuffer        (GL_COLOR_ATTACHMENT0)); // important!

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        GLCHK(glDeleteFramebuffers(1, &fbo));
        fbo = 0;
    }
    GLCHK(glBindFramebuffer(GL_FRAMEBUFFER,0));
}

GLTextureCube::GLTextureCube(const QStringList& fileNames, int size)
{
    // TODO: Add error handling.

    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));

    int index = 0;
    foreach (QString file, fileNames) {
        QImage image(file);
        if (image.isNull()) {
            m_failed = true;
            break;
        }

        image = image.convertToFormat(QImage::Format_ARGB32);

        //qDebug() << "Image size:" << image.width() << "x" << image.height();
        if (size <= 0)
            size = image.width();
        if (size != image.width() || size != image.height())
            image = image.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // Works on x86, so probably works on all little-endian systems.
        // Does it work on big-endian systems?
        GLCHK(glTexImage2D(
                  GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0,
                  GL_RGBA, image.width(), image.height(), 0,
                  GL_BGRA, GL_UNSIGNED_BYTE, image.bits()
                  ));

        if (++index == 6)
            break;
    }

    // Clear remaining faces.
    while (index < 6) {
        GLCHK(glTexImage2D(
                  GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, GL_RGBA,
                  size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0
                  ));
        ++index;
    }

    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));    
    GLCHK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCHK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

    // get number of mip maps
    if ( (numMipmaps = textureCalcLevels(GL_TEXTURE_CUBE_MAP_POSITIVE_X)) == -1 ) {
            int max_level;
            glGetTexParameteriv( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, &max_level );
            numMipmaps = 1 + floor(log2(size > max_level?size:max_level) );
    }

    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    qDebug() << "Generated number of mipmaps:" << numMipmaps;

}

void GLTextureCube::load(int size, int face, QRgb *data)
{
    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));
    GLCHK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 4, size, size, 0,
            GL_BGRA, GL_UNSIGNED_BYTE, data));
    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

void GLTextureCube::bind()
{
    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture));
//    GLCHK(glEnable(GL_TEXTURE_CUBE_MAP));
}

void GLTextureCube::bindFBO(){
    GLCHK(glBindFramebuffer   (GL_FRAMEBUFFER, fbo));
    GLCHK(glDrawBuffer        (GL_COLOR_ATTACHMENT0)); // important!
}


void GLTextureCube::unbind()
{
    GLCHK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    GLCHK(glDisable(GL_TEXTURE_CUBE_MAP));
}

int GLTextureCube::textureCalcLevels(GLenum target)
{
  int max_level;
  GLCHK(glGetTexParameteriv( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, &max_level ));
  int max_mipmap = -1;
  for ( int i = 0; i < max_level; ++i ) {
    int width;
    GLCHK(glGetTexLevelParameteriv( target, i, GL_TEXTURE_WIDTH, &width ));
    if ( 0 == width || GL_INVALID_VALUE == width) {
      max_mipmap = i - 1;
      break;
    }
  }
  return max_mipmap;
}



//============================================================================//
//                            GLFrameBufferObject                             //
//============================================================================//

GLFrameBufferObject::GLFrameBufferObject(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_failed(false)
{
    GLCHK(initializeOpenGLFunctions());

    fbo = NULL;
    attachments.clear();
    QGLFramebufferObjectFormat format;
    format.setInternalTextureFormat(TEXTURE_3DRENDER_FORMAT);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setMipmap(true);
    format.setAttachment(QGLFramebufferObject::Depth);
    fbo = new QGLFramebufferObject(width, height, format);

    GLCHK(glBindTexture(GL_TEXTURE_2D, fbo->texture()));
    GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
    GLCHK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
    GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCHK(glBindTexture(GL_TEXTURE_2D, 0));

    GLCHK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

GLFrameBufferObject::~GLFrameBufferObject()
{
    fbo->release();
    for(unsigned int i = 0;i< attachments.size();i++){
        GLCHK(glDeleteTextures(1,&attachments[i]));
    }

    attachments.erase(attachments.begin(),attachments.end());

    delete fbo;
}


bool GLFrameBufferObject::isComplete()
{
    return GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER);
}
void GLFrameBufferObject::bind(){
    GLCHK(fbo->bind());
}
void GLFrameBufferObject::bindDefault(){
    GLCHK(fbo->bindDefault());
}

bool GLFrameBufferObject::addTexture(GLenum COLOR_ATTACHMENTn){

    GLuint tex[1];
    GLCHK(glGenTextures(1, &tex[0]));
    GLCHK(glBindTexture(GL_TEXTURE_2D, tex[0]));
    GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
    GLCHK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
    GLCHK(glTexImage2D(GL_TEXTURE_2D, 0, TEXTURE_3DRENDER_FORMAT, fbo->width(), fbo->height(), 0,GL_RGB, GL_UNSIGNED_BYTE, 0));
    GLCHK(glBindTexture(GL_TEXTURE_2D, 0));
    if(!glIsTexture(tex[0])){
        qDebug() << "Error: Cannot create additional texture. Process stopped." << endl;
        return false;
    }
    GLCHK(fbo->bind());

    GLCHK(glFramebufferTexture2D(GL_FRAMEBUFFER, COLOR_ATTACHMENTn,GL_TEXTURE_2D, tex[0], 0));

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        qDebug() << "Cannot add new texture to current FBO! FBO is incomplete.";
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    // switch back to window-system-provided framebuffer
    GLCHK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    attachments.push_back(tex[0]);
    return true;
}

const GLuint& GLFrameBufferObject::getAttachedTexture(GLuint index){
    return attachments[index];
}

