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

  int threshold = 160, distance = 0;
  cv::namedWindow("camera", cv::WINDOW_FULLSCREEN | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("red_threshold", "camera", &threshold, 255);
  cv::createTrackbar("distance", "camera", NULL, 10000);

  cv::Mat frame;
  while(true) {
    cv::setTrackbarPos("distance", "camera", ++distance);
    camera.read(frame);
    if (frame.empty()) {
      std::cerr << "[Fatal] cannot read from the camera" << std::endl;
      return 1;
    }

    cv::Mat undistorted_image;
    cv::undistort(frame, undistorted_image, camera_mat, dist_coeffs);
    cv::imshow("camera (original)", undistorted_image);

    cv::Mat bgr[3];
    cv::split(undistorted_image, bgr);

    cv::Mat mono;
    cv::threshold(bgr[2], mono, threshold, 255, cv::THRESH_BINARY);

    cv::imshow("camera", mono);

    std::vector<std::vector<cv::Point>> contours, contours_poly;
    cv::findContours(mono, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> bound_rect;
    for (int i=0; i < contours.size(); ++i) {
      cv::approxPolyDP(contours[i], contours_poly[i], 3, true);
      bound_rect.push_back(cv::boundingRect(contours_poly[i]));
    }

    for (auto rect : bound_rect) {
      std::cout << rect << std::endl;
    }

    if (cv::waitKey(1) != -1) {
      break;
    }
  }
  return 0;
}

