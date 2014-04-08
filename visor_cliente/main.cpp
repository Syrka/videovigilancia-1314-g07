#include "viewerwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ViewerWindow w;

    QCoreApplication::setOrganizationName("Sara Baez");
    QCoreApplication::setOrganizationDomain("sara.baez");
    QCoreApplication::setApplicationName("viewer");

    w.show();

    return a.exec();
}
