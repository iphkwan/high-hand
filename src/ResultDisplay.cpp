#include "ResultDisplay.h"

bool ResultDisplay::InitWindows() {
    isStop = isTest = true;
    cvNamedWindow("trace", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("trace", 0, 0);
    cvNamedWindow("handMask", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("handMask", 450, 380);
    cvNamedWindow("YCbCrMask", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("YCbCrMask", 0, 380);
    cvNamedWindow("foreground", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("foreground", 450, 0);
    return true;
}

void ResultDisplay::DisplayResult(string &result) {
    cout << "Predict gesture: " << result << endl;
}

void ResultDisplay::DisplayYCbCrMask(Mat &yuvMask) {
    imshow("YCbCrMask", yuvMask);
}

void ResultDisplay::DisplayForeground(Mat &foreground) {
    imshow("foreground", foreground);
}

void ResultDisplay::AddSource(Mat &src) {
    trace = src;
}

void ResultDisplay::AddFinger(Point &handCenter, Point &handHotspot) {
    line(trace, handCenter, handHotspot, Scalar(0, 255, 0), 2);
    circle(trace, handCenter, 10, Scalar(0, 0, 255), 5);
    circle(trace, handHotspot, 10, Scalar(0, 0, 255), 5);
}

void ResultDisplay::AddTrace(vector<Point> &trc) {
    Point cur, nxt;
    for (int i = 3; i < trc.size() - 1; i++) {
        if (i == 3)
            cur = trc[i - 1];
        nxt.x = (cur.x + trc[i].x + trc[i + 1].x) / 3.0;
        nxt.y = (cur.y + trc[i].y + trc[i + 1].y) / 3.0;
        line(trace, cur, nxt, Scalar(255, 255, 0), 2);
        cur = nxt;
    }
    return;
}

void ResultDisplay::AddConvex(vector< vector<Point> > &convex) {
    drawContours(trace, convex, -1, Scalar(255, 0, 0), 2);
    return;
}

void ResultDisplay::DisplayTrace(Mat &src, Point &handCenter, vector<Point> &trc, vector< vector<Point> > &filter) {
    AddSource(src);
    if(filter.size() > 0) {
        AddFinger(handCenter, trc.back());
        AddTrace(trc);
        AddConvex(filter);
    }
    imshow("trace", trace);
}

bool ResultDisplay::StartDisplay() {
    isStop = false;
    return true;
}


bool ResultDisplay::StopDisplay() {
    isStop = true;
    return true;
}

bool ResultDisplay::TraceGenerate(vector<Point> &trace) {
    return true;
}
