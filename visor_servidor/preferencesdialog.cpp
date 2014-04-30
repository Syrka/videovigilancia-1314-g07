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

    ui->lineEditIp->setText(ipDirP);
    ui->lineEditPort->setText(nPortP);
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
    delete settings;
}

void PreferencesDialog::on_okButton_clicked() {

    settings->setValue("viewer/server/ip", ipDirP);
    settings->setValue("viewer/server/port", nPortP);

    ipDirP = ui->lineEditIp->text();
    nPortP = ui->lineEditPort->text();

    close();
}

void PreferencesDialog::on_salirPrefButton_clicked() {
    close();
}
