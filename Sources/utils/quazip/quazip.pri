
INCLUDEPATH += $$PWD
DEFINES += QUAZIP_BUILD QUAZIP_STATIC
DEPENDPATH += $$PWD
SOURCES += \
  $$PWD/JlCompress.cpp \
  $$PWD/qioapi.cpp \
  $$PWD/quaadler32.cpp \
  $$PWD/quacrc32.cpp \
  $$PWD/quagzipfile.cpp \
  $$PWD/quaziodevice.cpp \
  $$PWD/quazip.cpp \
  $$PWD/quazipdir.cpp \
  $$PWD/quazipfile.cpp \
  $$PWD/quazipfileinfo.cpp \
  $$PWD/quazipnewinfo.cpp

HEADERS += \
  $$PWD/crypt.h \
  $$PWD/ioapi.h \
  $$PWD/JlCompress.h \
  $$PWD/quaadler32.h \
  $$PWD/quachecksum32.h \
  $$PWD/quacrc32.h \
  $$PWD/quagzipfile.h \
  $$PWD/quaziodevice.h \
  $$PWD/quazip_global.h \
  $$PWD/quazip.h \
  $$PWD/quazipdir.h \
  $$PWD/quazipfile.h \
  $$PWD/quazipfileinfo.h \
  $$PWD/quazipnewinfo.h

CONFIG(quazip_include_zip) {
  SOURCES += $$PWD/zip.c
  HEADERS += $$PWD/zip.h
}

CONFIG(quazip_include_unzip) {
  SOURCES += $$PWD/unzip.c
  HEADERS += $$PWD/unzip.h
}


OTHRE_FILES += $$PWD/quazip.pri

LIBS += -lz
