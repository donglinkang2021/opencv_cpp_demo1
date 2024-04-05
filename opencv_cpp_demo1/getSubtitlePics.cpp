#include <opencv2/opencv.hpp>  

cv::Rect subtitleRegion;

void onMouse(int event, int x, int y, int flags, void* userdata) {
    static cv::Point point;
    if (event == cv::EVENT_LBUTTONDOWN)
        point = cv::Point(x, y);
    else if (event == cv::EVENT_LBUTTONUP) 
        subtitleRegion = cv::Rect(
            std::min(point.x, x), 
            std::min(point.y, y), 
            abs(x - point.x), 
            abs(y - point.y)
        );
}

void annotateSubtitleRegion(cv::Mat& frame) {
    cv::String windowName = "Select subtitle region and Press Enter";
    cv::namedWindow(windowName);
    cv::setMouseCallback(windowName, onMouse, (void*)&frame);
    while (true) {
        cv::imshow(windowName, frame);
        char key = cv::waitKey(10);
        if (key != -1) break;
    }
    cv::destroyAllWindows();
}

std::string getFrameNumber(const cv::String& filename) {
    size_t lastSlashPos = filename.find_last_of('\\');
    size_t lastDotPos = filename.find_last_of('.');
    return filename.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
}

int main(int argc, char* argv[]) {

    cv::String pattern = "..\\..\\data\\Sample1\\*.jpg"; 
    cv::String result_dir = "..\\..\\data\\Result\\Sample1\\";

    std::vector<cv::String> filenames;
    cv::glob(pattern, filenames, false);

    cv::Mat frame = cv::imread(filenames[0]);
    annotateSubtitleRegion(frame);

    cv::Mat subtitle_frame;

    int frame_freq = 60;
    int delay = 1000 / frame_freq;
    for (const auto& filename : filenames) {
        frame = cv::imread(filename);
        subtitle_frame = frame(subtitleRegion).clone();
        cv::rectangle(frame, subtitleRegion, cv::Scalar(0, 255, 0), 2);
        cv::imshow("frame", frame);
        cv::imshow("subtitle", subtitle_frame);
        cv::imwrite(
            result_dir + getFrameNumber(filename) + ".jpg", 
            subtitle_frame
        );
        if (cv::waitKey(delay) == 27) break; // ESC key to quit
    }

    return 0;
}