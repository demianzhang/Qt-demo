#include "widget.h"
#include "ui_widget.h"
#include "global.h"
#include <QDateTime>
#include <QtWidgets>
#include <QVideoWidget>
#include <cmath>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //playButton
    ui->m_playButton->setEnabled(false);
    ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(ui->m_playButton, &QAbstractButton::clicked,this, &Widget::play);

    //openButton
    connect(ui->openButton, &QAbstractButton::clicked, this, &Widget::openFile);

//    //QMediaPlayer & QVideoWidget
//    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
//    QVideoWidget *videoWidget = new QVideoWidget;
//    QBoxLayout * layout_video = new QVBoxLayout;
//    layout_video->setMargin(0);
//    videoWidget->resize(ui->label_player->size());
//    layout_video->addWidget(videoWidget);
//    ui->label_player->setLayout(layout_video);
//    m_mediaPlayer->setVideoOutput(videoWidget);

    //Slider
    ui->horizontalSlider->setRange(0, 0);
    connect(ui->horizontalSlider, &QAbstractSlider::sliderMoved,this, &Widget::setPosition);

    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);


    isplay=0;
    maxSize = 31; // 只存储最新的 31 个数据
    maxX = 1000;
    maxY = 10;
    splineSeries = new QSplineSeries();
    scatterSeries = new QScatterSeries();
    scatterSeries->setMarkerSize(8);
    chart = new QChart();
    chart->addSeries(splineSeries);
    chart->addSeries(scatterSeries);
    chart->legend()->hide();
    //chart->setTitle("实时动态曲线");
    chart->createDefaultAxes();
    chart->axisX()->setRange(0, maxX);
    chart->axisY()->setRange(0, maxY);
    chart->removeAxis(chart->axisX());
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(chartView);
    ui->label2->setLayout(layout);
    qsrand(QDateTime::currentDateTime().toTime_t());
    

    


//    connect(m_mediaPlayer, &QMediaPlayer::stateChanged,this, &Widget::mediaStateChanged);
//    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &Widget::positionChanged);
//    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &Widget::durationChanged);
//    connect(m_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),this, &Widget::handleError);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::play()
{
    switch (isplay) 
    {
     case true:
        isplay=false;
        controller->stopProcessingThread();
        //timerId = -1;
        break;
     default:
        controller->connectToVideo();
        //将处理线程的“产生新的一帧”信号和GUI线程的“更新帧”槽连接
        connect(controller->processingThread,SIGNAL(newFrame(QImage)),this,SLOT(updateFrame(QImage)));
        connect(controller->processingThread,SIGNAL(newPoint(vector<pair<float,float>>)),this,SLOT(updatePoint(vector<pair<float,float>>)));
	    isplay=1;
        timerId = -1;//startTimer(200);
        break;
    }
}



//void Widget::mediaStateChanged(QMediaPlayer::State state)
//{
//    switch(state) {
//    case QMediaPlayer::PlayingState:
//        ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
//        break;
//    default:
//        ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
//        break;
//    }
//}

void Widget::positionChanged(qint64 position)
{
    ui->horizontalSlider->setValue(position);
}

void Widget::durationChanged(qint64 duration)
{
    ui->horizontalSlider->setRange(0, duration);
}

void Widget::setPosition(int position)
{
    //m_mediaPlayer->setPosition(position);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Widget::updateFrame(const QImage &frame)
{
    ui->label_player->setPixmap(QPixmap::fromImage(frame));
} // updateFrame()


void Widget::updatePoint(const vector<pair<float,float>> & points)
{
    float toCompare[12][2];

    toCompare[0][0]=points[4].first-points[3].first;
    toCompare[0][1]=points[4].second-points[3].second;
    toCompare[1][0]=points[3].first-points[2].first;
    toCompare[1][1]=points[3].second-points[2].second;
    toCompare[2][0]=points[1].first-points[2].first;
    toCompare[2][1]=points[1].second-points[2].second;
    toCompare[3][0]=points[5].first-points[1].first;
    toCompare[3][1]=points[5].second-points[1].second;
    toCompare[4][0]=points[6].first-points[5].first;
    toCompare[4][1]=points[6].second-points[5].second;
    toCompare[5][0]=points[7].first-points[6].first;
    toCompare[5][1]=points[7].second-points[6].second;
    toCompare[6][0]=points[8].first-points[1].first;
    toCompare[6][1]=points[8].second-points[1].second;
    toCompare[7][0]=points[9].first-points[8].first;
    toCompare[7][1]=points[9].second-points[8].second;
    toCompare[8][0]=points[10].first-points[9].first;
    toCompare[8][1]=points[10].second-points[9].second;
    toCompare[9][0]=points[11].first-points[1].first;
    toCompare[9][1]=points[11].second-points[1].second;
    toCompare[10][0]=points[12].first-points[11].first;
    toCompare[10][1]=points[12].second-points[11].second;
    toCompare[11][0]=points[13].first-points[12].first;
    toCompare[11][1]=points[13].second-points[12].second;

    for(int i=0;i<12;i++)
    {
       float tmp = sqrt(toCompare[i][0]*toCompare[i][0]+toCompare[i][1]*toCompare[i][1]);
       toCompare[i][0]=toCompare[i][0]/tmp;
       toCompare[i][1]=toCompare[i][1]/tmp;
       //qDebug() <<"point+" <<toCompare[i][0]<<" "<<toCompare[i][1] ;
    }

    int maxPoint=-1,maxValue=-1,i;
    float sum;
    for(i=poseNum;i<=min(poseNum+2,10);i++)
    {
        sum=0;
        for(int j=0;j<12;j++)
            sum+=posePoint[i][j][0]*toCompare[j][0]+posePoint[i][j][1]*toCompare[j][1];

        if(maxValue<sum)
        {
            maxValue = sum;
            maxPoint = i;
        }
    }

    if(maxValue>=11.9)poseNum=maxPoint+1;
    if(maxValue<11.9)maxValue=0;
    int data = max(maxValue-2,0);
    maxPoint=-1,maxValue=-1;
    dataReceived(data);
}

void Widget::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Movie"));
    QStringList supportedMimeTypes; //= m_mediaPlayer->supportedMimeTypes();
    if (!supportedMimeTypes.isEmpty())
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        setUrl(fileDialog.selectedUrls().constFirst());
}

void Widget::setUrl(const QUrl &url)
{
    m_errorLabel->setText(QString());
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    //m_mediaPlayer->setMedia(url);
    std::string s = url.toString().toStdString();
    cap.open(s.c_str());
    ui->m_playButton->setEnabled(true);
}



void Widget::handleError()
{
    ui->m_playButton->setEnabled(false);
//    const QString errorString = m_mediaPlayer->errorString();
//    QString message = "Error: ";
//    if (errorString.isEmpty())
//        message += " #" + QString::number(int(m_mediaPlayer->error()));	
//    else
//        message += errorString;
//    m_errorLabel->setText(message);
}


void Widget::timerEvent(QTimerEvent *event) {
    // 产生一个数据，模拟不停的接收到新数据
    if (event->timerId() == timerId) {
        int newData = qrand() % (maxY + 1);
        dataReceived(newData);
    }
}


void Widget::dataReceived(int value) {
    data << value;
    // 数据个数超过了最大数量，则删除最先接收到的数据，实现曲线向前移动
    while (data.size() > maxSize) {
        data.removeFirst();
    }
    // 界面被隐藏后就没有必要绘制数据的曲线了
    if (isVisible()) {
        splineSeries->clear();
        scatterSeries->clear();
        int dx = maxX / (maxSize-1);
        int less = maxSize - data.size();
        for (int i = 0; i < data.size(); ++i) {
            splineSeries->append(less*dx+i*dx, data.at(i));
            scatterSeries->append(less*dx+i*dx, data.at(i));
        }
    }
}
