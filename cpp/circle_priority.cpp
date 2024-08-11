#include "circle_priority.hpp"

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

// TODO: make the clusters a stack

// clusters the circles based on how close they are with each other
void cluster_circles(Circles &circles, CirclesCluster &clusters, float distanceThreshold) { // Passed as reference
  for (const cv::Vec3f &circle : circles) {
    bool addedToCluster = false;
    for (auto &cluster : clusters) {
      for (const auto &existingCircle : cluster) {

        float dx = circle[0] - existingCircle[0];
        float dy = circle[1] - existingCircle[1];
        float distance = std::sqrt(dx * dx + dy * dy); // euclidian distance used

        if (distance < distanceThreshold) {
          cluster.push_back(circle);
          addedToCluster = true;
          break;
        }
      }
      if (addedToCluster)
        break;
    }
    if (!addedToCluster) {
      clusters.push_back({circle});
    }
  }
}

// which circles are on top of each other
int get_priority_circles(CirclesCluster &clusterCircles) {
  // see if cluster contains just one element -> pass

  // get difference between a partial arc and the total arc of the circle
  // the ones that have less proportion are the ones behind
}
