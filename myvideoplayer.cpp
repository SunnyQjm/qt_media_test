#include "myvideoplayer.h"

MyVideoPlayer::MyVideoPlayer(QWidget *parent): QWidget(parent)
{
    init();
}


void MyVideoPlayer::update(const QVideoFrame &frame)
{
    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    bool result = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()) == QImage::Format_Invalid;
    qDebug() << "update-" << timestamp <<": " << (result ? "true" : "false") << frame.pixelFormat() << endl;
    this->currentFrame = new QVideoFrame(frame);
    this->repaint();
}

void MyVideoPlayer::paintEvent(QPaintEvent *pe)
{
    if(currentFrame == nullptr) {
        return;
    }
    if(this->currentFrame->map(QAbstractVideoBuffer::ReadOnly)) {

        QPainter painter(this);
        QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(currentFrame->pixelFormat());
        QImage image;
        qDebug() << currentFrame->mappedBytes() << endl;
        if(imageFormat == QImage::Format_Invalid) {
            int nbytes = currentFrame->mappedBytes();
            image = QImage::fromData(currentFrame->bits(), nbytes);
        } else {
            image = QImage(currentFrame->bits(), currentFrame->width(), currentFrame->height(), currentFrame->bytesPerLine(), imageFormat);
        }
        QRect rect(0, 0, 640, 480);
        painter.drawImage(rect, image);
        this->currentFrame->unmap();
    }
}

void MyVideoPlayer::init()
{
    painter = new QPainter(this);
}
