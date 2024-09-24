#ifndef CIRCLE_PRIORITY_H
#define CIRCLE_PRIORITY_H

#include "opencv2/core/matx.hpp"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

using CirclesCluster = vector<vector<cv::Vec3f>>; // TODO: change for stack
using Circles = vector<cv::Vec3f>;

typedef struct {
  string team;
  string name;
  unsigned short radius;
  cv::Point center;
} Champion;

cv::Rect getBoundingBox(cv::Mat &src, unsigned short radius, cv::Point center);

void putCenteredText(cv::Mat &image, const std::string &text, cv::Scalar color);

void cluster_circles(Circles &circles, CirclesCluster &clusters,
                     float distanceThreshold);

vector<Champion> get_priority_circles(cv::Mat &src,
                                      CirclesCluster &clusterCircles);

#endif
