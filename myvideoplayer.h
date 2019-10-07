#ifndef MYVIDEOPLAYER_H
#define MYVIDEOPLAYER_H

#include <QtWidgets/QtWidgets>
#include <QPainter>
#include <QVideoFrame>

class MyVideoPlayer: public QWidget
{
public:
    MyVideoPlayer(QWidget* parent);
    void update(const QVideoFrame& frame);
protected:
    void paintEvent(QPaintEvent *pe);
private:
    QPainter *painter;
    QVideoFrame *currentFrame = nullptr;
    void init();
};

#endif // MYVIDEOPLAYER_H
