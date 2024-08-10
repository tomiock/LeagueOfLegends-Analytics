#include "remove_terrain.hpp"
#include "opencv2/core/matx.hpp"
#include "opencv2/imgproc.hpp"

#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace std;

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

cv::Mat update_image(cv::Mat& src) {
    cv::Mat hsv_src;
    cv::cvtColor(src, hsv_src, cv::COLOR_RGB2HSV);

    cv::Vec3f tolerance = {100, 50, 50};
    
    // the colors to be removed are defined in the header file
    std::vector<cv::Vec3f> hsv_constants = {TERRAIN, SHADOW, GROUND, RIVER, RIVER_SHADOW};

    cv::Mat mask = cv::Mat::zeros(src.size(), CV_8U);

    for (const cv::Vec3f& hsv_value : hsv_constants) {
        auto [lower_bound, upper_bound] = hsv_to_bounds(hsv_value, tolerance);
        cv::Mat temp_mask;
        cv::inRange(hsv_src, lower_bound, upper_bound, temp_mask);
        mask |= temp_mask;  // Combine the masks using bitwise OR
    }

    cv::Mat target;
    cv::bitwise_and(src, src, target, mask);
    
    cv::Mat result = src - target;
    return result;
}
