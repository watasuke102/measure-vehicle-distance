#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <iostream>
#include <limits>
#include <opencv4/opencv2/opencv.hpp>
#include <vector>

// car
int c_lo_b = 0, c_lo_g = 0, c_lo_r = 150;
int c_up_b = 40, c_up_g = 30, c_up_r = 250;
int s = 30, v = 12;
cv::Rect extract_car(cv::Mat origin, cv::Mat dst) {
  cv::Mat hsv;
  cv::cvtColor(origin, hsv, cv::COLOR_BGR2HSV_FULL);
  // cv::imshow("origin", origin);

  std::array<cv::Mat, 3> channels;
  cv::split(hsv, channels);
  channels[1] *= s / 10.;
  channels[2] *= v / 10.;
  cv::Mat merged;
  cv::merge(channels, merged);
  cv::Mat bgr;
  cv::cvtColor(merged, bgr, cv::COLOR_HSV2BGR_FULL);
  // cv::imshow("bgr", bgr);

  cv::Mat extracted;
  cv::inRange(bgr, cv::Scalar(c_lo_b, c_lo_g, c_lo_r),
              cv::Scalar(c_up_b, c_up_g, c_up_r), extracted);
  cv::imshow("extracted", extracted);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(extracted, contours, cv::RETR_EXTERNAL,
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

int l_lo_b = 245, l_lo_g = 245, l_lo_r = 245;
int l_up_b = 255, l_up_g = 255, l_up_r = 255;
int alpha = 80, beta = 550, th = 174;
cv::Mat extract_led(cv::Mat origin, cv::Rect rect) {
  cv::Mat mask = cv::Mat::zeros(origin.size(), CV_8UC1);
  cv::rectangle(mask, rect, cv::Scalar(255), cv::FILLED);
  cv::Mat car;
  origin.copyTo(car, mask);
  // cv::imshow("masked-car", car);

  cv::Mat contrast;
  car.convertTo(contrast, -1, alpha / 100., beta / 10.);

  cv::Mat extracted;
  cv::inRange(contrast, cv::Scalar(l_lo_b, l_lo_g, l_lo_r),
              cv::Scalar(l_up_b, l_up_g, l_up_r), extracted);

  return extracted.clone();
}

int calc_distance(cv::Mat origin, cv::Mat dst) {
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(origin, contours, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);

  std::vector<cv::Point> points;
  for (int i = 0; i < contours.size(); ++i) {
    const cv::Rect rect = cv::boundingRect(contours[i]);
    const cv::Size size = rect.size();
    if (size.width < 3 || size.height < 3) {
      continue;
    }
    std::printf("     %d, %d\n", size.width, size.height);
    cv::rectangle(dst, rect, cv::Scalar(255, 255, 0), 2);
    points.push_back(
        cv::Point(rect.tl().x + size.width / 2, rect.tl().y + size.height / 2));
  }
  if (points.size() != 2) {
    return -1;
  }

  const cv::Point diff = points[0] - points[1];
  if (diff.y > 12) {
    return -1;
  }
  const int distance = (diff.x * diff.x) + (diff.y * diff.y);
  std::printf("(%d, %d) - (%d, %d) => (%d, %d) : %d\n",  //
              points[0].x, points[0].y,                  //
              points[1].x, points[1].y,                  //
              diff.x, diff.y, distance);
  return distance;
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
  // cv::VideoCapture source("output.avi");
  cv::VideoCapture source("video.avi");
  if (!source.isOpened()) {
    std::cerr << "[Fatal] cannot open the source" << std::endl;
    return 1;
  }

  cv::namedWindow("extracted", cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("c_lo_b", "extracted", &c_lo_b, 255);
  cv::createTrackbar("c_lo_g", "extracted", &c_lo_g, 255);
  cv::createTrackbar("c_lo_r", "extracted", &c_lo_r, 255);
  cv::createTrackbar("c_up_b", "extracted", &c_up_b, 255);
  cv::createTrackbar("c_up_g", "extracted", &c_up_g, 255);
  cv::createTrackbar("c_up_r", "extracted", &c_up_r, 255);

  cv::namedWindow("car", cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("s", "car", &s, 55);
  cv::createTrackbar("v", "car", &v, 55);

  cv::namedWindow("led", cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("l_lo_b", "led", &l_lo_b, 255);
  cv::createTrackbar("l_lo_g", "led", &l_lo_g, 255);
  cv::createTrackbar("l_lo_r", "led", &l_lo_r, 255);
  cv::createTrackbar("l_up_b", "led", &l_up_b, 255);
  cv::createTrackbar("l_up_g", "led", &l_up_g, 255);
  cv::createTrackbar("l_up_r", "led", &l_up_r, 255);
  cv::createTrackbar("alpha", "led", &alpha, 2000);
  cv::createTrackbar("beta", "led", &beta, 1200);
  cv::createTrackbar("th", "led", &th, 255);

  cv::namedWindow("result", cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("distance", "result", NULL, 10000);

  int stopping = false;
  cv::Mat frame;
  while (true) {
    if (!stopping) {
      source.read(frame);
    }
    if (frame.empty()) {
      std::cerr << "[Fatal] cannot read from the source" << std::endl;
      return 1;
    }

    cv::Mat undistorted_image;
    cv::undistort(frame, undistorted_image, camera_mat, dist_coeffs);

    cv::Mat car = frame.clone();
    const cv::Rect rect = extract_car(undistorted_image, car);
    cv::imshow("car", car);

    cv::Mat led = extract_led(frame, rect);
    cv::imshow("led", led);

    cv::Mat result = frame.clone();
    cv::setTrackbarPos("distance", "result", calc_distance(led, result));
    cv::imshow("result", result);

    const int key = cv::waitKey(100 * !stopping);
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
