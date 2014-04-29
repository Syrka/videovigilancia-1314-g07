#ifndef SVVPROTOCOL_H
#define SVVPROTOCOL_H

#include <QString>
#include <QDateTime>
#include <QSslSocket>
#include <QBuffer>
#include <QPixmap>
#include <QImage>
#include <QImageWriter>
#include <QtEndian>
#include <QMessageBox>

class svvProtocol {

public:
    svvProtocol(QString idcamera,QDateTime timestamp);
    svvProtocol();
    bool sendPackage(QSslSocket *receptor, QImage &image);
    QImage recibePackage(QSslSocket *emitter);//se guarda lo que se recibe en una imagen
    QDateTime getTimeStamp();       //devuelve el timestamp de la ultima foto enviada/recibida
    QString getIdCamera();      //Devuelve el id de la camara de la ultima foto enviada/recibida

private:
    quint32 idprotocol_;
    QString idcamera_;
    QDateTime timestamp_;
    quint32 size_idcamera_;
    quint32 size_timestamp_;
    quint32 size_image_;
    int state_; //si 1→ espera Qstring de cabecera
                //si 2→ espera tamaño idcamera
                //si 3→ espera Qstring de idcamera
                //si 4→ espera tamaño timestamp
                //si 5→ espera QDateTime en QString timestamp
                //si 6→ espera tamaño image
                //si 7→ espera QImage image
};

#endif // SVVPROTOCOL_H
