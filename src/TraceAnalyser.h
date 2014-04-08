#ifndef TRACEANALYSER_H
#define TRACEANALYSER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

class TraceAnalyser {
private:
    vector<Point> centerTrace, hotspotTrace;
    vector< pair<string, string> > gestureList;
    string dirStr;
    int SqrDis(Point &x, Point &y);
public:
    bool InitGestureList(const char *filepath);
    bool AddCenter(Point handCenter);
    bool AddHotspot(Point handHotspot);
    bool AnalyseTrace(Point handCenter, Point handHotspot);
    string PredictResult();
    vector<Point> GetTraceList();
    bool CleanTrace();
    char JudgeDirection(Point &last, Point &cur);
    int CalEditDistance(String &tmpStr);
};

#endif
