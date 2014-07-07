#include "viewerwindow.h"
#include "ui_viewerwindow.h"

int ViewerWindow::sigHupSd[2];
int ViewerWindow::sigTermSd[2];
int ViewerWindow::sigIntSd[2];

ViewerWindow::ViewerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewerWindow),
    movie(NULL) {

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

        ui->setupUi(this);
        ui->stopButton->setEnabled(false);

        settings = new QSettings(APP_CONFDIR);
        ui->checkBox->setChecked(settings->value("viewer/checkBox", true).toBool());
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
}

ViewerWindow::~ViewerWindow() {

    motionThread->quit();
    motionThread->wait();
    delete motionThread;

    delete ui;
    delete movie;
    delete camera;
    delete sslSocket;
    delete settings;
    delete motionDetector;
}

void ViewerWindow::on_Quit_clicked() {
    qApp->quit();
}

void ViewerWindow::on_actionSalir_triggered() {
    qApp->quit();
}

//
// Visor de video
//

void ViewerWindow::on_actionAbrir_triggered() {
    QString filename = QFileDialog::getOpenFileName(this, "Abrir video", QString(), "Videos (*.mjpeg)");

    if(camera != NULL) {
        ui->startButton->setEnabled(true);
        delete camera;
    }

    if(!filename.isEmpty()) {
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "Abrir archivo", "El archivo no pudo ser abierto");
        }
        else {
            movie = new QMovie(filename);
            ui->stopButton->setEnabled(true);
            connect(movie, SIGNAL(updated(const QRect&)),
                    this, SLOT(en_movie_updated()));
            if(ui->checkBox->isChecked()) {
                movie->start();
            }
        }
    }
}

void ViewerWindow::on_startButton_clicked() {
    if (movie->state() == 1 || movie->state() == 0){
        movie->start();
    }
    else {
        movie->setPaused(true);
    }
}

void ViewerWindow::on_stopButton_clicked() {
    movie->stop();
}

void ViewerWindow::en_movie_updated() {
    QPixmap pixmap = movie->currentPixmap();
    ui->label->setPixmap(pixmap);
}

void ViewerWindow::on_checkBox_stateChanged() {

    settings->setValue("viewer/checkBox", ui->checkBox->isChecked());
}

//
// Captura de Webcam
//

void ViewerWindow::on_actionCapturar_triggered() {

    ui->checkBox->hide();
    ui->stopButton->hide();
    ui->startButton->hide();

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

    qDebug() << "Estado del Socket:"
             << sslSocket->state();
    if(sslSocket->state() != 3 && sslSocket->state() != 4)
        qDebug() << "Error:"
                 << sslSocket->errorString();

    connect(captureBuffer, SIGNAL(image_signal(QImage)), this, SLOT(image_slot(QImage)));
    connect(sslSocket, SIGNAL(encrypted()), this, SLOT(connected()));

}

void ViewerWindow::image_slot(const QImage &image) {

    emit to_motion_detector(image);
}

void ViewerWindow::send_processed(const QImage &image, const QVector<QRect> &VRect) {

    QDateTime time = QDateTime::currentDateTime();
    QString timeS = time.toString();

    SvvProtocol sendProtocol ("Host",time); //protocolo para enviar las imagenes de la forma correcta

    QPixmap pixmap;
    pixmap = pixmap.fromImage(image);

    QPainter paint(&pixmap);
    paint.setPen(Qt::white);
    paint.drawText(20, 20, timeS);

    QImage imageToSend = image;
    ui->label->setPixmap(pixmap);

    sendProtocol.sendPackage(sslSocket, imageToSend, VRect);
}

void ViewerWindow::connected() {
        qDebug() << "Conectado";
        camera->start();
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
    sslSocket->disconnect();
    QCoreApplication::quit();

    //Activar de nuevo la monitorizacion
    sigTermNotifier->setEnabled(true);
}
