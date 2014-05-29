#include "capturebuffer.h"

CaptureBuffer::CaptureBuffer() {}

CaptureBuffer::~CaptureBuffer() {
    //delete this;
}

bool CaptureBuffer:: present(const QVideoFrame &frame) {

    QVideoFrame frame_(frame);
    frame_.map(QAbstractVideoBuffer::ReadOnly);
    qDebug() << frame.isValid();


    QImage frameAsImage = QImage(frame_.bits(), frame_.width(), frame_.height(), frame_.bytesPerLine(),
                                  QVideoFrame::imageFormatFromPixelFormat(frame_.pixelFormat()));

    if (frame.isValid()){
        frameAsImage.copy();
        emit image_signal(frameAsImage);
    }
    frame_.unmap();
    return true;
}
