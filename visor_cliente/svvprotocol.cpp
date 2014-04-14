#include "svvprotocol.h"
#include <QBuffer>
#include <QPixmap>
#include <QImage>
#include <QImageWriter>
#include <qendian.h>
#include <QtEndian>
#include <QMessageBox>

svvProtocol::svvProtocol(QString idcamera, QTime timestamp){
    idprotocol_="73 76 76 30 37"; //svv07 → en hexadecimal
    idcamera_=idcamera;
    timestamp_=timestamp;
}

//envia un paquete del protocolo, cabecera, timestamp e imagen
bool svvProtocol::sendPackage(QTcpSocket *receptor, QImage &image){ //devuelve true si la imagen ha sido enviada
    if(receptor->isWritable()){

        QBuffer buffer;
        QImageWriter writer(&buffer,"jpeg");

        writer.setCompression(70);
        writer.write(image);

        QByteArray bytes_image;
        bytes_image = buffer.buffer();

        ///enviamos la cabecera del protocolo
        qint32 size_idprotocol = idprotocol_.size();
        receptor->write((const char *)&size_idprotocol, sizeof(qint32));//tamaño id protocolo
        receptor->write((const char *)&idprotocol_, size_idprotocol);   //id protocolo

        qint32 size_idcamera = idcamera_.size();
        receptor->write((const char *)&size_idcamera, sizeof(qint32));  //tamaño id camara
        receptor->write((const char *)&idcamera_, size_idcamera);  //id camara
        ///enviamos la imagen
        qint32 bytes_image_length = bytes_image.length();
        qDebug() << "Tamaño de la imagen: " << bytes_image.size();
        receptor->write((const char *)&bytes_image_length, sizeof(qint32));

        qDebug() << "Enviando Imagen... ";
        receptor->write(bytes_image);
        return true;
    }
    else{
        QMessageBox::information(0, "Error", ""+receptor->errorString());
        return false;
    }
}
