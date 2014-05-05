#include "motiondetector.h"

MotionDetector::MotionDetector() {

    backgroundSubtractor = new cv::BackgroundSubtractorMOG2();

    backgroundSubtractor->set("nmixtures", 3);

    // Desactivar la detección de sombras
    backgroundSubtractor->set("detectShadows", false);

    foregroundMask = new cv::Mat();
}

MotionDetector::~MotionDetector() {

    delete backgroundSubtractor;
    delete foregroundMask;
}

void MotionDetector::detect_motion(const QImage &image) {

    qDebug() << "Motion detector, procesando imagen...";

    QImage imageFormat = image.convertToFormat(QImage::Format_RGB32, Qt::ColorOnly);
    cv::Mat image_ = QtOcv::image2Mat(imageFormat);

    (*backgroundSubtractor)(image_, *foregroundMask);
    //backgroundSubtractor->operator ()(image_, *foregroundMask);

    // Operaciones morfolóficas para eliminar las regiones de
    // pequeño tamaño. Erode() las encoge y dilate() las vuelve a
    // agrandar.
    cv::erode(*foregroundMask, *foregroundMask, cv::Mat());
    cv::dilate(*foregroundMask, *foregroundMask, cv::Mat());

    // Obtener los contornos que bordean las regiones externas
    // (CV_RETR_EXTERNAL) encontradas. Cada contorno es un vector
    // de puntos y se devuelve uno por región en la máscara.
    ContoursType contours;
    cv::findContours(*foregroundMask, contours, CV_RETR_EXTERNAL,
                     CV_CHAIN_APPROX_NONE);

    // Aquí va el código ódigo que usa los contornos encontrados...
    // P. ej. usar cv::boundingRect() para obtener el cuadro
    // delimitador de cada uno y pintarlo en la imagen original

    //std::vector<QRect> boundingRects;

    VRect.clear();
    for(ContoursType::const_iterator i = contours.begin(); i < contours.end(); ++i) {
        cv::Rect rect = cv::boundingRect(*i);
        QRect rect_(rect.x, rect.y, rect.width, rect.height);
        VRect.push_back(rect_);
    }

    qDebug() << "Imagen procesada";
    emit processed_image(imageFormat, VRect);

}
