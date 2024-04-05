#include <opencv2/opencv.hpp>  

double calculateImageDifference(const cv::Mat& gray_image1, const cv::Mat& gray_image2) {
    cv::Mat diff;
    cv::absdiff(gray_image1, gray_image2, diff);
    cv::Scalar diff_sum = cv::sum(diff);
    double num_pixels = gray_image1.rows * gray_image1.cols;
    return diff_sum[0] / num_pixels;
}

void concatenateSimilarFrames(const std::vector<cv::Mat>& frames) {
    if (frames.empty()) return;
    cv::Mat concatenated_image = frames[0].clone();
    int frame_width = frames[0].cols;
    int frame_half_width = frame_width / 2;
    int frame_height = frames[0].rows;
    cv::Rect roi_left(0, 0, frame_half_width, frame_height); // x, y, w, h
    cv::Rect roi_right(frame_half_width, 0, frame_half_width, frame_height);
    cv::Mat cur_template = frames[0](roi_right).clone();

    for (size_t i = 1; i < frames.size(); ++i) {
        double mindiff = 10000;
        size_t min_index = 0;
        for (size_t j = 0; j < 100; ++j) {
            if (i + j >= frames.size()) break;
            cv::Mat cur_frame_left = frames[i + j](roi_left).clone();
            double diff = calculateImageDifference(cur_template, cur_frame_left);
            std::cout << "diff: " << diff << std::endl;
            if (diff < mindiff) {
                mindiff = diff;
                min_index = j;
            }
        }
        if (min_index < 50) continue;
        i += min_index;
        std::cout << "min_index: " << min_index << "min diff" << mindiff << std::endl;
        cv::Mat cur_frame_right = frames[i](roi_right).clone();
        cv::hconcat(concatenated_image, cur_frame_right, concatenated_image);
        cur_template = cur_frame_right.clone();
        cv::imshow("cur template right", cur_template);
    }


    std::cout << "-----Last frame:-----" << std::endl;
    // process final frame
    double mindiff = 10000;
    size_t min_x_index = 0;
    cv::Mat last_frame = frames.back();
    for (size_t x = 0; x + frame_half_width < frame_width; ++x) {
        cv::Rect roi(x, 0, frame_half_width, frame_height);
        cv::Mat cur_frame = last_frame(roi).clone();
        double diff = calculateImageDifference(cur_template, cur_frame);
        if (diff < mindiff) {
            std::cout << "diff: " << diff << std::endl;
            std::cout << "min_x_index: " << min_x_index << std::endl;
            mindiff = diff;
            min_x_index = x;
        }
    }
    size_t x = min_x_index + frame_half_width;
    cv::Rect roi(x, 0, frame_width - x, frame_height);
    cv::Mat cur_frame_right = last_frame(roi).clone();
    cv::hconcat(concatenated_image, cur_frame_right, concatenated_image);
    
    cv::imshow("Concatenated Similar Frames", concatenated_image);
    cv::waitKey(0);
}


int main(int argc, char* argv[]) {

    cv::String pattern = "..\\..\\data\\Result\\Sample2\\*.jpg"; 

    std::vector<cv::String> filenames;
    cv::glob(pattern, filenames, false);

    // cv::Mat frame;
    // int frame_freq = 60;
    // int delay = 1000 / frame_freq;
    // for (const auto& filename : filenames) {
    //     frame = cv::imread(filename);
    //     cv::imshow("subtitle frame", frame);
    //     cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    //     cv::imshow("subtitle frame_gray", frame);
    //     // binary thresholding
    //     cv::Mat binary_frame;
    //     cv::threshold(frame, binary_frame, 127, 255, cv::THRESH_BINARY);
    //     cv::imshow("subtitle frame_binary", binary_frame);
    //     if (cv::waitKey(delay) == 27) break; // ESC key to quit
    // }

    std::vector<cv::Mat> frames;

    int frame_width = 0, frame_height = 0;
    cv::Mat frame = cv::imread(filenames[0]);
    frame_width = frame.cols;
    frame_height = frame.rows;
    for (const auto& filename : filenames) {
        frame = cv::imread(filename);
        cv::resize(frame, frame, cv::Size(frame_width, frame_height));
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::threshold(frame, frame, 127, 255, cv::THRESH_BINARY);
        cv::imshow("frame binary", frame);
        if (!frame.empty()) frames.push_back(frame);        
    }

    concatenateSimilarFrames(frames);

    return 0;
}