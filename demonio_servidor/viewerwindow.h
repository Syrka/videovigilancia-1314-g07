#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
#include <QCamera>
#include <QBuffer>
#include <QDir>
#include <QSslSocket>
#include <QPainter>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "capturebuffer.h"
#include "server.h"
#include "svvprotocol.h"
/*
namespace Ui {
class ViewerWindow;
}*/

class ViewerWindow : public QObject
{
    Q_OBJECT
    
public:
    explicit ViewerWindow();
    ~ViewerWindow();
    
private slots:
    //void on_Quit_clicked();

    //void on_actionSalir_triggered();

    //void image_slot(const QImage&, const QVector<QRect> &VRect);

    void on_actionNetwork_capture_triggered();

    //void read_image(const QImage &image, const QVector<QRect> &VRect);

    void save_images(const QImage&);


private:
    //Ui::ViewerWindow *ui;
    QCamera *camera;
    CaptureBuffer *captureBuffer;
    QString ipDir, nPort;
    uint imageNum;
    Server *server;
    QSettings *settings;
    SvvProtocol *emitter;
    QSqlDatabase db;
};

#endif // VIEWERWINDOW_H
