#ifndef CAPTUREBUFFER_H
#define CAPTUREBUFFER_H

#include <QAbstractVideoSurface>

class CaptureBuffer : public QAbstractVideoSurface
{
    Q_OBJECT

public:

    CaptureBuffer();
    ~CaptureBuffer();
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
                QAbstractVideoBuffer::HandleType =
                QAbstractVideoBuffer::NoHandle) const
    {
        // A través de este método nos preguntan que formatos de
        // vídeo soportamos. Como vamos a guardar los frames en
        // objetos QImage nos sirve cualquiera de los formatos
        // sorportados por dicha clase: http://kcy.me/z6pa
        QList<QVideoFrame::PixelFormat> formats;
        formats << QVideoFrame::Format_ARGB32;
        formats << QVideoFrame::Format_ARGB32_Premultiplied;
        formats << QVideoFrame::Format_RGB32;
        formats << QVideoFrame::Format_RGB24;
        formats << QVideoFrame::Format_RGB565;
        formats << QVideoFrame::Format_RGB555;
        return formats;
    }

    bool present(const QVideoFrame &frame);

signals:
    void image_signal(const QImage&);

};

#endif // CAPTUREBUFFER_H
