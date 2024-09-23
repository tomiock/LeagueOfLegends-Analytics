#include "detect_champ.hpp"
#include "circle_priority.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/highgui.hpp"
#include "remove_terrain.hpp"

#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const std::vector<std::string> BLUE = {"blitzcrank", "samira", "veigar",
                                       "diana", "poppy"};
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

void drawCircles(cv::Mat &src, Circles &circles) {
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

cv::Mat resize_image(const cv::Mat &img, const cv::Size &size) {
  cv::Mat resized_img;
  cv::resize(img, resized_img, size);
  return resized_img;
}

cv::Mat crop_icon(const cv::Mat &img, int tolerance = 0) {
  // Get the center and radius for the circular crop
  int center_x = img.cols / 2;
  int center_y = img.rows / 2;
  int radius = std::min(center_x, center_y);

  // Create a mask with the same size as the input image
  cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);

  // Draw a filled circle on the mask with the given tolerance
  cv::circle(mask, cv::Point(center_x, center_y), radius + tolerance,
             cv::Scalar(255), -1);

  // Apply the mask to the input image
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

  // Find matches using KNN (k=2)
  std::vector<std::vector<cv::DMatch>> matches;
  flann.knnMatch(des1, des2, matches, 2);

  // Filter good matches using Lowe's ratio test
  std::vector<cv::DMatch> good_matches;
  for (const auto &m : matches) {
    if (m[0].distance < 0.75 * m[1].distance) {
      good_matches.push_back(m[0]);
    }
  }

  // Calculate similarity index
  if (kp1.empty()) { // Prevent division by zero
    return 0.0f;
  }

  float similarity_index = static_cast<float>(good_matches.size()) / kp1.size();

  return similarity_index;
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
  cv::Mat hsvImage;
  cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

  // delete terrain colors (dirt)
  cv::Scalar color_terrain = {75, 26, 61};
  cv::Scalar tolerances_terrain = {10, 255, 255};
  cv::Mat image_updated =
      applyMask(hsvImage, color_terrain, tolerances_terrain);

  // delete river colors (blue)
  cv::Scalar color_river = {188, 83.9, 75.7};
  cv::Scalar tolerances_rives = {1, 30, 255};
  image_updated = applyMask(image_updated, color_river, tolerances_rives);

  std::vector<cv::Vec3f> circles = detectCircles(image_updated, 29, 150, 15, 3);

  CirclesCluster clusters;
  cluster_circles(circles, clusters, 55); // issue here

  vector<Champion> champions = get_priority_circles(image_updated, clusters);

  cv::cvtColor(image_updated, image_updated, cv::COLOR_HSV2BGR);

  // drawCirclesClusters(image_updated, clusters);

  for (auto champion : champions) {
    cv::Scalar team_color;
    if (champion.team == "red") {
      team_color = {0, 0, 255};
    } else if (champion.team == "blue") {
      team_color = {255, 0, 0};
    } else {
      team_color = {0, 0, 0};
    }

    cv::Point center = {static_cast<int>(champion.center_x),
                        static_cast<int>(champion.center_y)};
    cv::circle(image_updated, center, champion.radius, team_color, 1);

    cv::Rect limiter_box =
        getBoundingBox(image_updated, champion.radius, center);

    cv::Mat box = image_updated(limiter_box);

    std::string detected = compare_champs(box, champion.team);
    cv::Scalar color;
    if (std::find(RED.begin(), RED.end(), detected) != RED.end()) {
      color = {0, 0, 255};
    } else {
      color = {255, 0, 0};
    }

    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 2;
    int baseline = 0;

    cv::Size textSize =
        cv::getTextSize(detected, fontFace, fontScale, thickness, &baseline);
    cv::Point textOrg((center.x - textSize.width),
                      (center.y + textSize.height));

    cv::putText(image_updated, detected, textOrg, fontFace, fontScale, color,
                thickness);
  }

  cv::imshow("Processed Image", image_updated);
  while ((cv::waitKey() & 0xEFFFFF) != 81)
    ;

}
