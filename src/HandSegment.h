#ifndef HANDSEGMENT_H
#define HANDSEGMENT_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
using namespace std;
using namespace cv;

class HandSegment {
private:
    Mat yuvImg, yuvMask, filterElement, skinModel;
public:
    HandSegment(){};
    ~HandSegment(){};
    bool InitSkinModel();
    bool RBG2YCbCr(Mat &src);
    bool ImgFilting(Mat &src);
    bool SkinExtract();
    Mat GetYUVMask();
    Mat GetYUVSrc();
    bool ClearYUVMask();
};

#endif
