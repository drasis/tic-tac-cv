#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>

int max(int a, int b) {
	return a > b ? a : b;
}

int min(int a, int b) {
	return a < b ? a : b;
}

void slidingGameOfLife(cv::Mat& src, cv::Mat& dst, int windowSize = 20, float thresh = .2) {
  int height = src.rows;
  int width = src.cols;
  cv::Mat new_dst = cv::Mat::zeros(src.size(), src.type());

  for (int row = 0; row < height; row++) {
  	for (int col = 0; col < width; col++) {
  		cv::Rect border = cv::Rect(
  			max(0, col - windowSize/2),
  			max(0, row - windowSize/2),
  			width - col < windowSize/2 ? width - col : windowSize/2,
  			height - row < windowSize/2 ? height - row : windowSize/2
  			);
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

int main(int argc, char** argv) {
  cv::CommandLineParser parser( argc, argv, "{@input | ../data/fruits.jpg | input image}" );
  cv::Mat src = cv::imread( parser.get<cv::String>( "@input" ), cv::IMREAD_COLOR ); // Load an image
  cv::Mat afterThresh;
  cv::Mat srcHsv;
  cv::Mat GOLoutput;
  std::vector<std::vector<cv::Point>> contourPoints;
  std::vector<cv::Vec4i> hierarchy;
  if( src.empty() )
  {
    std::cout << "Could not open or find the image!\n" << std::endl;
    std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
    return -1;
  }
  cv::cvtColor(src, srcHsv, cv::COLOR_BGR2HSV);
  // colors of paper
  int hUpper = 137;
  int sUpper = 162;
  int vUpper = 255;
  int hLower = 96;
  int sLower = 14;
  int vLower = 118;

  // threshold the Hsv image
  cv::inRange(srcHsv, cv::Scalar(hLower, sLower, vLower), 
  	cv::Scalar(hUpper, sUpper, vUpper), afterThresh);
  cv::imshow("thresholded", afterThresh);
  cv::waitKey(0);

  // get contour
  // cv::findContours(afterThresh, contourPoints, hierarchy,
  //         cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); 

  // for (int i = 0; i < contourPoints.size(); i++) {
  //   cv::Scalar color(0, 0, 255);
  //   cv::drawContours(src, contourPoints, i, color, 2, 8, hierarchy, 0, cv::Point());
  // }

  //play 20 dimensional game of life
  for (int i = 0; i < 3; i++) {
	  slidingGameOfLife(afterThresh, afterThresh);
	  cv::imshow("gol" + std::to_string(i), afterThresh);
	  cv::waitKey(0);
  }

  // cv::imshow("life", GOLoutput);
  cv::waitKey(0);
  return 0;
}
