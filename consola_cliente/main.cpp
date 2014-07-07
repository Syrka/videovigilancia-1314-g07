#include "viewerwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <iostream>
#include <QSettings>

typedef std::vector<std::vector<cv::Point> > ContoursType;
typedef std::vector<cv::Mat> ImagesType;

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    //DELETE THIS
    QApplication a_(argc, argv);

    QCoreApplication::setOrganizationName("Videovigilancia");
    QCoreApplication::setOrganizationDomain("https://github.com/ull-etsii-sistemas-operativos/videovigilancia-1314-g07");
    QCoreApplication::setApplicationName("viewer");

    QSettings settings(APP_CONFDIR);

    int numDevice;
    QString ipDir, nPort;

    numDevice = settings.value("viewer/device", 0).toInt();
    ipDir= settings.value("viewer/server/ip", "127.0.0.1").toString();
    nPort= settings.value("viewer/server/port", 15000).toInt();

    if (argc >= 2) {
        std::cout << "Introduce ? para mostrar la ayuda.\n";
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i],"?") == 0) {
                std::cout << "Cambiar dispositivo 'D #'\n ";
                std::cout << "Cambiar puerto 'P #'\n ";
                std::cout << "Cambiar ip 'IP #'\n " ;
                std::cout << "# es un numero entero.\n ";
            }
            if (strcmp(argv[i],"D") == 0) {
                 QString aux = argv[i+1];
                 numDevice = aux.toInt();
            }
            if(strcmp(argv[i],"P") == 0) {
                 nPort = argv[i+1];
            }
            if(strcmp(argv[i],"IP") == 0) {
                 ipDir = argv[i+1];
            }
        }
    }

    /*
    Console m(host,port.toInt(),nameClient, disp.toInt());
    setupUnixSignalHandlers();
    */

    ViewerWindow w;
    w.show();

    return a.exec();
}
