#ifndef GLOBAL_H
#define GLOBAL_H
#include <QMutex>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <chrono>
#include <thread>

// GFlags: DEFINE_bool, _int32, _int64, _uint64, _double, _string
#include <gflags/gflags.h>
// Allow Google Flags in Ubuntu 14
#ifndef GFLAGS_GFLAGS_H_
namespace gflags = google;
#endif
// OpenPose dependencies
#include <openpose/headers.hpp>
extern cv::VideoCapture cap;
extern QMutex stoppedMutex;
extern volatile bool stopped;
extern QMutex stoppedMutex1;
extern volatile bool stopped1;
extern int poseNum;

extern float posePoint[11][12][2];
#endif // GLOBAL_H
