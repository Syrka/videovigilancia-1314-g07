#ifndef SVVPROTOCOL_H
#define SVVPROTOCOL_H
#include <QString>
#include <QDateTime>
#include <QTcpSocket>

class svvProtocol
{
public:
    svvProtocol(QString idcamera,QDateTime timestamp);
    bool sendPackage(QTcpSocket *receptor, QImage &image);
    QImage recibePackage(QTcpSocket *emitter);//se guarda lo que se recibe en una imagen
    QDate getTimeStamp();       //devuelve el timestamp de la ultima foto enviada/recibida
    QString getIdCamera();      //Devuelve el id de la camara de la ultima foto enviada/recibida
private:
    QString idprotocol_;
    qint32 size_idprotocol_;
    QString idcamera_;
    QDateTime timestamp_;
    int state_; //si 0→ espera tamaño cabecera
                //si 1→ espera Qstring de cabecera
                //si 2→ espera tamaño idcamera
                //si 3→ espera Qstring de idcamera
                //si 4→ espera tamaño timestamp
                //si 5→ espera QDateTime en QString timestamp
                //si 6→ espera tamaño image
                //si 7→ espera QImage image
};

#endif // SVVPROTOCOL_H
