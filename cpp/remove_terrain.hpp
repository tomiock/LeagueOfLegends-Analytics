#ifndef UPDATE_IMAGE_HPP
#define UPDATE_IMAGE_HPP

#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>

// Colors to be removed from the image
const cv::Vec3f GROUND = {75.7, 26.9, 61.2};
const cv::Vec3f SHADOW = {78.8, 28.1, 22.4};
const cv::Vec3f TERRAIN = {100, 33.3, 3.5};
const cv::Vec3f RIVER = {188.9, 83.9, 75.7};
const cv::Vec3f RIVER_SHADOW = {188.1, 84.3, 27.5};

// Function to calculate HSV bounds
std::tuple<cv::Vec3f, cv::Vec3f> hsv_to_bounds(cv::Vec3f hsvValues,
                                               cv::Vec3f tolerance);

// Function to update the image by removing specified regions
cv::Mat update_image(cv::Mat &src);

#endif // UPDATE_IMAGE_HPP
