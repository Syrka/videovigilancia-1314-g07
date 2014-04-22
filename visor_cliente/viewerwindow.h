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

#include "dialogabout.h"
#include "preferencesdialog.h"
#include "capturebuffer.h"

namespace Ui {
class ViewerWindow;
}

class ViewerWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ViewerWindow(QWidget *parent = 0);
    ~ViewerWindow();
    
private slots:
    void on_Quit_clicked();

    void on_actionSalir_triggered();

    void on_actionAbrir_triggered();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void en_movie_updated();

    void on_checkBox_stateChanged();

    void on_actionAcerca_de_triggered();

    void on_actionCapturar_triggered();

    void image_slot(const QImage&);

    void on_actionPrefrencias_triggered();

    void connected();

private:
    Ui::ViewerWindow *ui;
    QMovie *movie;
    QCamera *camera;
    CaptureBuffer *captureBuffer;
    PreferencesDialog *preferences;
    QList<QByteArray> devices;
    int numDevice, defaultDevice;
    QTcpSocket *tcpSocket;
    QSslSocket *sslSocket;
    QString ipDir, nPort;
    QSettings *settings;
};

#endif // VIEWERWINDOW_H
