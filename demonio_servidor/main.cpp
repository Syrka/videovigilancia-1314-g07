#include "viewerwindow.h"

#include <QCoreApplication>
#include <QDebug>
#include <errno.h>
#include <stdio.h>
#include <syslog.h>
//#include <fcntl.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include <pwd.h>
//#include <grp.h>
#include <iostream>

int main(int argc, char *argv[]) {

    QCoreApplication::setOrganizationName("Videovigilancia");
    QCoreApplication::setOrganizationDomain("https://github.com/ull-etsii-sistemas-operativos/videovigilancia-1314-g07");
    QCoreApplication::setApplicationName("viewer");


    pid_t pid;
    pid_t sid;

    //nos clonamos nosotros mismos creando un proceso hijo
    pid = fork();

    //Si pid < 0, entonces fork() fallo
    if (pid < 0) {
        //Mostrar error y terminar
        std::cerr << strerror(errno) << std::endl;
        exit(10);
    }

    //Si pid > 0, estamos en el proceso padre
    if (pid > 0) {
        //terminar el proceso
        exit(0);
    }

    //Si la ejecucion llega a este punto, estamos en el hijo

    qDebug() << "Estamos en el proceso HIJO";
    //Damos todos los permisos al nuevo proceso
    umask(0);

    //Abrir una conexion al demonio syslog
    openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);

    /// ...

    //Intentar crear una nueva sesion
    sid = setsid();

    if (sid < 0) {
        syslog(LOG_ERR, "No fue posible crear una nueva sesion|n");
        exit(11);
    }

    //Cambiar el directorio de trabajo del proceso
    if ((chdir("/")) < 0 ) {
        syslog(LOG_ERR, "No fue posible cambiar el directorio de "
               "trabajo a /\n");
        exit(12);
    }

    //Reabrir los descriptores estandar

    /// ...

    //Enviar un mensaje al demonio syslog
    syslog(LOG_NOTICE, "Demonio iniciado con exito\n");

    /// ...

    QCoreApplication a(argc, argv);
    ViewerWindow w;
    //w.show();

    //
    //Cuando el demonio termine, cerrar la conexion con el servicio syslog
    ////closelog();
    //

    return a.exec();
}
