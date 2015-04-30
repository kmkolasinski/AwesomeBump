CONFIG       += c++11
QT           += opengl gui widgets

CONFIG(release_gl330) {
    #This is a release build
    DEFINES += USE_OPENGL_330
    TARGET = AwesomeBumpGL330
} else {
    TARGET = AwesomeBump
    #This is a debug build
}

VPATH += ../shared
INCLUDEPATH += ../shared

HEADERS       = glwidget.h \
    mainwindow.h \
    CommonObjects.h \
    formimageprop.h \
    glimageeditor.h \
    camera.h \
    dialogheightcalculator.h \
    qopenglerrorcheck.h \
    utils/Mesh.hpp \
    utils/tinyobj/tiny_obj_loader.h \
    formsettingsfield.h \
    formsettingscontainer.h \
    utils/qglbuffers.h \
    dialoglogger.h \
    glwidgetbase.h \
    formbasemapconversionlevels.h \
    formmaterialindicesmanager.h \
    dialogshortcuts.h \
    formimagebase.h

SOURCES       = glwidget.cpp \
                main.cpp \
    mainwindow.cpp \
    formimageprop.cpp \
    glimageeditor.cpp \
    CommonObjects.cpp \
    camera.cpp \
    dialogheightcalculator.cpp \
    utils/Mesh.cpp \
    utils/tinyobj/tiny_obj_loader.cc \
    formsettingsfield.cpp \
    formsettingscontainer.cpp \
    utils/qglbuffers.cpp \
    dialoglogger.cpp \
    glwidgetbase.cpp \
    formbasemapconversionlevels.cpp \
    formmaterialindicesmanager.cpp \
    dialogshortcuts.cpp \
    formimagebase.cpp


# install
INSTALLS += target

RESOURCES += \
    content.qrc

RC_FILE = icon.rc

FORMS += \
    mainwindow.ui \
    formimageprop.ui \
    dialogheightcalculator.ui \
    formsettingsfield.ui \
    formsettingscontainer.ui \
    dialoglogger.ui \
    formbasemapconversionlevels.ui \
    formmaterialindicesmanager.ui \
    dialogshortcuts.ui

ICON = resources/icon.icns
INCLUDEPATH += include

DISTFILES += \
    resources/quad.obj


