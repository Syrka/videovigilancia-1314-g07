#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QFile>
#include <QSslKey>
#include <QSettings>
#include "svvprotocol.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

public slots:

    void incomingConnection(qintptr socketDescriptor);//slot que recibe la señal de que hay una nueva conexion disponible

    QImage incomingImage();//

    void disconnect();

private:
    QList<svvProtocol*> protocol;    //se encarga de enviar y recibir los packages, no es el cliente en si mismo, es el protocolo (index)
    QList<QSslSocket*> emitters;     //lista con una serie de emisores, o clientes                                                (index)
    QByteArray key;
    QByteArray certificate;
    QSettings *settings;

};

#endif // SERVER_H
