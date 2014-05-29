#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    settings = new QSettings;
    ipDirP = settings->value("viewer/server/ip","127.0.0.1").toString();
    nPortP = settings->value("viewer/server/port","15000").toString();
    sslPath_ = settings->value("viewer/SSL/key").toString();

    ui->lineEditIp->setText(ipDirP);
    ui->lineEditPort->setText(nPortP);
    ui->lineEditSslPath->setText(sslPath_);

}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
    delete settings;
}

void PreferencesDialog::on_okButton_clicked() {

    //cargamos los valores que se han introducido
    ipDirP = ui->lineEditIp->text();
    nPortP = ui->lineEditPort->text();
    sslPath_ = ui->lineEditSslPath->text();

    settings->setValue("viewer/server/ip", ipDirP);
    settings->setValue("viewer/server/port", nPortP);
    settings->setValue("viewer/SSL/path", sslPath_);
    close();
}

void PreferencesDialog::on_salirPrefButton_clicked() {
    close();
}
