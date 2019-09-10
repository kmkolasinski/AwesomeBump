TARGET        = AwesomeBump

TEMPLATE      = app
CONFIG       += c++11
QT           += opengl gui widgets

isEmpty(TOP_DIR) {
        ERROR("Run build process from the top directory")
}

VERSION_STRING = 5.1
VERSION_FULL = 5.1.0

DEFINES += VERSION_STRING=\\\"$$VERSION_STRING\\\"

QTN=utils/QtnProperty
include($$QTN/PEG.pri)


PEG_SOURCES += properties/Filter3DDOF.pef \
               properties/Filter3DBloom.pef \
               properties/Filter3DLensFlares.pef \
               properties/Filter3DToneMapping.pef \
               properties/GLSLParsedFragShader.pef \
               properties/ImageProperties.pef \
               properties/Filters3D.pef

gl330: DEFINES += USE_OPENGL_330

CONFIG(debug, debug|release): DBG = -dgb
GL = -gl4
gl330: GL = -gl3

# Windows settings
win32{
    msvc: LIBS += Opengl32.lib
}

SPEC=$$[QMAKE_SPEC]$$DBG$$GL
DESTDIR = $$TOP_DIR/workdir/$$SPEC/bin
OBJECTS_DIR = $$TOP_DIR/workdir/$$SPEC/obj
MOC_DIR = $$TOP_DIR/workdir/$$SPEC/gen
UI_DIR = $$TOP_DIR/workdir/$$SPEC/gen
RCC_DIR = $$TOP_DIR/workdir/$$SPEC/gen

write_file("$$TOP_DIR/workdir/current", SPEC)


# It's now required to define the path for resource files
# at compile time
# To keep compatibility with older releases, the application
# continues to look for these resource files in its current
# directory's subfolders (Config/* and Core/*) when using
# qmake to compile
DEFINES += RESOURCE_BASE=\\\"./\\\"

VPATH += ../shared
INCLUDEPATH += ../shared include utils utils/QtnProperty utils/contextinfo

HEADERS = glwidget.h \
    mainwindow.h \
    CommonObjects.h \
    formimageprop.h \
    glimageeditor.h \
    camera.h \
    dialogheightcalculator.h \
    qopenglerrorcheck.h \
    formsettingsfield.h \
    formsettingscontainer.h \
    dialoglogger.h \
    glwidgetbase.h \
    formmaterialindicesmanager.h \
    dialogshortcuts.h \
    allaboutdialog.h \
    formimagebase.h \
    dockwidget3dsettings.h \
    gpuinfo.h \
    properties/propertyconstructor.h \
    properties/propertydelegateabfloatslider.h \
    properties/PropertyABColor.h \
    properties/PropertyDelegateABColor.h \
    properties/Dialog3DGeneralSettings.h \
    utils/DebugMetricsMonitor.h \
    utils/Mesh.hpp \
    utils/qglbuffers.h \
    utils/tinyobj/tiny_obj_loader.h \
    utils/glslshaderparser.h \
    utils/glslparsedshadercontainer.h \
    utils/contextinfo/contextwidget.h \
    utils/contextinfo/renderwindow.h \
    formimagebatch.h

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
    formmaterialindicesmanager.cpp \
    dialogshortcuts.cpp \
    allaboutdialog.cpp \
    formimagebase.cpp \
    dockwidget3dsettings.cpp \
    gpuinfo.cpp \
    properties/Dialog3DGeneralSettings.cpp \
    utils/DebugMetricsMonitor.cpp \
    utils/glslshaderparser.cpp \
    utils/glslparsedshadercontainer.cpp \
    properties/propertydelegateabfloatslider.cpp \
    properties/PropertyABColor.cpp \
    properties/PropertyDelegateABColor.cpp \
    utils/contextinfo/contextwidget.cpp \
    utils/contextinfo/renderwindow.cpp \
    formimagebatch.cpp


RESOURCES += content.qrc

exists("runtime.qrc") {
        # build runtime archive
        runtimeTarget.target = runtime.rcc
        runtimeTarget.depends = $$PWD/runtime.qrc
        runtimeTarget.commands = $$[QT_INSTALL_PREFIX]/bin/rcc -binary $$PWD/runtime.qrc -o $$OUT_PWD/runtime.rcc
        QMAKE_EXTRA_TARGETS += runtimeTarget
        PRE_TARGETDEPS += runtime.rcc
}

RC_FILE = resources/icon.rc

FORMS += \
    mainwindow.ui \
    formimageprop.ui \
    dialogheightcalculator.ui \
    formsettingsfield.ui \
    formsettingscontainer.ui \
    dialoglogger.ui \
    formmaterialindicesmanager.ui \
    allaboutdialog.ui \
    dialogshortcuts.ui \
    dockwidget3dsettings.ui \
    properties/Dialog3DGeneralSettings.ui \
    formimagebatch.ui

ICON = resources/icons/icon.icns

DISTFILES += \
    resources/quad.obj \
    properties/Filter3DDOF.pef \
    properties/Filter3DBloom.pef \
    properties/Filter3DLensFlares.pef \
    properties/Filter3DToneMapping.pef \
    properties/Filters3D.pef \
    properties/GLSLParsedFragShader.pef \
    properties/ImageProperties.pef


# install additional files into target destination
# (require "make install")
config.path = $$DESTDIR
config.files += $$TOP_DIR/Bin/Configs $$TOP_DIR/Bin/Core
INSTALLS += config

exists("utils/qtcopydialog/qtcopydialog.pri") {
        message("*** Adding 'copydialog' module.")
        DEFINES += HAVE_RTCOPY
        include("utils/qtcopydialog/qtcopydialog.pri")
}

exists("utils/QtnProperty/QtnProperty.pri") {
  message("*** Adding 'qtnproperty' module.")
  DEFINES += HAVE_QTNPROP
  include("utils/QtnProperty/QtnProperty.pri")
} else {
  error("QtnProperty not found. Did you forget to 'git submodule init/update'")
}

exists("utils/quazip/quazip.pri") {
        message("*** Adding 'quazip' module.")
        DEFINES += HAVE_QUAZIP
        CONFIG += quazip_include_zip quazip_include_unzip
        include("utils/quazip/quazip.pri")
}

exists("utils/fervor/Fervor.pri") {
        message("*** Adding 'fervor' module.")
        DEFINES += HAVE_FERVOR
        FV_APP_VERSION = $$VERSION_FULL
        include("utils/fervor/Fervor.pri")
}
