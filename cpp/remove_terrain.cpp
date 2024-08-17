#include "remove_terrain.hpp"

#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

#include <opencv2/opencv.hpp>

// works in NOT FULL hsv color space where the HUE is between 0 and 180 degrees.
colorBounds getColorBounds(cv::Scalar hsvColor, int hueTolerance, int saturationTolerance, int valueTolerance) {
    // Calculate the lower bound for HSV
    
    int hue = static_cast<int>(hsvColor[0] / 2.0); // Scale 0-360 to 0-179
    int saturation = static_cast<int>(hsvColor[1] * 2.55); // Scale 0-100 to 0-255
    int value = static_cast<int>(hsvColor[2] * 2.55); // Scale 0-100 to 0-255
    
    cv::Scalar lower_bound = cv::Scalar(
        std::max(0, (int)hsvColor[0] - hueTolerance),        // Lower bound for H
        std::max(0, (int)hsvColor[1] - saturationTolerance), // Lower bound for S
        std::max(0, (int)hsvColor[2] - valueTolerance)       // Lower bound for V
    );

    // Calculate the upper bound for HSV
    cv::Scalar upper_bound = cv::Scalar(
        std::min(179, (int)hsvColor[0] + hueTolerance),      // Upper bound for H
        std::min(255, (int)hsvColor[1] + saturationTolerance), // Upper bound for S
        std::min(255, (int)hsvColor[2] + valueTolerance)     // Upper bound for V
    );

    return std::make_tuple(lower_bound, upper_bound);
}

cv::Mat applyMask(cv::Mat& image, cv::Scalar targetColor) {
    // Define tolerances
    int hueTolerance = 20;
    int saturationTolerance = 100;
    int valueTolerance = 100;

    cv::Scalar lowerBound, upperBound;
    std::tie(lowerBound, upperBound) = getColorBounds(targetColor, hueTolerance, saturationTolerance, valueTolerance);

    cv::Mat mask, result;
    cv::inRange(image, lowerBound, upperBound, mask); // Create the mask
    cv::bitwise_not(mask, mask); // Invert the mask to remove the selected color
    image.copyTo(result, mask); // Apply the mask to the original image
    return result;
}
