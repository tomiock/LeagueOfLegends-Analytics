#ifndef CIRCLE_PRIORITY_H
#define CIRCLE_PRIORITY_H

#include "opencv2/core/matx.hpp"
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;

using CirclesCluster = vector<vector<cv::Vec3f>>; // TODO: change for stack
using Circles = vector<cv::Vec3f>;

void cluster_circles(Circles &circles, CirclesCluster &clusters, float distanceThreshold); // Passed as reference

void get_priority_circles(cv::Mat& src, CirclesCluster &clusterCircles);

#endif
