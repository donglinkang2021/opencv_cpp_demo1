#include <opencv2/opencv.hpp>  
int main(int argc, char* argv[]) {

    cv::String pattern = "../../data/Sample1/*.jpg"; 

    // 读取文件夹中所有的图像帧
    std::vector<cv::String> filenames;
    cv::glob(pattern, filenames, false);

    for (const auto& filename : filenames) std::cout << filename << std::endl;
    
    return 0;
}