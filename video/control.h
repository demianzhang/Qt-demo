#ifndef CONTROL_H
#define CONTROL_H

#include "processingthread.h"

// Qt header files
#include <QtGui>
// OpenCV header files
#include <opencv/highgui.h>



class Control : public QObject
{
    Q_OBJECT

public:
    Control();
    ~Control();
    //ImageBuffer *imageBuffer;
    ProcessingThread *processingThread;
    
    void connectToVideo();
    void stopProcessingThread();
    void deleteProcessingThread();
   
private:
    int imageSize;
};


#endif // CONTROL_H
