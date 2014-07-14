#include "viewerwindow.h"

int ViewerWindow::sigHupSd[2];
int ViewerWindow::sigTermSd[2];
int ViewerWindow::sigIntSd[2];

ViewerWindow::ViewerWindow() :
    camera(NULL),
    captureBuffer(NULL),
    server(NULL) {

        ///
        // Crear las parejas de sockets UNIX
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigHupSd))
            qFatal("Couldn't create HUP socketpair");
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigTermSd))
            qFatal("Couldn't create TERM socketpair");
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigIntSd))
            qFatal("Couldn't create INT socketpair");

        // Crear los objetos para monitorizar uno de los socket de cada pareja
        sigHupNotifier = new QSocketNotifier(sigHupSd[1],
            QSocketNotifier::Read, this);
        sigTermNotifier = new QSocketNotifier(sigTermSd[1],
            QSocketNotifier::Read, this);
        sigIntNotifier = new QSocketNotifier(sigIntSd[1],
            QSocketNotifier::Read, this);

        // Conectar la señal activated() de cada objeto
        // QSocketNotifier con el slot correspondiente. Esta señal
        // será emitida cuando hayan datos para ser leidos en el
        // socket monitorizado.
        connect(sigHupNotifier, SIGNAL(activated(int)), this,
            SLOT(handleSigHup()));
        connect(sigTermNotifier, SIGNAL(activated(int)), this,
            SLOT(handleSigTerm()));
        connect(sigIntNotifier, SIGNAL(activated(int)), this,
            SLOT(handleSigInt()));
        ///

        emitter = new SvvProtocol();

        settings = new QSettings(APP_CONFDIR);
        settings->setValue("viewer/server/ip", "127.0.0.1");
        settings->setValue("viewer/server/port", 15000);

        imageNum = 0;

        on_actionNetwork_capture_triggered();

/////////
///////// SQLITE
/////////
/*
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("data.sqlite");

        if (!db.open()) {
            QMessageBox::critical(NULL, tr("Error"),
                                  tr("No se pudo acceder a los datos"));
        }

        // Datos nombre de tabla1 para id, timestamp, imagen, numero de roi
        QSqlQuery query_1;
        query_1.exec("CREATE TABLE IF NOT EXISTS Datos "
                     "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                     " idcamera VARCHAR(40), "
                     " timestamp LONG, "
                     " image VARCHAR(200))"
        );

        QSqlQuery query_2;
        query_2.exec("CREATE TABLE IF NOT EXISTS ROI "
                     "(id INTEGER PRIMARY KEY, "
                     " x LONG, "
                     " y LONG, "
                     " width LONG, "
                     " height LONG )"
        );*/
}

ViewerWindow::~ViewerWindow() {
    delete camera;
    delete captureBuffer;
    delete settings;

    if(server)
        delete server;

    delete sigHupNotifier;
    delete sigTermNotifier;
    delete sigIntNotifier;
}

void ViewerWindow::save_images(const QImage &image) {

    qDebug() << "HOLA_2";

    QString imageName, aux;

    imageName.setNum(imageNum, 16);
    imageName = aux.fill('0', 20 - imageName.length()) + imageName;

    QDir dir(APP_VARDIR);//abrimos el directorio que almacena las imagenes

    dir.mkpath(QString(APP_VARDIR)
               + "/" + imageName.mid(0,5)
               + "/" + imageName.mid(5,5)
               + "/" + imageName.mid(10,5));

    image.save(QDir::toNativeSeparators( QString(APP_VARDIR)
                                        + "/" + imageName.mid(0,5)
                                        + "/" + imageName.mid(5,5)
                                        + "/" + imageName.mid(10,5)
                                        + "/" + imageName + ".jpeg"),"JPEG");
}

void ViewerWindow::image_slot(const QImage &image, const QVector<QRect> &VRect) {

    imageNum++;
    save_images(image);

}

void ViewerWindow::on_actionNetwork_capture_triggered() {

    nPort = settings->value("viewer/server/port").toString();

    qDebug() << "Capturando";

    server = new Server(this);
    server->listen(QHostAddress::Any, nPort.toInt());

    //conectamos la señal new image al slot read_image
    connect(server,SIGNAL(new_image(QImage,QVector<QRect>)),this,
            SLOT(image_slot(QImage,QVector<QRect>)));
}

// Manejadores de las señales SIGHUP, SIGTERM y SIGINT
void ViewerWindow::hupSignalHandler(int) {
    char a = 1;
    write(sigHupSd[0], &a, sizeof(a));
}

void ViewerWindow::termSignalHandler(int) {
    char a = 1;
    write(sigTermSd[0], &a, sizeof(a));
}

void ViewerWindow::intSignalHandler(int) {
    char a = 1;
    write(sigIntSd[0], &a, sizeof(a));
}

void ViewerWindow::handleSigHup() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigHupNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigHupSd[1], &tmp, sizeof(tmp));

    //CODIGO DE LA SEÑAL
    qDebug() << "SigHup.";

    server->disconnect();
    on_actionNetwork_capture_triggered();

    //Activar de nuevo la monitorizacion
    sigHupNotifier->setEnabled(true);
}

void ViewerWindow::handleSigInt() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigIntNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigIntSd[1], &tmp, sizeof(tmp));

    //CODIGO DE LA SEÑAL
    qDebug() << "trl + C, SigInt.";
    qDebug() << "Cerrando conexion...";
    server->disconnect();

    QFile::remove("/var/run/midemoniod.pid");
    // Cuando el demonio termine, cerrar la conexión con
    // el servicio syslog
    //closelog();

    QCoreApplication::quit();

    //Activar de nuevo la monitorizacion
    sigIntNotifier->setEnabled(true);
}

void ViewerWindow::handleSigTerm() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigTermNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigTermSd[1], &tmp, sizeof(tmp));

    //CODIGO DE LA SEÑAL
    qDebug() << "SigTerm.";
    qDebug() << "Cerrando aplicacion...";
    server->disconnect();

    QFile::remove("/var/run/midemoniod.pid");
    // Cuando el demonio termine, cerrar la conexión con
    // el servicio syslog-
    //closelog();

    QCoreApplication::quit();

    //Activar de nuevo la monitorizacion
    sigTermNotifier->setEnabled(true);
}

//
// Configurar los manejadores de señal
//
int setupUnixSignalHandlers() {
    struct ::sigaction term_, hup_, int_;

    ///TERM
    term_.sa_handler = &ViewerWindow::termSignalHandler;

    //Vaciamos la mascara para indicar que no queremos bloquear la
    //llegada de ninguna señal POSIX
    sigemptyset(&term_.sa_mask);

    //SA_RESTART indica que si la señal interrumpe llamadas al sist. lanzada
    // desde otro sitio, al volver del manejador, la llamada debe continuar.
    // Si no la llamada retorna indicando un error
    term_.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGTERM
    if (sigaction(SIGTERM, &term_, 0) > 0)
    return 1;

    ///HUP
    hup_.sa_handler = &ViewerWindow::hupSignalHandler;
    sigemptyset(&hup_.sa_mask);
    hup_.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGHUP
    if (sigaction(SIGHUP, &hup_, 0) > 0)
    return 2;

    ///INT
    int_.sa_handler = &ViewerWindow::intSignalHandler;
    sigemptyset(&int_.sa_mask);
    int_.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGINT
    if (sigaction(SIGINT, &int_, 0) > 0)
    return 3;

    return 0;
}
