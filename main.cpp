#include <stdio.h>
#include <opencv/cv.hpp>

int main(int argc, char* argv[]) {
  cv::Mat image = cv::imread(argv[1], 1);
  cv::imshow("main", image);
  cv::waitKey(0);
  return 0;
}

