#include "remove_terrain.hpp"
#include "opencv2/core/matx.hpp"

#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace std;

const cv::Vec3f GROUND = {75.7, 26.9, 61.2};
const cv::Vec3f SHADOW = {78.8, 28.1, 22.4};
const cv::Vec3f TERRAIN = {100, 33.3, 3.5};
const cv::Vec3f RIVER = {188.9, 83.9, 75.7};

tuple<cv::Vec3f, cv::Vec3f> hsv_to_bounds(cv::Vec3f hsvValues, cv::Vec3f tolerance) {
    float H = hsvValues[0]; 
    float S = hsvValues[1];
    float V = hsvValues[2];

    H = H / 2;
    S = S / 100 * 255;
    H = H / 100 * 255;

    cv::Vec3f lower_bound = {
        max(H - tolerance[0], 0.0f),
        max(S - tolerance[1], 0.0f),
        max(V - tolerance[2], 0.0f),
    };
    
    cv::Vec3f upper_bound = {
        min(H + tolerance[0], 180.0f),
        min(S + tolerance[1], 255.0f),
        min(V + tolerance[2], 180.0f),
    };

    return {lower_bound, upper_bound};
}
