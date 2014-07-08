#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QMovie>
#include <QSettings>
#include <QCamera>
#include <QTime>
#include <QPainter>
#include <QTcpSocket>
#include <QBuffer>
#include <QImageWriter>
#include <QSslSocket>
#include <QThread>
#include <QSocketNotifier>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <QCoreApplication>

#include "capturebuffer.h"
#include "svvprotocol.h"
#include "motiondetector.h"

typedef std::vector<cv::Mat> ImagesType;
typedef std::vector<std::vector<cv::Point> > ContoursType;

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

    void on_actionCapturar_triggered();

    void image_slot(const QImage&);

    void connected();

    void send_processed(const QImage &image, const QVector<QRect> &VRect);

private:
    QCamera *camera;
    CaptureBuffer *captureBuffer;
    QList<QByteArray> devices;
    int numDevice, defaultDevice;
    QSslSocket *sslSocket;
    QString ipDir, nPort;
    QSettings *settings;
    MotionDetector *motionDetector;
    QThread *motionThread;

    //Pareja de sockets. Un par por señal a manejar
    static int sigHupSd[2];
    static int sigTermSd[2];
    static int sigIntSd[2];

    //Objeto para monitorizar la pareja de sockets
    QSocketNotifier *sigHupNotifier;
    QSocketNotifier *sigTermNotifier;
    QSocketNotifier *sigIntNotifier;

signals:
    void to_motion_detector(const QImage &image);

};

int setupUnixSignalHandlers();

#endif // VIEWERWINDOW_H
