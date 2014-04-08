#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QSettings>
#include <QDebug>
#include <QCamera>

PreferencesDialog::PreferencesDialog(const QList<QByteArray> &devices, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    qDebug() << "Capturando de... "
             << QCamera::deviceDescription(devices[0]);
    int num = 0;
    while(devices.size() > num) {
        ui->comboCamOptsBox->addItem(QCamera::deviceDescription(devices[num]));
        num++;
    }

    QSettings settings;
    ipDirP = settings.value("viewer/server/ip").toString();
    nPortP = settings.value("viewer/server/port").toString();

    ui->lineEditIp->setText(ipDirP);
    ui->lineEditPort->setText(nPortP);
}

PreferencesDialog::~PreferencesDialog() {
    delete ui;
}

void PreferencesDialog::on_okButton_clicked() {

    numDevice = ui->comboCamOptsBox->currentIndex();
    QSettings settings;
    settings.setValue("viewer/device", numDevice);

    settings.setValue("viewer/server/ip", ui->lineEditIp->text());
    settings.setValue("viewer/server/port", ui->lineEditPort->text());

    close();
}

void PreferencesDialog::on_salirPrefButton_clicked() {
    close();
}
