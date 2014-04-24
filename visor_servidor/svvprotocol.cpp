#include "svvprotocol.h"
#include <QBuffer>
#include <QPixmap>
#include <QImage>
#include <QImageWriter>
#include <QtEndian>
#include <QMessageBox>

svvProtocol::svvProtocol(QString idcamera, QDateTime timestamp=QDateTime::currentDateTime()){
    idprotocol_ =73767637; //svv7 → en hexadecimal
    idcamera_ = idcamera;
    timestamp_ = timestamp;
    state_ = 1;
}

svvProtocol::svvProtocol(){
    idprotocol_ =73767637; //svv7 → en hexadecimal
    state_ = 1;
}

//envia un paquete del protocolo, cabecera, timestamp e imagen
bool svvProtocol::sendPackage(QTcpSocket *receptor, QImage &image){ //devuelve true si la imagen ha sido enviada
    if(receptor->isWritable()){

        QBuffer buffer;
        QImageWriter writer(&buffer,"jpeg");

        writer.setCompression(70);
        writer.write(image);

        QByteArray bytes_image;
        bytes_image = buffer.buffer(); //BigEndian

        ///enviamos la cabecera del protocolo
        //idprotocol
        receptor->write( qToLittleEndian((const char *)&idprotocol_),  sizeof(qint32));//tamaño id protocolo
        //idcamera
        qint32 size_idcamera = idcamera_.size();
        receptor->write( qToLittleEndian((const char *)&size_idcamera), sizeof(qint32));  //tamaño id camara
        receptor->write( qToLittleEndian((const char *)&idcamera_), size_idcamera);  //id camara
        //timestamp
        //QString timestamp = timestamp_.to;
        qint32 size_timestamp = sizeof(timestamp_);
        receptor->write( qToLittleEndian((const char*)&size_timestamp), sizeof(qint32));//tamaño timestamp
        receptor->write( qToLittleEndian((const char*)&timestamp_), size_timestamp);//timestamp

        ///enviamos la imagen
        qint32 size_bytes_image = bytes_image.length();
        qDebug() << "Tamaño de la imagen: " << bytes_image.size();
        receptor->write( qToLittleEndian((const char*)&size_bytes_image), sizeof(qint32));//tamaño image
        qDebug() << "Enviando Imagen... ";
        receptor->write( qToLittleEndian(bytes_image), size_bytes_image);                  //image
        return true;
    }
    else{
        QMessageBox::information(0, "Error al emitir", ""+receptor->errorString());
        return false;
    }
}

//se guarda lo que se recibe en una imagen y se almacena el timestamp y demas info
QImage svvProtocol::recibePackage(QTcpSocket *emitter){
    QImage image;
    qint32 size_idprotocol;
    QByteArray bytes_toread;
    while(emitter->isReadable()){
        qDebug() <<state_;
        switch(state_){
//        case 0://si 0→ espera tamaño cabecera
//            if(emitter->bytesAvailable() >= sizeof(qint32)){
//                 bytes_toread = qFromLittleEndian( emitter->read(sizeof(qint32)));
//                 size_idprotocol = bytes_toread.toInt();
//                 qDebug()<<size_idprotocol<<" == "<<size_idprotocol_;
//                if (size_idprotocol == size_idprotocol_){ //si viene la cabecera correcta
//                    state_++;
//                }
//                else{//si la cabecera no es correcta se descarta la conexion
//                    emitter->abort();
//                    QMessageBox::information(0,"Conexion abortada","Por cuestiones de seguridad se ha abortado una conexion entrante");
//                    return image;
//                }
//            }
//            break;
        case 1://si 1→ espera qint32 de cabecera
            if(emitter->bytesAvailable() >= sizeof(qint32)){
                bytes_toread =  qFromLittleEndian( emitter->read(size_idprotocol));
                qint32 idprotocol= bytes_toread.toInt();
                if(idprotocol == idprotocol_){
                    state_++;
                }
                else{
                    emitter->abort();
                    QMessageBox::information(0,"Conexion abortada","Por cuestiones de seguridad se ha abortado una conexion entrante");
                    return image;
                }
            }
            break;
        case 2://si 2→ espera tamaño idcamera
            if(emitter->bytesAvailable() >= sizeof(qint32)){
                bytes_toread = qFromLittleEndian( emitter->read(sizeof(qint32)));
                size_idcamera_ = bytes_toread.toInt();
                state_++;
            }
            break;
        case 3://si 3→ espera Qstring de idcamera
            if(emitter->bytesAvailable() >= size_idcamera_){
                idcamera_ = qFromLittleEndian( emitter->read(size_idcamera_));
                state_++;
            }
            break;
        case 4://si 4→ espera tamaño timestamp
            if(emitter->bytesAvailable() >= sizeof(qint32)){
                bytes_toread = qFromLittleEndian( emitter->read(sizeof(qint32)));
                size_timestamp_ = bytes_toread.toInt();
                state_++;
            }
            break;
        case 5://si 5→ espera QDateTime en QString timestamp
            if(emitter->bytesAvailable() >= size_timestamp_){
                timestamp_.fromString(qFromLittleEndian( emitter->read(size_timestamp_)));
                state_++;
            }
            break;
        case 6://si 6→ espera tamaño image
            if(emitter->bytesAvailable() >= sizeof(qint32)){
                bytes_toread = qFromLittleEndian( emitter->read(sizeof(qint32)));
                size_image_ = bytes_toread.toInt();
                state_++;
            }
            break;
        case 7://si 7→ espera QImage image
            if(emitter->bytesAvailable() >= size_image_){
                QBuffer buffer;
                buffer.setData(emitter->read(size_image_));
                image.load(&buffer, "jpeg");
                state_=1;
                return image;
            }
            break;
        }
    }
    QMessageBox::information(0, "Error al recibir", ""+emitter->errorString());
    return image;
}
