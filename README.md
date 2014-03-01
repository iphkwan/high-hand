high-hand
=========

Hand tracker and gesture learner based on OpenCV.

> Rapid coding storm

## Requirement
* A cpp/python demo of hand tracker.
```
Step.0. Learn OpenCV. (FINISHED)
Step.1. Read image from video device. (FINISHED)
Step.2. Record the background and set stdDEV threshold. (OPTIONAL)
Step.3. Calculate the diff between current image and the recorded background. (OPTIONAL)
Step.4.0 Extract the skin using YUV ellispe-skin-model. (FINISHED)
Step.4.1 Remove the face using facedectect model of OpenCV. (OPTIONAL)
Step.5. Draw the hand's contour and display it on the screen (FINISHED)
Step.6. Track the center of hand's contour. (FINISHED)
```

* Experiment of gesture analyse and recognition based on [Deep Learning](http://deeplearning.stanford.edu/wiki/index.php/UFLDL%E6%95%99%E7%A8%8B)
```
0. Study the field of picture recogniton, OCR, SVM, etc.
1. Features extraction
2. Training and testing samples generation
3. Machine learning algorithm experiment
4. Result analyse
```

## Issue
* The camera doesn't perform well. So it needs some Filtering (corrosion and expansion) or Noise Reduction algorithms.
* The pace is spike, so it needs to be smooth.

## Demo
* The convex hull of hand
![convex hull demo](https://github.com/iphkwan/high-hand/blob/master/img/demo_0.png?raw=true)

* Gesture recognition
![gesture demo](https://github.com/iphkwan/high-hand/blob/master/img/demo_3.png?raw=true)

## Reference
[OpenCV API Reference](http://docs.opencv.org/modules/refman.html)
