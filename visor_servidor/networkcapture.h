#ifndef NETWORKCAPTURE_H
#define NETWORKCAPTURE_H

#include <QDialog>

namespace Ui {
class NetworkCapture;
}

class NetworkCapture : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkCapture(QWidget *parent = 0);
    ~NetworkCapture();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NetworkCapture *ui;
};

#endif // NETWORKCAPTURE_H
