#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


int main(void) {
    cv::VideoCapture cap(2);
    if (!cap.isOpened()) {
        std::cout << "couldn't open capture\n";
        return -1;
    }

    std::cout << "video capture opened\n";

    cv::Mat frame;
    while (true) {
        cap.read(frame);
        std::cout << "frame read\n";
        if (frame.empty()) {
            std::cerr << "ERROR no frame\n";
            break;
        }
        std::cout << "checked if frame is empty\n";
        cv::imshow("live", frame);
        std::cout << "imshow\n";
        if (cv::waitKey(5) >= 0) {
            break;
        }
        std::cout << "waited for key\n";
    }

    return 0;
}
