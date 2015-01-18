VPATH += ../shared
INCLUDEPATH += ../shared

HEADERS       = glwidget.h \
    mainwindow.h \
    CommonObjects.h \
    formimageprop.h \
    glimageeditor.h \
    camera.h \
    dialogheightcalculator.h
SOURCES       = glwidget.cpp \
                main.cpp \
    mainwindow.cpp \
    formimageprop.cpp \
    glimageeditor.cpp \
    CommonObjects.cpp \
    camera.cpp \
    dialogheightcalculator.cpp
QT           += opengl widgets

# install
INSTALLS += target

RESOURCES += \
    content.qrc

FORMS += \
    mainwindow.ui \
    formimageprop.ui \
    dialogheightcalculator.ui

OTHER_FILES += \
    cube.frag
