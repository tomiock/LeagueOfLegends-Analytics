#include <opencv2/opencv.hpp>
#include <vector>

std::vector<cv::Point> detectColoredCircles(cv::Mat &image, const cv::Scalar &circleColor, double matchThreshold) {
    // Assume image is square, otherwise adjust radius calculation accordingly
    int imageSize = image.cols; // Assuming a square image
    int radius = imageSize / 20;          // Circle radius
    int borderWidth = radius / 20;        // Border width

    // Create the circle template with specified color and border
    int templateSize = radius * 2 + 2 * borderWidth;
    cv::Mat templateImage(templateSize, templateSize, CV_8UC3, cv::Scalar(0, 0, 0));
    
    // Draw a filled circle and then draw the inner circle to create the border effect
    cv::circle(templateImage, cv::Point(templateSize / 2, templateSize / 2), radius, circleColor, borderWidth);

    cv::Mat grayImage, grayTemplate;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    cv::cvtColor(templateImage, grayTemplate, cv::COLOR_BGR2GRAY);

    cv::Mat result;
    cv::matchTemplate(grayImage, grayTemplate, result, cv::TM_CCOEFF_NORMED);

    std::vector<cv::Point> detectedCircles;
    for (int y = 0; y < result.rows; y++) {
        for (int x = 0; x < result.cols; x++) {
            if (result.at<float>(y, x) >= matchThreshold) {
                detectedCircles.push_back(cv::Point(x, y));
            }
        }
    }

    return detectedCircles;
}
