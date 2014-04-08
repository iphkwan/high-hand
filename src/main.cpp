#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

#include "VideoDevice.h"
#include "HandSegment.h"

using namespace std;
using namespace cv;

int main() {
    VideoDevice vd;
    HandSegment hs;
    Mat src, yuv;
    vd.StartCamera(1);
    hs.InitSkinModel();
    while (1) {
        src = vd.GetNextFrame();
        hs.RBG2YCbCr(src);
        hs.SkinExtract();
        yuv = hs.GetYUVMask();
        imshow("src", src);
        imshow("yuv", yuv);

        char key = (char)waitKey(1);
        if (key == 'q' || key == 'Q' || key == 27)
            break;
    }
    return 0;
}
