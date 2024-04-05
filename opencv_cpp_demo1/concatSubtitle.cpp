#include <opencv2/opencv.hpp>  


int main(int argc, char* argv[]) {

    cv::String pattern = "..\\..\\data\\Result\\Sample1\\*.jpg"; 

    std::vector<cv::String> filenames;
    cv::glob(pattern, filenames, false);

    cv::Mat frame;
    int frame_freq = 60;
    int delay = 1000 / frame_freq;
    for (const auto& filename : filenames) {
        frame = cv::imread(filename);
        cv::imshow("subtitle frame", frame);

        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::imshow("subtitle frame_gray", frame);

        // binary thresholding
        cv::Mat binary_frame;
        cv::threshold(frame, binary_frame, 100, 255, cv::THRESH_BINARY);
        cv::imshow("subtitle frame_binary", binary_frame);

        // concat subtitle

        
        if (cv::waitKey(delay) == 27) break; // ESC key to quit
    }

    return 0;
}