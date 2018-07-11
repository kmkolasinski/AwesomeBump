#include "threadrenderer.h"
#include "glimageeditor.h"
#include "qtofflinegl.h"


QList<QThread *> ThreadRenderer::threads;
QQueue<TextureTypes> ThreadRenderer::requests;

ThreadRenderer::ThreadRenderer() : m_renderThread(0) {
    m_renderThread = new RenderThread(QSize(512, 512), m_worker);
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

void RenderThread::run()
{
    forever
    {
        m_mutex.lock();
        m_worker.wait(&m_mutex);
        if (ThreadRenderer::requests.size()) {
            TextureTypes tt = ThreadRenderer::requests.dequeue();
            // GLImage::updateImage();
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
