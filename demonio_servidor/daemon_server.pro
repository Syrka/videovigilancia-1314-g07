#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T12:39:34
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visor_server
TEMPLATE = app

#DESTDIR += /usr/bin
#Especifica el directorio donde colocar el binario una vez generado.

SOURCES += main.cpp\
        viewerwindow.cpp \
    capturebuffer.cpp \
    server.cpp \
    svvprotocol.cpp

HEADERS  += viewerwindow.h \
    capturebuffer.h \
    server.h \
    svvprotocol.h

FORMS    +=

unix {  # Esta configuración específica de Linux y UNIX
        # Variables
    CONFIG += qt release
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    BINDIR  = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    CONFDIR = /etc/$$TARGET
    isEmpty(VARDIR) {
        VARDIR  = /var/lib/$$TARGET
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
    config.files += $${TARGET}.conf server.key server.crt

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

include(QtOpenCV.pri)
add_opencv_modules(core video imgproc)
