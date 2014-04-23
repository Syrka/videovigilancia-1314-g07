#include "server.h"

Server::Server(QObject *parent) : QTcpServer(parent) {

    socket = new QSslSocket;

    settings = new QSettings;
    settings->setValue("viewer/key", "SSL/server.key");
    settings->setValue("viewer/certificate", "SSL/server.crt");
}

Server::~Server() {

    delete socket;
    delete settings;
}

void Server::incomingConnection(qintptr socketDescriptor) {

    if(socket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(socket);

        key = settings->value("viewer/key").toByteArray();
        certificate = settings->value("viewer/certificate").toByteArray();

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
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connection_failure()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(disconnect()));

        QList<QSslError> errors;
        errors.append(QSslError::SelfSignedCertificate);
        errors.append(QSslError::CertificateUntrusted);

        socket->ignoreSslErrors(errors);
    }
}

void Server::signal_to_viewer() {

    emit signal();
}

void Server::connection_failure() {

    qDebug() << "Fallo en la conexion" << socket->errorString();
    socket->disconnect();
    socket->deleteLater();
    socket->ignoreSslErrors();
}

void Server::disconnect() {

    socket->disconnect();
    socket->deleteLater();
    qDebug() << "Conexion cerrada";
}
