#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QBuffer>
#include <QCameraInfo>
#include <QCameraViewfinder>

void MainWindow::initCamera()
{
    // 创建QCamera对象
    QCameraInfo curCameraInfo = QCameraInfo::defaultCamera();   // 获取缺省摄像头
    ui->comboCamera->addItem(curCameraInfo.description());
    ui->comboCamera->setCurrentIndex(0);

    // 创建摄像头对象
    curCamera = new QCamera(curCameraInfo, this);
    QCameraViewfinderSettings viewFinderSettings;
    viewFinderSettings.setResolution(640, 480);
    viewFinderSettings.setMinimumFrameRate(15.0);
    viewFinderSettings.setMaximumFrameRate(30.0);
    curCamera->setViewfinderSettings(viewFinderSettings);
    curCamera->setViewfinder(ui->viewFinder);
    curCamera->setCaptureMode(QCamera::CaptureViewfinder);

    // 判断摄像头是否支持抓图、录制视频
    ui->checkStillImage->setChecked(
                curCamera->isCaptureModeSupported(QCamera::CaptureStillImage));
    ui->checkVideo->setChecked(
                curCamera->isCaptureModeSupported(QCamera::CaptureVideo));

    // 连接信号和槽
    connect(curCamera, SIGNAL(stateChanged(QCamera::State)), this,
            SLOT(onCamerastatechanged(QCamera::State)));
    connect(curCamera, SIGNAL(captureModeChanged(QCamera::CaptureModes)), this,
            SLOT(onCameracapturemodechanged(QCamera::CaptureModes)));
}

void MainWindow::initImageCapture()
{
    // 创建QCameraImageCapture对象
    imageCapture = new QCameraImageCapture(curCamera, this);
    imageCapture->setBufferFormat(QVideoFrame::Format_Jpeg);    // 设置缓冲区格式
    imageCapture->setCaptureDestination(
                QCameraImageCapture::CaptureToFile);            // 保存目标
    connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)),
            this, SLOT(onImagereadyforcapture(bool)));
    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)),
            this, SLOT(onImagecaptured(int,QImage)));
    connect(imageCapture, SIGNAL(imageSaved(int,QString)),
            this, SLOT(onImagesaved(int,QString)));
}

void MainWindow::initVideoRecorder()
{

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    labelCameraState = new QLabel("摄像头state: ");
    labelCameraState->setMinimumWidth(150);
    ui->statusBar->addWidget(labelCameraState);
    labelCameraMode = new QLabel("抓取模式：");
    labelCameraMode->setMidLineWidth(150);
    ui->statusBar->addWidget(labelCameraMode);
    labelInfo = new QLabel("");
    ui->statusBar->addPermanentWidget(labelInfo);
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    qDebug() << cameras.size() << endl;
    if(cameras.size() > 0) {
        initCamera();               // 初始化摄像头
        initImageCapture();         // 初始化静态抓图
        initVideoRecorder();        // 初始化视频录制
        curCamera->start();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCamerastatechanged(QCamera::State state)
{
    qDebug() << "onCamerastatechanged" << endl;
    switch (state) {
    case QCamera::UnloadedState:
        labelCameraState->setText("摄像头state：UnloadedState");
        break;
    case QCamera::LoadedState:
        labelCameraState->setText("摄像头state：LoadedState");
        break;
    case QCamera::ActiveState:
        labelCameraState->setText("摄像头state：ActiveState");
        break;
    default:
        break;
    }
    ui->openCamera->setEnabled(state != QCamera::ActiveState);
    ui->closeCamera->setEnabled(state == QCamera::ActiveState);
}

void MainWindow::onCameracapturemodechanged(QCamera::CaptureModes mode)
{
    qDebug() << "onCameraCaptureModechanged" << endl;
    if(mode == QCamera::CaptureStillImage) {
        labelCameraMode->setText("抓取模式: StillImage");
    } else if (mode == QCamera::CaptureVideo) {
        labelCameraMode->setText("抓取模式：Video");
    } else {
        labelCameraMode->setText("抓取模式: ViewFinder");
    }
}

void MainWindow::onImagereadyforcapture(bool ready)
{
    // 可以抓图了
    ui->capture->setEnabled(ready);
}

void MainWindow::onImagecaptured(int id, const QImage &preview)
{
    // 抓取图片后显示
    Q_UNUSED(id);
//    QImage scaledImage = preview.scaled(ui->)
}

void MainWindow::onImagesaved(int id, const QString &fileName)
{

}

void MainWindow::onVideostatechanged(QMediaRecorder::State state)
{

}

void MainWindow::onVideodurationchanged(qint64 duration)
{

}

/**
 * 打开摄像头
 * @brief MainWindow::on_openCamera_triggered
 */
void MainWindow::on_openCamera_triggered()
{
    // 开启摄像头
    curCamera->start();
}

void MainWindow::on_closeCamera_triggered()
{
    // 关闭摄像头
    curCamera->stop();
}

void MainWindow::on_capture_triggered()
{

}

void MainWindow::on_startRecorder_triggered()
{

}

void MainWindow::on_stopRecorder_triggered()
{

}

void MainWindow::on_exit_triggered()
{
    this->close();
}
