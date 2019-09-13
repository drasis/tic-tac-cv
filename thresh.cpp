#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <vector>

template <typename T>
T max(T a, T b) {
	return a > b ? a : b;
}

template <typename T>
T min(T a,T b) {
	return a < b ? a : b;
}

template <typename T>
int findMaxIndex(std::vector<T>& v, bool (*cmp)(T a, T b)) {
    if (v.size() == 0) {
        return -1;
    }
    int currMaxIdx = 0;
    for (int i = 1; i < v.size(); i++) {
        if (cmp(v[i], v[currMaxIdx])) {
            currMaxIdx = i;
        }
    }
    return currMaxIdx;
}

template <typename T>
bool vectorSizeCmp(std::vector<T> a, std::vector<T> b) {
    return a.size() > b.size();
}

// default: windowSize=20, thresh=.2
void slidingGameOfLife(cv::Mat& src, cv::Mat& dst, int windowSize = 20, float thresh = .4) {
  int height = src.rows;
  int width = src.cols;
  cv::Mat new_dst = cv::Mat::zeros(src.size(), src.type());

  float to_left = 0.35;
  float to_right = 1 - to_left;
  for (int row = 0; row < height; row++) {
  	for (int col = 0; col < width; col++) { cv::Rect border = cv::Rect(
  			max(0, col - int(windowSize*to_left)),
  			max(0, row - int(windowSize*to_left)),
  			width - col < int(windowSize*to_right) ? width - col : int(to_right*windowSize),
  			height - row < int(windowSize*to_right) ? height - row : int(to_right*windowSize));
  		cv::Mat subImg = src(border);
  		double subImgSum = cv::sum(subImg)[0];
  		double subImgAvg = subImgSum / (border.width * border.height);
  		if (subImgAvg < thresh * 255) {
  			new_dst.at<uchar>(row, col) = 0;
  		} else {
  			new_dst.at<uchar>(row, col) = 255;
  		}
  	}
  }
  dst = new_dst;
}

void fillInCheese(cv::Mat& src, cv::Mat& dst) {
  cv::Mat floodFilled = src.clone();
  cv::floodFill(floodFilled, cv::Point(0,0), cv::Scalar(255));

  cv::Mat inv;
  cv::bitwise_not(floodFilled, inv);

  dst = (src | inv);
}

/*
 * int hUpper=137, int sUpper=162 int vUpper = 255,
 * int hLower = 96, int sLower = 14, int vLower = 118
 */

void cropToPaper(cv::Mat& src, cv::Mat& dst,
                 cv::Scalar lowThresh=cv::Scalar(96, 14, 118),
                 cv::Scalar highThresh=cv::Scalar(137, 162, 255),
                 bool dispContours=false) {
                 
  cv::Mat afterThresh;
  cv::Mat srcHsv;
  cv::Mat GOLoutput;
  cv::Mat notSwissCheese;
  std::vector<std::vector<cv::Point>> contourPoints;
  std::vector<cv::Vec4i> hierarchy;

  // turn src into hsv img 
  cv::cvtColor(src, srcHsv, cv::COLOR_BGR2HSV);

  // create a mask of paper colors defined by hUpper/hLower, etc.
  cv::inRange(srcHsv, lowThresh, highThresh, afterThresh);

  // perform novel computer vision algorithm to smooth paper mask
  slidingGameOfLife(afterThresh, GOLoutput);
  fillInCheese(GOLoutput, notSwissCheese);

  cv::findContours(notSwissCheese, contourPoints, hierarchy,
          cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); 

  // get largest contour
  int largestContourIndex = findMaxIndex(contourPoints, vectorSizeCmp);

  if (dispContours) {
    cv::Scalar color(0, 0, 255);
    cv::drawContours(src, contourPoints, largestContourIndex, color, 2, 8,
            hierarchy, 0, cv::Point());
  }

  // bounding rectangle around the largest contour which should be the paper
  cv::RotatedRect boundRect = cv::minAreaRect(contourPoints[largestContourIndex]);
  cv::Point2f vertices[4];
  boundRect.points(vertices);
  cv::Point2f dstPoints[4] = { cv::Point2f(0, src.rows), cv::Point2f(0,0),
      cv::Point2f(src.cols, 0), cv::Point2f(src.cols, src.rows) };

  // get homography between paper coordinates and screen coordinates
  cv::Mat homographyMatrix = cv::getPerspectiveTransform(vertices, dstPoints);
 
  // put the warped image in dst
  cv::warpPerspective(src, dst, homographyMatrix, src.size());
}

int main(int argc, char** argv) {
  cv::CommandLineParser parser( argc, argv, "{@input | ../data/fruits.jpg | input image}" );
  cv::Mat src = cv::imread( parser.get<cv::String>( "@input" ), cv::IMREAD_COLOR ); // Load an image
  if( src.empty() )
  {
    std::cout << "Could not open or find the image!\n" << std::endl;
    std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
    return -1;
  }

  cv::Mat paperImg;
  cropToPaper(src, paperImg);
  cv::imshow("contours", paperImg);
  cv::waitKey(0);

  return 0;
}
