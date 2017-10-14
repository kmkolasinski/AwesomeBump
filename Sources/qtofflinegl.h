#ifndef QTOFFLINEGL_H
#define QTOFFLINEGL_H

#include <qopengl.h>
#include <QOpenGLContext>
#include <QOffscreenSurface>

extern QOpenGLContext *shareContext();


class QtOfflineGL
{
public:
	QOpenGLContext *m_context;
	QOffscreenSurface *m_surface;

	QtOfflineGL() {
        m_context = new QOpenGLContext();
        m_surface = new QOffscreenSurface();
        m_context->setShareContext(shareContext());
        m_surface->create();
        m_context->create();
    }
	~QtOfflineGL() {
        delete m_surface;
        delete m_context;
    }

	void makeCurrent() {
        if (m_context) {
            m_context->makeCurrent(m_surface);
        }
    }
	void doneCurrent() {
        if (m_context) {
            m_context->doneCurrent();
        }
    }
    bool isValid() const { Q_ASSERT(m_context); return m_context->isValid(); }
    QOpenGLContext *context() const { return m_context; }
    QOpenGLFunctions *functions() const { Q_ASSERT(m_context); return m_context->functions(); }
    bool hasExtension(const QByteArray &extension) const {  Q_ASSERT(m_context); return m_context->hasExtension(extension); }
};

#endif
