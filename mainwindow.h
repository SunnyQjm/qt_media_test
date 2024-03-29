#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraImageCapture>
#include <QMediaRecorder>
#include <QMediaPlayer>
#include <QLabel>
#include <QVideoProbe>
namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QCamera *curCamera = Q_NULLPTR;
    QCameraImageCapture *imageCapture;      // 抓图
    QMediaRecorder *mediaRecorder;          // 录像
    QMediaPlayer *player;                   // 视频播放
    QLabel *labelCameraState;
    QLabel *labelInfo;
    QLabel *labelCameraMode;
    QString filePath;
    QVideoProbe *probe;


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

    void on_openCamera_triggered();


    void processVideoFrame(QVideoFrame frame);

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
