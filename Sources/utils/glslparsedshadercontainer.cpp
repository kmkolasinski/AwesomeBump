#include "glslparsedshadercontainer.h"

extern QString _find_data_dir(const QString& base);


GLSLParsedShaderContainer::GLSLParsedShaderContainer()
{
    // ------------------------------------------------------- //
    //            Parsing shaders from Render folder
    // ------------------------------------------------------- //
    qDebug() << "Parsing shaders in Render folder:";
    QDir currentDir(_find_data_dir(QString(RESOURCE_BASE) + "Core/Render"));
    currentDir.setFilter(QDir::Files);
    QStringList entries = currentDir.entryList();
    qDebug() << "Looking for shaders in Core/Render directory";
    for( QStringList::ConstIterator entry=entries.begin(); entry!=entries.end(); ++entry ){
        QString dirname=*entry;
        GLSLShaderParser* parser = new GLSLShaderParser;
        parser->parseShader("Core/Render/"+dirname);
        glslParsedShaders.push_back(parser);
    }// end of for

}

GLSLParsedShaderContainer::~GLSLParsedShaderContainer()
{
    for(int i = 0 ; i < glslParsedShaders.size() ; i++){
        delete glslParsedShaders[i];
    }
    glslParsedShaders.clear();
}

