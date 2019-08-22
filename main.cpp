#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <vector>

#define PI 3.1415926535897
// void saveEdgeDetectedImage(cv::Mat pic, std::string filename="./edges.jpg") {
//   cv::imwrite(filename, pic);
// }
void getHoughLines(cv::Mat& pic, std::vector<cv::Vec4i> &lines, double rho=1,
        double theta=PI/180, int threshold=80, double maxLineLength=30,
        double maxLineGap=10)
{
  HoughLinesP(pic, lines, rho, theta, threshold, maxLineLength, maxLineGap);   
}

void getEdges(cv::Mat& src, cv::Mat& dst, int threshold=20, int ratio=3, int kernel_size=3) {
  cv::Mat detected_edges;
  cv::Mat gray;
  cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
  cv::blur(gray, detected_edges, cv::Size(3,3));
  cv::Canny(detected_edges, dst, threshold, threshold*ratio, kernel_size);
}

void drawLine(cv::Mat& src, cv::Vec4i line) {
  cv::Point p1(line[0], line[1]);
  cv::Point p2(line[2], line[3]);
  cv::line(src, p1, p2, cv::Scalar(0, 0, 255), 3);
}

int main( int argc, char** argv ) {
  cv::CommandLineParser parser( argc, argv, "{@input | ../data/fruits.jpg | input image}" );
  cv::Mat src = cv::imread( parser.get<cv::String>( "@input" ), cv::IMREAD_COLOR ); // Load an image
  if( src.empty() )
  {
    std::cout << "Could not open or find the image!\n" << std::endl;
    std::cout << "Usage: " << argv[0] << " <Input image>" << std::endl;
    return -1;
  }
  cv::Mat edges;
  getEdges(src, edges);

  std::vector<cv::Vec4i> lines;
  getHoughLines(edges, lines);
  cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR);
  
  // hello
  for (int i = 0; i < lines.size(); i++) {
    std::cout << lines[i] << std::endl;
    drawLine(edges, lines[i]);
  }

  cv::imwrite("./edges.jpg", edges);
  std::cout << "done" << "\n";
  return 0;
}
