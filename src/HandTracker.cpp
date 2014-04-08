#include "HandTracker.h"

bool HandTracker::StartTracker() {
    isTracking = true;
    return true;
}

bool HandTracker::StopTracker() {
    isTracking = false;
    return false;
}

Point HandTracker::GetHandCenter() {
    return handCenter;
}

Point HandTracker::GetHandHotspot() {
    return handHotspot;
}

vector<Point> HandTracker::GetHandConvex() {
    if (contours.size() > 0)
        return contours[0];
    return vector<Point>();
}

bool HandTracker::GMMGenerate(Mat &src) {
    foreground = src;
    element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
    return true;
}

bool HandTracker::GMMUpdate(Mat &src) {
    mog(src, foreground, 0.005);
    threshold(foreground, foreground, 128, 255, THRESH_BINARY);

    //img filting
    for (int k = 0; k < 3; k++)
        erode(foreground, foreground, element);
    for (int k = 0; k < 10; k++)
        dilate(foreground, foreground, element);

    return true;
}

void HandTracker::BFS(int x, int y, Mat &yuv) {
    int dx[4] = {-1, 0, 0, 1};
    int dy[4] = {0, -1, 1, 0};
    int tx, ty, nx, ny;
    queue< pair<int, int> > q;
    q.push(pair<int, int>(x, y));
    handMask.at<uchar>(x, y) = 255;
    while (!q.empty()) {
        tx = q.front().first;
        ty = q.front().second;
        q.pop();
        for (int i = 0; i < 4; i++) {
            nx = tx + dx[i];
            ny = ty + dy[i];
            if (nx >= 0  && nx < yuv.rows && ny >= 0 && ny < yuv.cols
                    && handMask.at<uchar>(nx, ny) == 0
                    && yuv.at<uchar>(nx, ny) > 0) {
                handMask.at<uchar>(nx, ny) = 255;
                q.push(pair<int, int>(nx, ny));
            }
        }
    }
}

Point HandTracker::PolyCenter(vector<Point> &v) {
    Point ret(0.0, 0.0);
    for (int i = 0; i < v.size(); i++) {
        ret.x += v[i].x;
        ret.y += v[i].y;
    }
    if (v.size() > 0) {
        ret.x /= v.size();
        ret.y /= v.size();
    }
    return ret;
}

bool HandTracker::HandTracking(Mat &yuvMask, Mat &yuvSrc) {
    if (isTracking == false)
        return false;

    this->GMMUpdate(yuvSrc);

    handMask = Mat::zeros(yuvMask.size(), CV_8UC1);

    //skin extract
    for (int i = 0 ; i < yuvMask.rows; ++i) {
        for (int j = 0; j < yuvMask.cols; ++j) {
            if (handMask.at<uchar>(i, j) == 0 && yuvMask.at<uchar>(i, j) > 0 && (int)foreground.at<uchar>(i, j) == 255) {
                this->BFS(i, j, yuvMask);
            }
        }
    }

    for (int k = 0; k < 3; k++)
        dilate(handMask, handMask, element);

    //get contours
    contours.clear();
    filter.clear();
    structure.clear();

    findContours(handMask, contours, structure, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    int max_area = 0, cur_area;
    Point tmpCenter, tmpHotspot;
    for (int i = 0; i < contours.size(); i++) {
        convexHull(contours[i], contours[i]);
        cur_area = fabs(contourArea(Mat(contours[i])));

        if (cur_area * 2 < handMask.cols * handMask.rows
                && cur_area * 50 > handMask.cols * handMask.rows
                && cur_area > max_area) {
            max_area = cur_area;
            handCenter = PolyCenter(contours[i]);
            filter.clear();
            filter.push_back(contours[i]);
        }
    }
    if (filter.size() > 0) {

        //find hotspot
        int td, d = 500;
        for (int i = 0; i < filter[0].size(); i++) {
            td = filter[0][i].y;
            if (td < d) {
                d = td;
                handHotspot = filter[0][i];
            }
        }
        return true;
    }
    return false;
}
