#include "networkcapture.h"
#include "ui_networkcapture.h"

NetworkCapture::NetworkCapture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkCapture)
{
    ui->setupUi(this);
}

NetworkCapture::~NetworkCapture()
{
    delete ui;
}

void NetworkCapture::on_buttonBox_accepted()
{

}
