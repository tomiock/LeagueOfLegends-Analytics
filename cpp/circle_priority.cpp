#include "circle_priority.hpp"
#include "detect_champ.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "remove_terrain.hpp"

#include <algorithm>
#include <opencv2/opencv.hpp>
#include <vector>

const std::vector<std::string> BLUE = {"blitcrank", "samira", "veigar", "diana",
                                       "poppy"};
const std::vector<std::string> RED = {"pyke", "jhin", "talon", "gragas",
                                      "jayce"};
// TODO: make the clusters a stack

// clusters the circles based on how close they are with each other
void cluster_circles(Circles &circles, CirclesCluster &clusters,
                     float distanceThreshold) { // Passed as reference
  for (const cv::Vec3f &circle : circles) {
    bool addedToCluster = false;
    for (auto &cluster : clusters) {
      for (const auto &existingCircle : cluster) {

        float dx = circle[0] - existingCircle[0];
        float dy = circle[1] - existingCircle[1];
        float distance =
            std::sqrt(dx * dx + dy * dy); // euclidian distance used

        if (distance < distanceThreshold) {
          cluster.push_back(circle); // new element
          addedToCluster = true;
          break;
        }
      }
      if (addedToCluster)
        break;
    }
    if (!addedToCluster) {
      clusters.push_back({circle}); // new element
    }
  }
}

void putCenteredText(cv::Mat &image, const std::string &text,
                     cv::Scalar color) {
  int fontFace = cv::FONT_HERSHEY_SIMPLEX;
  double fontScale = 0.5;
  int thickness = 2;
  int baseline = 0;

  cv::Size textSize =
      cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
  cv::Point textOrg((image.cols - textSize.width) / 2,
                    (image.rows + textSize.height) / 2);

  cv::putText(image, text, textOrg, fontFace, fontScale, color, thickness);
}

const cv::Mat create_mask(const cv::Mat &src, cv::Point center,
                          unsigned short radius) {
  cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC3);

  cv::circle(mask, center, radius - 3, cv::Scalar(255, 255, 255), -1);
  cv::circle(mask, center, radius - radius / 5, cv::Scalar(0, 0, 0), -1);

  return mask;
}

cv::Rect getBoundingBox(cv::Mat &src, unsigned short radius, cv::Point center) {
  int x = center.x - radius;
  int y = center.y - radius;
  int width = 2 * radius;
  int height = 2 * radius;

  x = std::max(x, 0);
  y = std::max(y, 0);

  width = std::min(width, src.cols - x);
  height = std::min(height, src.rows - y);

  return cv::Rect(x, y, width, height);
}

cv::Scalar averageHSVColorExcludingBlack(const cv::Mat &hsvImage) {
  double sumHueX = 0.0, sumHueY = 0.0;
  double sumSaturation = 0.0, sumValue = 0.0;
  int count = 0;

  for (int y = 0; y < hsvImage.rows; ++y) {
    for (int x = 0; x < hsvImage.cols; ++x) {
      cv::Vec3b pixel = hsvImage.at<cv::Vec3b>(y, x);

      // black pixels are ignored
      if (pixel[1] > 0 || pixel[2] > 0) {
        int hue = pixel[0];        // range [0, 179]
        int saturation = pixel[1]; // range [0, 255]
        int value = pixel[2];      // range [0, 255]

        // convert Hue to cartesian coordinates
        double hueRadians = (hue * 2.0 * CV_PI) / 180.0; // convert to [0, 2*PI]
        sumHueX += cos(hueRadians);
        sumHueY += sin(hueRadians);

        sumSaturation += saturation;
        sumValue += value;

        count++;
      }
    }
  }

  cv::Scalar averageColor(0, 0, 0);
  if (count > 0) {
    // compute the average Hue by converting back from cartesian to angular form
    double avgHueRadians = atan2(sumHueY, sumHueX);
    if (avgHueRadians < 0) {
      avgHueRadians += 2.0 * CV_PI; // hue > 0
    }
    int avgHue = static_cast<int>((avgHueRadians * 180.0) / CV_PI /
                                  2.0); // convert to [0, 179]

    int avgSaturation = static_cast<int>(sumSaturation / count);
    int avgValue = static_cast<int>(sumValue / count);

    averageColor[0] = avgHue;
    averageColor[1] = avgSaturation;
    averageColor[2] = avgValue;
  }

  return averageColor;
}

std::string frequentColor(cv::Mat &src, cv::Point center,
                          unsigned short radius) {

  const cv::Mat mask = create_mask(src, center, radius);
  cv::bitwise_and(src, mask, mask);

  cv::Scalar color = averageHSVColorExcludingBlack(mask);

  std::string color_str;
  unsigned int intensity = 50;

  if (color[1] < 80 || color[2] < 80) {
    color_str = "undecided";
  } else if (color[0] < 30 || color[0] > 140) {
    color_str = "red";
  } else if (color[0] > 70 || color[0] < 130) {
    color_str = "blue";
  } else if (color[1] < 40 || color[2] < 40) {
    color_str = "undecided";
  } else {
    color_str = "undecided";
  }

  return color_str;
}

vector<Champion> get_priority_circles(cv::Mat &src,
                                      CirclesCluster &clusterCircles) {
  vector<Champion> champion_list;

  // see if cluster contains just one element -> pass
  for (auto &cluster : clusterCircles) {
    if (clusterCircles.size() != 1) {
      for (const cv::Vec3f &circle : cluster) {

        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        unsigned short radius =
            cvRound(circle[2]) + 5; // added to account for tolerances

        cv::Rect boundingBox = getBoundingBox(src, radius, center);
        cv::Mat croppedResult = src(boundingBox);

        cv::Point center_box = {radius, radius};

        std::string team = frequentColor(croppedResult, center_box, radius);

        cv::Mat display = cv::Mat::zeros(croppedResult.size(), CV_8UC1);

        cv::Mat lowerRedMask, upperRedMask, blueMask;
        cv::inRange(croppedResult, cv::Scalar(0, 120, 120),
                    cv::Scalar(8, 255, 255), lowerRedMask);
        cv::inRange(croppedResult, cv::Scalar(175, 120, 120),
                    cv::Scalar(179, 255, 255), upperRedMask);
        cv::bitwise_or(lowerRedMask, upperRedMask, display);

        cv::inRange(croppedResult, cv::Scalar(95, 120, 120),
                    cv::Scalar(128, 255, 255), blueMask);
        cv::bitwise_or(display, blueMask, display);

        cv::medianBlur(display, display, 3);

        Circles circles;
        cv::HoughCircles(display, circles, cv::HOUGH_GRADIENT, 1, 2 * radius,
                         350, 5, radius - 5, radius - 3);

        Champion champion;

        if (circles.size() == 0) {
        } else if (circles.size() > 1) {
        } else {
          champion = {
              team, // red or blue (string)
              "none",
              static_cast<unsigned short>(circles[0][2]), // radius
              center
          };
          champion_list.push_back(champion);
        }
      }
    }
  }

  // get difference between a partial arc and the total arc of the circle
  // the ones that have less proportion are the ones behind

  return champion_list;
}
