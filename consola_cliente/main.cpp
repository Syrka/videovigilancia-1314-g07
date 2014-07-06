#include "viewerwindow.h"
#include <QApplication>

typedef std::vector<std::vector<cv::Point> > ContoursType;
typedef std::vector<cv::Mat> ImagesType;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Videovigilancia");
    QCoreApplication::setOrganizationDomain("https://github.com/ull-etsii-sistemas-operativos/videovigilancia-1314-g07");
    QCoreApplication::setApplicationName("viewer");

    ViewerWindow w;
    w.show();

    return a.exec();
}
