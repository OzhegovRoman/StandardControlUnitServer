QT -= gui

QT += network
CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    ctcpipserver.cpp \
    ctcpipprocess.cpp \
    FTDI/cbiasunit.cpp \
    FTDI/cbiasunitreader.cpp \
    FTDI/cftdidevice.cpp \
    FTDI/cftdidevicesinfo.cpp \
    FTDI/cbiasunitdata.cpp \
    FTDI/cbiasunitchanneldata.cpp
HEADERS += \
    ctcpipserver.h \
    ctcpipprocess.h \
    FTDI/BiasUnitGeneralDefinitions.h \
    FTDI/cbiasunit.h \
    FTDI/cbiasunitreader.h \
    FTDI/cftdidevice.h \
    FTDI/cftdidevicesinfo.h \
    FTDI/cbiasunitdata.h \
    FTDI/cbiasunitchanneldata.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -L$$PWD/Lib/i386 -lftd2xx

win32: QMAKE_POST_LINK += windeployqt --release --no-translations --no-system-d3d-compiler --compiler-runtime --no-opengl-sw --dir \"$$OUT_PWD/bbServer\" \"$$OUT_PWD/release/TcpIpServer.exe\"

