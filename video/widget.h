#ifndef WIDGET_H
#define WIDGET_H
#include "control.h"

#include <QMediaPlayer>
#include <QWidget>
#include <QList>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QChart>
#include <QChartView>


namespace Ui {
class Widget;
}
using namespace QtCharts;

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void setUrl(const QUrl &url);

public slots:
    void openFile();
    void play();

private slots:
    //void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void handleError();
    void updateFrame(const QImage &frame);

private:
    Control *controller;
    int sourceWidth;
    int sourceHeight;
    bool isplay;
   

    QMediaPlayer* m_mediaPlayer;
    QAbstractButton *m_playButton;
    QSlider *m_positionSlider;
    QLabel *m_errorLabel;

    Ui::Widget *ui;


    /**
     * 接收到数据源发送来的数据，数据源可以下位机，采集卡，传感器等。
     */
    void dataReceived(int value);
    int timerId;
    int maxSize;  // data 最多存储 maxSize 个元素
    int maxX;
    int maxY;
    QList<double> data; // 存储业务数据的 list
    QChart *chart;
    QChartView *chartView;
    QSplineSeries *splineSeries;
    QScatterSeries *scatterSeries;

protected:
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

};
#endif // WIDGET_H
