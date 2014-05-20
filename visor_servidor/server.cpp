#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent) {

    connect(protocol,SIGNAL(packageComplete()),this,SLOT(downloadedImage());
    settings = new QSettings;
    settings->setValue("viewer/SSL/key", "~/SSL/server.key");
    settings->setValue("viewer/SSL/certificate", "~/SSL/server.crt");
}

Server::~Server() {

    delete settings;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QSslSocket *socket = new QSslSocket;
    if(socket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(socket);

        key = settings->value("viewer/SSL/path").toByteArray();
        certificate = settings->value("viewer/SSL/certificate").toByteArray();

        QFile file_key(key);

        if(file_key.open(QIODevice::ReadOnly)) {
            key = file_key.readAll();
            file_key.close();
        }
        else {
            qDebug() <<"Error key: "<< file_key.errorString();
        }

        QFile file_cert(certificate);
        if(file_cert.open(QIODevice::ReadOnly)) {
             certificate = file_cert.readAll();
             file_cert.close();
        }
        else {
            qDebug() <<"Error cert: "<< file_cert.errorString();
        }

        QSslKey ssl_key(key,QSsl::Rsa);
        QSslCertificate ssl_cert(certificate);
        qDebug() << ssl_key ;
        qDebug() << ssl_cert;
        socket->setPrivateKey(ssl_key);
        socket->setLocalCertificate(ssl_cert);

        qDebug() << "Starting server encryption...";
        socket->startServerEncryption();

        connect(socket, SIGNAL(encrypted()), this, SLOT(signal_to_viewer()));

        QList<QSslError> errors;
        errors.append(QSslError::SelfSignedCertificate);
        errors.append(QSslError::CertificateUntrusted);

        socket->ignoreSslErrors(errors);
        current_emitter = new QSslSocket(socket);
        connect(current_emitter, SIGNAL(disconnected()),this,SLOT(disconnect());    //disconected → disconect
        connect(current_emitter,SIGNAL(readyRead()),this,SLOT(incomingImage());     //readyRead   → incomingImage
        emitters.append(socket);
    }
}

void Server::signal_to_viewer() {

    emit signal();
}

void Server::connection_failure() {

    qDebug() << "Fallo en la conexion" << current_emitter->errorString();
    current_emitter->disconnect();
    current_emitter->deleteLater();
    current_emitter->ignoreSslErrors();
}

void Server::disconnect() {

    current_emitter->disconnect();
    current_emitter->deleteLater();
    qDebug() << "Conexion cerrada";
}

QImage Server::incomingImage(){
    QImage image;
    if(!current_emitter)
        return NULL;

    image = protocol.recibePackage(current_emitter);
    //protocol.getIdCamera();
    //protocol.getTimeStamp();
}
