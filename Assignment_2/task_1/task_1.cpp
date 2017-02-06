#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

int main()
{
    //Image String
    std::string chess_board_1 = "/home/jesse/Desktop/ECEN_631/Assignment_2/Calibration_Images_BMP/AR1.bmp";

    //Define board Size(width x height) of inner chessboard corners
    cv::Size board_size = cv::Size(10,7);

    //Name the window
    std::string window_name = "Task_1";

    //Define image objects
    cv::Mat image;
    cv::Mat image_gray;
    cv::Mat image_color;

    //Vector to store corners
    std::vector<cv::Point2f> corners;

    //Read the image
    image = cv::imread(chess_board_1, 1);
    image_color = cv::imread(chess_board_1, 1);

    //Convert_to_Grayscale
    cv::cvtColor(image, image_gray, 6); //convert to grayscale (COLOR_BGR2GRAY == 6)

    //find Chessboard corners
    bool corners_found = cv::findChessboardCorners(image_gray,board_size,corners,1 + 2);

    if(corners_found)
    {
        cv::cornerSubPix(image_gray, corners, cv::Size(11, 11), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        //Draw on the colored image so it's pretty
        cv::drawChessboardCorners(image_color, board_size, corners, corners_found);
    }
    else
    {
        std::cout << "Corners not found" << std::endl;
    }

    //show the image
    cv::imshow(window_name, image_color);
    cv::waitKey(0);

    return 0;
}
