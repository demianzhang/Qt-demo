#include "control.h"

#include "global.h"

// Qt header files
#include <QtGui>

Control::Control(){}

Control::~Control(){}

//capture video
void Control::connectToVideo()
{
    //创建处理进程
    processingThread = new ProcessingThread();

    //启动处理进程
    processingThread->start();

    return;
}



void Control::stopProcessingThread()
{
    qDebug() << "About to stop processing thread...";
    processingThread->stopProcessingThread();
    //processingThread->wait();
    qDebug() << "Processing thread successfully stopped.";
} // stopProcessingThread()


void Control::deleteProcessingThread()
{
    // Delete thread
    delete processingThread;
} // deleteProcessingThread()

