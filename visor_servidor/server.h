#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QFile>
#include <QSslKey>
#include <QSettings>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();

signals:
    void signal();

public slots:

    void incomingConnection(qintptr socketDescriptor);

    void connection_failure();

    void disconnect();

    void signal_to_viewer();

private:
    QSslSocket *socket;
    QByteArray key;
    QByteArray certificate;
    QSettings *settings;
};

#endif // SERVER_H
