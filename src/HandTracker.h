#ifndef HANDTRACKER_H
#define HANDTRACKER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <iostream>
#include <vector>
#include <queue>
using namespace std;
using namespace cv;

class HandTracker {
private:
    Mat foreground, handMask, element;
    bool isTracking;
    BackgroundSubtractorMOG2 mog;
    vector< vector<Point> > contours, filter;
    vector<Vec4i> structure;
    Point handCenter, handHotspot;
    void BFS(int x, int y, Mat &yuv);
    Point PolyCenter(vector<Point> &v);
public:
    bool GMMGenerate(Mat &src);
    bool GMMUpdate(Mat &src);
    bool HandTracking(Mat &yuvMask, Mat &yuvSrc);
    Point GetHandCenter();
    Point GetHandHotspot();
    vector< vector<Point> > GetHandConvex();
    Mat GetForeground();
    Mat GetHandMask();
    bool StartTracker();
    bool StopTracker();
};

#endif
