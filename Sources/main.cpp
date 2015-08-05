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

#include <QApplication>
#include <QDesktopWidget>
#include <QGLFormat>
#include <QtDebug>
#include "mainwindow.h"
#include "CommonObjects.h"
#include "glimageeditor.h"
#include "allaboutdialog.h"

// find data directory for each platform:
QString _find_data_dir(const QString& path)
{
   if (path.startsWith(":"))
     return path; // resource

   QString fpath = QApplication::applicationDirPath();
#if defined(Q_OS_MAC)
    fpath += "/../../../"+path;
#elif defined(Q_OS_WIN32)
    fpath = path;
#else
    fpath = path;
#endif
    qDebug() << "fphath2:" << fpath;
    return fpath;
}

// Redirect qDebug() to file log.txt file.
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   Q_UNUSED(context);

   QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
   QString txt = QString("[%1] ").arg(dt);

   switch (type)
   {
      case QtDebugMsg:
         txt += QString("{Debug} \t\t %1").arg(msg);
         break;
      case QtWarningMsg:
         txt += QString("{Warning} \t %1").arg(msg);
         break;
      case QtCriticalMsg:
         txt += QString("{Critical} \t %1").arg(msg);
         break;
      case QtFatalMsg:
         txt += QString("{Fatal} \t\t %1").arg(msg);
         abort();
         break;
   }

   // avoid recursive calling here: 
   QFile outFile(AB_LOG);
   if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
      // try any safe location:
      QString glob = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).arg(QFileInfo(AB_LOG).fileName());
      outFile.setFileName(glob);
      if (!outFile.open(QIODevice::WriteOnly | QIODevice::Append))
         return;
   }

   QTextStream textStream(&outFile);
   textStream << txt << endl;
}

bool checkOpenGL(){

    QGLWidget *glWidget = new QGLWidget;

    QGLContext* glContext = (QGLContext *) glWidget->context();
    GLCHK( glContext->makeCurrent() );

    qDebug() << "Running the " + QString(AWESOME_BUMP_VERSION);
    qDebug() << "Checking OpenGL version...";
    qDebug() << "Widget OpenGL:" << QString("%1.%2").arg(glContext->format().majorVersion()).arg(glContext->format().minorVersion());
    qDebug() << "Context valid:" << glContext->isValid() ;
    qDebug() << "OpenGL information:" ;
    qDebug() << "VENDOR:"       << (const char*)glGetString(GL_VENDOR) ;
    qDebug() << "RENDERER:"     << (const char*)glGetString(GL_RENDERER) ;
    qDebug() << "VERSION:"      << (const char*)glGetString(GL_VERSION) ;
    qDebug() << "GLSL VERSION:" << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION) ;

    float version = glContext->format().majorVersion() + 0.1 * glContext->format().minorVersion();
    Performance3DSettings::openGLVersion = version;
    #ifdef USE_OPENGL_330
        Performance3DSettings::openGLVersion = 3.3;
    #endif

    delete glWidget;

    qDebug() << "Version:" << version;

    #ifdef USE_OPENGL_330
        // check openGL version
        if( floor(version * 100) < 330 )
        {
           qDebug() << "Error: This version of AwesomeBump does not support openGL versions lower than 3.3 :(" ;
           return false;
        }
    #else
        // check openGL version
        if( floor(version * 100) < 400 )
        {
           qDebug() << "Error: AwesomeBump does not support openGL versions lower than 4.0 :(" ;
           return false;
        }
    #endif
    return true;

}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "Application dir:" << QApplication::applicationDirPath();
    qDebug() << "Data dir:" << _find_data_dir("");

    // Chossing proper GUI style from config.ini file.
    QSettings settings("config.ini", QSettings::IniFormat);
    // Dude, this default style is really amazing...
    // Seriously?
    // No...
    QString guiStyle = settings.value("gui_style","DefaultAwesomeStyle").toString();
    app.setStyle(QStyleFactory::create( guiStyle ));

    // Customize some elements:
    app.setStyleSheet("QGroupBox { font-weight: bold; } ");

    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(10);
    app.setFont(font);

    // removing old log file
    QFile::remove(AB_LOG);


    QGLFormat glFormat(QGL::SampleBuffers);

#ifdef Q_OS_MAC
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setVersion( 4, 1 );
#endif

#ifdef USE_OPENGL_330
#ifdef Q_OS_LINUX
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setVersion( 3, 3 );
#endif
#endif

    QGLFormat::setDefaultFormat(glFormat);


    qInstallMessageHandler(customMessageHandler);
    qDebug() << "Starting application:";

    if(!checkOpenGL()){

        AllAboutDialog msgBox;
        msgBox.setPixmap(":/resources/icon-off.png");
        msgBox.setText("Fatal Error!");

#ifdef USE_OPENGL_330
        msgBox.setInformativeText("Sorry but it seems that your graphics card does not support openGL 3.3.\n"
                                  "Program will not run :(\n"
                                  "See " AB_LOG " file for more info.");
#else
        msgBox.setInformativeText("Sorry but it seems that your graphics card does not support openGL 4.0.\n"
                                  "Program will not run :(\n"
                                  "See " AB_LOG " file for more info.");
#endif


        msgBox.show();

        return app.exec();
    }else{

        MainWindow window;
        window.setWindowTitle(AWESOME_BUMP_VERSION);
        window.resize(window.sizeHint());
        int desktopArea = QApplication::desktop()->width() *
                         QApplication::desktop()->height();
        int widgetArea = window.width() * window.height();
        if (((float)widgetArea / (float)desktopArea) < 0.75f)
            window.show();
        else
            window.showMaximized();

        return app.exec();

    }



}
