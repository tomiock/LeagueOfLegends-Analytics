#include "detect_champ.hpp"
#include "circle_priority.hpp"
#include "remove_terrain.hpp"

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const std::vector<std::string> BLUE = {"blitcrank", "samira", "veigar", "diana",
                                       "poppy"};
const std::vector<std::string> RED = {"pyke", "jhin", "talon", "gragas",
                                      "jayce"};

std::vector<cv::Vec3f> detectCircles(cv::Mat &image, unsigned int radius,
                                     unsigned int param1, unsigned int param2,
                                     int tolerance) {
  cv::Mat grayImage;
  int min_r = radius - tolerance;
  int max_r = radius + tolerance;

  if (image.channels() == 3) {
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
  } else {
    grayImage = image;
  }

  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(grayImage, circles, cv::HOUGH_GRADIENT, 1, 20, param1,
                   param2, min_r, max_r);

  return circles;
}

void drawCircles(cv::Mat &src, std::vector<cv::Vec3f> &circles) {
  for (const cv::Vec3f &circle : circles) {
    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);
    // circle center
    cv::circle(src, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
    // circle outline
    cv::circle(src, center, radius, cv::Scalar(255, 0, 255), 3, 8, 0);
  }
}

void drawCirclesClusters(cv::Mat &src, CirclesCluster &clusters) {
  for (Circles &circles : clusters) {

    cv::Scalar clusterColor(rand() % 256, rand() % 256, rand() % 256);

    for (const cv::Vec3f &circle : circles) {
      cv::Point center((cvRound(circle[0])), cvRound(circle[1]));
      int radius = cvRound(circle[2]);
      // circle center
      cv::circle(src, center, 3, clusterColor, -1, 8, 0);
      // circle outline
      cv::circle(src, center, radius, clusterColor, 3, 8, 0);
    }
  }
}

void detectChamp(cv::Mat &image) {
  cv::Mat hsvImage;
  cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

  cv::Scalar color = {75, 26, 61};

  double hueTolerance = 15;
  double saturationTolerance = 255;
  double valueTolerance = 255;

  cv::Mat image_updated =
      applyMask(hsvImage, color,
                cv::Scalar{hueTolerance, saturationTolerance, valueTolerance});

  std::vector<cv::Vec3f> circles = detectCircles(image_updated, 29, 300, 13, 3);

  CirclesCluster clusters;
  cluster_circles(circles, clusters, 55); // issue here

  drawCirclesClusters(image_updated, clusters);

  cv::cvtColor(image_updated, image_updated, cv::COLOR_HSV2BGR);
  cv::imshow("Processed Image", image_updated);
  while ((cv::waitKey() & 0xEFFFFF) != 81)
    ;

  // get_priority_circles(hsvImage, clusters);
}
