// -*- coding: utf-8 -*-

#include <iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;


int flag[300];
int cnt = 0;


Rect subtitleRegion; // ȫ�ֱ������ڱ�����Ļ����


void onMouse(int event, int x, int y, int flags, void* userdata) {
    static Point p; // ���ڱ�������������ʱ������

    if (event == EVENT_LBUTTONDOWN) {
        // ������������ʱ�����浱ǰ����
        p = Point(x, y);
    }
    else if (event == EVENT_LBUTTONUP) {
        // ���������ͷ�ʱ�����浱ǰ������Ϊ��Ļ������յ�
        subtitleRegion = Rect(min(p.x, x), min(p.y, y), abs(x - p.x), abs(y - p.y));
        printf("x1=%d,x2=%d,y1=%d,y2=%d\n", p.x, x, p.y, y);
    }

}

void rollscript(string Filename);


void processSubtitleRegion(string startFrame, string endFrame, Mat& frame) {
    // ��ע��Ļ����
    cout << "��ѡ��1.����̶���Ļ����\n2.���������Ļ����\n������1��2��\n";
    int a = 0;
    cin >> a;


    cout << "�����������϶�ѡ����Ļ������ɺ����������...\n";
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


    // ������Ļ����
    Mat currentFrame,nextFrame;
    bool isFixed = true;
    bool isFirstFrame = true;
    string outputFilename;
    while (true) {
        /*printf("round\n");*/

        Rect roi(subtitleRegion.x, subtitleRegion.y, subtitleRegion.width, subtitleRegion.height);
        if (isFirstFrame) {
            // ��ȡ��һ֡
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
       
        


        // ��ȡ��һ֡���ļ���
       
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
        // ��ȡ��һ֡ͼ��
        Mat nextFrame1 = imread(folderPath);
        
        namedWindow("nextFrame");
        // ��ȡ��Ļ���򲿷�
        Mat nextFrame;
        nextFrame = nextFrame1(roi).clone();
        imshow("nextFrame", nextFrame);
        char key = waitKey(10);
        if (key != -1)
            break;

        //ת��Ϊ�Ҷ�ͼ
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


        // ����
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

        //��Ϊ��������
        int rollframeWidth = rollframe.cols;
        int halfWidth = rollframeWidth / 2;
        Mat leftHalf = rollframe(Rect(0, 0, halfWidth, rollframe.rows)).clone();
        Mat rightHalf = rollframe(Rect(halfWidth, 0, halfWidth, rollframe.rows)).clone();

        Mat left2, right2;

        //ƴ��
        if (i == 20) {    //1.i==20;  2.i==0
            hconcat(leftHalf, rightHalf, combine);
            //imwrite(Filename+"/combined_image.jpg", combine);
        }
        
        //ת��Ϊ�Ҷ�ͼ
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
    string folderPath = "D:/��һ�� ѧϰ/��ѧ/����/������Ļƴ��/Sample1";

    // ��ȡ�ļ��������е�ͼ��֡
    vector<String> filenames;
    glob(folderPath + "/*.jpg", filenames, false);

    Mat frame;

    string startname = "D:/��һ�� ѧϰ/��ѧ/����/������Ļƴ��/Sample1/016660.jpg";
    string endname = "D:/��һ�� ѧϰ/��ѧ/����/������Ļƴ��/Sample1/016908.jpg";   //1.016908,2.016840
    frame = imread(startname);
   
    processSubtitleRegion(startname, endname, frame);

    return 0;
}
