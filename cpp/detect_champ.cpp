#include "detect_champ.hpp"
#include "circle_priority.hpp"
#include "opencv2/core/matx.hpp"
#include "remove_terrain.hpp"

#include <filesystem>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

/*
const std::vector<std::string> BLUE = {"blitzcrank", "samira", "veigar", "diana", "poppy"};
const std::vector<std::string> RED = {"pyke", "jhin", "talon", "gragas", "jayce"};
*/

enum class MaskMode {
    PRESERVE, // Preserve pixels in the masks
    REMOVE    // Remove pixels in the masks
};

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
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY); } else {
    grayImage = image;
  }


  cv::blur(grayImage, grayImage, cv::Size(5,5));
  //cv::medianBlur(grayImage, grayImage, 5);

  cv::Canny(grayImage, grayImage, 0, 100, 3);

  //cv::imshow("", grayImage);
  //while ((cv::waitKey() & 0xEFFFFF) != 81);

  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(grayImage, circles, cv::HOUGH_GRADIENT, 1, 13, param1,
                   param2, min_r, max_r);

  return circles;
}

void drawCircles(cv::Mat &src, Circles &circles) {
  for (const cv::Vec3f &circle : circles) {
    cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
    int radius = cvRound(circle[2]);
    cv::circle(src, center, 2, cv::Scalar(0, 255, 0), -1, 8, 0);
    cv::circle(src, center, radius, cv::Scalar(255, 0, 255), 1, 8, 0);
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
    cout << "empty" << endl;
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
          cv::resize(ref_image, ref_image, detected.size());

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

void combineMasks(const cv::Mat& input, cv::Mat& output, const std::vector<cv::Mat>& masks, MaskMode mode = MaskMode::PRESERVE) {
    if (masks.empty()) {
        if (mode == MaskMode::PRESERVE) {
            output = cv::Mat::zeros(input.size(), CV_8UC1); // Empty mask for PRESERVE
        } else {
            output = cv::Mat::ones(input.size(), CV_8UC1) * 255; // White mask for REMOVE
        }
        return;
    }

    output = masks[0].clone();

    for (size_t i = 1; i < masks.size(); ++i) {
        cv::bitwise_or(output, masks[i], output);
    }

    if (mode == MaskMode::REMOVE) {
        cv::bitwise_not(output, output); // Invert the mask for REMOVE mode
    }
}

void detectChamp(cv::Mat &image) {
  cv::Mat image_HSV;
  cv::cvtColor(image, image_HSV, cv::COLOR_BGR2HSV);

  cv::Scalar color_red = {359, 40, 40};
  cv::Scalar tolerances_red = {20, 100, 100};
  cv::Mat mask_red = getMask(image_HSV, color_red, tolerances_red);

  cv::Scalar color_red2 = {1, 40, 40};
  cv::Scalar tolerances_red2 = {20, 100, 100};
  cv::Mat mask_red2 = getMask(image_HSV, color_red2, tolerances_red2);

  cv::Scalar color_blue = {190, 60, 60};
  cv::Scalar tolerances_blue = {20, 65, 70};
  cv::Mat mask_blue = getMask(image_HSV, color_blue, tolerances_blue);

  std::vector<cv::Mat> masks = {mask_red, mask_red2, mask_blue};

  cv::Mat mask;
  combineMasks(image_HSV, mask, masks);

  //cv::imshow("", mask);
  //while ((cv::waitKey() & 0xEFFFFF) != 81);

  cv::Mat image_updated;
  image_HSV.copyTo(image_updated, mask);

  //cv::imshow("", image_updated);
  //while ((cv::waitKey() & 0xEFFFFF) != 81);

  int radius = image.rows / 18;
  Circles circles = detectCircles(image_updated, radius, 180, 9, 2);

  CirclesCluster clusters;
  cluster_circles(circles, clusters, radius * 1.2);

  //drawCirclesClusters(image, clusters);

  //vector<Champion> champions = get_priority_circles(image_updated, clusters);

  vector<Champion> champions;
  for (const cv::Vec3f &circle : circles) {
    cv::Point center = {static_cast<int>(circle[0]), static_cast<int>(circle[1])};
    cv::Rect limiter_box = getBoundingBox(image_updated, circle[2], center, 3);

    cv::Mat box = image(limiter_box);

    /*
    cv::Mat box_detect;
    box.copyTo(box_detect);
    //cv::medianBlur(box_detect, box_detect, 3);

    cv::Mat mask2;
    cv::cvtColor(box_detect, box_detect, cv::COLOR_BGR2HSV);
    combineMasks(box_detect, mask2, masks);

    cv::cvtColor(mask2, mask2, cv::COLOR_HSV2BGR);
    cv::cvtColor(mask2, mask2, cv::COLOR_BGR2GRAY);
    */

    /*

    float radius_detect = 14;
    cv::Vec3f new_circle;
    cv::HoughCircles(mask2, 
                     new_circle, 
                     cv::HOUGH_GRADIENT, 
                     1,  // the inverse ratio of resolution?
                     radius_detect / 2 , // minimum distance between detected centers
                     350, // internal canny edge detector
                     8, // center detection
                     radius_detect + 1, 
                     radius_detect - 1);

    cv::Mat circleMask;

    cv::imshow("", circleMask);
    while ((cv::waitKey() & 0xEFFFFF) != 81);

    circleMask = cv::Mat::zeros(mask2.size(), CV_8UC1); 

    // Draw the circle in white on the output image
    cv::circle(circleMask, 
               cv::Point(circle[0], circle[1]), 
               circle[2], 
               cv::Scalar(255), // White color
               -1); // Filled circle

    //cv::Mat maskedOutput; // Create a separate Mat for the output
    //cv::bitwise_and(box, box, maskedOutput, circleMask); // Apply mask

    cv::imshow("", circleMask);
    while ((cv::waitKey() & 0xEFFFFF) != 81);
    */

    Champion champion = {
      "blue",
      "none",
      static_cast<unsigned short>(circle[2]), // radius
      center
    };

    string detected = compare_champs(box, champion.team);
    champion.name = detected;
    champions.push_back(champion);

    cout << detected << endl;
  }

  /*
  for (Champion &champion : champions) {
    cv::Rect limiter_box =
        getBoundingBox(image_updated, champion.radius_detect, champion.center);

    cout << limiter_box << endl;

    cv::Mat box = image_updated(limiter_box);

    cv::imshow("", box);
    while ((cv::waitKey() & 0xEFFFFF) != 81)
      ;

    string detected = compare_champs(box, champion.team);
    champion.name = detected;
    cout << detected << endl;
  }
  */

  for (const Champion &champion : champions) {
    cv::Scalar color;
    if (std::find(RED.begin(), RED.end(), champion.name) != RED.end()) {
      color = {0, 0, 255};
    } else if (std::find(BLUE.begin(), BLUE.end(), champion.name) != BLUE.end()) {
      color = {255, 0, 0};
    } else {
      color = {255, 255, 255};
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

  cv::imshow("", image);
  while ((cv::waitKey() & 0xEFFFFF) != 81)
    ;

}
