TEMPLATE      = app
CONFIG       += c++11
QT           += opengl gui widgets

DESTDIR = Build/Bin
OBJECTS_DIR = Build/Obj
MOC_DIR = Build/Ui
UI_DIR = Build/Ui
RCC_DIR = Build/Obj


CONFIG(release_gl330) {
    #This is a release build
    DEFINES += USE_OPENGL_330
    TARGET = AwesomeBumpGL330
} else {
    TARGET = AwesomeBump
    #This is a debug build
}

# It's now required to define the path for resource files
# at compile time
# To keep compatibility with older releases, the application
# continues to look for these resource files in its current
# directory's subfolders (Config/* and Core/*) when using
# qmake to compile
DEFINES += RESOURCE_BASE=\\\"./\\\"

VPATH += ../shared
INCLUDEPATH += ../shared include

HEADERS = glwidget.h \
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
    allaboutdialog.h \    
    formimagebase.h \
    dockwidget3dsettings.h \
    gpuinfo.h \
    unitysupportgui.h

SOURCES = glwidget.cpp \
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
    allaboutdialog.cpp \
    formimagebase.cpp \
    dockwidget3dsettings.cpp \
    gpuinfo.cpp \
    unitysupportgui.cpp


RESOURCES += content.qrc

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
    allaboutdialog.ui \
    dialogshortcuts.ui \
    dockwidget3dsettings.ui \
    unitysupportgui.ui

ICON = resources/icon.icns

DISTFILES += \
    resources/quad.obj

# install additional files into target destination
# (require "make install")
config.path = $$OUT_PWD/$$DESTDIR
config.files += ../Bin/Configs ../Bin/Core
INSTALLS += config

exists("../Third/QtnProperty/QtnProperty.pri") {
  DEFINES += HAVE_QTNPROP
  include("../Third/QtnProperty/QtnProperty.pri")
}
