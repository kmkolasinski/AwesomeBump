#include "glslshaderparser.h"

GLSLShaderParser::GLSLShaderParser()
{
    initializeOpenGLFunctions();
    reservedNames   << "num_mipmaps"
                    << "gui_bSpecular"
                    << "bOcclusion"
                    << "gui_bHeight"
                    << "gui_bDiffuse"
                    << "gui_bOcclusion"
                    << "gui_bNormal"
                    << "gui_bRoughness"
                    << "gui_bMetallic"
                    << "gui_LightPower"
                    << "gui_LightRadius"
                    << "gui_noPBRRays"
                    << "gui_bUseSimplePBR"
                    << "gui_bMaterialsPreviewEnabled"
                    << "gui_bShowTriangleEdges"
                    << "gui_depthScale"
                    << "gui_SpecularIntensity"
                    << "gui_DiffuseIntensity"
                    << "gui_shading_type"
                    << "gui_shading_model"
                    << "cameraPos"
                    << "lightDirection"
                    << "lightPos"
                    << "ModelMatrix"
                    << "ModelViewMatrix";

    supportedTypes  << "int" << "float";

    supportedParams << "[ ]*max[ ]*=[ ]*[+-]?[0-9]*\.?[0-9]+"
                    << "[ ]*min[ ]*=[ ]*[+-]?[0-9]*\.?[0-9]+"
                    << "[ ]*value[ ]*=[ ]*[+-]?[0-9]*\.?[0-9]+"
                    << "[ ]*step[ ]*=[ ]*[+-]?[0-9]*\.?[0-9]+"
                    << "[ ]*name[ ]*=[ ]*\"[a-zA-Z0-9 -+'()]*\""
                    << "[ ]*description[ ]*=[ ]*\"[a-zA-Z0-9 -+'()]*\"";
    program = NULL;

}

void GLSLShaderParser::cleanup(){
    if(program != NULL )delete program;
}

GLSLShaderParser::~GLSLShaderParser()
{
    cleanup();
}


bool GLSLShaderParser::parseShader(QString path){

    QFile file(path);
    shaderPath = path;


    if(!file.exists()){
        qDebug() << "GLSL Parser::File:" << path << " does not extist.";
        return false;
    }
    QFileInfo fileInfo(file);
    shaderName = fileInfo.baseName();
    qDebug() << "GLSL Parser::Shader name:" << shaderName;

    // Scann whole file and look for uniforms
    if (file.open(QIODevice::ReadOnly))
    {
       QTextStream in(&file);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          parseLine(line);
       }
       file.close();
    }


    return true;
}

void GLSLShaderParser::reparseShader(){
    uniforms.clear();
    parseShader(shaderPath);
}

void GLSLShaderParser::parseLine(const QString &line){

    if(line.contains("uniform", Qt::CaseSensitive)){
        QStringList lineList = line.split(" ",QString::SkipEmptyParts);
        QString uName = lineList.at(2);
        // get name and remove semicolon
        uName = uName.split(";",QString::SkipEmptyParts).at(0);
        QString uType = lineList.at(1); // type is usually second after "uniform" key
        // check if name is in the resevedNames and type is supported
        if(!reservedNames.contains(uName,Qt::CaseSensitive) &&
           supportedTypes.contains(uType,Qt::CaseSensitive)){
            // Parsing parameters of uniform variable
            UniformData parsedUniform;
            if(uType == "int")   parsedUniform.type = UNIFORM_TYPE_INT;
            if(uType == "float") parsedUniform.type = UNIFORM_TYPE_FLOAT;
            parsedUniform.varName = uName;
            QStringListIterator iterator(supportedParams);
            while (iterator.hasNext())
                parseUniformParameters(parsedUniform,line,iterator.next());

            qDebug() << parsedUniform.toString();
            uniforms.push_back(parsedUniform);
        }
    }
}


void GLSLShaderParser::parseUniformParameters(UniformData& parsedUniform,const QString &line, const QString &param){

    QRegularExpression regLine(param);
    QRegularExpressionMatch match = regLine.match(line);
    // Reading data
    if (match.hasMatch()) {
        QString matched = match.captured(0);
        QStringList splitted = matched.split("=",QString::SkipEmptyParts);

        if(splitted[0].contains("min")){
            parsedUniform.min = QVariant(splitted[1]).toFloat();
        }else if(splitted[0].contains("max")){
            parsedUniform.max = QVariant(splitted[1]).toFloat();
        }else if(splitted[0].contains("value")){
            parsedUniform.value = QVariant(splitted[1]).toFloat();
        }else if(splitted[0].contains("step")){
            parsedUniform.step = QVariant(splitted[1]).toFloat();
        }else if(splitted[0].contains("name")){
            parsedUniform.name = (splitted[1]);
        }else if(splitted[0].contains("description")){
            parsedUniform.description = (splitted[1]);
        }
    }
}


void GLSLShaderParser::setParsedUniforms(){
    for(unsigned int u = 0 ; u < uniforms.size() ; u++){
        UniformData& uniform = uniforms[u];
        switch (uniform.type) {
        case UNIFORM_TYPE_FLOAT:
            GLCHK( program->setUniformValue(uniform.varName.toStdString().c_str(), (float)uniform.value) );
            break;
        case UNIFORM_TYPE_INT:
            GLCHK( program->setUniformValue(uniform.varName.toStdString().c_str(), (int)uniform.value) );
            break;
        default:
            break;
        }
    }
}
