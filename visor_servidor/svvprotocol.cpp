#include "svvprotocol.h"

svvProtocol::svvProtocol(QString idcamera, QDateTime timestamp) : QObject(0){
    idprotocol_ =73767637; //svv7 → en hexadecimal
    idcamera_ = idcamera;
    timestamp_ = timestamp;
    state_ = 1;
}

svvProtocol::svvProtocol() : QObject(0){
    idprotocol_ = 73767637; //svv7 → en hexadecimal
    state_ = 1;
}

svvProtocol::~svvProtocol()
{
}

//envia un paquete del protocolo, cabecera, timestamp e imagen
bool svvProtocol::sendPackage(QSslSocket *receptor, QImage &image){ //devuelve true si la imagen ha sido enviada
    if(receptor->isWritable()){

        QBuffer buffer;
        QImageWriter writer(&buffer,"jpeg");

        writer.setCompression(70);
        writer.write(image);

        QByteArray bytes_image;
        bytes_image = buffer.buffer();

        ///enviamos la cabecera del protocolo
        //idprotocol
        idprotocol_ = qToLittleEndian(idprotocol_);
        receptor->write((const char *)&idprotocol_,  sizeof(quint32));//tamaño id protocolo

        //idcamera
        quint32 size_idcamera = idcamera_.size();
        receptor->write( qToLittleEndian((const char *)&size_idcamera), sizeof(quint32));  //tamaño id camara
        receptor->write((char *)&idcamera_, size_idcamera);  //id camara

        //timestamp
        QString time_string = timestamp_.toString();
        quint32 size_timestamp = time_string.size();
        receptor->write( qToLittleEndian((const char*)&size_timestamp), sizeof(quint32));//tamaño timestamp
        receptor->write( (const char*)&time_string, size_timestamp);//timestamp

        ///enviamos la imagen
        quint32 size_bytes_image = bytes_image.length();
        qDebug() << "svvP.Tamaño de la imagen: " << bytes_image.size();
        receptor->write( qToLittleEndian((const char*)&size_bytes_image), sizeof(quint32));//tamaño image
        qDebug() << "svvP.Enviando Imagen... ";
        receptor->write( qToLittleEndian(bytes_image), size_bytes_image);                  //image
        return true;
    }
    else{
        QMessageBox::information(0, "Error al emitir", ""+receptor->errorString());
        return false;
    }
}

//se guarda lo que se recibe en una imagen y se almacena el timestamp y demas info
QImage svvProtocol::recibePackage(QSslSocket *emitter){
    QImage image;
    quint32 idprotocol;
    QByteArray bytes_toread;
    qDebug()<<"Recibiendo paquete svvP";

    while(1){

        if(state_ == 1){
            qDebug()<<"svvP.Recibiendo cabecera";
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
            else break;
        }

        if(state_ == 2){
            qDebug()<<"Recibiendo tamaño idcamera";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char*)&size_idcamera_,sizeof(quint32));
                size_idcamera_ = qFromLittleEndian (size_idcamera_);
                qDebug()<<"\t→"<<size_idcamera_;
                state_++;
            }
            else break;
        }


        if(state_ == 3){
            qDebug()<<"Recibiendo idcamera";
            if(emitter->bytesAvailable() >= size_idcamera_){
                bytes_toread = emitter->read(size_idcamera_);
                for(uint i=0; i<size_idcamera_; i++){
                    idcamera_ += bytes_toread[i];
                }
                qDebug()<<idcamera_;
                state_++;
            }
            else break;
        }

        if(state_ == 4){
            qDebug()<<"Recibiendo tamaño timestamp";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char*)&size_timestamp_, sizeof(quint32));
                size_timestamp_ = qFromLittleEndian(size_timestamp_);
                state_++;
            }
            else break;
        }

        if(state_ == 5){
            qDebug()<<"Recibiendo timestamp";
            if(emitter->bytesAvailable() >= size_timestamp_){
                QString time_string;
                bytes_toread = emitter->read(size_timestamp_);
                for(uint i=0;i<size_timestamp_;i++){
                    time_string += bytes_toread[i];
                }
                timestamp_.fromString(time_string);
                qDebug()<<time_string;
                qDebug()<<timestamp_.toString();
                state_++;
            }
            else break;
        }

        if(state_ == 6){
            qDebug()<<"Recibiendo tamaño imagen";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char *)&size_image_, sizeof(quint32));
                size_image_ = qFromLittleEndian(size_image_);
                qDebug() << size_image_;
                state_++;
            }
            else break;
        }

        if(state_ == 7){
            qDebug()<<"Recibiendo imagen";
            if(emitter->bytesAvailable() >= size_image_){
                QBuffer buffer;
                buffer.setData(emitter->read(size_image_));
                image.load(&buffer, "jpeg");
                state_=1;
                return image;
            }
            else break;
        }
    }

    return image;
}

QString svvProtocol::getIdCamera(){
    return idcamera_;
}

QDateTime svvProtocol::getTimeStamp(){
    return timestamp_;
}
