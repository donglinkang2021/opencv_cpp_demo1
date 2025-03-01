#include <opencv2/opencv.hpp>  

// annotate the subtitle region

cv::Rect subtitleRegion;
bool isRegionSelected = false;

void onMouse(int event, int x, int y, int flags, void* userdata) {
    static cv::Point point;
    if (event == cv::EVENT_LBUTTONDOWN)
        point = cv::Point(x, y);
    else if (event == cv::EVENT_LBUTTONUP) {
        subtitleRegion = cv::Rect(
            std::min(point.x, x), 
            std::min(point.y, y), 
            abs(x - point.x), 
            abs(y - point.y)
        );
        isRegionSelected = true;
    }
        
}

void annotateSubtitleRegion(cv::Mat& frame) {
    cv::String windowName = "Select subtitle region and Press Enter";
    cv::namedWindow(windowName);
    cv::setMouseCallback(windowName, onMouse, (void*)&frame);
    while (true) {
        cv::imshow(windowName, frame);
        char key = cv::waitKey(10);
        if (key != -1 || isRegionSelected) break;
    }
    cv::destroyAllWindows();
}

std::string getFrameNumber(const cv::String& filename) {
    size_t lastSlashPos = filename.find_last_of('\\');
    size_t lastDotPos = filename.find_last_of('.');
    return filename.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
}

// concatenate similar frames

enum class SubTitleType {
    FIXED,
    MOVING
};

double calculateImageDifference(const cv::Mat& image1, const cv::Mat& image2) {
    cv::Mat diff;
    cv::absdiff(image1, image2, diff);
    cv::Scalar diff_sum = cv::sum(diff);
    
    double diff_sum_total = 0;
    size_t num_channels = image1.channels();
    for (size_t i = 0; i < num_channels; ++i) diff_sum_total += diff_sum[i];

    double num_pixels = image1.rows * image1.cols;
    return diff_sum_total / (num_pixels * num_channels);
}

SubTitleType detectSubtitleType(const std::vector<cv::Mat>& frames) {
    double diff_sum = 0;
    SubTitleType type;
    size_t num_samples = std::min(frames.size() - 1, (size_t)10);
    for (size_t i = 0; i < num_samples; ++i) {
        cv::Mat image1, image2;
        image1 = frames[i];
        image2 = frames[i + 1];
        diff_sum += calculateImageDifference(image1, image2);
    }
    double avg_diff = diff_sum / num_samples;
    if (avg_diff < 10) type = SubTitleType::FIXED;
    else type = SubTitleType::MOVING;
    return type;
}


void concatenateMovingFrames(const std::vector<cv::Mat>& frames) {
    if (frames.empty()) return;

    cv::Mat concatenated_image = frames[0].clone();
    int frame_width = frames[0].cols;
    int frame_half_width = frame_width / 2;
    std::cout << "frame width: " << frame_width << std::endl;
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

void binarizeImage(cv::Mat& image) {
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    cv::threshold(image, image, 127, 255, cv::THRESH_BINARY);
}

void concatenateFixedFrames(const std::vector<cv::Mat>& frames) {
    if (frames.empty()) return;
    cv::Mat concatenated_image = frames[0].clone();
    cv::Mat cur_template = frames[0].clone();
    for (size_t i = 0; i < frames.size() - 1; ++i) {
        double maxdiff = 0;
        size_t max_index = 0;
        for (size_t j = 0; j < 30; ++j) {
            if (i + j >= frames.size()) break;
            cv::Mat cur_frame_left = frames[i + j].clone();
            double diff = calculateImageDifference(cur_template, cur_frame_left);
            if (diff > maxdiff) {
                maxdiff = diff;
                max_index = j;
            }
        }
        if (max_index < 10) continue;
        i += max_index;
        cv::vconcat(concatenated_image, frames[i], concatenated_image);
        cur_template = frames[i].clone();
    }
    cv::imshow("Concatenated Fixed Frames", concatenated_image);
    cv::waitKey(0);
}



int main(int argc, char* argv[]) {
    // change this to your data path
    std::string data_path = "..\\..\\data\\Sample3"; 

    std::vector<cv::String> filenames;
    cv::String pattern = data_path + "\\*.jpg";
    cv::glob(pattern, filenames, false);

    cv::Mat frame = cv::imread(filenames[0]);
    annotateSubtitleRegion(frame);

    std::vector<cv::Mat> frames;

    // play config 60 frames per second
    int frame_freq = 60;
    int delay = 1000 / frame_freq;

    cv::Mat subtitle_frame;
    int frame_width = subtitleRegion.width;
    int frame_height = subtitleRegion.height;
    for (const auto& filename : filenames) {
        frame = cv::imread(filename);
        subtitle_frame = frame(subtitleRegion).clone();
        cv::rectangle(frame, subtitleRegion, cv::Scalar(0, 255, 0), 2);
        cv::imshow("get subtitle region from original frame", frame);
        cv::resize(subtitle_frame, subtitle_frame, cv::Size(frame_width, frame_height));
        if (!subtitle_frame.empty()) frames.push_back(subtitle_frame);   
        if (cv::waitKey(delay) == 27) break; // ESC key to quit
    }

    SubTitleType subtitle_type = detectSubtitleType(frames);
    if (subtitle_type == SubTitleType::FIXED) concatenateFixedFrames(frames);
    else concatenateMovingFrames(frames);
    return 0;
}