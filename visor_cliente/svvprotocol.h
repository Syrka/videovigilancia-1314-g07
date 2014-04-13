#ifndef SVVPROTOCOL_H
#define SVVPROTOCOL_H
#include <QString>
#include <QDate>
#include <QTcpSocket>

class svvProtocol
{
public:
    svvProtocol(QString idcamera,QDate timestamp);
    bool sendPackage(QTcpSocket &receptor, QImage &image);
    QImage recibePackage(QTcpSocket &emitter);//se guarda lo que se recibe en una imagen
    QDate getTimeStamp();       //devuelve el timestamp de la ultima foto enviada/recibida
    QString getIdCamera();      //Devuelve el id de la camara de la ultima foto enviada/recibida
private:
    QString idprotocol_;
    QString idcamera_;
    QDate timestamp_;
};

#endif // SVVPROTOCOL_H
