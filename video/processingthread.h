#ifndef PROCESSINGTHREAD_H
#define PROCESSINGTHREAD_H


#include <QThread>
#include <QtGui>
#include <chrono>
#include <thread>
#include <vector>
#include <utility>
#include <QMetaType>
using namespace std;
// GFlags: DEFINE_bool, _int32, _int64, _uint64, _double, _string
#include <gflags/gflags.h>
// Allow Google Flags in Ubuntu 14
#ifndef GFLAGS_GFLAGS_H_
namespace gflags = google;
#endif
// OpenPose dependencies
#include <openpose/headers.hpp>
#include <opencv/highgui.h>


using namespace cv;



class ProcessingThread : public QThread
{
    Q_OBJECT

public:
    ProcessingThread();
    ~ProcessingThread();
    void stopProcessingThread();

private:
    //  volatile bool stopped;
    int inputSourceWidth;
    int inputSourceHeight;

    Mat currentFrame;
    QImage frame;
    //   QMutex stoppedMutex;
    QMutex updateMembersMutex;
    Size frameSize;
    Point framePoint;
    QString name;
    // Image processing flags

protected:
    void run();
signals:
    void newFrame(const QImage &frame);
    void newName(const QString &name);
    void newPoint(const vector<pair<float,float> > &);
};

#endif // PROCESSINGTHREAD_H
