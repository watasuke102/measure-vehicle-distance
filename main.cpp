#include <iostream>
#include <vector>
#include <opencv/cv.hpp>

int main() {
  auto camera = cv::VideoCapture(0);
  if (!camera.isOpened()) {
    std::cerr << "[Fatal] cannot open the camera" << std::endl;
    return 1;
  }

  std::vector<std::vector<cv::Point3f>> obj_points;
  std::vector<std::vector<cv::Point2f>> img_points;

  cv::Mat frame;
  while(true) {
    camera.read(frame);
    if (frame.empty()) {
      std::cerr << "[Fatal] cannot read from the camera" << std::endl;
      return 1;
    }
    cv::imshow("camera", frame);
    if (cv::waitKey(10) != -1) {
      break;
    }
  }
  return 0;
}

