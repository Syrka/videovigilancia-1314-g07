#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QMessageBox>
#include <QSettings>
#include <QCamera>
#include <QBuffer>
#include <QDir>
#include <QSslSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSocketNotifier>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <QCoreApplication>

#include "capturebuffer.h"
#include "server.h"
#include "svvprotocol.h"

int setupUnixSignalHandlers();

class ViewerWindow : public QObject
{
    Q_OBJECT
    
public:
    explicit ViewerWindow();
    ~ViewerWindow();
    
    //Manejadores de señal POSIX
    //Debe ser static para poder pasar el metodo como manejador al invocar signal()
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);
    static void intSignalHandler(int unused);

public slots:
    void handleSigHup();
    void handleSigTerm();
    void handleSigInt();

private slots:

    void on_actionNetwork_capture_triggered();

    void image_slot(const QImage&, const QVector<QRect> &VRect);

    void save_images(const QImage&);

private:
    QCamera *camera;
    CaptureBuffer *captureBuffer;
    QString ipDir, nPort;
    uint imageNum;
    Server *server;
    QSettings *settings;
    SvvProtocol *emitter;
    QSqlDatabase db;

    //Pareja de sockets. Un par por señal a manejar
    static int sigHupSd[2];
    static int sigTermSd[2];
    static int sigIntSd[2];

    //Objeto para monitorizar la pareja de sockets
    QSocketNotifier *sigHupNotifier;
    QSocketNotifier *sigTermNotifier;
    QSocketNotifier *sigIntNotifier;

};

#endif // VIEWERWINDOW_H
