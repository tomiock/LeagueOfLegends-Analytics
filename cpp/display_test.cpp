#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    // Check if the image path is provided
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }

    // Read the image file
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);

    // Check for failure
    if (image.empty())
    {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    // Display the image in a window
    cv::imshow("Display window", image);

    // Print the current UI framework
    //std::cout << "Current UI framework: " << cv::currentUIFramework() << std::endl;

    // Wait for any keystroke in the window
    cv::waitKey(0);

    return 0;
}
