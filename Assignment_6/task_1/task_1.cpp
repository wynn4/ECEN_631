#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
    //set which image set are we working with
    std::string imageFolder = "parallel_cube";
    std::string imageName = "ParallelCube";


    //Read first and last points from file (from last assignment) and store in vectors
    std::vector<cv::Point2f> firstPoints;
    std::vector<cv::Point2f> lastPoints;

    cv::FileStorage fs("/home/jesse/Desktop/ECEN_631/Assignment_6/first_and_last_" + imageFolder + ".xml", cv::FileStorage::READ);
    fs["first_points"] >> firstPoints;
    fs["last_points"] >> lastPoints;


    //Use first and last points to estimate fundamental matrix F
    cv::Mat F;
    F = cv::findFundamentalMat(firstPoints,lastPoints);


    //Use stereoRectifyUncalibrated() to compute rectification homographies H1 and H2
    cv::Mat H1;
    cv::Mat H2;

    cv::stereoRectifyUncalibrated(firstPoints,lastPoints,F,cv::Size(640,480),H1,H2);


    //Make a guess for the camera calibration matrix (intrinsic matrix) M1 and M2 and distCoefficients
    double fx = 820;
    double fy = 820;
    double cx = 320;
    double cy = 240;

    double k1 = 0;
    double k2 = 0;
    double p1 = 0;
    double p2 = 0;
    double k3 = 0;

    double intrinsicGuess[9] = {fx, 0, cx,
                                0, fy, cy,
                                0, 0, 1};

    double distGuess[5] = {k1, k2, p1, p2, k3};


    cv::Mat M1(3,3, CV_64F, intrinsicGuess);
    cv::Mat M2(3,3, CV_64F, intrinsicGuess);

    cv::Mat dist1(5,1, CV_64F, distGuess);
    cv::Mat dist2(5,1, CV_64F, distGuess);

    //Calculate R1 and R2
    cv::Mat R1 = M1.inv()*H1*M1;
    cv::Mat R2 = M2.inv()*H2*M2;

    //get the undistortRectifyMap
    cv::Mat map1First;
    cv::Mat map2First;
    cv::Mat map1Last;
    cv::Mat map2Last;

    cv::initUndistortRectifyMap(M1,dist1,R1,M1,cv::Size(640,480),CV_32FC1,map1First,map2First);







    cv::destroyAllWindows();

    return 0;
}







