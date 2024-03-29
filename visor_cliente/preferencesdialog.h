#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(const QList<QByteArray> &devices, QWidget *parent = 0);
    ~PreferencesDialog();

private slots:
    void on_okButton_clicked();

    void on_salirPrefButton_clicked();

private:
    Ui::PreferencesDialog *ui;
    int numDevice;
    QString ipDirP, nPortP;
    QSettings *settings;
};

#endif // PREFERENCESDIALOG_H
