#include "clientconsole.h"

int ClientConsole::sigHupSd[2];
int ClientConsole::sigTermSd[2];
int ClientConsole::sigIntSd[2];

ClientConsole::ClientConsole() {

    camera = NULL;
    captureBuffer = NULL;

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

        settings = new QSettings(APP_CONFDIR);
        settings->setValue("viewer/device", 0);
        numDevice = settings->value("viewer/device").toInt();
        devices = QCamera::availableDevices();
        camera = new QCamera(devices[numDevice]);
        settings->setValue("viewer/server/ip", "127.0.0.1");
        settings->setValue("viewer/server/port", 15000);

        sslSocket = new QSslSocket(this);

        qRegisterMetaType <QVector <QRect> >("QVector<QRect>");
        motionThread = new QThread();
        motionDetector = new MotionDetector();
        motionDetector->moveToThread(motionThread);

        connect(this, SIGNAL(to_motion_detector(QImage)),
                motionDetector, SLOT(detect_motion(QImage)));

        connect(motionDetector, SIGNAL(processed_image(QImage, QVector<QRect>)),
                this, SLOT(send_processed(QImage, QVector<QRect>)));

        motionThread->start();

        on_actionCapturar_triggered();
}

ClientConsole::~ClientConsole() {

    motionThread->quit();
    motionThread->wait();
    delete motionThread;

    if(camera)
        delete camera;

    if(sslSocket)
        delete sslSocket;
    delete settings;

    if(motionDetector)
        delete motionDetector;

    delete sigHupNotifier;
    delete sigTermNotifier;
    delete sigIntNotifier;
}

//
// Captura de Webcam
//

void ClientConsole::on_actionCapturar_triggered() {

    numDevice = settings->value("viewer/device").toInt();
    camera = new QCamera(devices[numDevice]);

    qDebug() << "Capturando de... "
             << QCamera::deviceDescription(devices[numDevice]);

    captureBuffer = new CaptureBuffer();
    camera->setViewfinder(captureBuffer);

    ipDir = settings->value("viewer/server/ip").toString();
    nPort = settings->value("viewer/server/port").toString();

    qDebug() << "Intentando conexion";

    sslSocket->connectToHostEncrypted(ipDir, nPort.toInt());
    sslSocket->ignoreSslErrors();

    connect(captureBuffer, SIGNAL(image_signal(QImage)), this, SLOT(image_slot(QImage)));
    connect(sslSocket, SIGNAL(encrypted()), this, SLOT(connected()));

}

void ClientConsole::image_slot(const QImage &image) {

    emit to_motion_detector(image);
}

void ClientConsole::send_processed(const QImage &image, const QVector<QRect> &VRect) {

    QDateTime time = QDateTime::currentDateTime();
    SvvProtocol sendProtocol ("Host",time); //protocolo para enviar las imagenes de la forma correcta
    QImage imageToSend = image;

    sendProtocol.sendPackage(sslSocket, imageToSend, VRect);
}

void ClientConsole::connected() {
        qDebug() << "Conectado";
        qDebug() << "IP: " << ipDir << "Puerto: " << nPort;
        qDebug() << "Enviando protocolo e imagenes procesadas...";
        camera->start();
}

// Manejadores de las señales SIGHUP, SIGTERM y SIGINT
void ClientConsole::hupSignalHandler(int) {
    char a = 1;
    write(sigHupSd[0], &a, sizeof(a));
}

void ClientConsole::termSignalHandler(int) {
    char a = 1;
    write(sigTermSd[0], &a, sizeof(a));
}

void ClientConsole::intSignalHandler(int) {
    char a = 1;
    write(sigIntSd[0], &a, sizeof(a));
}

void ClientConsole::handleSigHup() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigHupNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigHupSd[1], &tmp, sizeof(tmp));

    //CODIGO DE LA SEÑAL
    qDebug() << "SigHup.";

    sslSocket->disconnect();
    sslSocket->deleteLater();
    QCoreApplication::quit();

    //Activar de nuevo la monitorizacion
    sigHupNotifier->setEnabled(true);
}

void ClientConsole::handleSigInt() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigIntNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigIntSd[1], &tmp, sizeof(tmp));

    //CODIGO DE LA SEÑAL
    qDebug() << "trl + C, SigInt.";
    qDebug() << "Cerrando conexion...";
    sslSocket->disconnect();
    QCoreApplication::quit();

    //Activar de nuevo la monitorizacion
    sigIntNotifier->setEnabled(true);
}

void ClientConsole::handleSigTerm() {
    //Desactivar la monitorizacion para que por el momento no lleguen mas señales de QT
    sigTermNotifier->setEnabled(false);

    //Leer y desechar el byte enviado
    char tmp;
    read(sigTermSd[1], &tmp, sizeof(tmp));

    //CODIGO DE LA SEÑAL
    qDebug() << "SigTerm.";
    qDebug() << "Cerrando aplicacion...";
    sslSocket->disconnect();
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
    term_.sa_handler = &ClientConsole::termSignalHandler;

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
    hup_.sa_handler = &ClientConsole::hupSignalHandler;
    sigemptyset(&hup_.sa_mask);
    hup_.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGHUP
    if (sigaction(SIGHUP, &hup_, 0) > 0)
    return 2;

    ///INT
    int_.sa_handler = &ClientConsole::intSignalHandler;
    sigemptyset(&int_.sa_mask);
    int_.sa_flags = SA_RESTART;

    // Establecer manejador de la señal SIGINT
    if (sigaction(SIGINT, &int_, 0) > 0)
    return 3;

    return 0;
}
