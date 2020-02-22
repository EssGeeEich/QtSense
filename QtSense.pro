QT       += core gui multimedia network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.0.0

CONFIG += c++11
LIBS += -lopenal
DEFINES += PRINT_LOADED_SOUNDS=1
DEFINES += VERSION=\"\\\"$${VERSION}\\\"\"
DEFINES += DEBUG=1

# NOTE: Qt requires gst-plugins-good on Linux to decode files.

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mainwindow_editor.cpp \
    networkaccess.cpp \
    qtsense.cpp \
    qtsensefilemonitor.cpp \
    random.cpp \
    settings.cpp \
    sound.cpp \
    sound_shared.cpp \
    soundchannelmanager.cpp \
    sounddevice.cpp \
    sounddeviceenumerator.cpp \
    soundfademanager.cpp \
    soundinstance.cpp \
    soundlistener.cpp \
    soundloader.cpp \
    xmlutils.cpp

HEADERS += \
    mainwindow.h \
    mainwindow_editor.h \
    networkaccess.h \
    qtsense.h \
    qtsense_structs.h \
    qtsensefilemonitor.h \
    random.h \
    settings.h \
    sound.h \
    sound_shared.h \
    soundchannelmanager.h \
    sounddevice.h \
    sounddeviceenumerator.h \
    soundfademanager.h \
    soundinstance.h \
    soundlistener.h \
    soundloader.h \
    xmlutils.h

FORMS += \
    mainwindow.ui \
    settings.ui

TRANSLATIONS += \
    QtSense_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
