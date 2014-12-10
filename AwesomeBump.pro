VPATH += ../shared
INCLUDEPATH += ../shared

HEADERS       = glwidget.h \
    mainwindow.h \
    CommonObjects.h \
    formimageprop.h \
    glimageeditor.h
SOURCES       = glwidget.cpp \
                main.cpp \
    mainwindow.cpp \
    formimageprop.cpp \
    glimageeditor.cpp \
    CommonObjects.cpp
QT           += opengl widgets

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl
INSTALLS += target
CONFIG += staticlib

contains(QT_CONFIG, opengles.) {
    contains(QT_CONFIG, angle): \
        warning("Qt was built with ANGLE, which provides only OpenGL ES 2.0 on top of DirectX 9.0c")
    error("This example requires Qt to be configured with -opengl desktop")
}

RESOURCES += \
    content.qrc

FORMS += \
    mainwindow.ui \
    formimageprop.ui

OTHER_FILES += \
    cube.frag
