#include "glwidgetbase.h"
#include <QThread>

GLWidgetBase::GLWidgetBase(const QGLFormat& format, QWidget *parent, QGLWidget * shareWidget)
    : QGLWidget(format, parent, shareWidget),
      updateIsQueued(false)
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
}
