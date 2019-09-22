#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QVideoFrame>
#include <QBuffer>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QMessageBox>
#include <QFile>
#include <QVideoProbe>
#include <QDir>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>
#include <QThread>

void MainWindow::initCamera()
{
    // 创建QCamera对象
    QCameraInfo curCameraInfo = QCameraInfo::defaultCamera();   // 获取缺省摄像头
    ui->comboCamera->addItem(curCameraInfo.description());
    ui->comboCamera->setCurrentIndex(0);

    qDebug() << curCameraInfo.description() << endl;
    qDebug() << curCameraInfo.deviceName() << endl;
    // 创建摄像头对象
    curCamera = new QCamera(curCameraInfo, this);
    curCamera->setCaptureMode(QCamera::CaptureVideo);
//    QCameraViewfinderSettings viewFinderSettings;
//    viewFinderSettings.setResolution(640, 480);
//    viewFinderSettings.setMinimumFrameRate(15.0);
//    viewFinderSettings.setMaximumFrameRate(30.0);
//    curCamera->setViewfinderSettings(viewFinderSettings);
//    curCamera->setViewfinder(ui->viewFinder);
//    curCamera->setCaptureMode(QCamera::CaptureViewfinder);

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
    // 创建QMediaRecorder对象
    mediaRecorder = new QMediaRecorder(curCamera, this);
    ui->chkMute->setChecked(mediaRecorder->isMuted());
    connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(onVideostatechanged(QMediaRecorder::State)));
    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(onVideodurationchanged(qint64)));
}

void MainWindow::initVideoPlayer()
{
    player = new QMediaPlayer(this);
    player->setNotifyInterval(2000);
    player->setVideoOutput(ui->videoWidget);

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
        initVideoPlayer();
//        curCamera->start();
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
    Q_UNUSED(id)
    QImage scaledImage = preview.scaled(ui->labelCaptureImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelCaptureImage->setPixmap(QPixmap::fromImage(scaledImage));
}

void MainWindow::onImagesaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    // 文件保存后显示保存的文件名
    labelInfo->setText("图片保存为：" + fileName);
}

void MainWindow::onVideostatechanged(QMediaRecorder::State state)
{
    // 状态变化
    ui->startRecorder->setEnabled(state != QMediaRecorder::RecordingState);
    ui->stopRecorder->setEnabled(state == QMediaRecorder::RecordingState);
}

void MainWindow::onVideodurationchanged(qint64 duration)
{
    ui->labelDuration->setText(QString("录制时间：%1 秒").arg(duration / 1000));
    qDebug() << duration << endl;

    if(duration < 1000) {
        QString command = "smplayer /home/mingj/Videos/" + filePath + ".ogg -minigui";
        QtConcurrent::run([](const QString &command) {
            QProcess p(0);
            p.execute(command);
        }, QString(command));
    }
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
    // 抓图按钮
    if(curCamera->captureMode() != QCamera::CaptureStillImage) {
        curCamera->setCaptureMode(QCamera::CaptureStillImage);
    }
    imageCapture->capture();
}

void MainWindow::on_startRecorder_triggered()
{
    // 开始录像
    if(!mediaRecorder->isAvailable()) {
        QMessageBox::critical(this, "错误", "不支持视频录制 \n mediaRecorder->isAvailable() == false");
        return;
    }
    if(curCamera->captureMode() != QCamera::CaptureVideo) {
        curCamera->setCaptureMode(QCamera::CaptureVideo);
    }
    QString selectedFile = ui->editOutputFile->text().trimmed();
    if(selectedFile.isEmpty()) {
        QMessageBox::critical(this, "错误", "请先设置录音输出文件");
        return;
    }
    if(QFile::exists(selectedFile)) {
        if(!QFile::remove(selectedFile)) {
            QMessageBox::critical(this, "错误", "所设置的录音输出文件被占用，无法删除");
        }
    }
    filePath = selectedFile;
    mediaRecorder->setOutputLocation(QUrl::fromLocalFile(selectedFile));
    mediaRecorder->record();



}

void MainWindow::on_stopRecorder_triggered()
{
    mediaRecorder->stop();
}

void MainWindow::on_exit_triggered()
{
    this->close();
}

void MainWindow::on_openVideo_clicked()
{
    // 打开文件
    QString curPath = QDir::homePath();         // 获取系统当前目录
    QString dlgTitle = "选择视频文件";            // 对话框标题
    QString filter = "所有文件(*.*);;wmv文件(*.wmv);;mp4文件(*.mp4)";
    QString file = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);
    if(file.isEmpty())
        return;
    player->setMedia(QUrl::fromLocalFile(file));
    player->play();


//    }
}
