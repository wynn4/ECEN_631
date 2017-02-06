#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

int main()
{
    //Image String
    std::string chess_board_1 = "/home/jesse/Desktop/ECEN_631/Assignment_2/Calibration_Images_BMP/AR1.bmp";

    //Create a namedWindow
    std::string window_name = "Task_1";
    cv::namedWindow(window_name, 1);

    //Define image objects
    cv::Mat image;
    cv::Mat image_gray;

    //Read the image
    image = cv::imread(chess_board_1, 1);

    //Convert_to_Grayscale
    cv::cvtColor(image, image_gray, 6); //convert to grayscale (COLOR_BGR2GRAY == 6)


    //show the image
    cv::imshow(window_name, image_gray);
    cv::waitKey(0);

    return 0;
}
