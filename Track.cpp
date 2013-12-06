#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
using namespace std;
using namespace cv;

#define VIDEO_DEVICE_NO 1

class Tracker {
public:
    Tracker() {
        capture = NULL;
    }
    ~Tracker() {
        if (capture != NULL)
            capture->release();
    }
    bool InitSkinModel() {
        this->skin_model = Mat::zeros(Size(256, 256), CV_8UC1);
        ellipse(this->skin_model, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
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
        if (capture != NULL) {
            capture->release();
            waitKey(1);
        }
        return true;
    }
    bool GetNextFrame() {
        if (!capture->read(this->src_img))
            return false;

        src_img.convertTo(src_img, CV_32FC3);
        normalize(src_img, src_img, 1.0, 0.0, CV_MINMAX);
        return true;
    }
    void Display() {
        imshow("source", this->src_img);
    }
    void Run() {
        if (InitSkinModel() == false)
            return;
        if (StartCamera() == false)
            return;
        while (GetNextFrame() == true) {
            Display();
            char key = (char)waitKey(1);
            if (key == 'q' || key == 'Q' || key == 27)
                break;
        }
        StopCamera();
        return;
    }
private:
    VideoCapture *capture;
    Mat src_img;

    //ellipse-skin-model
    Mat skin_model, mask;
};




int main() {
    Tracker tracker;
    tracker.Run();
    return 0;
}
