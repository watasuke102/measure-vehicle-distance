#include <iostream>
#include <vector>
#include <opencv/cv.hpp>

int main() {
  cv::Mat camera_mat, dist_coeffs;
  {
    cv::FileStorage fs("camera.yml", cv::FileStorage::READ);
    fs["camera_mat"] >> camera_mat;
    fs["dist_coeffs"] >> dist_coeffs;
    fs.release();
  }

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

    cv::Mat undistorted_image;
    cv::undistort(frame, undistorted_image, camera_mat, dist_coeffs);
    cv::imshow("camera", undistorted_image);

    if (cv::waitKey(1) != -1) {
      break;
    }
  }
  return 0;
}

