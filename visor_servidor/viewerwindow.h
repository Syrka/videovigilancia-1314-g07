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

#include "dialogabout.h"
#include "preferencesdialog.h"
#include "capturebuffer.h"
#include "server.h"
#include "svvprotocol.h"

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

    void on_actionAcerca_de_triggered();

    void image_slot(const QImage&);

    void on_actionPrefrencias_triggered();

    void on_actionNetwork_capture_triggered();

    void read_image();

    void save_images(const QImage&);


private:
    Ui::ViewerWindow *ui;
    QCamera *camera;
    CaptureBuffer *captureBuffer;
    PreferencesDialog *preferences;
    QString ipDir, nPort;
    uint imageNum;
    Server *server;
    QSettings *settings;
    svvProtocol *emitter;
};

#endif // VIEWERWINDOW_H
