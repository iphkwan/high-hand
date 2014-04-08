#ifndef RESULTDISPLAY_H
#define RESULTDISPLAY_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

class ResultDisplay {
private:
    bool isStop, isTest;
    Mat trace;
public:
    bool InitWindows();
    void DisplayResult(string &result);
    void DisplayYCbCrMask(Mat &yuvMask);
    void DisplayForeground(Mat &foreground);
    void DisplayTrace(Mat &src, Point &handCenter, vector<Point> &trc, vector< vector<Point> > &filter);
    void AddSource(Mat &src);
    void AddFinger(Point &handCenter, Point &handHotspot);
    void AddTrace(vector<Point> &trc);
    void AddConvex(vector< vector<Point> > &convex);
    bool StartDisplay();
    bool StopDisplay();
    bool TraceGenerate(vector<Point> &trace);
};

#endif
