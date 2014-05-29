#include "server.h"
#include "svvprotocol.h"
Server::Server(QObject *parent) : QTcpServer(parent) {

    settings = new QSettings(APP_CONFDIR);
    settings->setValue("viewer/SSL/key", QString(APP_CONFDIR) + "/server.key");
    settings->setValue("viewer/SSL/certificate", QString(APP_CONFDIR) + "/server.crt");
    emitters.clear();
    protocol.clear();
}

Server::~Server() {

    delete settings;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QSslSocket *socket = new QSslSocket;
    if(socket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(socket);

        key = settings->value("viewer/SSL/key").toByteArray();
        certificate = settings->value("viewer/SSL/certificate").toByteArray();

        QFile file_key(key);
        qDebug() << key;
        if(file_key.open(QIODevice::ReadOnly)) {

            key = file_key.readAll();
            file_key.close();
        }
        else {
            qDebug() << "Error key: "<< file_key.errorString();
        }

        qDebug() << certificate;
        QFile file_cert(certificate);
        if(file_cert.open(QIODevice::ReadOnly)) {

             certificate = file_cert.readAll();
             file_cert.close();
        }
        else {
            qDebug() << "Error cert: "<< file_cert.errorString();
        }

        QSslKey ssl_key(key,QSsl::Rsa);
        QSslCertificate ssl_cert(certificate);
        qDebug() << ssl_key ;
        qDebug() << ssl_cert;
        socket->setPrivateKey(ssl_key);
        socket->setLocalCertificate(ssl_cert);

        qDebug() << "Starting server encryption...";
        socket->startServerEncryption();

        QList<QSslError> errors;
        errors.append(QSslError::SelfSignedCertificate);
        errors.append(QSslError::CertificateUntrusted);

        socket->ignoreSslErrors(errors);

        SvvProtocol *current_protocol;
        current_protocol = new SvvProtocol;

        protocol.append(current_protocol);   //añadimos el protocolo
        emitters.append(socket);            //y el socket correspondiente. Tienen mismo indice

        connect(protocol.last(), SIGNAL(ready_image(QImage,QVector<QRect>)), this, SLOT(incomingImage(QImage, QVector<QRect>)));

        //dynamic_cast<QSslSocket*>(sender()) es el que manda la señal
        connect(emitters.last(), SIGNAL(disconnected()),    this,SLOT(disconnect()));    //disconected → disconect
        qDebug() <<"connecting client signal disconnected to server slot disconnect";
        connect(emitters.last(), SIGNAL(readyRead()),       this,SLOT(incoming()));     //readyRead   → incomingImage de cada socket
        //qDebug() <<"connecting client signal readyRead to server slot incomingImage";
    }
}

void Server::disconnect() {

    int index = emitters.indexOf(qobject_cast<QSslSocket*>(sender()));
    if(index>=0 && index<emitters.size()){
        emitters.removeAt(index);
        protocol.removeAt(index);
        qDebug() << "Conexion cerrada con un cliente";
    }
    else{
        qDebug() << "Server.Error: disconnect()";
    }
}

void Server::incomingImage(const QImage &image, const QVector<QRect> &VRect) {
    emit new_image(image, VRect);
}

void Server::incoming(){
    //calculamos el indice para saber que protocolo usar
    int index = emitters.indexOf(qobject_cast<QSslSocket*>(sender()));

    if(index>=0 && emitters[index]->isReadable()){
        qDebug()<<"Server.incomingImage";
        protocol[index]->recibePackage(emitters[index]);
    }
    else{
        qDebug()<<"Server.Error: incomingImage, can not read from client\nIndex: "<<index;
        if(emitters[index]->isReadable()){
            qDebug() <<"Is readable: True";
        }
        else{
            qDebug() <<"Is readable: False";
        }
    }
}

QImage Server::getImage(){
    return image;
}

QString Server::getIdcamera(){
    return idcamera;
}

QDateTime Server::getTimestamp(){
    return timestamp;
}
