#include "svvprotocol.h"
#include "QBuffer"
svvProtocol::svvProtocol(){
    idprotocol_="svv07";

}
//envia un paquete del protocolo, cabecera, timestamp e imagen
bool svvProtocol::sendPackage(QTcpSocket &receptor, QImage &image){ //devuelve true si la imagen ha sido enviada
    if(tcpSocket->isWritable()){

        QBuffer buffer;
        QImageWriter writer(&buffer,"jpeg");

        QImage image;
        image = pixmap.toImage();
        writer.setCompression(70);
        writer.write(image);

        QByteArray bytes_image;
        bytes = buffer.buffer();

        ///enviamos la cabecera del protocolo
        qint32 size_idprotocol = idprotocol_.size();
        receptor->write((const char *)&size_idprotocol, sizeof(qint32));//tamaño id protocolo
        receptor->write((const char *)&idprotocol_, size_idprotocol);   //id protocolo

        qint32 size_idcamera = idcamera_.size();
        receptor.write((const char *)&size_idcamera, sizeof(qint32))  //tamaño id camara
        receptor.write((const char *)&idcamera_, size_idcamera)  //id camara
        ///enviamos la imagen
        qint32 bytes_image_length = bytes_image.length();
        qDebug() << "Tamaño de la imagen: " << bytes.size();
        tcpSocket->write((const char *)&bytes_image_length, sizeof(qint32));

        qDebug() << "Enviando Imagen... ";
        tcpSocket->write(bytes);
        return true;
    }
    else
        return false;
}
