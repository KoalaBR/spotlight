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
    linuxdesktopprovider.cpp \
    managetags.cpp \
    windowsdesktopsupport.cpp \
    finddialog.cpp \
    dialogrename.cpp \
    reverseimagesearch.cpp

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
    linuxdesktopprovider.h \
    managetags.h \
    windowsdesktopsupport.h \
    finddialog.h \
    dialogrename.h \
    reverseimagesearch.h

FORMS    += mainwindow.ui \
    managetags.ui \
    finddialog.ui \
    dialogrename.ui

TRANSLATIONS = spotlight_de.ts \
               spotlight_en.ts
# Only for Visual Studio 2016
windows {
    *-g++* {
            # MinGW
           }
    *-msvc* {
        # MSVC
        SDKVERSION = $$(UCRTVersion)
        SDKBASEDIR = $$(UniversalCRTSdkDir)
        isEmpty(SDKVERSION) {
            error("Visual Studio environment not configured correctly! (1)")
        }
        isEmpty(SDKBASEDIR) {
            error("Visual Studio environment not configured correctly! (2)")
        }
        INCLUDEPATH += "$$SDKBASEDIR\\include\\$$SDKVERSION\\ucrt"
        LIBS += "-L$$SDKBASEDIR\\lib\\$$SDKVERSION\\um\\x64"
        # Curl default path
        exists("$$PWD/curl.config") {
            CURLBASEDIR=$$cat($$PWD\\curl.config)
            message("found curl.config using $$CURLBASEDIR")
        }
        !exists($$PWD/curl.config) {
            CURLBASEDIR=c:/Projekte/curl/
            message("curl.config is missing, defaulting to   $$CURLBASEDIR")
        }
        INCLUDEPATH += $$CURLBASEDIR/include
        LIBS += -L$$CURLBASEDIR
        LIBS += -lUser32 -lOle32 -lcurl
    }
}
linux
{
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib
    LIBS += -lcurl
}

RESOURCES += \
    resources.qrc

copydatade.commands = lrelease $$PWD/spotlight_de.ts -qm $$OUT_PWD/spotlight_de.qm
copydataen.commands = lrelease $$PWD/spotlight_en.ts -qm $$OUT_PWD/spotlight_en.qm
first.depends = $(first) copydatade copydataen
export(first.depends)
export(copydatade.commands)
export(copydataen.commands)
QMAKE_EXTRA_TARGETS += first copydataen copydatade
