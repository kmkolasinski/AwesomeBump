#include "glwidgetbase.h"
#include <QThread>
#include <QMouseEvent>

GLWidgetBase::GLWidgetBase(const QGLFormat& format, QWidget *parent, QGLWidget * shareWidget)
    : QGLWidget(format, parent, shareWidget),
      updateIsQueued(false),
      mouseUpdateIsQueued(false),
      eventLoopStarted(false),
      dx(0),
      dy(0),
      buttons(0)
{
    connect(this, &GLWidgetBase::updateGLLater, this, &GLWidgetBase::updateGLNow, Qt::QueuedConnection);
    connect(this, &GLWidgetBase::handleAccumulatedMouseMovementLater, this, &GLWidgetBase::handleAccumulatedMouseMovement, Qt::QueuedConnection);
    setMouseTracking(true);
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

    // reset the mouse handling state with, to avoid a bad state
    blockMouseMovement = false;
    mouseUpdateIsQueued = false;
}

void GLWidgetBase::mouseMoveEvent(QMouseEvent *event)
{
    if(blockMouseMovement)
    {
        // If the mouse was wrapped manually, ignore all mouse events until
        // the event caused by wrapping itself appears to avoid flickering.
        if(event->pos() != lastCursorPos)
            return;
        blockMouseMovement = false;
    }

    // Accumulate the mouse events
    dx += event->x() - lastCursorPos.x();
    dy += event->y() - lastCursorPos.y();
    buttons |= event->buttons();

    lastCursorPos = event->pos();

    // Don't handle mouse movements directly, instead accumulate all queued mouse
    // movements and execute all at once.
    handleMovement();
}

void GLWidgetBase::handleMovement()
{
    // mouseUpdateIsQueued is used to make sure to only handle the accumulated
    // mouse events once.
    if(mouseUpdateIsQueued == false)
    {
        mouseUpdateIsQueued = true;

        // Queue handling teh accumulated mouse events at a later point in time
        handleAccumulatedMouseMovementLater();
    }
}

void GLWidgetBase::handleAccumulatedMouseMovement()
{
    // As we are handling all queued mouse events, we can accumulate new events
    // from now on
    mouseUpdateIsQueued = false;

    bool wrapMouse = true;

    relativeMouseMoveEvent(dx, dy, &wrapMouse, buttons);

    dx = 0;
    dy = 0;
    buttons = 0;

    if(wrapMouse){

        bool changed = false;

        if(lastCursorPos.x() > width()-10){
            lastCursorPos.setX(10);
            changed = true;
        }
        if(lastCursorPos.x() < 10){
            lastCursorPos.setX(width()-10);
            changed = true;
        }

        if(lastCursorPos.y() > height()-10){
            lastCursorPos.setY(10);
            changed = true;
        }
        if(lastCursorPos.y() < 10){
            lastCursorPos.setY(height()-10);
            changed = true;
        }

        if(changed)
        {
            QCursor c = cursor();
            c.setPos(mapToGlobal(lastCursorPos));
            setCursor(c);

            // There will be mouse events, which were queued before the mouse
            // cursor was set. We have to ignore that events to avoid flickering.
            blockMouseMovement = true;
        }

        updateGL();
    }

    eventLoopStarted = true;
}
