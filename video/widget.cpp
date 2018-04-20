#include "widget.h"
#include "ui_widget.h"

#include <QDateTime>
#include <QtWidgets>
#include <QVideoWidget>

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

    //QMediaPlayer & QVideoWidget
    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    QVideoWidget *videoWidget = new QVideoWidget;
    QBoxLayout * layout_video = new QVBoxLayout;
    layout_video->setMargin(0);
    videoWidget->resize(ui->label_player->size());
    layout_video->addWidget(videoWidget);
    ui->label_player->setLayout(layout_video);
    m_mediaPlayer->setVideoOutput(videoWidget);
    //Slider
    ui->horizontalSlider->setRange(0, 0);
    connect(ui->horizontalSlider, &QAbstractSlider::sliderMoved,this, &Widget::setPosition);

    m_errorLabel = new QLabel;
    m_errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

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

    connect(m_mediaPlayer, &QMediaPlayer::stateChanged,this, &Widget::mediaStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &Widget::positionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &Widget::durationChanged);
    connect(m_mediaPlayer, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),this, &Widget::handleError);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::openFile()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Movie"));
    QStringList supportedMimeTypes = m_mediaPlayer->supportedMimeTypes();
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
    m_mediaPlayer->setMedia(url);
    ui->m_playButton->setEnabled(true);
}

void Widget::play()
{
    switch (m_mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        m_mediaPlayer->pause();
        timerId = -1;
        break;
    default:
        m_mediaPlayer->play();
        timerId = startTimer(200);
        break;
    }
}

void Widget::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

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
    m_mediaPlayer->setPosition(position);
}

void Widget::handleError()
{
    ui->m_playButton->setEnabled(false);
    const QString errorString = m_mediaPlayer->errorString();
    QString message = "Error: ";
    if (errorString.isEmpty())
        message += " #" + QString::number(int(m_mediaPlayer->error()));
    else
        message += errorString;
    m_errorLabel->setText(message);
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
