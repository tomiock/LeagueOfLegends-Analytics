#include "circle_priority.hpp"
#include "detect_champ.hpp"
#include "opencv2/imgproc.hpp"
#include "remove_terrain.hpp"

#include <algorithm>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

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

cv::Scalar averageColorExcludingBlack(const cv::Mat &image) {
  cv::Scalar sumColor(0, 0, 0);
  int count = 0;

  for (int y = 0; y < image.rows; ++y) {
    for (int x = 0; x < image.cols; ++x) {
      cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
      if (pixel != cv::Vec3b(0, 0, 0)) {
        sumColor[0] += pixel[0];
        // sumColor[1] += pixel[1];
        sumColor[2] += pixel[2];
        count++;
      }
    }
  }

  if (count > 0) {
    sumColor[0] /= count;
    // sumColor[1] /= count;
    sumColor[2] /= count;
  }

  return sumColor;
}

std::string frequentColor(cv::Mat &src, cv::Point center,
                          unsigned short radius) {

  const cv::Mat mask = create_mask(src, center, radius);
  cv::bitwise_and(src, mask, mask);
  cv::Scalar color = averageColorExcludingBlack(mask);

  std::string color_str;
  unsigned int THRESHHOLD_INTENSITY = 50;
  if (color[0] > THRESHHOLD_INTENSITY && color[0] > color[2]) {
    color_str = "blue";
  } else if (color[2] > THRESHHOLD_INTENSITY && color[2] > color[0]) {
    color_str = "red";
  } else if (color[0] < THRESHHOLD_INTENSITY &&
             color[2] < THRESHHOLD_INTENSITY) {
    color_str = "undecided";
  } else {
    color_str = "undecided";
  }

  std::cout << color_str << endl;

  return color_str;
}

// which circles are on top of each other
void get_priority_circles(cv::Mat &src, CirclesCluster &clusterCircles) {
  // see if cluster contains just one element -> pass

  cv::cvtColor(src, src, cv::COLOR_HSV2BGR);

  for (auto &cluster : clusterCircles) {
    if (clusterCircles.size() != 1) {
      for (const cv::Vec3f &circle : cluster) {
        // crop image to fit circle
        cv::Point center(cvRound(circle[0]), cvRound(circle[1]));
        unsigned short radius = cvRound(circle[2]) + 5;

        // Define the bounding box of the circle
        int x = center.x - radius;
        int y = center.y - radius;
        int width = 2 * radius;
        int height = 2 * radius;

        // Ensure the bounding box is within the image bounds
        x = std::max(x, 0);
        y = std::max(y, 0);

        width = std::min(width, src.cols - x);
        height = std::min(height, src.rows - y);

        cv::Rect boundingBox(x, y, width, height);
        cv::Mat croppedResult = src(boundingBox);

        cv::Point center_box = {radius, radius};

        std::string color = frequentColor(croppedResult, center_box, radius);
        putCenteredText(croppedResult, color, cv::Scalar{255, 255, 255});
      }
    }
  }

  // get difference between a partial arc and the total arc of the circle
  // the ones that have less proportion are the ones behind
}
