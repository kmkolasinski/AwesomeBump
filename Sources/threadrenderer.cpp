#include "threadrenderer.h"
#include "glimage.h"
#include "qtofflinegl.h"


QList<QThread *> ThreadRenderer::threads;

ThreadRenderer::ThreadRenderer() : m_renderThread(0) {
    m_renderThread = new RenderThread(QSize(512, 512));
}

void ThreadRenderer::ready()
{
    m_renderThread->surface = new QOffscreenSurface();
    m_renderThread->surface->setFormat(m_renderThread->context->format());
    m_renderThread->surface->create();

    m_renderThread->moveToThread(m_renderThread);

    m_renderThread->start();
    update();
}

void ThreadRenderer::update()
{
    if (!m_renderThread->context) {
        m_renderThread->context = new QOpenGLContext();
        m_renderThread->context->setShareContext(shareContext());
        m_renderThread->context->create();
        m_renderThread->context->moveToThread(m_renderThread);

        QMetaObject::invokeMethod(this, "ready");
        return;
    }
}


void RenderThread::renderContent()
{
}

void RenderThread::run()
    while(!shutdown)
    {
        m_mutex.lock();
        m_worker.wait(&m_mutex);
        if (ThreadRenderer::requests.size()) {
            TextureTypes tt = ThreadRenderer::requests.dequeue();
            GLImage::updateImage();
            emit textureReady(tt);
        }
        m_mutex.unlock();
    }
}

void RenderThread::initializeContentRenderer()
{
}

void RenderThread::renderContent()
{
}
