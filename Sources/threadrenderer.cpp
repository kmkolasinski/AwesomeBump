#include "threadrenderer.h"
#include <QOffscreenSurface>


QList<QThread *> ThreadRenderer::threads;

void ThreadRenderer::ready()
{
    m_renderThread->surface = new QOffscreenSurface();
    m_renderThread->surface->setFormat(m_renderThread->context->format());
    m_renderThread->surface->create();

    m_renderThread->moveToThread(m_renderThread);

    m_renderThread->start();
    update();
}
