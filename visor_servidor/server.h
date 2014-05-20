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

signals:
    void signal();

public slots:

    void incomingConnection(qintptr socketDescriptor);//slot que recibe la señal de que hay una nueva conexion disponible

    QImage incomingImage();

    void connection_failure();

    void disconnect();

    void signal_to_viewer();

    void downloadedImage();

private:
    svvProtocol protocol;           //se encarga de enviar y recibir los packages, no es el cliente en si mismo, es el protocolo
    QList<QSslSocket> emitters;     //lista con una serie de emisores, o clientes
    QSslSocket * current_emitter;   //apunta al emisor o cliente actual
    QByteArray key;
    QByteArray certificate;
    QSettings *settings;

};

#endif // SERVER_H
