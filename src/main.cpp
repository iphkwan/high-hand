#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

#include "VideoDevice.h"
#include "HandSegment.h"
#include "HandTracker.h"
#include "TraceAnalyser.h"
#include "ResultDisplay.h"

using namespace std;
using namespace cv;

int main() {
    VideoDevice vd;
    HandSegment hs;
    HandTracker ht;
    TraceAnalyser ta;
    ResultDisplay rd;

    Mat src, yuvMask, yuvSrc, frg, handMask;
    vd.StartCamera(1);
    src = vd.GetNextFrame();
    hs.InitSkinModel();
    hs.RBG2YCbCr(src);
    hs.SkinExtract();
    yuvSrc = hs.GetYUVSrc();
    ht.GMMGenerate(yuvSrc);
    ht.StartTracker();
    ta.InitGestureList("./gestures.txt");
    rd.InitWindows();

    int frame_of_null = 0;

    while (1) {
        src = vd.GetNextFrame();
        hs.RBG2YCbCr(src);
        hs.SkinExtract();
        yuvMask = hs.GetYUVMask();
        yuvSrc = hs.GetYUVSrc();
        bool flag = ht.HandTracking(yuvMask, yuvSrc);
        frg = ht.GetForeground();
        handMask = ht.GetHandMask();
        //cout << yuvMask.cols << endl;
        ta.AnalyseTrace(ht.GetHandCenter(), ht.GetHandHotspot());

        rd.DisplayYCbCrMask(yuvMask);
        rd.DisplayHandMask(handMask);
        rd.DisplayForeground(frg);
        rd.DisplayTrace(src, ht.GetHandCenter(), ht.GetHandHotspot(), ta.GetTraceList(), ht.GetHandConvex());
        //rd.DisplayResult(ta.PredictResult());

        if (flag == false) {
            frame_of_null++;
            if (frame_of_null >= 5) {
                rd.DisplayResult(ta.PredictResult());
            }
            if (frame_of_null >= 30) {
                frame_of_null = 0;
                ta.CleanTrace();
            }
        }
        //imshow("src", src);
        //imshow("yuv", yuvMask);

        char key = (char)waitKey(1);
        if (key == 'q' || key == 'Q' || key == 27)
            break;
    }
    return 0;
}
