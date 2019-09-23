#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QMediaPlayer>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QCamera *curCamera = Q_NULLPTR;
    QCameraImageCapture *imageCapture;      // 抓图
    QMediaRecorder *mediaRecorder;          // 录像
    QMediaPlayer *player;                   // 视频播放
    // VLC
    VlcInstance *_instance;
    VlcMedia *_media;
    VlcMediaPlayer *_player;
    QLabel *labelCameraState;
    QLabel *labelInfo;
    QLabel *labelCameraMode;
    QString filePath;
    void initCamera();              // 初始化摄像头
    void initImageCapture();        // 初始化静态抓图
    void initVideoRecorder();       // 初始化视频录制
    void initVideoPlayer();         // 初始化视频播放


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // QCamera的槽函数
    void onCamerastatechanged(QCamera::State state);
    void onCameracapturemodechanged(QCamera::CaptureModes mode);
    // QCameraImageCapture的槽函数
    void onImagereadyforcapture(bool ready);
    void onImagecaptured(int id, const QImage &preview);
    void onImagesaved(int id, const QString &fileName);
    // QMediaRecorder的槽函数
    void onVideostatechanged(QMediaRecorder::State state);
    void onVideodurationchanged(qint64 duration);
    void on_openCamera_triggered();

    void handleTimeChange(int time);
    void handleLengthChange(int length);
    void handleStateChange();
    void handleRecorderStatusChanged(QMediaRecorder::Status status);

    void on_closeCamera_triggered();

    void on_capture_triggered();

    void on_startRecorder_triggered();

    void on_stopRecorder_triggered();

    void on_exit_triggered();

    void on_openVideo_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
