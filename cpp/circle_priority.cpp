#include "opencv2/core/matx.hpp"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

using CirclesCluster = vector<vector<cv::Vec3f>>;
using Circles = vector<cv::Vec3f>;

// clusters the circles based on how close they are with each other
CirclesCluster cluster_circles(Circles& circles) { // Passed as reference
  for (const cv::Vec3f &circle : circles) {
  }
}

// which circles are on top of each other
int get_priority_circles(CirclesCluster& clusterCircles) {
  // see if cluster contains just one element -> pass
  
  // get difference between a partial arc and the total arc of the circle
  // the ones that have less proportion are the ones behind
}
