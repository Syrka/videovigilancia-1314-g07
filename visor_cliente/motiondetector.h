#ifndef MOTIONDETECTOR_H
#define MOTIONDETECTOR_H

#include "cvmatandqimage.h"
#include <opencv2/opencv.hpp>
#include <QObject>
#include <QDebug>

// std::vector<cv::Mat> images = <vector de imágenes en cv::Mat>

// Definimos algunos tipos para que el código se lea mejor
typedef std::vector<cv::Mat> ImagesType;
typedef std::vector<std::vector<cv::Point> > ContoursType;

class MotionDetector : public QObject
{
    Q_OBJECT

signals:
    //se emite cuando se ha procesado la imagen
    void processed_image(const QImage &image, const QVector<QRect> &VRect);

public slots:

    void detect_motion(const QImage &image);

private:

    // Instancia de la clase del sustractor de fondo
    // cv::BackgroundSubtractorMOG2(history=500,
    //                              varThreshold=16,
    //                              bShadowDetection=true)
    cv::BackgroundSubtractorMOG2 *backgroundSubtractor;
    cv::Mat *foregroundMask;

    QVector<QRect> VRect;

public:

    MotionDetector();
    ~MotionDetector();

};

#endif // MOTIONDETECTOR_H
