#ifndef GLIMAGEEDITOR_H
#define GLIMAGEEDITOR_H

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

#include <QWidget>
#include <QGLWidget>
#include <QtOpenGL>
#include <math.h>
#include <map>
//#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_0_Core>
#include "CommonObjects.h"

QT_FORWARD_DECLARE_CLASS(QGLShaderProgram);


//! [0]
class GLImage : public QGLWidget , protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT

public:
    GLImage(QWidget *parent = 0 );
    ~GLImage();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void setActiveImage(FBOImageProporties* ptr);
    FBOImageProporties* getActiveImage(){return activeImage;}
    void enableShadowRender(bool enable);
    void enableRecalculateOcclusion(bool enable);
    void render();



    FBOImageProporties* targetImage;
    FBOImageProporties* targetImage2;
    FBOImageProporties* targetImageNormal;
    FBOImageProporties* targetImageHeight;
    FBOImageProporties* targetImageSpecular;
    FBOImageProporties* targetImageOcclusion;
public slots:

signals:
    void rendered();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void applyGaussFilter(QGLFramebufferObject* sourceFBO, QGLFramebufferObject *auxFBO,
                          QGLFramebufferObject* outputFBO, int no_iter);

    void applyInverseColorFilter(QGLFramebufferObject* inputFBO,
                                 QGLFramebufferObject* outputFBO);

    void applyNormalFilter(  QGLFramebufferObject* inputFBO,
                             QGLFramebufferObject* outputFBO);


    void applyCompressedFormatFilter(QGLFramebufferObject* baseFBO,
                                     QGLFramebufferObject* alphaFBO,
                                     QGLFramebufferObject* outputFBO);

    void applyOverlayFilter( QGLFramebufferObject* layerAFBO,
                             QGLFramebufferObject* layerBFBO,
                             QGLFramebufferObject* outputFBO);
    void applySeamlessFilter(QGLFramebufferObject* inputFBO,
                             QGLFramebufferObject* outputFBO, float radius);
    void applyDGaussiansFilter(QGLFramebufferObject* inputFBO,
                             QGLFramebufferObject *auxFBO,
                             QGLFramebufferObject* outputFBO);
    void applyContrastFilter(QGLFramebufferObject* inputFBO,
                             QGLFramebufferObject* outputFBO);
    void applySmallDetailsFilter(QGLFramebufferObject* inputFBO,
                                 QGLFramebufferObject *auxFBO,
                                 QGLFramebufferObject* outputFBO);
    void applyMediumDetailsFilter(QGLFramebufferObject* inputFBO,
                                          QGLFramebufferObject* auxFBO,
                                 QGLFramebufferObject* outputFBO);
    void applyGrayScaleFilter(QGLFramebufferObject* inputFBO,
                              QGLFramebufferObject* outputFBO);

    void applyInvertComponentsFilter(QGLFramebufferObject* inputFBO,
                                    QGLFramebufferObject* outputFBO);

    void applySharpenBlurFilter(QGLFramebufferObject* inputFBO,
                                QGLFramebufferObject *auxFBO,
                                QGLFramebufferObject* outputFBO);

    void applyNormalsStepFilter(QGLFramebufferObject* inputFBO,
                                QGLFramebufferObject* outputFBO);
    void applySobelToNormalFilter(QGLFramebufferObject* inputFBO,
                                  QGLFramebufferObject* outputFBO);

    void applySelectiveGaussFilter(QGLFramebufferObject* inputFBO,
                                  QGLFramebufferObject* outputFBO);

    void applyCPUNormalizationFilter(QGLFramebufferObject* inputFBO,
                                  QGLFramebufferObject* outputFBO);

    void applyHeightToNormal(QGLFramebufferObject* inputFBO,
                             QGLFramebufferObject* outputFBO);

    void applyNormalToHeight(FBOImageProporties *image,
                             QGLFramebufferObject* normalFBO,
                             QGLFramebufferObject* heightFBO,
                             QGLFramebufferObject* outputFBO);

    void applyBaseMapConversion(QGLFramebufferObject* baseMapFBO,
                                QGLFramebufferObject *auxFBO,
                                QGLFramebufferObject* outputFBO);
    void applyPreSmoothFilter(QGLFramebufferObject* inputFBO,
                              QGLFramebufferObject *auxFBO,
                             QGLFramebufferObject* outputFBO);

    void applyOcclusionFilter(QGLFramebufferObject* inputFBO,
                              QGLFramebufferObject* outputFBO);

    void applyCombineNormalHeightFilter(QGLFramebufferObject* normalFBO,
                                        QGLFramebufferObject *heightFBO,
                                        QGLFramebufferObject* outputFBO);

    void copyFBO(QGLFramebufferObject* src,QGLFramebufferObject* dst);

//! [3]
private:
    void makeScreenQuad();

    QGLShaderProgram *program;
    FBOImageProporties* activeImage;

    std::map<std::string,GLuint> subroutines;

    GLuint vbos[3];
    bool bShadowRender;
    bool bRecalculateOcclusion;
    float ratio;

};
//! [3]


#endif // GLIMAGEEDITOR_H
