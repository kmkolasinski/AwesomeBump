#include "glwidgetbase.h"
#include <QThread>
#include <QMouseEvent>

GLWidgetBase::GLWidgetBase(const QGLFormat& format, QWidget *parent, QGLWidget * shareWidget)
    : QGLWidget(format, parent, shareWidget),
      updateIsQueued(false),
      eventLoopStarted(false)
{
    connect(this, &GLWidgetBase::updateGLLater, this, &GLWidgetBase::updateGLNow, Qt::QueuedConnection);
}

GLWidgetBase::~GLWidgetBase()
{
}

void GLWidgetBase::updateGLNow()
{
    // Now, after the area is actually drawn, we should be able to quue the next draws
    updateIsQueued = false;

    // Call the default updateGL implementation, which will call the paint method
    QGLWidget::updateGL();
}

void GLWidgetBase::updateGL()
{
    if(updateIsQueued == false)
    {
        // Queue the updating the OpenGL Widget
        updateIsQueued = true;
        updateGLLater();
    }

    // Workaround: When the viewport gets drawn for the first time, the rendering
    // has to be done multiple times for the end result to look correct.
    // This workaround passes every drawcall, until mouse events are received for
    // the first time.
    if(!eventLoopStarted)
        updateGLNow();
}

void GLWidgetBase::mousePressEvent(QMouseEvent *event)
{
    lastCursorPos = event->pos();
}

void GLWidgetBase::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastCursorPos.x();
    int dy = event->y() - lastCursorPos.y();
    bool wrapMouse = true;

    relativeMouseMoveEvent(dx, dy, &wrapMouse, event);

#ifndef Q_OS_LINUX
    lastCursorPos = event->pos();
#endif
    // mouse looping in view window

    if(wrapMouse){
        if(event->x() > width()-10){
            lastCursorPos.setX(10);
        }
        if(event->x() < 10){
            lastCursorPos.setX(width()-10);
        }

        if(event->y() > height()-10){
            lastCursorPos.setY(10);
        }
        if(event->y() < 10){
            lastCursorPos.setY(height()-10);
        }

        QCursor c = cursor();
        c.setPos(mapToGlobal(lastCursorPos));
        setCursor(c);

        updateGL();
    }

    eventLoopStarted = true;
}
