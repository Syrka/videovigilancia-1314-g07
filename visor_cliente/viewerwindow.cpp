#include "viewerwindow.h"
#include "ui_viewerwindow.h"

ViewerWindow::ViewerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewerWindow),
    movie(NULL),
    camera(NULL) {
        ui->setupUi(this);
        ui->stopButton->setEnabled(false);

        QSettings settings;
        ui->checkBox->setChecked(settings.value("viewer/checkBox", true).toBool());
        defaultDevice = settings.value("viewer/device", 0).toInt();
        numDevice = defaultDevice;
        devices = QCamera::availableDevices();

        //ipDir = settings.value("viewer/server/ip", "127.0.0.1").toString();
        //nPort = settings.value("viewer/server/port", 15000).toString();

        settings.setValue("viewer/server/ip", "127.0.0.1");
        settings.setValue("viewer/server/port", 15000);


        //tcpSocket = new QTcpSocket(this);
        sslSocket = new QSslSocket(this);
}

ViewerWindow::~ViewerWindow() {
    delete ui;
    delete movie;
    delete camera;
    //delete tcpSocket;
    delete sslSocket;
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
    QSettings settings;
    settings.setValue("viewer/checkBox", ui->checkBox->isChecked());
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

    if (defaultDevice != numDevice) {
        camera = new QCamera(devices[numDevice]);
    }
    else {
        camera = new QCamera(devices[defaultDevice]);
    }

    qDebug() << "Capturando de... "
             << QCamera::deviceDescription(devices[numDevice]);

    captureBuffer = new CaptureBuffer();
    camera->setViewfinder(captureBuffer);

    QSettings settings;
    ipDir = settings.value("viewer/server/ip").toString();
    nPort = settings.value("viewer/server/port").toString();

    qDebug() << "Intentando conexion";

    /*tcpSocket->connectToHost(ipDir, nPort.toInt());
    if (tcpSocket->waitForConnected())
            qDebug() << "Conectado";

    qDebug() << "Estado del Socket:"
             << tcpSocket->state();
    if(tcpSocket->state() != 3 && tcpSocket->state() != 4)
        qDebug() << "Error:"
                 << tcpSocket->errorString();*/

    sslSocket->connectToHostEncrypted(ipDir, nPort.toInt());
    sslSocket->ignoreSslErrors();

    //if (sslSocket->waitForConnected())
            //qDebug() << "Conectado";

    qDebug() << "Estado del Socket:"
             << sslSocket->state();
    if(sslSocket->state() != 3 && sslSocket->state() != 4)
        qDebug() << "Error:"
                 << sslSocket->errorString();

    connect(captureBuffer, SIGNAL(image_signal(QImage)), this, SLOT(image_slot(QImage)));
    connect(sslSocket, SIGNAL(encrypted()), this, SLOT(connected()));

}

void ViewerWindow::image_slot(const QImage &image) {

    QTime time = QTime::currentTime();
    QString timeS = time.toString();

    QPixmap pixmap;
    pixmap = pixmap.fromImage(image);
    QPainter paint(&pixmap);
    paint.setPen(Qt::white);
    paint.drawText(20, 20, timeS);

    ui->label->setPixmap(pixmap);

    if(sslSocket->isWritable()){

        //
        //Enviar imagenes
        //

        QBuffer buffer;
        QImageWriter writer(&buffer,"jpeg");

        QImage image_;
        image_ = pixmap.toImage();
        writer.setCompression(70);
        writer.write(image_);

        QByteArray bytes;
        bytes = buffer.buffer();

        qint32 bytes_length = bytes.length();
        qDebug() << "Tamaño de la imagen: " << bytes.size();
        sslSocket->write((const char *)&bytes_length, sizeof(qint32));

        qDebug() << "Enviando Imagen... ";
        sslSocket->write(bytes);
    }

}

void ViewerWindow::connected() {
        qDebug() << "Conectado";
        camera->start();
}

void ViewerWindow::on_actionPrefrencias_triggered() {

    preferences = new PreferencesDialog(devices);
    preferences->exec();

    QSettings settings;
    numDevice = settings.value("viewer/device").toInt();

    if(camera != NULL) {
        camera->stop();
        delete camera;
        on_actionCapturar_triggered();
    }
}
