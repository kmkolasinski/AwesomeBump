#ifndef QOPENGLERRORCHECK_H
#define QOPENGLERRORCHECK_H

#include <QtCore/qglobal.h>
#include <QtCore/qdebug.h>

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)
#include <QtOpenGL>


QT_BEGIN_NAMESPACE

static void checkOpenGLError( const char* stmt, const char* function, const char* file, int line, const char* info )
{
  GLenum err = glGetError();
  while (err != GL_NO_ERROR)
    {
      QString error="?";
      switch(err) {
      case GL_NO_ERROR:               error="NO_ERROR";          break;
      case GL_INVALID_OPERATION:      error="INVALID_OPERATION"; break;
      case GL_INVALID_ENUM:           error="INVALID_ENUM";      break;
      case GL_INVALID_VALUE:          error="INVALID_VALUE";     break;
      case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";     break;
# ifdef GL_INVALID_FRAMEBUFFER_OPERATION
      case GL_INVALID_FRAMEBUFFER_OPERATION: error="INVALID_FRAMEBUFFER_OPERATION"; break;
# elif defined GL_INVALID_FRAMEBUFFER_OPERATION_EXT
      case GL_INVALID_FRAMEBUFFER_OPERATION_EXT: error="INVALID_FRAMEBUFFER_OPERATION"; break;
# endif
#ifdef GL_STACK_OVERFLOW
      case GL_STACK_OVERFLOW:         error="STACK_OVERFLOW";    break;
#endif
#ifdef GL_STACK_UNDERFLOW
      case GL_STACK_UNDERFLOW:        error="STACK_UNDERFLOW";   break;
#endif
#ifdef GL_TABLE_TOO_LARGE
      case GL_TABLE_TOO_LARGE:        error="TABLE_TOO_LARGE";   break;
#endif
    // GLU
#ifdef GLU_INVALID_ENUM
      case GLU_INVALID_ENUM:          error="GLU_INVALID_ENUM";  break;
      case GLU_INVALID_VALUE:         error="GLU_INVALID_VALUE"; break;
      case GLU_OUT_OF_MEMORY:         error="GLU_OUT_OF_MEMORY"; break;
      case GLU_INCOMPATIBLE_GL_VERSION: error="GLU_INCOMPATIBLE_GL_VERSION"; break;
      case GLU_INVALID_OPERATION:     error="GLU_INVALID_OPERATION"; break;
#endif
      }
      qDebug() << "OpenGL error" << error << hex << err << dec << "at" << stmt << "called from" << function << "in file" << file << "line" << line << (info?info:"");
# ifdef ABORT_ON_GL_ERR
      abort();
# endif
      err = glGetError();
    }
}

#define GLCHK(stmt) {                                                   \
        (stmt);                                                         \
        checkOpenGLError(#stmt, Q_FUNC_INFO, __FILE__, __LINE__, NULL); \
    }

#ifdef GNU_C
    // In GNU_C we can use statement macros as expressions...
    #define GLCHK2(stmt, R) {(                                          \
        R __ret=(stmt);                                                 \
        checkOpenGLError(#stmt, Q_FUNC_INFO, __FILE__, __LINE__, NULL); \
        __ret;					    \
    })
#else
    //... otherwise we just use lambda functions
    #define GLCHK2(stmt, R) [](){                                         \
          R __ret=(stmt);                                                 \
          checkOpenGLError(#stmt, Q_FUNC_INFO, __FILE__, __LINE__, NULL); \
          return __ret;                                                   \
    }();
#endif

#endif

#endif // QOPENGLERRORCHECK_H

