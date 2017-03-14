#ifndef THREADRENDERER_H
#define THREADRENDERER_H

#include <QThread>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QGuiApplication>
#include <QOffscreenSurface>

class RenderThread;

class ThreadRenderer : public QObject
{
    Q_OBJECT

public:
    template<class R> ThreadRenderer() : m_renderThread(0) {
      m_renderThread = new R(QSize(512, 512));
    }

    static QList<QThread *> threads;

public Q_SLOTS:
    void ready();
    void update() { }

private:
    RenderThread *m_renderThread;
};

/*
 * The render thread shares a context with the scene graph and will
 * render into two separate FBOs, one to use for display and one
 * to use for rendering
 */
class RenderThread : public QThread
{
    Q_OBJECT
public:
    RenderThread(const QSize &size)
        : surface(0)
        , context(0)
        , m_renderFbo(0)
        , m_displayFbo(0)
        , m_size(size)
    {
        ThreadRenderer::threads << this;
    }

    QOffscreenSurface *surface;
    QOpenGLContext *context;

public slots:
    void renderNext()
    {
        context->makeCurrent(surface);

        if (!m_renderFbo) {
            // Initialize the buffers and renderer
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            m_renderFbo = new QOpenGLFramebufferObject(m_size, format);
            m_displayFbo = new QOpenGLFramebufferObject(m_size, format);
            initializeContentRenderer();
        }

        m_renderFbo->bind();
        context->functions()->glViewport(0, 0, m_size.width(), m_size.height());

	renderContent();

        // We need to flush the contents to the FBO before posting
        // the texture to the other thread, otherwise, we might
        // get unexpected results.
        context->functions()->glFlush();

        m_renderFbo->bindDefault();
        qSwap(m_renderFbo, m_displayFbo);

        emit textureReady(m_displayFbo->texture(), m_size);
    }

    void shutDown()
    {
        context->makeCurrent(surface);
        delete m_renderFbo;
        delete m_displayFbo;
        context->doneCurrent();
        delete context;

        // schedule this to be deleted only after we're done cleaning up
        surface->deleteLater();

        // Stop event processing, move the thread to GUI and make sure it is deleted.
        exit();
        moveToThread(QGuiApplication::instance()->thread());
    }

    virtual void initializeContentRenderer() = 0;
    virtual void renderContent() = 0;
    
signals:
    void textureReady(int id, const QSize &size);

private:
    QOpenGLFramebufferObject *m_renderFbo;
    QOpenGLFramebufferObject *m_displayFbo;

    QSize m_size;
};

#endif
