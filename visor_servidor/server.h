#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QFile>
#include <QSslKey>

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

    void stepToMain();

private:
    QSslSocket *socket;
};

#endif // SERVER_H
