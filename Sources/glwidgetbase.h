#ifndef GLWIDGETBASE_H
#define GLWIDGETBASE_H

#include <QGLWidget>

class GLWidgetBase : public QGLWidget
{
    Q_OBJECT
public:
    GLWidgetBase(const QGLFormat& format,
                 QWidget *parent=0,
                 QGLWidget *shareWidget=0);
    ~GLWidgetBase();

public:
    // Instead of updating the opengl area immediatly, this queues drawing and
    // makes sure, to do it only once until updateGLNow was called.
    void updateGL() Q_DECL_FINAL Q_DECL_OVERRIDE;

signals:
    void updateGLLater();

private slots:
    void updateGLNow();

private:
    bool updateIsQueued;
};

#endif // GLWIDGETBASE_H
