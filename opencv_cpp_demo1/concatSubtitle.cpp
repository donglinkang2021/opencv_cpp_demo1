#include <opencv2/opencv.hpp>  

double calculateImageDifference(const cv::Mat& gray_image1, const cv::Mat& gray_image2) {
    cv::Mat diff;
    cv::absdiff(gray_image1, gray_image2, diff);
    cv::Scalar diff_sum = cv::sum(diff);
    
    double diff_sum_total = 0;
    size_t num_channels = gray_image1.channels();
    for (size_t i = 0; i < num_channels; ++i) diff_sum_total += diff_sum[i];

    double num_pixels = gray_image1.rows * gray_image1.cols;
    return diff_sum_total / (num_pixels * num_channels);
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
            if (diff < mindiff) {
                mindiff = diff;
                min_index = j;
            }
        }
        if (min_index < 50) continue;
        i += min_index;
        cv::Mat cur_frame_right = frames[i](roi_right).clone();
        cv::hconcat(concatenated_image, cur_frame_right, concatenated_image);
        cur_template = cur_frame_right.clone();
    }


    // process final frame
    double mindiff = 10000;
    size_t min_x_index = 0;
    cv::Mat last_frame = frames.back();
    for (size_t x = 0; x + frame_half_width < frame_width; ++x) {
        cv::Rect roi(x, 0, frame_half_width, frame_height);
        cv::Mat cur_frame = last_frame(roi).clone();
        double diff = calculateImageDifference(cur_template, cur_frame);
        if (diff < mindiff) {
            mindiff = diff;
            min_x_index = x;
        }
    }
    int x = min_x_index + frame_half_width;
    cv::Rect roi(x, 0, frame_width - x, frame_height);
    cv::Mat cur_frame_right = last_frame(roi).clone();
    cv::hconcat(concatenated_image, cur_frame_right, concatenated_image);
    
    cv::imshow("Concatenated Similar Frames", concatenated_image);
    cv::waitKey(0);
}


int main(int argc, char* argv[]) {

    std::string data_path = "..\\..\\data\\Result\\Sample1\\";
    cv::String pattern = data_path + "*.jpg";

    std::vector<cv::String> filenames;
    cv::glob(pattern, filenames, false);

    std::vector<cv::Mat> frames;

    int frame_width = 0, frame_height = 0;
    cv::Mat frame = cv::imread(filenames[0]);
    frame_width = frame.cols;
    frame_height = frame.rows;
    for (const auto& filename : filenames) {
        frame = cv::imread(filename);
        cv::resize(frame, frame, cv::Size(frame_width, frame_height));
        // if you want your result to be in grayscale
        // feel free to uncomment the following lines
        // cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        // cv::threshold(frame, frame, 127, 255, cv::THRESH_BINARY);
        if (!frame.empty()) frames.push_back(frame);        
    }

    concatenateSimilarFrames(frames);

    return 0;
}