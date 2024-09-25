#include "remove_terrain.hpp"

#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

#include <opencv2/opencv.hpp>

// works in NOT FULL hsv color space where the HUE is between 0 and 180 degrees.
// this function is made to take as input the HSV values as seen in photo
// editing software like GIMP
colorBounds getColorBounds(cv::Scalar hsvColor, int hueTolerance,
                           int saturationTolerance, int valueTolerance) {
  int hue = static_cast<int>(hsvColor[0] / 2.0);         // Scale 0-360 to 0-179
  int saturation = static_cast<int>(hsvColor[1] * 2.55); // Scale 0-100 to 0-255
  int value = static_cast<int>(hsvColor[2] * 2.55);      // Scale 0-100 to 0-255

  // is not necessary to normalize the hue value since opencv does it for us
  // (negative values are accepted
  int lower_hue = static_cast<int>(hue - hueTolerance);
  int upper_hue = static_cast<int>(hue + hueTolerance);

  cv::Scalar lower_bound =
      cv::Scalar(lower_hue, std::max(0, saturation - saturationTolerance),
                 std::max(0, value - valueTolerance));

  cv::Scalar upper_bound =
      cv::Scalar(upper_hue, std::min(255, saturation + saturationTolerance),
                 std::min(255, value + valueTolerance));

  return std::make_tuple(lower_bound, upper_bound);
}

cv::Mat applyMask(cv::Mat &image, cv::Scalar targetColor,
                  cv::Scalar tolerances) {
  // Define tolerances

  cv::Scalar lowerBound, upperBound;
  std::tie(lowerBound, upperBound) =
      getColorBounds(targetColor, tolerances[0], tolerances[1], tolerances[2]);

  cv::Mat mask, result;
  cv::inRange(image, lowerBound, upperBound, mask); // Create the mask
  cv::bitwise_not(mask, mask); // Invert the mask to remove the selected color
  image.copyTo(result, mask);  // Apply the mask to the original image
  return result;
}
