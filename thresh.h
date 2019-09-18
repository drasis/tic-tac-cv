#ifndef __THRESH_H__
#define __THRESH_H__

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <vector>

typedef enum _BoxState {
    BOX_EMPTY = 0,
    BOX_X,
    BOX_O
} BoxState;

// assumes that both are pictures of the paper
bool handInFrame(cv::Mat& baseline, cv::Mat& current);
bool checkForO(cv::Mat& frame, cv::Rect& boardBounds, BoxState board[9]);
void findHomography(cv::Mat& frame, cv::Mat& homography,
        cv::Scalar lowThresh=cv::Scalar(96, 14, 118), 
        cv::Scalar highThresh=cv::Scalar(137, 162, 255),
        bool dispContours=false);
bool findBoardBounds(cv::VideoCapture& cap, cv::Mat& homographyMatrix, cv::Rect& boardBounds); 
#endif
