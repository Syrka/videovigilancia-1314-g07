#include "clientconsole.h"
#include <QCoreApplication>
#include <iostream>
#include <QSettings>

typedef std::vector<std::vector<cv::Point> > ContoursType;
typedef std::vector<cv::Mat> ImagesType;

int main(int argc, char *argv[]) {

    QCoreApplication::setOrganizationName("Videovigilancia");
    QCoreApplication::setOrganizationDomain("https://github.com/ull-etsii-sistemas-operativos/videovigilancia-1314-g07");
    QCoreApplication::setApplicationName("viewer");

    QCoreApplication a(argc, argv);
    ClientConsole c;
    setupUnixSignalHandlers();

    return a.exec();
}
