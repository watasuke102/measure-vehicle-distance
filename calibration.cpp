#include <iostream>
#include <vector>
#include <opencv/cv.hpp>

constexpr int BOARD_ROW    = 6;
constexpr int BOARD_COLUMN = 9;
constexpr float BOARD_SIZE   = 25;

int main() {
  const cv::Size board_grid(9, 6);
  std::vector<std::vector<cv::Point2f>> img_points;
  
  {
    std::vector<cv::String> images;
    cv::glob("data/*", images);
    for (auto image_path : images) {
      const cv::Mat image = cv::imread(image_path);
      cv::Mat gray;
      cv::threshold(image, gray, 190, 255, cv::THRESH_BINARY);
      cv::imshow("gray_" + image_path, gray);

      std::vector<cv::Point2f> point_buffer;
      if (!cv::findChessboardCorners(gray, board_grid, point_buffer,
          CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS)) {
        std::cerr << "[ERROR] Failed to find corners from '" << image_path << "'" << std::endl;
        continue;
      }
      img_points.push_back(point_buffer);
      cv::drawChessboardCorners(image, board_grid, cv::Mat(point_buffer), true);
      cv::imshow(image_path, image);
    }
  }

  std::vector<std::vector<cv::Point3f>> obj_points(1);
  for (int i = 0; i < BOARD_ROW;    ++i) {
  for (int j = 0; j < BOARD_COLUMN; ++j) {
      obj_points[0].push_back(cv::Point3f{i * BOARD_SIZE, j * BOARD_SIZE, 0});
    }
  }
  obj_points.resize(img_points.size(), obj_points[0]);
  
  cv::Mat camera_mat, dist_coeffs;
  std::vector<cv::Mat> rvecs, tvecs;
  cv::calibrateCamera(obj_points, img_points, cv::Size(640, 480), camera_mat, dist_coeffs, rvecs, tvecs);

  cv::FileStorage fs("camera.yml", cv::FileStorage::WRITE);
  fs << "camera_mat" << camera_mat;
  fs << "dist_coeffs" << dist_coeffs;
  fs.release();

  cv::waitKey(0);
  return 0;
}

