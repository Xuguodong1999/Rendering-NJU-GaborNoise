QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG  += c++11

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwidget.cpp

HEADERS += \
    src/gaborkernel.h  \
    src/gabornoise.h  \
    src/gaborutil.h  \
    src/mainwidget.h  \
    src/qtutil.h

FORMS += \
    src/mainwidget.ui
msvc {
    RC_ICONS = img/win.ico
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8 /openmp
}
unix {
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -lgomp -lpthread
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
