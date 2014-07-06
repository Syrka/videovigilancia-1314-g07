#include "viewerwindow.h"
#include "ui_viewerwindow.h"

ViewerWindow::ViewerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewerWindow),
    camera(NULL),
    captureBuffer(NULL),
    server(NULL) {

        ui->setupUi(this);
        ui->checkBox->hide();
        ui->stopButton->hide();
        ui->startButton->hide();

        emitter = new SvvProtocol();

        settings = new QSettings(APP_CONFDIR);
        settings->setValue("viewer/server/ip", "127.0.0.1");
        settings->setValue("viewer/server/port", 15000);

        imageNum = 0;

/////////
///////// SQLITE
/////////

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
        );

}

ViewerWindow::~ViewerWindow() {
    delete ui;
    delete camera;
    delete captureBuffer;
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

void ViewerWindow::image_slot(const QImage &image, const QVector<QRect> &VRect) {

    imageNum++;
    save_images(image);
    QPixmap pixmap;
    pixmap = pixmap.fromImage(image);

    QPainter paint(&pixmap);
    paint.setPen(Qt::red);
    paint.drawRects(VRect);

    ui->label->setPixmap(pixmap);
}

void ViewerWindow::save_images(const QImage &image) {

    //qDebug() << imageNum;
    QString imageName, aux;

    imageName.setNum(imageNum, 16);
    imageName = aux.fill('0', 20 - imageName.length()) + imageName;

    QDir dir(APP_VARDIR);//abrimos el directorio que almacena las imagenes

    dir.mkpath( imageName.mid(0,5)
               + "/" + imageName.mid(5,5)
               + "/" + imageName.mid(10,5));

    image.save(QDir::toNativeSeparators( QString(APP_VARDIR) + "/" + imageName.mid(0,5)
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
    //conectamos la señal new image al slot read_image
    connect(server,SIGNAL(new_image(QImage,QVector<QRect>)),this,SLOT(image_slot(QImage,QVector<QRect>)));
}

