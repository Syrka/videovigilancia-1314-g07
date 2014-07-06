#include "viewerwindow.h"
#include "ui_viewerwindow.h"

ViewerWindow::ViewerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewerWindow),
    movie(NULL) {
    //camera(NULL)
        ui->setupUi(this);
        ui->stopButton->setEnabled(false);

        settings = new QSettings;
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

void ViewerWindow::on_actionAcerca_de_triggered() {
    DialogAbout dialog(this);
    dialog.exec();
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

void ViewerWindow::on_actionPrefrencias_triggered() {

    preferences = new PreferencesDialog(devices);
    preferences->exec();

    numDevice = settings->value("viewer/device").toInt();

    if(camera != NULL) {
        camera->stop();
        delete camera;
        camera = new QCamera(devices[numDevice]);
        on_actionCapturar_triggered();
    }
}
