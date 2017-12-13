#-------------------------------------------------
#
# Project created by QtCreator 2017-02-09T20:14:38
#
#-------------------------------------------------

QT       += core gui widgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = Spotlight
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    filedownloader.cpp \
    imageitem.cpp \
    database.cpp \
    addimagethread.cpp \
    abstractprovider.cpp \
    spotlightprovider.cpp \
    bingprovider.cpp \
    chromecastprovider.cpp \
    tableitemdelegate.cpp \
    abstractdesktopsupport.cpp \
    linuxdesktopprovider.cpp

HEADERS  += mainwindow.h \
    filedownloader.h \
    imageitem.h \
    database.h \
    addimagethread.h \
    abstractprovider.h \
    spotlightprovider.h \
    bingprovider.h \
    chromecastprovider.h \
    tableitemdelegate.h \
    abstractdesktopsupport.h \
    linuxdesktopprovider.h

FORMS    += mainwindow.ui
# Only for Visual Studio 2016
windows {
    *-g++* {
            # MinGW
           }
    *-msvc* {
        # MSVC
        INCLUDEPATH += "C:/Program Files (x86)/Windows Kits/10/Include/10.0.10240.0/ucrt"
        LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.16299.0/um/x64"
        LIBS += -lUser32
    }
}

RESOURCES += \
    resources.qrc
