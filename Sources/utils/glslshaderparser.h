#ifndef GLSLSHADERPARSER_H
#define GLSLSHADERPARSER_H


#include <QOpenGLFunctions_3_3_Core>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QDir>
#include "CommonObjects.h"


enum UniformDataType{
  UNIFORM_TYPE_INT = 0,
  UNIFORM_TYPE_FLOAT
};
struct UniformData{
    UniformDataType type;
    float value;
    float min;
    float max;
    float step;
    QString varName;
    QString name;
    QString description;
    UniformData(){
        value = 50.0;
        min   = 0.0;
        max   = 100.0;
        step  = 1.0;
        name = "param#";
        description = "None";
    }
    QString toString(){
        QString uniformString =
                "Uniform    :" + name + "\n"
                "Type       :" + QString::number(type) + "\n"
                "Data       :" + QString::number(value) + " range=["+ QString::number(min)+","+ QString::number(max)+";"+ QString::number(step)+"]\n"
                "Description:" + description;
        return uniformString;
    }
};

class GLSLShaderParser: public QOpenGLFunctions_3_3_Core
{
public:
    GLSLShaderParser();
    bool parseShader(QString path);
    void reparseShader();
    void setParsedUniforms();
    ~GLSLShaderParser();

private:
    void cleanup();
    void parseLine(const QString& line);
    void parseUniformParameters(UniformData &uniform, const QString &line, const QString& param);

    QStringList reservedNames;  // List of uniforms names which will be not parsed
    QStringList supportedTypes; // List of types (int,float,...) which can be parsed
    QStringList supportedParams;// List of regular expresions to obtain the maxium, minium etc parameters of the uniform

public:
    QString shaderName;
    QVector<UniformData> uniforms; // Contains all editable parsed uniforms
    QString shaderPath;
    QOpenGLShaderProgram *program; // glsl shader

};


//QOpenGLShaderProgram* program_ptr = program_ptrs[pindex];
//GLCHK( program_ptr->bind() );

//GLCHK( program_ptr->setUniformValue("ProjectionMatrix", projectionMatrix) );

#endif // GLSLSHADERPARSER_H
