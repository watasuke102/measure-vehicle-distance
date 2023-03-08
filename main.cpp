#include <iostream>
#include <opencv/cv.hpp>

int main(int argc, char* argv[]) {
  auto camera = cv::VideoCapture(0);
  if (!camera.isOpened()) {
    std::cerr << "[Fatal] cannot open the camera" << std::endl;
    return 1;
  }

  cv::Mat frame;
  while(true) {
    camera.read(frame);
    if (frame.empty()) {
      std::cerr << "[Fatal] cannot read from the camera" << std::endl;
      return 1;
    }
    cv::imshow("camera", frame);
    if (cv::waitKey(50) != -1) {
      break;
    }
  }
  return 0;
}

