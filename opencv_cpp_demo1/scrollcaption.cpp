// -*- coding: utf-8 -*-

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


int flag[300];
int cnt = 0;


Rect subtitleRegion; // 全局变量用于保存字幕区域


void onMouse(int event, int x, int y, int flags, void* userdata) {
    static Point p; // 用于保存鼠标左键按下时的坐标

    if (event == EVENT_LBUTTONDOWN) {
        // 在鼠标左键按下时，保存当前坐标
        p = Point(x, y);
    }
    else if (event == EVENT_LBUTTONUP) {
        // 在鼠标左键释放时，保存当前坐标作为字幕区域的终点
        subtitleRegion = Rect(min(p.x, x), min(p.y, y), abs(x - p.x), abs(y - p.y));
        printf("x1=%d,x2=%d,y1=%d,y2=%d\n", p.x, x, p.y, y);
    }

}

void rollscript(string Filename);


void processSubtitleRegion(string startFrame, string endFrame, Mat& frame) {
    // 标注字幕区域
    cout << "请选择：1.框出固定字幕区域\n2.框出滚动字幕区域\n（输入1或2）\n";
    int a = 0;
    cin >> a;


    cout << "请用鼠标左键拖动选择字幕区域，完成后按任意键继续...\n";
    namedWindow("Frame");
    setMouseCallback("Frame", onMouse, (void*)&frame);
    while (true) {
        imshow("Frame", frame);
        char key = waitKey(10);
        if (key != -1)
            break;
    }
    destroyAllWindows();


    size_t lastSlashPos = startFrame.find_last_of('/');
    size_t lastDotPos = startFrame.find_last_of('.');

    string numString = startFrame.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
    int frameNum = stoi(numString);

    cout << "start Frame number: " << frameNum << endl;

    size_t lastSlashPos2 = endFrame.find_last_of('/');
    size_t lastDotPos2 = endFrame.find_last_of('.');

    string numString2 = endFrame.substr(lastSlashPos2 + 1, lastDotPos2 - lastSlashPos2 - 1);
    int frameNum2 = stoi(numString2);


    // 处理字幕区域
    Mat currentFrame,nextFrame;
    bool isFixed = true;
    bool isFirstFrame = true;
    string outputFilename;
    while (true) {
        /*printf("round\n");*/

        Rect roi(subtitleRegion.x, subtitleRegion.y, subtitleRegion.width, subtitleRegion.height);
        if (isFirstFrame) {
            // 读取第一帧
            //namedWindow("currentFrame");
            
            currentFrame = frame(roi).clone();
            isFirstFrame = false;
            imshow("currentFrame", currentFrame);
            char key = waitKey(10);
            if (key != -1)
                break;

            string outputFilename0;
            if(a==1) outputFilename0 = startFrame.substr(0, lastSlashPos) + "/result/fix/frame_" + to_string(frameNum) + ".jpg";
            else if(a==2) outputFilename0 = startFrame.substr(0, lastSlashPos) + "/result/roll/frame_" + to_string(frameNum) + ".jpg";
            imwrite(outputFilename0, currentFrame);
            cout << "Saved different frame: " << outputFilename0 << endl;
        }
       
        


        // 获取下一帧的文件名
       
        string folderPath;
        if (frameNum < frameNum2) {
            frameNum++;
            size_t lastSlashPos = startFrame.find_last_of('/');
            folderPath = startFrame.substr(0, lastSlashPos)+"/0"+to_string(frameNum)+".jpg";
           
        }
        else {
            break;
        }
        
        
        std::cout<<"framenamenew:"<<folderPath<<endl;
        // 读取下一帧图像
        Mat nextFrame1 = imread(folderPath);
        
        namedWindow("nextFrame");
        // 读取字幕区域部分
        Mat nextFrame;
        nextFrame = nextFrame1(roi).clone();
        imshow("nextFrame", nextFrame);
        char key = waitKey(10);
        if (key != -1)
            break;

        //转换为灰度图
        Mat grayCurrentFrame, grayNextFrame;
        cvtColor(currentFrame, grayCurrentFrame, COLOR_BGR2GRAY);
        cvtColor(nextFrame, grayNextFrame, COLOR_BGR2GRAY);

        Mat diff;
        absdiff(grayCurrentFrame, grayNextFrame, diff);

        Mat binaryDiff;
        threshold(diff, binaryDiff, 10, 255, THRESH_BINARY);


        if (a == 1) {
            if (countNonZero(binaryDiff) > 1000) {
                //printf("not fix\n");
                outputFilename = startFrame.substr(0, lastSlashPos) + "/result/fix2/frame_" + to_string(frameNum) + ".jpg";
                imwrite(outputFilename, currentFrame);
                cout << "Saved different frame: " << outputFilename << endl;

                isFixed = false;
                flag[cnt] = frameNum;
                cnt++;
            }
        }
        else if (a == 2) {
            if (countNonZero(binaryDiff) > 1000) {
                //printf("not fix\n");
                outputFilename = startFrame.substr(0, lastSlashPos) + "/result/roll/frame_" + to_string(frameNum) + ".jpg";
                imwrite(outputFilename, currentFrame);
                cout << "Saved different frame: " << outputFilename << endl;

                isFixed = false;
                flag[cnt] = frameNum;
                cnt++;

                
            }
        }


        // 更新
        currentFrame = nextFrame.clone();
        isFixed = true;


    }

    
    for (int i = 0; i < 300; i++) {
        cout <<"i="<<flag[i] << endl;
    }

    if (a == 2) {
        cout << outputFilename.substr(0, lastSlashPos) << endl;
        string tmp = outputFilename.substr(0, lastSlashPos) + "/result/roll";
        rollscript(tmp);
    }

}



void rollscript(string Filename) {

    Mat combine;
    for (int i = 20; i < 300; i++) {   //1.i=20; 2.i=0
        if (flag[i] == 0) break;
        string currentFilename = Filename + "/frame_" + to_string(flag[i]) + ".jpg";
        //cout << "roll:" << currentFilename << endl;
        Mat rollframe = imread(currentFilename);
        namedWindow("rollframe");
        imshow("rollframe", rollframe);

        //分为左右两侧
        int rollframeWidth = rollframe.cols;
        int halfWidth = rollframeWidth / 2;
        Mat leftHalf = rollframe(Rect(0, 0, halfWidth, rollframe.rows)).clone();
        Mat rightHalf = rollframe(Rect(halfWidth, 0, halfWidth, rollframe.rows)).clone();

        Mat left2, right2;

        //拼接
        if (i == 20) {    //1.i==20;  2.i==0
            hconcat(leftHalf, rightHalf, combine);
            //imwrite(Filename+"/combined_image.jpg", combine);
        }
        
        //转换为灰度图
        Mat grayright;
        cvtColor(rightHalf, grayright, COLOR_BGR2GRAY);

      
        int j = 0;
        for (j = i +1; j < 300; j++) {
            if (flag[j] == 0) break;
            currentFilename = Filename + "/frame_" + to_string(flag[j]) + ".jpg";
            Mat rollframe2 = imread(currentFilename);
            namedWindow("rollframe2");
            imshow("rollframe2", rollframe2);

            left2 = rollframe2(Rect(0, 0, halfWidth, rollframe2.rows)).clone();
            right2 = rollframe2(Rect(halfWidth, 0, halfWidth, rollframe2.rows)).clone();

           // cout << "halfWidth=" << halfWidth << endl;
            //cout << "hight=" << rollframe2.rows << endl;

            Mat grayleft;
            cvtColor(left2, grayleft, COLOR_BGR2GRAY);

            Mat diff;
            absdiff(grayright, grayleft, diff);

            Mat binaryDiff;
            threshold(diff, binaryDiff, 10, 255, THRESH_BINARY);


            if (countNonZero(binaryDiff) < 4500) {   //1.4500, 2.3500
                printf("same!!!!!!!!!!!!!!!!!!!!!!\n");

                hconcat(combine, right2, combine);
                imwrite(Filename + "/combined_image_"+to_string(j)+".jpg", combine);
                /*namedWindow("left2");
                imshow("left2", left2);*/
                i = j ;

                
                break;
            }

             

        }

        // todo
        
    }
    imwrite(Filename+"/combined_image.jpg", combine);
}



int main(int argc, char** argv) {
    string folderPath = "D:/王一言 学习/大学/科研/滚动字幕拼接/Sample1";

    // 读取文件夹中所有的图像帧
    vector<String> filenames;
    glob(folderPath + "/*.jpg", filenames, false);

    Mat frame;

    string startname = "D:/王一言 学习/大学/科研/滚动字幕拼接/Sample1/016660.jpg";
    string endname = "D:/王一言 学习/大学/科研/滚动字幕拼接/Sample1/016908.jpg";   //1.016908,2.016840
    frame = imread(startname);
   
    processSubtitleRegion(startname, endname, frame);

    return 0;
}
