#-------------------------------------------------
#
# Project created by QtCreator 2017-02-09T20:14:38
#
#-------------------------------------------------
# Uncomment this line, if you want reverse image search
# DEFINES += REVERSE_IMAGE

CONFIG += debug_and_release

# When starting, look for libs here first
QMAKE_RPATHDIR += .

QT       += core gui widgets sql
contains(DEFINES, REVERSE_IMAGE) {
    QT += webenginewidgets
}

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
    dialogrename.cpp

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
    dialogrename.h

FORMS    += mainwindow.ui \
    managetags.ui \
    finddialog.ui \
    dialogrename.ui

TRANSLATIONS = spotlight_de.ts \
               spotlight_en.ts


# if reverse image search is requested, add files and library
contains(DEFINES, REVERSE_IMAGE) {
    HEADERS += reverseimagesearch.h
    SOURCES += reverseimagesearch.cpp

    linux {
        # for Linux, all we need is to add the library
        message("Add Linux Curl libs")
        # include and library path for curl
        # not necessary if supplied by distro in
        # default paths
        INCLUDEPATH += /usr/local/include
        LIBS += -L/usr/local/lib
        LIBS += -lcurl
        CURLBASEDIR = /usr/local/lib
    } # linux
    windows {
        message("Add Windows libs and includes")
        # Curl default path only if reverse image search
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
        LIBS += -lcurl
    } # windows
}

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
        LIBS += -lUser32 -lOle32
    }
    sqlplugin = qsqlite.dll
    deploydir = $$shadowed($$PWD)/deploy
    imageplugin = qjpeg.dll
}
linux
{
    deploydir = $$shadowed($$PWD)/deploy
    sqlplugin = libqsqlite.so
    imageplugin = libqjpeg.so
}

RESOURCES += \
    resources.qrc

copydatade.commands = $$[QT_INSTALL_BINS]/lrelease $$PWD/spotlight_de.ts -qm $$OUT_PWD/spotlight_de.qm
copydataen.commands = $$[QT_INSTALL_BINS]/lrelease $$PWD/spotlight_en.ts -qm $$OUT_PWD/spotlight_en.qm
first.depends = $(first) copydatade copydataen deploy
export(first.depends)
export(copydatade.commands)
export(copydataen.commands)
windows:QMAKE_DEL_FILE = del /q
windows:QMAKE_DEL_DIR  = rmdir /s /q
linux:QMAKE_DEL_DIR  = rm -rf
Release:DESTDIR=release
Debug:DESTDIR=debug

# Clean first
deploy.depends = cleandeploy
#deploy.commands = /bin/echo "BIN=$$[QT_INSTALL_LIBS]"
# this commands for Linux
linux {
    echo ="/usr/bin/echo"
    !exists($$echo) {
        echo = "/bin/echo"
    }
    cleandeploy.commands = $$QMAKE_DEL_DIR \""$$deploydir"\" ;
    deploy.commands += $$QMAKE_MKDIR "$$deploydir" ;
    deploy.commands += $$QMAKE_MKDIR $$shell_path("$$deploydir"/platforms) ;
    deploy.commands += $$QMAKE_MKDIR $$shell_path("$$deploydir"/sqldrivers) ;
    deploy.commands += $$QMAKE_MKDIR $$shell_path("$$deploydir"/imageformats) ;
    deploy.commands += $$QMAKE_COPY_FILE $$OUT_PWD/spotlight_de.qm $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$OUT_PWD/spotlight_en.qm $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$TARGET $$deploydir ;
    deploy.commands += $$echo \'$$LITERAL_HASH!/bin/bash\' > $$deploydir/link.sh ;
    deploy.commands += $$echo "cd $$deploydir" >> $$deploydir/link.sh ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Widgets.so.$$[QT_VERSION] $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Core.so.$$[QT_VERSION] $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Sql.so.$$[QT_VERSION] $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Gui.so.$$[QT_VERSION] $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Network.so.$$[QT_VERSION] $$deploydir ;
    # create links, so that rpath finds needed so
    deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Widgets.so.$$[QT_VERSION] libQt5Widgets.so.$$QT_MAJOR_VERSION" >> $$deploydir/link.sh ;
    deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Core.so.$$[QT_VERSION]    libQt5Core.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
    deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Sql.so.$$[QT_VERSION]     libQt5Sql.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
    deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Gui.so.$$[QT_VERSION]     libQt5Gui.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
    deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Network.so.$$[QT_VERSION] libQt5Network.so.$$QT_MAJOR_VERSION "  >> $$deploydir/link.sh ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libicui18n.so.56 $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libicudata.so.56 $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libicuuc.so.56 $$deploydir ;
    deploy.commands += $$QMAKE_COPY_FILE $$PWD/spotlight.sh $$deploydir ;
    deploy.commands += chmod 755 $$deploydir/link.sh ;
    # if we have Reverse Image search included, we need a bunch of libs more...
    contains(DEFINES, REVERSE_IMAGE) {
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5WebEngineWidgets.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5QuickWidgets.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Positioning.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Qml.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5WebChannel.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5Quick.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5WebEngineCore.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_LIBS]/libQt5PrintSupport.so.$$[QT_VERSION] $$deploydir ;
        deploy.commands += $$QMAKE_COPY_FILE $$CURLBASEDIR/libcurl.so $$deploydir ;
        # And now for the links
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5WebEngineWidgets.so.$$[QT_VERSION] libQt5WebEngineWidgets.so.$$QT_MAJOR_VERSION" >> $$deploydir/link.sh ;
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5QuickWidgets.so.$$[QT_VERSION]     libQt5QuickWidgets.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Positioning.so.$$[QT_VERSION]      libQt5Positioning.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Qml.so.$$[QT_VERSION]              libQt5Qml.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5WebChannel.so.$$[QT_VERSION]       libQt5WebChannel.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5Quick.so.$$[QT_VERSION]            libQt5Quick.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5WebEngineCore.so.$$[QT_VERSION]    libQt5WebEngineCore.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
        deploy.commands += $$echo "$$QMAKE_LN_SHLIB  libQt5PrintSupport.so.$$[QT_VERSION]     libQt5PrintSupport.so.$$QT_MAJOR_VERSION"  >> $$deploydir/link.sh ;
    }
    deploy.commands += $$deploydir/link.sh ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_PLUGINS]/sqldrivers/$$sqlplugin "$$deploydir"/sqldrivers ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_PLUGINS]/platforms/* "$$deploydir"/platforms ;
    deploy.commands += $$QMAKE_COPY_FILE $$[QT_INSTALL_PLUGINS]/imageformats/$$imageplugin "$$deploydir"/imageformats ;
}
# and these for Windows
windows {
contains(QT_ARCH, i386) {
    message("32-bit")
} else {
    message("64-bit")
}
    deploydir=$$shell_path($$deploydir)
    cleandeploy.commands = if exist $$deploydir $$QMAKE_DEL_DIR \""$$deploydir"\" &
    deploy.commands += $$QMAKE_MKDIR $$shell_path("$$deploydir"/platforms) $$shell_path("$$deploydir"/sqldrivers) $$shell_path("$$deploydir"/imageformats) &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$OUT_PWD/spotlight_de.qm) $$deploydir &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$OUT_PWD/spotlight_en.qm) $$deploydir &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$OUT_PWD/$$DESTDIR/$$TARGET).exe $$deploydir &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Widgets.dll) $$deploydir &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Core.dll) $$deploydir &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Sql.dll) $$deploydir &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Gui.dll) $$deploydir &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Network.dll) $$deploydir &
    # if we have Reverse Image search included, we need a bunch of libs more...
    contains(DEFINES, REVERSE_IMAGE) {
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5WebEngineWidgets.dll)  $$deploydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5QuickWidgets.dll)      $$deploydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Positioning.dll)       $$deploydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Qml.dll $$deploydir)   $$deplaydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5WebChannel.dll)        $$deploydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5Quick.dll)             $$deploydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5WebEngineCore.dll)     $$deploydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_BINS]/Qt5PrintSupport.dll)      $$deploydir &
        deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$CURLBASEDIR/curl.dll)                     $$deploydir &
    }
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_PLUGINS]/sqldrivers/qsqlite.dll)    $$shell_path("$$deploydir"/sqldrivers) &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_PLUGINS]/platforms/*.dll)        $$shell_path("$$deploydir"/platforms) &
    deploy.commands += $$QMAKE_COPY_FILE $$shell_path($$[QT_INSTALL_PLUGINS]/imageformats/qjpeg.dll) $$shell_path("$$deploydir"/imageformats) &
}

QMAKE_EXTRA_TARGETS += first copydataen copydatade cleandeploy deploy

