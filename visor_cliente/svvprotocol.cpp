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
        receptor->write( qToLittleEndian((const char *)&idprotocol_),  sizeof(quint32));//tamaño id protocolo
        //idcamera
        quint32 size_idcamera = idcamera_.size();
        qDebug()<<"tamaño idcamera: "<<size_idcamera;
        receptor->write( qToLittleEndian((const char *)&size_idcamera), sizeof(quint32));  //tamaño id camara
        receptor->write( qToLittleEndian((char *)&idcamera_), size_idcamera);  //id camara
        //timestamp
        QString time_string = timestamp_.toString();
        quint32 size_timestamp = time_string.size();
        receptor->write( qToLittleEndian((const char*)&size_timestamp), sizeof(quint32));//tamaño timestamp
        receptor->write( qToLittleEndian((const char*)&time_string), size_timestamp);//timestamp

        ///enviamos la imagen
        quint32 size_bytes_image = bytes_image.length();
        qDebug() << "Tamaño de la imagen: " << bytes_image.size();
        receptor->write( qToLittleEndian((const char*)&size_bytes_image), sizeof(quint32));//tamaño image
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
    quint32 idprotocol;
    QByteArray bytes_toread;
    qDebug()<<"Recibiendo paquete svvP";
    while(emitter->isReadable()){
        qDebug() <<"estado→"<<state_;
        switch(state_){
        case 1://si 1→ espera quint32 de cabecera
            qDebug()<<"Recibiendo cabecera";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char *)&idprotocol,sizeof(quint32));
                idprotocol = qFromLittleEndian(idprotocol);
                qDebug()<< idprotocol <<" = "<<idprotocol_<<"?";
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
            qDebug()<<"Recibiendo tamaño idcamera";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char*)&size_idcamera_,sizeof(quint32));
                size_idcamera_ = qFromLittleEndian (size_idcamera_);
                qDebug()<<"\t→"<<size_idcamera_;
                state_++;
            }
            break;
        case 3://si 3→ espera Qstring de idcamera
            qDebug()<<"Recibiendo idcamera";
            if(emitter->bytesAvailable() >= size_idcamera_){
                bytes_toread = emitter->read(size_idcamera_);
                bytes_toread = qFromLittleEndian(bytes_toread);
                for(int i=0; i<size_idcamera_; i++){
                    idcamera_ += bytes_toread[i];
                }
                qDebug()<<idcamera_;
                state_++;
            }
            break;
        case 4://si 4→ espera tamaño timestamp
            qDebug()<<"Recibiendo tamaño timestamp";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char*)&size_timestamp_, sizeof(quint32));
                size_timestamp_ = qFromLittleEndian(size_timestamp_);
                state_++;
            }
            break;
        case 5://si 5→ espera QDateTime en QString timestamp
            qDebug()<<"Recibiendo timestamp";
            if(emitter->bytesAvailable() >= size_timestamp_){
                QString time_string;
                emitter->read((char*)&time_string, size_timestamp_);
                time_string = qFromLittleEndian(time_string);
                timestamp_.fromString(time_string);
                qDebug()<<time_string;
                qDebug()<<timestamp_.toString();
                state_++;
            }
            break;
        case 6://si 6→ espera tamaño image
            qDebug()<<"Recibiendo tamaño imagen";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char *)&size_image_, sizeof(quint32));
                size_image_ = qFromLittleEndian(size_image_);
                state_++;
            }
            break;
        case 7://si 7→ espera QImage image
            qDebug()<<"Recibiendo imagen";
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
