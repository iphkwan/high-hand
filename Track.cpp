#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <time.h>
using namespace std;
using namespace cv;

#define VIDEO_DEVICE_NO 1
#define AREA_LIMIT 1500
#define ARC_LENGTH_LIMIT 3000
#define TRACE_LENGTH_LIMIT_LOW 50
#define TRACE_LENGTH_LIMIT_HIGH 300
#define PACE_THRESHOLD 30
#define START_DRAW 3

#define GESTURE_PATH "./gestures.txt"
#define EPS 1e-8

#define DEBUG 0
#define YUV_TEST 1


//Algorithm libs
float SqrDis (Point &a, Point &b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

Point PolyCenter (vector<Point> &vt) {
    Point ret(0.0, 0.0);
#if DEBUG
    cout << "Num of Points = " << vt.size() << endl;
#endif
    for (int i = 0; i < vt.size(); i++) {
        ret.x += vt[i].x;
        ret.y += vt[i].y;
#if DEBUG
        cout << vt[i] << " ";
#endif
    }
#if DEBUG
    cout << endl;
#endif
    if (vt.size() > 0) {
        ret.x /= vt.size();
        ret.y /= vt.size();
    }
    return ret;
}

class Analyser {
public:
    Analyser() {
        count = 0;
        readGesture();
    }
    ~Analyser() {}

    void readGesture() {
        g_map.clear();
        string a, b;
        ifstream fin(GESTURE_PATH);
        while (fin >> a >> b)
            g_map.push_back(pair<string, string>(a, b));
        fin.close();
    }
    void clear() {
        count = 0;
        gesture.clear();
    }
    void judge(Point p) {
        if (count == 0)
            cur_point = p;
        else {
            //float d = (p.x - cur_point.x) * (p.x - cur_point.x) + (p.y - cur_point.y) * (p.y - cur_point.y);
            float d = SqrDis(p, cur_point);
            if (d > TRACE_LENGTH_LIMIT_LOW * TRACE_LENGTH_LIMIT_LOW && d < TRACE_LENGTH_LIMIT_HIGH * TRACE_LENGTH_LIMIT_HIGH) {
                if (fabs(p.x - cur_point.x) < EPS) {
                    if (p.y < cur_point.y)
                        gesture.push_back(NORTH);
                    else
                        gesture.push_back(SOUTH);
                } else {
                    float slope = (cur_point.y - p.y) / (cur_point.x - p.x);
                    if (cur_point.y > p.y) {
                        if (slope > 1 || slope < -1)
                            gesture.push_back(NORTH);
                        else if (cur_point.x > p.x)
                            gesture.push_back(WEST);
                        else
                            gesture.push_back(EAST);
                    } else {
                        if (slope > 1 || slope < -1)
                            gesture.push_back(SOUTH);
                        else if (cur_point.x > p.x)
                            gesture.push_back(WEST);
                        else
                            gesture.push_back(EAST);
                    }
                }
                cur_point = p;
            }
        }
        count++;

        if (gesture.size() > 1) {
            int sz = gesture.size() - 1;
            if (gesture[sz] == gesture[sz - 1])
                gesture.pop_back();
        }
    }
    void printGesture() {
        if (gesture.size() == 0) {
            return;
        }
        printf("Gesture: ");
        for (int i = 0; i < gesture.size(); i++) {
            if (gesture[i] == NORTH)
                printf("↑ ");
            else if (gesture[i] == EAST)
                printf("→ ");
            else if (gesture[i] == SOUTH)
                printf("↓ ");
            else
                printf("← ");
        }
        printf("\n");
        if (gesture.size() > 2 && gesture.size() < 7)
            predictGesture();
    }
private:
    //N = 0, E = 1, S = 2, W = 3
    enum DIRECTION {NORTH, EAST, SOUTH, WEST};
    vector <DIRECTION> gesture;
    Point cur_point;
    int count;

    vector< pair<string, string> > g_map;

    int dp[16][16];
    int editDistance (string &str) {
        int len1 = gesture.size();
        int len2 = str.length();
        if (len1 == 0 || len2 == 0)
            return len1 + len2;
        for (int i = 0; i <= len1; i++)
            dp[i][0] = i;
        for (int i = 0; i <= len2; i++)
            dp[0][i] = i;
        for (int i = 0; i <= len1; i++) {
            for (int j = 0; j <= len2; j++) {
                dp[i + 1][j + 1] = dp[i][j] + ((int)gesture[i] + '0' == str[j] ? 0 : 1);
                dp[i + 1][j + 1] = min(dp[i + 1][j + 1], dp[i][j + 1] + 1);
                dp[i + 1][j + 1] = min(dp[i + 1][j + 1], dp[i + 1][j] + 1);
            }
        }
        return dp[len1][len2];
    }

    void predictGesture() {
        if (g_map.size() == 0 || gesture.size() == 0) {
            printf("None input gesture.\n");
            return;
        }
        int max_dis = 16, p = -1, tmp;
        for (int i = 0; i < g_map.size(); i++) {
            tmp = editDistance(g_map[i].first);
            if (tmp < max_dis) {
                p = i;
                max_dis = tmp;
            }
        }
        if (p != -1 && max_dis <= 1)
            cout << "predictGesture: " << g_map[p].second << endl;
    }
};


class Tracker {
public:
    Tracker() {
        capture = NULL;
        this->frame_of_null = 0;
        this->last_trace_distance = -1.0;
        this->last_center = Point(-1.0, -1.0);
        this->start_tracking = true;
        //this->bitmap = NULL;

        //Gauss BG Model init
/*        pFrImg = pFrame = pBkImg = NULL;
        bg_model = NULL;
        params.win_size = 500;
        params.bg_threshold = 0.7;
        params.weight_init =  0.05;
        params.variance_init = 30;
        params.minArea = 15.f;
        params.n_gauss = 5;
        params.std_threshold = 2.5;
        cout << "init succeed\n";
*/
        //set windows
        cvNamedWindow("trace", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("trace", 450, 0);
        cvNamedWindow("mask", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("mask", 450, 380);
#if YUV_TEST
        cvNamedWindow("YCbCr_mask", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("YCbCr_mask", 0, 380);
#endif
    }
    ~Tracker() {
        //if (bitmap != NULL)
        //    delete bitmap;
        //bitmap = NULL;
        if (capture != NULL)
            capture->release();
    }
    bool InitSkinModel() {
        this->skin_model = Mat::zeros(Size(256, 256), CV_8UC1);
        ellipse(this->skin_model, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
        //ellipse(this->skin_model, Point(113, 155.6), Size(11.7, 7.6), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
        return true;
    }
    bool StartCamera() {
        if (capture != NULL)
            return false;
        capture = new VideoCapture(VIDEO_DEVICE_NO);
        if (capture->isOpened())
            return true;
        return false;
    }
    bool StopCamera() {
        //if (bitmap != NULL)
        //    delete bitmap;
        //bitmap = NULL;
        if (capture != NULL) {
            capture->release();
            waitKey(1);
        }
        return true;
    }
    bool GetNextFrame() {
        if (!capture->read(this->src_img))
            return false;

//        if (src_img.rows * src_img.cols > 240 * 320)
//            resize(this->src_img, this->src_img, Size(320, 240));
            resize(this->src_img, this->src_img, Size(400, 320));
        //if (this->bitmap == NULL)
        //    this->bitmap = new bool[src_img.cols * src_img.rows];
        //memset(bitmap, false, sizeof(bitmap));

        flip(src_img, src_img, 1);
        src_img.convertTo(src_img, CV_32FC3);
        medianBlur(src_img, src_img, 5);
/*        cout << "convert\n";
        tmpImage  = IplImage(src_img);
        pFrame = &tmpImage;
        cout << "convert succeed\n";
*/        normalize(src_img, src_img, 1.0, 0.0, CV_MINMAX);
        return true;
    }

    void CleanTracking() {
        frame_of_null = 0;
        vpace.clear();
        analyser.clear();
        last_trace_distance = -1.0;
        this->last_center = Point(-1.0, -1.0);
    }
    void Display() {
        //imshow("source", this->src_img);
        if (this->start_tracking) {
            //imshow("mask", this->mask);
            imshow("trace", this->trace);
        }
    }

    bool ResetBackground() {
        if (GetNextFrame() == false)
            return false;
        //src_img.copyTo(background);
        cout << "reset background...";
/*        if (pFrame)
            delete pFrame;
        if (pBkImg)
            delete pBkImg;
        if (pFrImg)
            delete pFrImg;
        if (bg_model)
            delete bg_model;
        //pFrame = Iplimage(src_img);
        pBkImg = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 3);
        pFrImg = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 1);
        bg_model = (CvGaussBGModel *)cvCreateGaussianBGModel(pFrame, &params);
*/
        Mat yuv;
        cvtColor(src_img, yuv, CV_BGR2YCrCb);
        Mat mv[3];
        split(yuv, mv);
        foreground = mv[0];
        cout << "succeed\n";
        return true;
    }

    bool UpdateBackground(Mat &src) {
        /*cvSmooth(pFrame, pFrame, CV_GAUSSIAN, 3, 0, 0, 0);
        cvUpdateBGStatModel(pFrame, (CvBGStatModel *)bg_model, -0.00001);
        cvCopy(bg_model->foreground, pFrImg, 0);
        cvCopy(bg_model->background, pBkImg, 0);
        cvErode(pFrImg, pFrImg, 0, 1);
        cvDilate(pFrImg, pFrImg, 0, 3);
        cvShowImage("pFrImg", pFrImg);*/
        mog(src, foreground, 0.005);
        threshold(foreground, foreground, 128, 255, THRESH_BINARY);
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
        for (int k = 0; k < 3; k++)
            erode(foreground, foreground, element);
        for (int k = 0; k < 10; k++)
            dilate(foreground, foreground, element);
        return true;
    }

    void BFS(int x, int y, Mat &yuv) {
        //if (bitmap == NULL)
        //    return;
        int dx[4] = {-1, 0, 0, 1};
        int dy[4] = {0, -1, 1, 0};
        int tx, ty, nx, ny;
        queue< pair<int, int> > q;
        q.push(pair<int, int>(x, y));
        //bitmap[x * src_img.cols + y] = true;
        mask.at<uchar>(x, y) = 255;
        int total = 0;
#if DEBUG
        int cb_min, cb_max, cr_min, cr_max;
        cb_min = cr_min = 255;
        cb_max = cr_max = 0;
#endif
        int std_cb = yuv.at<Vec3b>(x, y)[1];
        int std_cr = yuv.at<Vec3b>(x, y)[2];
        while (!q.empty()) {
            tx = q.front().first;
            ty = q.front().second;
            q.pop();
            for (int i = 0; i < 4; i++) {
                nx = tx + dx[i];
                ny = ty + dy[i];
                //cout << "nx = " << nx << " ny = " << ny << endl;
                //cout << mask.rows << " " << mask.cols << endl;
                if (nx >= 0 && nx < src_img.rows && ny >= 0 && ny < src_img.cols
                        //&& bitmap[nx * src_img.cols + ny] == false) {
                        && mask.at<uchar>(nx, ny) == false) {
                    Vec3b ycrcb = yuv.at<Vec3b>(nx, ny);
#if DEBUG
                    cb_min = min(cb_min, (int)ycrcb[1]);
                    cb_max = max(cb_max, (int)ycrcb[1]);
                    cr_min = min(cr_min, (int)ycrcb[2]);
                    cr_max = max(cr_max, (int)ycrcb[2]);
#endif
                    if (skin_model.at<uchar>(ycrcb[1], ycrcb[2]) > 0 
                            && ycrcb[1] < std_cb + 20 && std_cb - 20 < ycrcb[1] 
                            && ycrcb[2] < std_cr + 20 && std_cr - 20 < ycrcb[2]) {
                        mask.at<uchar>(nx, ny) = 255;
                        q.push(pair<int, int>(nx, ny));
                        total++;
                    }
                    //bitmap[nx * src_img.cols + ny] = true;
                }
            }
        }
#if DEBUG
        if (total > 0)
            printf("cr_min = %d, cr_max = %d, cb_min = %d, cb_max = %d\n", cr_min, cr_max, cb_min, cb_max);
#endif
        //cout << "x = " << x << ", y = " << y << " " << "total = " << total << " ";
        //cout << "finish bfs\n";
    }
    void SkinExtract() {
        src_img.convertTo(src_img, CV_8UC3, 255);
        mask = Mat::zeros(src_img.size(), CV_8UC1);

        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
        erode(src_img, src_img, element);
        erode(src_img, src_img, element);
        dilate(src_img, src_img, element);
        Mat yuv;
        cvtColor(src_img, yuv, CV_BGR2YCrCb);

#if YUV_TEST
        {
            Mat mv[3];
            split(yuv, mv);
            //equalizeHist(mv[1], mv[1]);
            //equalizeHist(mv[2], mv[2]);
            //merge(mv, 3, yuv);
            //imshow("Y", mv[0]);
            //imshow("Cr", mv[1]);
            //imshow("Cb", mv[2]);
            UpdateBackground(mv[0]);
        }
        for (int i = 0; i < src_img.cols; ++i) {
            for (int j = 0; j < src_img.rows; ++j) {
                Vec3b ycrcb = yuv.at<Vec3b>(j, i);
                if (skin_model.at<uchar>(ycrcb[1], ycrcb[2]) > 0) {
                    mask.at<uchar>(j, i) = 255;
                }
            }
        }
        imshow("YCbCr_mask", mask);
        mask = Mat::zeros(src_img.size(), CV_8UC1);
#endif
        for (int i = 0; i < src_img.cols; ++i) {
            for (int j = 0; j < src_img.rows; ++j) {
                Vec3b ycrcb = yuv.at<Vec3b>(j, i);
                //if (bitmap[j * src_img.cols + i] == false && skin_model.at<uchar>(ycrcb[1], ycrcb[2]) > 0
                if (mask.at<uchar>(j, i) == 0 && skin_model.at<uchar>(ycrcb[1], ycrcb[2]) > 0
                        && (int)foreground.at<uchar>(j, i) == 255) {
                    //mask.at<uchar>(j, i) = 255;
                    //cout << (int)foreground.at<uchar>(j, i) << endl;
                    BFS(j, i, yuv);
                }
            }
        }

        //erode(mask, mask, element);
        //dilate(mask, mask, element);
        //bitwise_and(foreground, mask, mask);
        for (int k = 0; k < 3; k++)
            dilate(mask, mask, element);
        imshow("mask", mask);
        imshow("foreground", foreground);
        src_img.copyTo(src_img, mask);
    }

    void DrawTrace() {
        contours.clear();
        filter.clear();
        structure.clear();

        findContours(mask, contours, structure, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        //filter
        int max_area = 0, cur_area;
        Point cur_center, tmp_center;
        for (int i = 0; i < contours.size(); ++i) {
            convexHull(contours[i], contours[i]);

            //each points of the convex should not be located at the edge of frame
            bool flag = true;
            for (int j = 0; flag && j < contours[i].size(); j++) {
                if (contours[i][j].x <= 5 || contours[i][j].y <= 5)
                    flag = false;
            }
            if (!flag)
                continue;

            cur_area = fabs(contourArea(Mat(contours[i])));
            if (cur_area > AREA_LIMIT &&
                fabs(arcLength(Mat(contours[i]), true)) < ARC_LENGTH_LIMIT) {

                //find the most neighbor convex as hand
                if (!vpace.empty()) {
                    tmp_center = PolyCenter(contours[i]);
#if DEBUG
                    cout << "tmp center = " << tmp_center << endl << "cur center = " << cur_center << endl << "last center = " << last_center << endl;
#endif
                    if (i == 0 || SqrDis(cur_center, last_center) > SqrDis(tmp_center, last_center)) {
                        cur_center.x = tmp_center.x;
                        cur_center.y = tmp_center.y;
                        filter.clear();
                        filter.push_back(contours[i]);
                    }
                    continue;
                }

                //find the max convex as hand
                if (cur_area > max_area) {
                    filter.clear();
                    filter.push_back(contours[i]);
                    max_area = cur_area;
                }
            }
        }
        src_img.copyTo(trace);
        drawContours(trace, filter, -1, Scalar(255, 0, 0), 2);

        //draw convex center
        if (!filter.empty()) {
            float x = 0, y = 0;
/*            for (int i = 0; i < filter[0].size(); i++) {
                x += (float)filter[0][i].x;
                y += (float)filter[0][i].y;
#if DEBUG
                cout << "(" << filter[0][i].x << ", " << filter[0][i].y << ") ";
#endif
            }
#if DEBUG
            cout << endl;
#endif
            x /= filter[0].size();
            y /= filter[0].size();
*/
            cur_center = PolyCenter(filter[0]);
            last_center = cur_center;
            x = cur_center.x, y = cur_center.y;

            //find the longest finger
            float px, py, d = 500, td;
            for (int i = 0; i < filter[0].size(); i++) {
                //td = sqrt((filter[0][i].x - x) * (filter[0][i].x - x) + (filter[0][i].y - y) * (filter[0][i].y - y));
                td = filter[0][i].y;
                if (td < d) {
                    px = filter[0][i].x;
                    py = filter[0][i].y;
                    d = td;
                }
            }

            //judge whether (px, py) is the point we tracked in the last frame
            if (!vpace.empty()) {
                //td = (vpace.back().x - px) * (vpace.back().x - px) + (vpace.back().y - py) * (vpace.back().y - py);
                Point finger(px, py);
                td = SqrDis(vpace.back(), finger);
                if (td > TRACE_LENGTH_LIMIT_LOW * TRACE_LENGTH_LIMIT_LOW || (last_trace_distance > 64 && td > 16 * last_trace_distance)) {
                    frame_of_null++;
                    if (frame_of_null > PACE_THRESHOLD) {
                        CleanTracking();
                    } /*else {
                        for (int i = START_DRAW; i < vpace.size(); i++) {
                            line(trace, vpace[i - 1], vpace[i], Scalar(255, 255, 0), 2);
                        }
                    }*/
                    return;
                }
                last_trace_distance = td;
            }

            //draw the convex's center
            circle(trace, Point(x, y), 10, Scalar(0, 0, 255), 5);

            //draw the longest finger
            line(trace, Point(x, y), Point(px, py), Scalar(0, 255, 0), 2);

            //draw trace
            analyser.judge(Point(px, py));
            vpace.push_back(Point(px, py));
            frame_of_null = 0;
            Point cur, nxt;
            for (int i = START_DRAW; i < vpace.size() - 1; i++) {
                if (i == START_DRAW)
                    cur = vpace[i - 1];
                nxt.x = (cur.x + vpace[i].x + vpace[i + 1].x) / 3.0;
                nxt.y = (cur.y + vpace[i].y + vpace[i + 1].y) / 3.0;
                //line(trace, vpace[i - 1], vpace[i], Scalar(255, 255, 0), 2);
                line(trace, cur, nxt, Scalar(255, 255, 0), 2);
                cur = nxt;
            }
            analyser.printGesture();
        } else {
            frame_of_null++;
            if (frame_of_null > PACE_THRESHOLD) {
                CleanTracking();
            } else {
                for (int i = START_DRAW; i < vpace.size(); i++) {
                    line(trace, vpace[i - 1], vpace[i], Scalar(255, 255, 0), 2);
                }
            }
        }
    }

    void Run() {
        if (InitSkinModel() == false)
            return;
        if (StartCamera() == false)
            return;
        cout << "StartCamera succeed\n";
        if (ResetBackground() == false)
            return;
        double start, finish;
        while (GetNextFrame() == true) {
            start = clock();
            if (this->start_tracking == true) {
                SkinExtract();
                DrawTrace();
            }
            Display();
            finish = clock();
            cout << "run time: " << ((finish - start) * 1.0 / (double)CLOCKS_PER_SEC) << "s" << endl;
            char key = (char)waitKey(1);
            if (key == 'q' || key == 'Q' || key == 27)
                break;
            if (key == 'R' || key == 'r')
                this->start_tracking = !this->start_tracking;
            if (key == 'S' || key == 's') {
                imwrite("./src.png", src_img);
                imwrite("./foreground.png", foreground);
                imwrite("./mask.png", mask);
            }
        }
        StopCamera();
        return;
    }
private:
    VideoCapture *capture;
    Mat src_img;

    //ellipse-skin-model
    Mat skin_model;
    Mat mask;

    //trace of hand
    vector< vector<Point> > contours;
    vector< vector<Point> > filter;
    vector<Vec4i> structure;
    Mat trace;
    vector<Point> vpace;
    int frame_of_null;
    double last_trace_distance;
    Point last_center;
    Analyser analyser;

    //control flag
    bool start_tracking;

    //Gauss BG Model
    //IplImage *pFrame, *pBkImg, *pFrImg;
    //IplImage tmpImage;
    //CvGaussBGModel *bg_model;
    //CvGaussBGStatModelParams params;
    BackgroundSubtractorMOG2 mog;
    Mat foreground;
};


int main() {
    Tracker tracker;
    tracker.Run();
    return 0;
}
