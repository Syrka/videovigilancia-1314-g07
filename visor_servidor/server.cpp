#include "server.h"
#include "svvprotocol.h"
Server::Server(QObject *parent) : QTcpServer(parent) {

    settings = new QSettings;
    settings->setValue("viewer/SSL/key", "/usr/local/SSL/server.key");
    emitters.clear();
    protocol.clear();
    settings->setValue("viewer/SSL/certificate", "/usr/local/SSL/server.crt");
}

Server::~Server() {

    delete settings;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QSslSocket *socket = new QSslSocket;
    if(socket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(socket);

        key = settings->value("viewer/SSL/path","/usr/local/SSL").toByteArray();
        certificate = settings->value("viewer/SSL/certificate","/usr/local/SSL").toByteArray();

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

        QList<QSslError> errors;
        errors.append(QSslError::SelfSignedCertificate);
        errors.append(QSslError::CertificateUntrusted);

        socket->ignoreSslErrors(errors);

        svvProtocol *current_protocol;
        current_protocol = new svvProtocol;

        protocol.append(current_protocol);   //añadimos el protocolo
        emitters.append(socket);            //y el socket correspondiente. Tienen mismo indice

        //dynamic_cast<QSslSocket*>(sender()) es el que manda la señal
        connect(emitters.last(), SIGNAL(disconnected()),    this,SLOT(disconnect()));    //disconected → disconect
        qDebug() <<"connecting client signal disconnected to server slot disconnect";
        connect(emitters.last(), SIGNAL(readyRead()),       this,SLOT(incomingImage()));     //readyRead   → incomingImage de cada socket
        qDebug() <<"connecting client signal readyRead to server slot incomingImage";
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

void Server::incomingImage(){

    //calculamos el indice para saber que protocolo usar
    int index = emitters.indexOf(qobject_cast<QSslSocket*>(sender()));

    if(index>=0 && emitters[index]->isReadable()){
        qDebug()<<"Server.incomingImage";
        QSslSocket *current_emitter = new QSslSocket(emitters[index]);
        image = protocol[index]->recibePackage(current_emitter);
        if(!image.isNull()){ //si tenemos la imagen, avisamos a la ventana para que la guarde
            emit new_image();
            idcamera = protocol[index]->getIdCamera();
            timestamp = protocol[index]->getTimeStamp();
        }
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
