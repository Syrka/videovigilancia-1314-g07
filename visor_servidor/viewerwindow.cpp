#include "viewerwindow.h"
#include "ui_viewerwindow.h"

ViewerWindow::ViewerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewerWindow),
    camera(NULL),
    captureBuffer(NULL),
    client(NULL),
    tcpServer(NULL){

        ui->setupUi(this);
        ui->checkBox->hide();
        ui->stopButton->hide();
        ui->startButton->hide();

        QSettings settings;
        ipDir = settings.value("viewer/server/ip", "127.0.0.1").toString();
        nPort = settings.value("viewer/server/port", 15000).toString();

        count = 5;
}

ViewerWindow::~ViewerWindow() {
    delete ui;
    delete camera;
    delete captureBuffer;
    delete client;

    if(tcpServer != NULL) {
        delete tcpServer;
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
    //count++;

    QPixmap pixmap;
    pixmap = pixmap.fromImage(image);

    ui->label->setPixmap(pixmap);
}

void ViewerWindow::save_images(const QImage &image) {

    QString imageName;
    imageName.setNum(count, 6);
    imageName = imageName.fill('0', 32-imageName.length()) + imageName;

    QDir dir;
    dir.mkpath(QDir::currentPath() + "/" + imageName.mid(0,2) + "/" +
               imageName.mid(3,4) + "/" + imageName.mid(5,6) + "/" +
               imageName.mid(5,6));
    image.save(QDir::toNativeSeparators(QDir::currentPath() + "/" + imageName.mid(0,2) + "/" +
                                        imageName.mid(3,4) + "/" + imageName.mid(5,6) + "/" +
                                        imageName.mid(5,6) + "/" + imageName + ".jpeg"),"JPEG");

}

void ViewerWindow::on_actionPrefrencias_triggered() {

    preferences = new PreferencesDialog();
    preferences->exec();
}

void ViewerWindow::on_actionNetwork_capture_triggered() {

    qDebug() << "Capturando";

    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, 15000);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(new_connection()));
}

void ViewerWindow::new_connection() {

    while(tcpServer->hasPendingConnections()) {
        client = tcpServer->nextPendingConnection();
        connect(client, SIGNAL(readyRead()), this, SLOT(read_image()));
        clientState = 0;
        nextImgSize = 0;
    }
}

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
                count++;
                clientState = 0;
            }break;
    }

}
