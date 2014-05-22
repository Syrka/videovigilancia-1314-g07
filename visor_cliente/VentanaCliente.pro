#-------------------------------------------------
#
# Project created by QtCreator 2014-02-11T18:05:21
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ViewerClient
TEMPLATE = app

    #DESTDIR += /usr/bin #Especifica el directorio donde colocar el binario una vez generado.
    SOURCES += main.cpp\
        capturebuffer.cpp\
        dialogabout.cpp\
        svvprotocol.cpp\
        viewerwindow.cpp\
        preferencesdialog.cpp

    HEADERS  += capturebuffer.h\
        dialogabout.h\
        svvprotocol.h\
        viewerwindow.h\
        preferencesdialog.h


    FORMS    += dialogabout.ui\
        preferencesdialog.ui\
        viewerwindow.ui

unix {  # Esta configuración específica de Linux y UNIX
        # Variables
        #
    #CONFIG += qt release
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    BINDIR  = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    CONFDIR = /etc
    isEmpty(VARDIR) {
        VARDIR  = /var/lib/$${TARGET}
    }

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
