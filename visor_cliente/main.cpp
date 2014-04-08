#include "viewerwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ViewerWindow w;

    QCoreApplication::setOrganizationName("Videovigilancia");
    QCoreApplication::setOrganizationDomain("https://github.com/ull-etsii-sistemas-operativos/videovigilancia-1314-g07");
    QCoreApplication::setApplicationName("viewer");

    w.show();

    return a.exec();
}
