#include "detect_champ.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>" << std::endl;
        return -1;
    }

    fs::path directoryPath = argv[1];

    if (!fs::is_directory(directoryPath)) {
        std::cerr << "The provided path is not a directory." << std::endl;
        return -1;
    } else {
        std::cerr << "Valid directory provided" << std::endl;
    }

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_COLOR);

            // Check if the image was loaded successfully
            if (image.empty()) {
                std::cerr << "Could not open or find the image: " << entry.path() << std::endl;
                continue;
            } else {
                detectChamp(image);
            }
        }
    }

    return 0;
}
