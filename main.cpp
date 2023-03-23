#include <algorithm>
#include <ctime>
#include <iostream>
#include <limits>
#include <opencv/cv.hpp>
#include <vector>

int lo_b = 100, lo_g = 90, lo_r = 160;
int up_b = 130, up_g = 140, up_r = 200;

cv::Rect extract_car(cv::Mat origin, cv::Mat dst) {
  cv::Mat masked;
  cv::inRange(origin, cv::Scalar(lo_b, lo_g, lo_r),
              cv::Scalar(up_b, up_g, up_r), masked);
  cv::imshow("source", masked);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(masked, contours, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);

  // cv::drawContours(dst, contours, -1, cv::Scalar(0, 255, 0), 4);
  int left = std::numeric_limits<int>::max(), right = -1;
  int top = std::numeric_limits<int>::max(), bottom = -1;
  for (int i = 0; i < contours.size(); ++i) {
    const cv::Rect rect = cv::boundingRect(contours[i]);
    const cv::Size size = rect.size();
    if (size.width < 12 || size.height < 12) {
      continue;
    }
    cv::rectangle(dst, rect, cv::Scalar(255, 0, 0), 1);
    const cv::Point tl = rect.tl();
    const cv::Point br = rect.br();
    left = std::min(tl.x, left);
    right = std::max(br.x, right);
    top = std::min(tl.y, top);
    bottom = std::max(br.y, bottom);
  }
  const cv::Rect rect(cv::Point(left, top), cv::Point(right, bottom));
  if (right > 0) {
    cv::rectangle(dst, rect, cv::Scalar(0, 255, 0), 2);
  }

  return rect;
}

int main() {
  cv::Mat camera_mat, dist_coeffs;
  {
    cv::FileStorage fs("camera.yml", cv::FileStorage::READ);
    fs["camera_mat"] >> camera_mat;
    fs["dist_coeffs"] >> dist_coeffs;
    fs.release();
  }

  // cv::VideoCapture source(0);
  cv::VideoCapture source("video.avi");
  if (!source.isOpened()) {
    std::cerr << "[Fatal] cannot open the source" << std::endl;
    return 1;
  }

  int distance = 0;
  cv::namedWindow("source", cv::WINDOW_FULLSCREEN | cv::WINDOW_KEEPRATIO |
                                cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("distance", "source", NULL, 10000);

  cv::createTrackbar("lo_b", "source", &lo_b, 255);
  cv::createTrackbar("lo_g", "source", &lo_g, 255);
  cv::createTrackbar("lo_r", "source", &lo_r, 255);
  cv::createTrackbar("up_b", "source", &up_b, 255);
  cv::createTrackbar("up_g", "source", &up_g, 255);
  cv::createTrackbar("up_r", "source", &up_r, 255);

  int stopping = false;
  cv::Mat frame;
  while (true) {
    cv::setTrackbarPos("distance", "source", ++distance);
    if (!stopping) {
      source.read(frame);
    }
    if (frame.empty()) {
      std::cerr << "[Fatal] cannot read from the source" << std::endl;
      return 1;
    }

    cv::Mat undistorted_image;
    cv::undistort(frame, undistorted_image, camera_mat, dist_coeffs);

    cv::Mat result = frame.clone();
    const cv::Rect rect = extract_car(undistorted_image, result);
    cv::imshow("result", result);

    const int key = cv::waitKey(1);
    if (key == 'w') {
      std::time_t now = std::time(nullptr);
      char time_str[24];
      std::strftime(time_str, sizeof(time_str), "%m%d_%H%M%S",
                    std::localtime(&now));
      std::string filename = time_str + std::string(".jpg");
      cv::imwrite(filename, frame);
    }
    if (key == 32) {
      stopping = !stopping;
    }
    if (key == 27) {
      break;
    }
  }
  return 0;
}
