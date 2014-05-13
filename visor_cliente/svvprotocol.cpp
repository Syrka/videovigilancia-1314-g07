#include "svvprotocol.h"

SvvProtocol::SvvProtocol(QString idcamera, QDateTime timestamp=QDateTime::currentDateTime()){
    idprotocol_ =73767637; //svv7 → en hexadecimal
    idcamera_ = idcamera;
    timestamp_ = timestamp;
    state_ = 1;
}

SvvProtocol::SvvProtocol(){
    idprotocol_ = 73767637; //svv7 → en hexadecimal
    state_ = 1;
}

//envia un paquete del protocolo, cabecera, timestamp e imagen
bool SvvProtocol::sendPackage(QSslSocket *receptor, QImage &image, QVector<QRect> VRect){ //devuelve true si la imagen ha sido enviada
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
        size_idcamera_ = idcamera_.size();
        //qDebug()<<"tamaño idcamera: "<<size_idcamera_;
        size_idcamera_ = qToLittleEndian(size_idcamera_);
        receptor->write((const char *)&size_idcamera_, sizeof(quint32));  //tamaño id camara
        receptor->write((char *)&idcamera_, size_idcamera_);  //id camara

        //timestamp
        QString time_string = timestamp_.toString();
        size_timestamp_ = time_string.size();
        size_timestamp_ = qToLittleEndian(size_timestamp_);
        receptor->write((const char*)&size_timestamp_, sizeof(quint32)); //tamaño timestamp
        receptor->write((const char*)&time_string, size_timestamp_); //timestamp

        ///enviamos la imagen
        quint32 size_bytes_image = bytes_image.length();
        //qDebug() << "Tamaño de la imagen: " << bytes_image.size();
        size_bytes_image = qToLittleEndian(size_bytes_image);
        receptor->write((const char*)&size_bytes_image, sizeof(quint32));//tamaño image
        //qDebug() << "Enviando Imagen... ";
        receptor->write(qToLittleEndian(bytes_image), size_bytes_image);                 //image

        //
        //
        //

        nRects = VRect.size();
        nRects = qToLittleEndian(nRects);
            qDebug() << nRects;
        receptor->write((const char*)&nRects, sizeof(qint32));

        for (QVector<QRect>::const_iterator i = VRect.begin(); i < VRect.end(); ++i) {
            QRect rect_ = *i;

            x_ = qToLittleEndian(rect_.x());
            receptor->write((const char*)&x_, sizeof(qint32));

            y_ = qToLittleEndian(rect_.y());
            receptor->write((const char*)&y_, sizeof(qint32));

            width_ = qToLittleEndian(rect_.width());
            receptor->write((const char*)&width_, sizeof(qint32));

            height_ = qToLittleEndian(rect_.height());
            receptor->write((const char*)&height_, sizeof(qint32));

            qDebug()<< "x" << x_ << "y" << y_ << "width" << width_ << "height" << height_;
        }
        return true;
    }

    else {
        QMessageBox::information(0, "Error al emitir", ""+receptor->errorString());
        return false;
    }
}
