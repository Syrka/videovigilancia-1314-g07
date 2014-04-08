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

    QPixmap pixmap;
    pixmap = pixmap.fromImage(image);

    ui->label->setPixmap(pixmap);
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
            if (client->bytesAvailable() >= 4) {
                qDebug() << "Recibiendo tamaño.";
                int size;
                client->read((char *) &size, 4);
                nextImgSize = size;
                clientState = 1;
            } break;
        case 1:
            if (client->bytesAvailable() >= nextImgSize) {
                qDebug() << "Recibiendo imagen...";
                QBuffer buffer;
                buffer.setData(client->read(nextImgSize));

                //convert to qimage and call on_frame
                QImage img;
                img.load(&buffer, "jpeg");
                image_slot(img);
                //on_frame(img); //slot q modifica la imagen
                clientState = 0;
            }break;
    }

}
