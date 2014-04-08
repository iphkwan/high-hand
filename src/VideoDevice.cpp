#include "VideoDevice.h"

VideoDevice::VideoDevice() {
    this->capture = NULL;
    this->isStop = true;
}

VideoDevice::~VideoDevice() {
    this->isStop = true;
    if (this->capture != NULL)
        this->capture->release();
}

Mat VideoDevice::GetNextFrame() {
    if (capture->read(this->srcImg) == true) {
        resize(this->srcImg, this->srcImg, Size(320, 240));
        srcImg.convertTo(srcImg, CV_32FC3);
        flip(this->srcImg, this->srcImg, 1);
        medianBlur(this->srcImg, this->srcImg, 5);
        normalize(this->srcImg, this->srcImg, 1.0, 0.0, CV_MINMAX);
        srcImg.convertTo(srcImg, CV_8UC3, 255);
    }
    return this->srcImg;
}

Size VideoDevice::GetFrameSize() {
    return this->frameSize;
}

bool VideoDevice::StartCamera(int video_device_no) {
    if (this->capture != NULL)
        return false;
    this->capture = new VideoCapture(video_device_no);
    if (this->capture->isOpened() == false)
        return false;

    GetNextFrame();
    frameSize = srcImg.size();
    this->isStop = false;
    return true;
}

bool VideoDevice::StopCamera() {
    this->isStop = true;
    if (this->capture != NULL) {
        this->capture->release();
        waitKey(1);
    }
    return true;
}

bool VideoDevice::ResetCamera() {
    return StartCamera(0);
}
