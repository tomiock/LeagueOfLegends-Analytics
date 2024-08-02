#ifndef DETECT_CHAMP_H
#define DETECT_CHAMP_H

#include <opencv2/opencv.hpp>

std::vector<cv::Vec3f> detectCircles(cv::Mat& image, unsigned int radius, unsigned int param1, unsigned int param2, int tolerance);

void drawCircles(cv::Mat& src,  std::vector<cv::Vec3f>& circles);

void detectChamp(cv::Mat& image);

#endif // PROCESS_IMAGE_H

