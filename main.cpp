#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

// void saveEdgeDetectedImage(cv::Mat pic, std::string filename="./edges.jpg") {
//   cv::imwrite(filename, pic);
// }

cv::Mat getEdges(cv::Mat pic, int threshold=20, int ratio=3, int kernel_size=3) {
  cv::Mat detected_edges;
  cv::Mat gray;
  cv::cvtColor(pic, gray, cv::COLOR_BGR2GRAY);
  cv::blur(gray, detected_edges, cv::Size(3,3));
  cv::Canny(detected_edges, detected_edges, threshold, threshold*ratio, kernel_size);
  return detected_edges;
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
  cv::Mat edges = getEdges(src);
  cv::imwrite("./edges.jpg", edges);
  std::cout << "done" << "\n";
  return 0;
}