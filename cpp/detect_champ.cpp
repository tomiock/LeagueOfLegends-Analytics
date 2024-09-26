#include "detect_champ.hpp"
#include "circle_priority.hpp"
#include "remove_terrain.hpp"

#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

/*
const std::vector<std::string> BLUE = {"blitzcrank", "samira", "veigar",
"diana", "poppy"}; const std::vector<std::string> RED = {"pyke", "jhin",
"talon", "gragas", "jayce"};
*/

const std::vector<std::string> BLUE = {"ksante", "ivern", "smolder", "ezreal",
                                       "rell"};
const std::vector<std::string> RED = {"poppy", "gwen", "corki", "zeri",
                                      "rakan"};

Circles detectCircles(cv::Mat &image, unsigned int radius, unsigned int param1,
                      unsigned int param2, int tolerance) {
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

void drawCircles(cv::Mat &src, Circles &circles) {
  for (const cv::Vec3f &circle : circles) {
    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);
    cv::circle(src, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
    cv::circle(src, center, radius, cv::Scalar(255, 0, 255), 3, 8, 0);
  }
}

void drawCirclesClusters(cv::Mat &src, CirclesCluster &clusters) {
  for (Circles &circles : clusters) {

    cv::Scalar clusterColor(rand() % 256, rand() % 256, rand() % 256);

    for (const cv::Vec3f &circle : circles) {
      cv::Point center((cvRound(circle[0])), cvRound(circle[1]));
      int radius = cvRound(circle[2]);
      cv::circle(src, center, 3, clusterColor, -1, 8, 0);
      cv::circle(src, center, radius, clusterColor, 3, 8, 0);
    }
  }
}

cv::Mat resize_image(const cv::Mat &img, const cv::Size &size) {
  cv::Mat resized_img;
  cv::resize(img, resized_img, size);
  return resized_img;
}

cv::Mat crop_icon(const cv::Mat &img, int tolerance = 0) {
  int center_x = img.cols / 2;
  int center_y = img.rows / 2;
  int radius = std::min(center_x, center_y);

  cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);

  cv::circle(mask, cv::Point(center_x, center_y), radius + tolerance,
             cv::Scalar(255), -1);

  cv::Mat masked_img;
  img.copyTo(masked_img, mask);

  return masked_img;
}

float SIFT_similarity(const cv::Mat &img1, const cv::Mat &img2) {

  cv::Ptr<cv::SIFT> sift = cv::SIFT::create();

  // Detect keypoints and compute descriptors for both images
  std::vector<cv::KeyPoint> kp1, kp2;
  cv::Mat des1, des2;
  sift->detectAndCompute(img1, cv::noArray(), kp1, des1);
  sift->detectAndCompute(img2, cv::noArray(), kp2, des2);

  // If descriptors are empty, return similarity of 0.0
  if (des1.empty() || des2.empty()) {
    return 0.0f;
  }

  // Parameters for FLANN-based matcher
  cv::FlannBasedMatcher flann(cv::makePtr<cv::flann::KDTreeIndexParams>(5),
                              cv::makePtr<cv::flann::SearchParams>(50));

  std::vector<std::vector<cv::DMatch>> matches;
  flann.knnMatch(des1, des2, matches, 2); // k = 2

  std::vector<cv::DMatch> good_matches;
  for (const auto &m : matches) {
    if (m[0].distance < 0.80 * m[1].distance) {
      good_matches.push_back(m[0]);
    }
  }

  // Calculate similarity index
  if (kp1.empty()) {
    return 0.0f;
  }

  return static_cast<float>(good_matches.size()) / kp1.size();
}

// std::string compare_champs(const cv::Mat &detected, const cv::Mat &mask) {
std::string compare_champs(const cv::Mat &detected, std::string team) {
  std::vector<std::pair<std::string, float>> results;

  std::string folder_path = "reference_images/champions/";

  std::string better_match = "none";
  float better_match_index = 0.0;
  float index;

  for (size_t i = 0; i < std::min(BLUE.size(), RED.size()); ++i) {
    std::string blue = BLUE[i];
    std::string red = RED[i];

    for (const auto &entry : std::filesystem::directory_iterator(folder_path)) {
      if (entry.is_regular_file()) {
        std::string file;
        if (team == "red") {
          file = red;
        } else if (team == "blue") {
          file = blue;
        }

        if (entry.path().filename() == file + ".png") {
          std::string file_path = entry.path().string();
          cv::Mat ref_image = cv::imread(file_path);
          index = SIFT_similarity(detected, ref_image);

          if (index > better_match_index) {
            better_match_index = index;
            better_match = file;
          }
        }
      }
    }
  }

  return better_match;
}

void detectChamp(cv::Mat &image) {
  cv::Mat image_HSV;
  cv::cvtColor(image, image_HSV, cv::COLOR_BGR2HSV);

  cv::Scalar color_red = {359, 60, 70};
  cv::Scalar tolerances_red = {15, 100, 100};
  cv::Mat mask_red = getMask(image_HSV, color_red, tolerances_red);

  cv::Scalar color_blue = {190, 60, 60};
  cv::Scalar tolerances_blue = {15, 100, 100};
  cv::Mat mask_blue = getMask(image_HSV, color_blue, tolerances_blue);

  cv::Mat mask;
  cv::bitwise_or(mask_blue, mask_red, mask);

  cv::Mat image_updated;
  image_HSV.copyTo(image_updated, mask);

  int radius = image.rows / 18;
  Circles circles = detectCircles(image_updated, radius, 150, 15, 2);

  CirclesCluster clusters;
  cluster_circles(circles, clusters, radius * 2);

  drawCirclesClusters(image, clusters);

  vector<Champion> champions = get_priority_circles(image_updated, clusters);

  cv::cvtColor(image_updated, image_updated, cv::COLOR_HSV2BGR);

  for (Champion &champion : champions) {
    cv::Rect limiter_box =
        getBoundingBox(image_updated, champion.radius, champion.center);

    cv::Mat box = image_updated(limiter_box);

    string detected = compare_champs(box, champion.team);
    champion.name = detected;
  }

  for (const Champion &champion : champions) {
    cv::Scalar color;
    if (std::find(RED.begin(), RED.end(), champion.name) != RED.end()) {
      color = {0, 0, 255};
    } else {
      color = {255, 0, 0};
    }

    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    int baseline = 0;

    cv::Size textSize = cv::getTextSize(champion.name, fontFace, fontScale,
                                        thickness, &baseline);
    cv::Point textOrg((champion.center.x - textSize.width),
                      (champion.center.y + textSize.height));

    cv::putText(image, champion.name, textOrg, fontFace, fontScale, color,
                thickness);
  }

  cv::imshow("Processed Image", image);
  while ((cv::waitKey() & 0xEFFFFF) != 81)
    ;

}
