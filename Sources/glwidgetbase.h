#ifndef GLWIDGETBASE_H
#define GLWIDGETBASE_H

#include <QOpenGLWidget>
#include <QDebug>
#include "CommonObjects.h"

class GLWidgetBase : public QOpenGLWidget
{
    Q_OBJECT
public:
    GLWidgetBase(QWidget *parent=0);
    ~GLWidgetBase();

public:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_FINAL Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_FINAL Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_FINAL Q_DECL_OVERRIDE;

signals:
    void handleAccumulatedMouseMovementLater();
    void changeCamPositionApplied(bool);

protected:
    virtual void relativeMouseMoveEvent(int dx, int dy, bool* bMouseDragged, Qt::MouseButtons buttons) = 0;
    static bool wrapMouse;

public slots:
    void toggleMouseWrap(bool toggle);
    void toggleChangeCamPosition(bool toggle);

private slots:
    void handleAccumulatedMouseMovement();

private:
    void handleMovement();

    QPoint lastCursorPos;
    bool mouseUpdateIsQueued;
    bool blockMouseMovement;
    bool eventLoopStarted;

    int dx, dy;
    Qt::MouseButtons buttons;

protected:
    Qt::Key keyPressed;
    QCursor centerCamCursor;
};

#endif // GLWIDGETBASE_H
