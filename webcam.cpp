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
    int save_count = 0;

    cv::Mat frame;
    while (true) {
        cap.read(frame);
        if (frame.empty()) {
            std::cerr << "ERROR no frame\n";
            break;
        }
        cv::imshow("live", frame);
        if (cv::waitKey(5) >= 0) {
            std::string img_name = "image" + std::to_string(save_count++) + ".jpg";
            std::cout << "Saving frame to " << img_name << std::endl; 
            cv::imwrite(img_name, frame);
        }
    }

    return 0;
}
