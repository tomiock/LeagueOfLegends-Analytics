#include "circle_priority.hpp"
#include "remove_terrain.hpp"

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

// which circles are on top of each other
void get_priority_circles(cv::Mat &src, CirclesCluster &clusterCircles) {
  // see if cluster contains just one element -> pass
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

        // TODO: add global variables to reference the image size
        width = std::min(width, src.cols - x);
        height = std::min(height, src.rows - y);

        cv::Rect boundingBox(x, y, width, height);

        cv::Mat croppedResult = src(boundingBox);

        // color to gray
        //cv::cvtColor(croppedResult, croppedResult, cv::COLOR_BGR2GRAY);
        cv::Mat threshold;

        cv::Vec3f hsv_value = {74, 20, 25};
        auto [lower_bound, upper_bound] = getColorBounds(hsv_value, 10, 50, 50);
        cv::inRange(croppedResult, lower_bound, upper_bound, threshold);

        // canny
        cv::Mat edges;
        cv::Canny(croppedResult, edges, 100, 200);

        // coutour detected

        // coutour perfect

        // diff countour

        cv::imshow("Cropped Circle", threshold);
        while ((cv::waitKey() & 0xEFFFFF) != 81);
      }
    }
  }

  // get difference between a partial arc and the total arc of the circle
  // the ones that have less proportion are the ones behind
}
