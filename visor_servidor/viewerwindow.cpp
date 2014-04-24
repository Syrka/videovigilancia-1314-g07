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



    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, 15000);

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(new_connection()));
    qDebug() << "Capturando";
}

void ViewerWindow::new_connection() {

    while(tcpServer->hasPendingConnections()) {
        qDebug() << "Nuevo cliente";
        client = tcpServer->nextPendingConnection();
        qDebug() << "Nuevo cliente";
        connect(client, SIGNAL(readyRead()), this, SLOT(read_image()));
    }
}

void ViewerWindow::read_image() {
    svvProtocol emitter;
    QImage img=emitter.recibePackage(client);
    //emitter.getIdCamera();
    //emitter.getTimeStamp();
    //
    image_slot(img);

}
