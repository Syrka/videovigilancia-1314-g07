#include "viewerwindow.h"
#include "ui_viewerwindow.h"

ViewerWindow::ViewerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewerWindow),
    camera(NULL),
    captureBuffer(NULL),
    client(NULL),
    server(NULL) {

        ui->setupUi(this);
        ui->checkBox->hide();
        ui->stopButton->hide();
        ui->startButton->hide();

        emitter = new svvProtocol();

        settings = new QSettings;
        //ipDir = settings->value("viewer/server/ip", "127.0.0.1").toString();
        //nPort = settings->value("viewer/server/port", 15000).toString();

        settings->setValue("viewer/server/ip", "127.0.0.1");
        settings->setValue("viewer/server/port", 15000);

        imageNum = 0;
}

ViewerWindow::~ViewerWindow() {
    delete ui;
    delete camera;
    delete captureBuffer;
    delete client;
    delete settings;

    if(server != NULL) {
        delete server;
    }
}

void ViewerWindow::on_Quit_clicked() {
    qApp->quit();
}

void ViewerWindow::on_actionSalir_triggered() {
    qApp->quit();
}

void ViewerWindow::on_actionAcerca_de_triggered() {
    DialogAbout dialog(this);
    dialog.exec();
}

void ViewerWindow::image_slot(const QImage &image) {

    save_images(image);
    QPixmap pixmap;
    pixmap = pixmap.fromImage(image);

    ui->label->setPixmap(pixmap);
}

void ViewerWindow::save_images(const QImage &image) {

    qDebug() << imageNum;
    QString imageName, aux;
    //imageName = QString("%1").arg(imageNum, 32, 16, 0);

    imageName.setNum(imageNum, 16);
    qDebug() << imageName;
    imageName = aux.fill('0', 20 - imageName.length()) + imageName;
    qDebug() << imageName;

    QDir dir;
    dir.mkpath(QDir::currentPath()
               + "/" + imageName.mid(0,5)
               + "/" + imageName.mid(5,5)
               + "/" + imageName.mid(10,5));

    image.save(QDir::toNativeSeparators(QDir::currentPath()
                                        + "/" + imageName.mid(0,5)
                                        + "/" + imageName.mid(5,5)
                                        + "/" + imageName.mid(10,5)
                                        + "/" + imageName + ".jpeg"),"JPEG");
}

void ViewerWindow::on_actionPrefrencias_triggered() {

    preferences = new PreferencesDialog();
    preferences->exec();
}

void ViewerWindow::on_actionNetwork_capture_triggered() {

    nPort = settings->value("viewer/server/port").toString();

    qDebug() << "Capturando";

//<<<<<<< HEAD
    server = new Server(this);
    server->listen(QHostAddress::Any, nPort.toInt());

    connect(server, SIGNAL(signal()), this, SLOT(new_connection()));
/*=======
    connect(server, SIGNAL(signal()), this, SLOT(new_connection()));
    qDebug() << "Capturando";
>>>>>>> protocolo*/
}

void ViewerWindow::new_connection() {

//<<<<<<< HEAD
    /*
    while(tcpServer->hasPendingConnections()) {
=======
    if(tcpServer->hasPendingConnections()) {
>>>>>>> protocolo
        client = tcpServer->nextPendingConnection();
        qDebug() << "Nuevo cliente";
        connect(client, SIGNAL(readyRead()), this, SLOT(read_image()));
<<<<<<< HEAD
        clientState = 0;
        nextImgSize = 0;
    }*/

    client = dynamic_cast<QSslSocket *>(server->nextPendingConnection());
    connect(client, SIGNAL(readyRead()), this, SLOT(read_image()));
    //clientState = 0;
    //nextImgSize = 0;
}
/*
void ViewerWindow::read_image() {

    switch(clientState) {
        case 0:
            if (client->bytesAvailable() >= sizeof(qint32)) {
                qDebug() << "Recibiendo tamaño.";
                qint32 size;
                client->read((char *) &size, sizeof(qint32));
                nextImgSize = size;
                clientState = 1;
            } break;
        case 1:
            if (client->bytesAvailable() >= nextImgSize) {
                qDebug() << "Recibiendo imagen...";
                QBuffer buffer;
                buffer.setData(client->read(nextImgSize));

                QImage img;
                img.load(&buffer, "jpeg");
                image_slot(img);
                imageNum++;
                clientState = 0;
            } break;
    }
=======
    }
}*/

void ViewerWindow::read_image() {
    QImage img = emitter->recibePackage(client);
    imageNum++;
    //emitter.getIdCamera();
    //emitter.getTimeStamp();
    image_slot(img);
}
