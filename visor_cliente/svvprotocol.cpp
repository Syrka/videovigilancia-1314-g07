#include "svvprotocol.h"
#include <QBuffer>
#include <QPixmap>
#include <QImage>
#include <QImageWriter>
#include <QtEndian>
#include <QMessageBox>

svvProtocol::svvProtocol(QString idcamera, QDateTime timestamp=QDateTime::currentDateTime()){
    idprotocol_ = "73 76 76 30 37"; //svv07 → en hexadecimal
    idcamera_ = idcamera;
    timestamp_ = timestamp;
    size_idprotocol_ = idprotocol_.size();
    state_ = 0;
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
        receptor->write( qToLittleEndian((const char *)&size_idprotocol_),  sizeof(qint32));//tamaño id protocolo
        receptor->write( qToLittleEndian((const char *)&idprotocol_),      size_idprotocol_);   //id protocolo
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
    //si 0→ espera tamaño cabecera
    //si 1→ espera Qstring de cabecera
    //si 2→ espera tamaño idcamera
    //si 3→ espera Qstring de idcamera
    //si 4→ espera tamaño timestamp
    //si 5→ espera QDateTime en QString timestamp
    //si 6→ espera tamaño image
    //si 7→ espera QImage image
    QImage image;
    while(emitter->isReadable()){
        qint32 size_idprotocol;
        qint32 size_timestamp;
        qint32 size_image;
        switch(state_){
        case 0:
            if(emitter->bytesAvailable() >= sizeof(qint32)){
                size_idprotocol = qFromLittleEndian( emitter->read(sizeof(qint32)).toInt());
                if (size_idprotocol == size_idprotocol_){ //si viene la cabecera correcta
                    state_++;
                }
                else{//si la cabecera no es correcta se descarta la conexion
                    emitter->abort();
                    return image;
                }
                break;
            }
            else{
                break;
            }
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:

            return image;
            break;
        }
    }
    QMessageBox::information(0, "Error al recibir", ""+emitter->errorString());
    return image;
}
