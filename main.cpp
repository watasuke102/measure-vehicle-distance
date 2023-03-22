#include <iostream>
#include <opencv/cv.hpp>
#include <vector>

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

  int threshold = 160, distance = 0;
  cv::namedWindow("source", cv::WINDOW_FULLSCREEN | cv::WINDOW_KEEPRATIO |
                                cv::WINDOW_GUI_EXPANDED);
  cv::createTrackbar("red_threshold", "source", &threshold, 255);
  cv::createTrackbar("distance", "source", NULL, 10000);

  int up_b = 255, up_g = 250, up_r = 255;
  int lo_b = 0, lo_g = 230, lo_r = 200;
  cv::createTrackbar("lo_b", "source", &lo_b, 255);
  cv::createTrackbar("lo_g", "source", &lo_g, 255);
  cv::createTrackbar("lo_r", "source", &lo_r, 255);
  cv::createTrackbar("up_b", "source", &up_b, 255);
  cv::createTrackbar("up_g", "source", &up_g, 255);
  cv::createTrackbar("up_r", "source", &up_r, 255);

  cv::Mat frame;
  while (true) {
    cv::setTrackbarPos("distance", "source", ++distance);
    source.read(frame);
    if (frame.empty()) {
      std::cerr << "[Fatal] cannot read from the source" << std::endl;
      return 1;
    }

    cv::Mat undistorted_image;
    cv::undistort(frame, undistorted_image, camera_mat, dist_coeffs);
    cv::imshow("source (original)", undistorted_image);

    // increase contrast
    cv::Mat contrast;
    undistorted_image.convertTo(contrast, -1, 1.1, 30.0);

    cv::Mat bilateral;
    cv::bilateralFilter(contrast, bilateral, 10, 20, 5);

    cv::Mat masked;
    cv::inRange(bilateral, cv::Scalar(lo_b, lo_g, lo_r),
                cv::Scalar(up_b, up_g, up_r), masked);
    cv::imshow("source", masked);

    // std::vector<std::vector<cv::Point>> contours;
    // cv::findContours(masked, contours, cv::RETR_EXTERNAL,
    //                  cv::CHAIN_APPROX_SIMPLE);
    // std::vector<cv::Rect> bound_rect;
    // std::vector<std::vector<cv::Point>> contours_poly(contours.size());
    // for (int i = 0; i < contours.size(); ++i) {
    //   cv::approxPolyDP(contours[i], contours_poly[i], 3, true);
    //   bound_rect.push_back(cv::boundingRect(contours_poly[i]));
    // }

    // cv::Mat result = masked.clone();
    // cv::drawContours(result, contours, -1, cv::Scalar(0, 255, 0), 4);
    // cv::imshow("result", result);

    if (cv::waitKey(1) != -1) {
      break;
    }
  }
  return 0;
}
