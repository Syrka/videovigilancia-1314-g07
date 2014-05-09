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
    //imageName = QString("%1").arg(imageNum, 32, 16, '0');

    imageName.setNum(imageNum, 16);
    imageName = aux.fill('0', 20 - imageName.length()) + imageName;

    QDir dir;//abrimos el directorio que almacena las imagenes
    dir.mkpath( APP_VARDIR
               +"/" + imageName.mid(0,5)
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

    server = new Server(this);
    server->listen(QHostAddress::Any, nPort.toInt());

    connect(server, SIGNAL(signal()), this, SLOT(new_connection()));
}

void ViewerWindow::new_connection() {

    client = dynamic_cast<QSslSocket *>(server->nextPendingConnection());
    connect(client, SIGNAL(readyRead()), this, SLOT(read_image()));
}


void ViewerWindow::read_image() {
    QImage img = emitter->recibePackage(client);
    imageNum++;
    //emitter.getIdCamera();
    //emitter.getTimeStamp();
    image_slot(img);
}
