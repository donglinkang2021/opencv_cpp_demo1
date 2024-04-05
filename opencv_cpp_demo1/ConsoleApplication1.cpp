﻿#include <opencv2/opencv.hpp>  
int main(int argc, char* argv[]) {

    cv::String pattern = "../../data/Sample2/*.jpg"; 

    // 读取文件夹中所有的图像帧
    std::vector<cv::String> filenames;
    cv::glob(pattern, filenames, false);

    cv::Mat frame;
    int frame_freq = 60;
    int delay = 1000 / frame_freq;
    for (const auto& filename : filenames) {
        frame = cv::imread(filename);
        cv::imshow("frame", frame);
        cv::waitKey(delay);
    }
    
    return 0;
}