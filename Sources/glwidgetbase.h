#ifndef GLWIDGETBASE_H
#define GLWIDGETBASE_H

#include <QGLWidget>
#include <QDebug>
#include "CommonObjects.h"

extern bool isOffScreenRenderingEnabled;

class GLWidgetBase : public QGLWidget
{
    Q_OBJECT
public:
    GLWidgetBase(const QGLFormat& format,
                 QWidget *parent=0,
                 QGLWidget *shareWidget=0);
    ~GLWidgetBase();

public:
    void doOffscreenRender();

    // Instead of updating the opengl area immediatly, this queues drawing and
    // makes sure, to do it only once until updateGLNow was called.
    void updateGL() Q_DECL_FINAL Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;


    void mouseMoveEvent(QMouseEvent *event) Q_DECL_FINAL Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) ;
    void keyReleaseEvent(QKeyEvent *event) ;

signals:
    void updateGLLater();
    void handleAccumulatedMouseMovementLater();
    void changeCamPositionApplied(bool);
protected:
    virtual void relativeMouseMoveEvent(int dx, int dy, bool* bMouseDragged, Qt::MouseButtons buttons) = 0;
    static bool wrapMouse;

public slots:
    void updateGLNow();
    void toggleMouseWrap(bool toggle);
    void toggleChangeCamPosition(bool toggle);

private slots:
    void handleAccumulatedMouseMovement();

private:
    void handleMovement();

    QPoint lastCursorPos;
    bool updateIsQueued;
    bool mouseUpdateIsQueued;
    bool blockMouseMovement;
    bool eventLoopStarted;

    int dx, dy;
    Qt::MouseButtons buttons;

    bool isInitOffscreen = false;

protected:
    Qt::Key keyPressed;
    QCursor centerCamCursor;
};

#endif // GLWIDGETBASE_H
