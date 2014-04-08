#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
using namespace std;
using namespace cv;

class VideoDevice {
private:
    VideoCapture *capture;
    Mat srcImg;
    Size frameSize;
    bool isStop;
public:
    VideoDevice();
    ~VideoDevice();
    bool StartCamera(int video_device_no);
    bool StopCamera();
    bool ResetCamera();
    Mat GetNextFrame();
    Size GetFrameSize();
};

#endif
