CONFIG       += c++11 debug release
QT           += opengl gui widgets

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
    formbasemapconversionlevels.h

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
    formbasemapconversionlevels.cpp


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
    formbasemapconversionlevels.ui

ICON = resources/icon.icns
INCLUDEPATH += include

DISTFILES += \
    resources/quad.obj


