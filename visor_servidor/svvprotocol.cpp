#include "svvprotocol.h"

SvvProtocol::SvvProtocol(QString idcamera, QDateTime timestamp) : QObject(0){
    idprotocol_ =73767637; //svv7 → en hexadecimal
    idcamera_ = idcamera;
    timestamp_ = timestamp;
    state_ = 1;
}

SvvProtocol::SvvProtocol() : QObject(0){
    idprotocol_ = 73767637; //svv7 → en hexadecimal
    state_ = 1;
}

SvvProtocol::~SvvProtocol(){}

//envia un paquete del protocolo, cabecera, timestamp e imagen
bool SvvProtocol::sendPackage(QSslSocket *receptor, QImage &image){ //devuelve true si la imagen ha sido enviada
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
QImage SvvProtocol::recibePackage(QSslSocket *emitter){
    quint32 idprotocol;
    QString bytes_toread;
    qDebug()<<"Recibiendo paquete svv";

    while(1){
        if(state_ == 1){
            qDebug()<<"Recibiendo cabecera";
            if(emitter->bytesAvailable() >= sizeof(quint32)){
                emitter->read((char *)&idprotocol,sizeof(quint32));
                idprotocol = qFromLittleEndian(idprotocol);
                if(idprotocol == idprotocol_) {
                    state_++;
                }
                else {
                    emitter->abort();
                    QMessageBox::information(0,"Conexion abortada","Por cuestiones de seguridad se ha abortado una conexion entrante");
                    return image_;
                }
            }
            else {
                break;
            }
        }

        if(state_ == 2) {
            qDebug()<<"Recibiendo tamaño idcamera";
            if(emitter->bytesAvailable() >= sizeof(quint32)) {
                emitter->read((char*)&size_idcamera_,sizeof(quint32));
                size_idcamera_ = qFromLittleEndian (size_idcamera_);
                state_++;
            }
            else {
                break;
            }
        }

        if(state_ == 3) {
            qDebug()<<"Recibiendo idcamera";
            if(emitter->bytesAvailable() >= size_idcamera_) {
                bytes_toread = emitter->read(size_idcamera_);
                idcamera_ = bytes_toread;
                state_++;
            }
            else {
                break;
            }
        }

        if(state_ == 4){
            qDebug()<<"Recibiendo tamaño timestamp";
            if(emitter->bytesAvailable() >= sizeof(qint32)){
                emitter->read((char*)&size_timestamp_, sizeof(qint32));
                size_timestamp_ = qFromLittleEndian(size_timestamp_);
                state_++;
            }
            else {
                break;
            }
        }

        if(state_ == 5){
            qDebug()<<"Recibiendo timestamp";
            if(emitter->bytesAvailable() >= size_timestamp_){
                QString time_string;
                bytes_toread = emitter->read(size_timestamp_);
                time_string = bytes_toread;
                timestamp_.fromString(time_string);
                state_++;
            }
            else {
                break;
            }
        }

        if(state_ == 6) {
            qDebug()<<"Recibiendo tamaño imagen";
            if(emitter->bytesAvailable() >= sizeof(qint32)) {
                emitter->read((char *)&size_image_, sizeof(qint32));
                size_image_ = qFromLittleEndian(size_image_);
                qDebug() << size_image_;
                state_++;
            }
            else {
                break;
            }
        }

        if(state_ == 7) {
            qDebug()<<"Recibiendo imagen";
            if(emitter->bytesAvailable() >= size_image_) {
                QBuffer buffer;
                buffer.setData(emitter->read(size_image_));
                image_.load(&buffer, "jpeg");
                state_++;
            }
            else {
                break;
            }
       }

       if(state_ == 8) {
            qDebug()<<"Recibiendo nRects";
            if(emitter->bytesAvailable() >= sizeof(qint32)) {
                emitter->read((char*)&nRects, sizeof(qint32));
                qDebug() << nRects;
                state_++;
            }
            else {
                break;
            }
        }

        if(state_ == 9) {
            if(emitter->bytesAvailable() >= (nRects * 4 * sizeof(qint32))) {

                for (uint i = 0; i < nRects; i++) {
                    emitter->read((char*)&x_, sizeof(qint32));
                    x_ = qFromLittleEndian(x_);

                    emitter->read((char*)&y_, sizeof(qint32));
                    y_ = qFromLittleEndian(y_);

                    emitter->read((char*)&width_, sizeof(qint32));
                    width_ = qFromLittleEndian(width_);

                    emitter->read((char*)&height_, sizeof(qint32));
                    height_ = qFromLittleEndian(height_);

                    QRect rect_;
                    rect_.setX(x_);
                    rect_.setY(y_);
                    rect_.setWidth(width_);
                    rect_.setHeight(height_);

                    VRect.push_back(rect_);
                }
                emit ready_image(image_, VRect);
                VRect.clear();
                state_= 1;
            }
            else
                break;
        }
    }
    return image_;
}

QString SvvProtocol::getIdCamera(){
    return idcamera_;
}

QDateTime SvvProtocol::getTimeStamp(){
    return timestamp_;
}
