#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T12:39:34
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visor_client
TEMPLATE = app


SOURCES += main.cpp\
        viewerwindow.cpp \
    dialogabout.cpp \
    capturebuffer.cpp \
    preferencesdialog.cpp \
    svvprotocol.cpp \
    motiondetector.cpp

HEADERS  += viewerwindow.h \
    dialogabout.h \
    capturebuffer.h \
    preferencesdialog.h \
    svvprotocol.h \
    motiondetector.h

FORMS    += viewerwindow.ui \
    dialogabout.ui \
    preferencesdialog.ui

include(QtOpenCV.pri)
add_opencv_modules(core video imgproc)

unix {  # Esta configuración específica de Linux y UNIX

    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
        # Variables
        #
    CONFIG += qt #release
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    BINDIR  = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    CONFDIR = /etc/$$TARGET
    isEmpty(VARDIR) {
        VARDIR  = /var/lib/$${TARGET}
    }
    DEFINES += APP_VARDIR=\\\"$$VARDIR\\\"
    DEFINES += APP_CONFDIR=\\\"$$CONFDIR\\\"

    # Install
    #
    INSTALLS += target config icon32 desktop vardir

    ## Instalar ejecutable
    target.path = $$BINDIR

    ## Instalar archivo de configuración
    config.path = $$CONFDIR
    config.files += $${TARGET}.conf

    ## Instalar acceso directo en el menú del escritorio
    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    ## Instalar icono de aplicación
    icon32.path = $$DATADIR/icons/hicolor/32x32/apps
    icon32.files += $${TARGET}.png

    ## Crear directorio de archivos variables
    vardir.path = $$VARDIR
    vardir.commands = :
}

win32{
    error("No permitido en windows")
}
