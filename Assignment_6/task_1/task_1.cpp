#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
    //set which image set we are working with
    std::string imageFolder = "turned_real";  //parallel_cube parallel_real turned_cube turned real
    std::string imageName = "TurnReal";     //ParallelCube ParallelReal TurnCube TurnReal


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
    double fx = 825;
    double fy = 825;
    double cx = 320;
    double cy = 240;

    double k1 = -0.1;
    double k2 = 0.05;
    double p1 = 0;
    double p2 = -0.001;
    double k3 = 0.2;

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

    std::cout << R1 << std::endl;
    std::cout << R2 << std::endl;

    //get the undistortRectifyMap
    cv::Mat map1First;
    cv::Mat map2First;
    cv::Mat map1Last;
    cv::Mat map2Last;

    cv::initUndistortRectifyMap(M1,dist1,R1,M1,cv::Size(640,480),CV_32FC1,map1First,map2First);
    cv::initUndistortRectifyMap(M2,dist2,R2,M2,cv::Size(640,480),CV_32FC1,map1Last,map2Last);


    //load images from file and remap()
    cv::Mat firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + "/" + imageName + "10.jpg", 1);
    cv::Mat lastFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + "/" + imageName + "15.jpg", 1);

    cv::remap(firstFrame,firstFrame,map1First,map2First,cv::INTER_NEAREST,cv::BORDER_CONSTANT);
    cv::remap(lastFrame,lastFrame,map1Last,map2Last,cv::INTER_NEAREST,cv::BORDER_CONSTANT);

    //draw some lines to see how well it rectified
//    //parallel_cube
//    int topLineY = 53;
//    int midLineY = 246;
//    int botLineY = 457;

//    //parallel_real
//    int topLineY = 63;
//    int midLineY = 258;
//    int botLineY = 415;

//    //turned_cube
//    int topLineY = 63;
//    int midLineY = 234;
//    int botLineY = 375;

    //turned_real
    int topLineY = 53;
    int midLineY = 246;
    int botLineY = 457;

    cv::line(firstFrame,cv::Point(0,topLineY),cv::Point(640,topLineY),cv::Scalar(0,255,0));
    cv::line(firstFrame,cv::Point(0,midLineY),cv::Point(640,midLineY),cv::Scalar(0,255,0));
    cv::line(firstFrame,cv::Point(0,botLineY),cv::Point(640,botLineY),cv::Scalar(0,255,0));

    cv::line(lastFrame,cv::Point(0,topLineY),cv::Point(640,topLineY),cv::Scalar(0,255,0));
    cv::line(lastFrame,cv::Point(0,midLineY),cv::Point(640,midLineY),cv::Scalar(0,255,0));
    cv::line(lastFrame,cv::Point(0,botLineY),cv::Point(640,botLineY),cv::Scalar(0,255,0));


    //show the rectified images
    cv::imshow("First Frame", firstFrame);
    cv::imshow("Last Frame", lastFrame);
    cv::waitKey(0);


//    //write images to file
//    cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_6/task_1/" + imageName + "First.jpg",firstFrame);
//    cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_6/task_1/" + imageName + "Last.jpg",lastFrame);




    cv::destroyAllWindows();

    return 0;
}







