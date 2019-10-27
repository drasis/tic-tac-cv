#ifndef __THRESH_H__
#define __THRESH_H__

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <vector>
#include "board.h"

typedef enum _BoxState {
    BOX_EMPTY = 0,
    BOX_X,
    BOX_O
} BoxState;

// assumes that both are pictures of the paper

/**
 * @brief      Given a baseline image and the current frame, determines if there is a blob large enough
 *    		   to be considered a hand in the frame. Assumes that baseline and current are images of the
 *    		   same scene.
 *
 * @param[in]      baseline  The baseline image
 * @param[in]      current   The current frame
 * @param[in]      thresh    The threshold to determine a difference in the pixel values between current and baseline.
 *
 * @return     Whether there is a hand in the frame
 */
bool handInFrame(cv::Mat& baseline, cv::Mat& current, float thresh=90.0);

/**
 * @brief      Given a frame, the bounding box of the board in the frame, and the current board state, check each empty
 *             box in the board and determines if there is now an 'O' in it.
 *
 * @param[in]      frame        The frame
 * @param[in]      boardBounds  The bounding rectangle of the board in the frame
 * @param[in,out]  board        The board state
 *
 * @return     True if it detected an O and updated the board, False otherwise
 */
bool checkForO(cv::Mat& frame, const cv::Rect& boardBounds, BoxState board[9]);

/**
 * @brief      Given an image and high and low HSV thresholds for the color of the paper, find the homography that transforms
 *             the corners of the paper to the corners of the image.
 * 
 *
 * @param[in]   frame         The frame
 * @param[out]  homography    The output homography
 * @param[in]   lowThresh     The low HSV threshold for the color of the paper
 * @param[in]   highThresh    The high HSV threshold for the color of the paper
 * @param[in]   dispContours  Debug flag to display the contours on an image.
 */
void findHomography(cv::Mat& frame, cv::Mat& homography,
        cv::Scalar lowThresh=cv::Scalar(96, 14, 118), 
        cv::Scalar highThresh=cv::Scalar(137, 162, 255),
        bool dispContours=false);

/**
 * @brief      Finds the bounding rectangle of the tic-tac-toe board in the scene.
 *
 * @param[in]       cap               The webcam capture stream
 * @param[in]       homographyMatrix  The homography matrix to transform from paper->image coords
 * @param[out]      boardBounds       The board bounds
 *
 * @return     Whether the bounding rectangle was found
 */
bool findBoardBounds(cv::VideoCapture& cap, cv::Mat& homographyMatrix, cv::Rect& boardBounds); 
#endif
