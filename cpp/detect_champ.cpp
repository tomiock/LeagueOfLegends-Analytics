#include "detect_champ.hpp"
#include "remove_terrain.hpp"

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>


const std::vector<std::string> BLUE = {"blitcrank", "samira", "veigar", "diana", "poppy"};
const std::vector<std::string> RED = {"pyke", "jhin", "talon", "gragas", "jayce"};

std::vector<cv::Vec3f> detectCircles(cv::Mat& image, unsigned int radius, unsigned int param1, unsigned int param2, int tolerance) {
    cv::Mat grayImage;
    int min_r = radius - tolerance;
    int max_r = radius + tolerance;

    if (image.channels() == 3) {
        cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    } else {
        grayImage = image;
    }

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(grayImage, circles, cv::HOUGH_GRADIENT, 1, 20, param1, param2, min_r, max_r);

    return circles;
}

void drawCircles(cv::Mat& src, std::vector<cv::Vec3f>& circles) {
    for (size_t i = 0; i < circles.size(); i++) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        cv::circle(src, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        // circle outline
        cv::circle(src, center, radius, cv::Scalar(255, 0, 255), 3, 8, 0);
    }
}

void detectChamp(cv::Mat& image) {
    image = update_image(image);

    std::vector<cv::Vec3f> circles = detectCircles(image, 30, 300, 14, 3);
    drawCircles(image, circles);

    cv::imshow("Processed Image", image);
    while((cv::waitKey() & 0xEFFFFF) != 81);
}
