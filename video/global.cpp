#include "global.h"
cv::VideoCapture cap;
QMutex stoppedMutex;
QMutex stoppedMutex1;
volatile bool stopped = true;
volatile bool stopped1 = true;
