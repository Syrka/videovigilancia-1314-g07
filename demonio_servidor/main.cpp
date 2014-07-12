#include "viewerwindow.h"

#include <QCoreApplication>
#include <QDebug>
#include <errno.h>
#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
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

    // Cerrar los descriptores de la E/S estandar
    close(STDIN_FILENO);            // fd 0
    close(STDOUT_FILENO);           // fd 1

    //Reabrir los descriptores pero mantenerlos conectados al archivo /dev/null
    int fd0 = open("/dev/null", O_RDONLY);  // fd0 == 0
    int fd1 = open("/dev/null", O_WRONLY);  // fd0 == 1
    int fd2 = open("/dev/null", O_WRONLY);  // fd0 == 2

    // Cambiar el usuario efectivo del proceso a 'midemonio'
    passwd* user = getpwnam("midemonio");
    seteuid(user->pw_uid);

    // Cambiar el grupo efectivo del proceso a 'midemonio'
    group* group = getgrnam("midemonio");
    setegid(group->gr_gid);

    /// ...

    //Enviar un mensaje al demonio syslog
    syslog(LOG_NOTICE, "Demonio iniciado con exito\n");

    // Archivo que contiene identificador de proceso del demonio
    QFile file("/var/run/midemoniod.pid");
    QTextStream out(&file);
    out << pid;
    file.close();

    /// ...
    QCoreApplication a(argc, argv);
    ViewerWindow w;

    setupUnixSignalHandlers();

    if(daemon){
        QFile::remove("/var/run/midemoniod.pid");
        // Cuando el demonio termine, cerrar la conexión con
        // el servicio syslog
        closelog();
    }

    return a.exec();
}
