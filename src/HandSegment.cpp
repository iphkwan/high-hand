#include "HandSegment.h"

bool HandSegment::InitSkinModel() {
    this->skinModel = Mat::zeros(Size(256, 256), CV_8UC1);
    ellipse(this->skinModel, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
    this->filterElement = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
    return true;
}

bool HandSegment::RBG2YCbCr(Mat &src) {
    cvtColor(src, this->yuvImg, CV_BGR2YCrCb);
    return true;
}

bool HandSegment::ClearYUVMask() {
    this->yuvMask = Mat::zeros(yuvImg.size(), CV_8UC1);
    return true;
}

Mat HandSegment::GetYUVMask() {
    return this->yuvMask;
}

bool HandSegment::ImgFilting(Mat &src) {
    erode(src, src, this->filterElement);
    dilate(src, src, this->filterElement);
    return true;
}

bool HandSegment::SkinExtract() {
    ClearYUVMask();
    for (int i = 0 ; i < yuvImg.cols; ++i) {
        for (int j = 0; j < yuvImg.rows; ++j) {
            Vec3b ycrcb = yuvImg.at<Vec3b>(j, i);
            if (skinModel.at<uchar>(ycrcb[1], ycrcb[2]) > 0) {
                yuvMask.at<uchar>(j, i) = 255;
            }
        }
    }
    ImgFilting(this->yuvMask);
    return true;
}
