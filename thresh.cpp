#include "thresh.h"

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

void findHomography(cv::Mat& src, cv::Mat& homographyMatrix,
                 cv::Scalar lowThresh, cv::Scalar highThresh,
                 bool dispContours) {
                 
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
  if (largestContourIndex < 0) {
    // something went wrong
    homographyMatrix = cv::Mat::eye(3, 3, CV_32F);
    return;
  } 

  std::cout << "got largest contour\n";
  if (dispContours) {
    cv::Scalar color(0, 0, 255);
    cv::drawContours(src, contourPoints, largestContourIndex, color, 2, 8,
            hierarchy, 0, cv::Point());
  }
  std::cout << "got drawn\n";
  std::cout << "largest idx: " << largestContourIndex << ", size: " << contourPoints.size() << std::endl;
  // bounding rectangle around the largest contour which should be the paper
  cv::RotatedRect boundRect = cv::minAreaRect(contourPoints[largestContourIndex]);
  std::cout << "bound rect\n";
  cv::Point2f vertices[4];
  boundRect.points(vertices);
  cv::Point2f dstPoints[4] = { cv::Point2f(0, src.rows), cv::Point2f(0,0),
      cv::Point2f(src.cols, 0), cv::Point2f(src.cols, src.rows) };

  std::cout << "getting homography\n";
  // get homography between paper coordinates and screen coordinates
  homographyMatrix = cv::getPerspectiveTransform(vertices, dstPoints);
 
}

void getEdges(cv::Mat& src, cv::Mat& dst, int threshold=25, int ratio=3, int kernel_size=3) {
  cv::Mat blurred;
  cv::Mat gray;
  cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY); 
  cv::blur(gray, blurred, cv::Size(3,3)); 
  cv::Canny(blurred, dst, threshold, threshold*ratio, kernel_size); 
}


void findRectcoords(cv::Mat& paperImg, std::vector<cv::RotatedRect>& boardCoords) {

}

void drawRectCoords(cv::Mat& paperImg, cv::Mat& boardOverlay, 
	std::vector<cv::RotatedRect>& boardCoords) {

}

void getRidOfPerimeter(cv::Mat& src, cv::Mat& dst) {
  cv::Mat floodFilled = src.clone();
  cv::floodFill(floodFilled, cv::Point(0,0), cv::Scalar(125));

  // cv::Mat inv;
  // cv::bitwise_not(floodFilled, dst);

  dst = floodFilled;//(src | inv);
}

bool findBoardBounds(cv::VideoCapture& cap, cv::Mat& homographyMatrix, cv::Rect& boardBounds) {
  std::vector<cv::Mat> toOrEventually;
  cv::Mat paperImg;
  cv::Mat boardOverlay;
  cv::Mat src;
  for(int i = 0; i < 50; ++i)  {
    cap.read(src);
    if (src.empty()) {
        std::cerr << "ERROR no frame\n";
        break;
    }

    cv::warpPerspective(src, paperImg, homographyMatrix, src.size());
    getEdges(paperImg, boardOverlay, 10);

    slidingGameOfLife(boardOverlay,boardOverlay, 10, .1);
    slidingGameOfLife(boardOverlay,boardOverlay, 4, .9);
    fillInCheese(boardOverlay, boardOverlay);
    
    if (toOrEventually.size() < 10) {
      toOrEventually.push_back(boardOverlay);
      continue;
    }
    std::cout << "here\n";

    cv::Mat s = toOrEventually[0];
    //boardOverlay = toOrEventually[0];
    for (int i = 1; i < toOrEventually.size(); i++) {
      cv::bitwise_or(toOrEventually[i], s, s);
    }
    slidingGameOfLife(s,s, 10, .3);

    std::vector<std::vector<cv::Point>> contourPoints;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(s, contourPoints, hierarchy,
          cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    std::cout << "contour points size: " << contourPoints.size() << std::endl;
    for (int i = 0; i < contourPoints.size(); ++i) {
      if (contourPoints[i].size() > 300) {
        cv::Rect boundingRect = cv::boundingRect(contourPoints[i]);
        std::cout << "contour > 300\n";
        if (boundingRect.y > 50 || boundingRect.y + boundingRect.height < paperImg.rows - 50
           || boundingRect.x < paperImg.cols - 100 || boundingRect.x > 50) {
          boardBounds = boundingRect;
          return true; //we actually found the bounding rect
        }
      }
    }
    toOrEventually.clear();
  }
  return false; // :(
}

void drawBoundingBoardRect(const cv::Mat& src, cv::Mat& dst, const cv::Rect& boundingRect) {
	cv::Scalar color(0, 0, 255);
    dst = src.clone();
    int boxWidth = boundingRect.width / 3;
    int boxHeight = boundingRect.height / 3;
    cv::Rect currRect; 
    currRect.width = boxWidth;
    currRect.height = boxHeight;
    for (int j = 0; j < 3; j++) {
        currRect.x = boundingRect.x + (j * boxWidth);
        for (int k = 0; k < 3; k++) {
          currRect.y = boundingRect.y + (k * boxHeight);
          cv::rectangle(dst, currRect, color);
        }
    }
}

int main(int argc, char** argv) {
  cv::VideoCapture cap(2);
  cv::Mat src;
  if (!cap.isOpened()) {
      std::cout << "couldn't open capture\n";
      return -1;
  }
  
  std::cout << "Opened\n";
  cap.read(src);
  std::cout << "Got first\n";
  cv::Mat homographyMatrix;
  std::vector<cv::Mat> toOrEventually;
  cv::Mat paperImg;
  cv::Mat boardOverlay;
  std::vector<cv::RotatedRect> boardCoords;

  std::cout << "getting cropped mat\n";
  findHomography(src, homographyMatrix);
  std::cout << "done\n";
  std::cout << "homography matrix: " << homographyMatrix << std::endl;


  cv::Rect boundingRect;
  while (!findBoardBounds(cap, homographyMatrix, boundingRect)) {
      std::cout << "couldn't find board\n";
  }

  while (true) {
    cap.read(src);
    if (src.empty()) {
        std::cerr << "ERROR no frame\n";
        break;
    }

    cv::warpPerspective(src, paperImg, homographyMatrix, src.size());

    drawBoundingBoardRect(paperImg, paperImg, boundingRect);
    
	cv::imshow("contours", paperImg);
	cv::waitKey(1);
	toOrEventually.clear();
  }

  return 0;
}
