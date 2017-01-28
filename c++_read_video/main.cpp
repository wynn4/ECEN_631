#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

int main()
{
    cv::VideoCapture cap(0);    //open camera
    cv::namedWindow("image", 1);
    cv::Mat frame;
    cv::Mat gray_frame;
    cv::Mat blur_frame;
    cv::Mat thresh_frame;

    //int esc_key = 1048603;  //desktop
    int esc_key = 27;       //laptop

    while(true)
    {
        cap >> frame;
        //cv::cvtColor(frame, gray_frame, 6); //COLOR_BGR2GRAY == 6
        //cv::GaussianBlur(gray_frame,blur_frame, cv::Size(7,7), 1.5, 1.5);
        //cv::threshold(blur_frame,thresh_frame,100,255,0);
        cv::imshow("image", frame);
        if(cv::waitKey(30) == esc_key)
            break;
    }
    return 0;
}
