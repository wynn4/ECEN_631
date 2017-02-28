#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>


int main()
{
    //_STEP_1: read in all of the camera calibration data
    //Read in the individual camera calibration intrinsic parameters from calibration in Assignment 3
    cv::Mat cameraMatrixL;
    cv::Mat cameraMatrixR;
    cv::Mat distCoeffL;
    cv::Mat distCoeffR;

    cv::FileStorage fsL("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_L_parameters.xml", cv::FileStorage::READ);
    fsL["Camera_Matrix"] >> cameraMatrixL;
    fsL["Distortion_Coefficients"] >> distCoeffL;

    cv::FileStorage fsR("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_R_parameters.xml", cv::FileStorage::READ);
    fsR["Camera_Matrix"] >> cameraMatrixR;
    fsR["Distortion_Coefficients"] >> distCoeffR;

    //Read in the stereo calibration extrinsic parameters from calibration in Assignment 3 task_2
    cv::Mat R;
    cv::Mat T;
    cv::Mat E;
    cv::Mat F;

    cv::FileStorage fsS("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_2-Desktop_Qt_5_7_0_GCC_64bit-Default/stereo_calibration_parameters.xml", cv::FileStorage::READ);
    fsS["Stereo_Rotation_Matrix"] >> R;
    fsS["Stereo_Translation_Vector"] >> T;
    fsS["Stereo_Essential_Matirx"] >> E;
    fsS["Stereo_Fundamental_Matirx"] >> F;


    //_STEP_2: loop through the stereo images
    //define number of stereo image pairs
    int numImages = 41;

    //define filepath strings
    std::string stereoL = "/home/jesse/Desktop/ECEN_631/Assignment_4/ball/ballL";
    std::string stereoR = "/home/jesse/Desktop/ECEN_631/Assignment_4/ball/ballR";

    //create variables for imageNumber and imNum
    int imageNumber;
    std::string imNum;

    //Define Mat objects to hold the images
    cv::Mat imageL;
    cv::Mat imageR;

    //define first image to be the image that all others are differenced from
    cv::Mat diffImageL = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/ball/ballL0.bmp",1);
    cv::Mat diffImageR = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/ball/ballR0.bmp",1);

    //convert to grayscale
    cv::cvtColor(diffImageL,diffImageL,cv::COLOR_BGR2GRAY);
    cv::cvtColor(diffImageR,diffImageR,cv::COLOR_BGR2GRAY);

    //define ROI Rect for each camera to avoid processing entire image
    cv::Rect roiL = cv::Rect(cv::Point(diffImageL.cols-340,diffImageL.rows-430),cv::Point(diffImageL.cols-190,diffImageL.rows));
    cv::Rect roiR = cv::Rect(cv::Point(0,diffImageR.rows-430),cv::Point(diffImageR.cols-330,diffImageR.rows-50));

    //shrink differenced images to match ROI
    diffImageL = diffImageL(roiL);
    diffImageR = diffImageR(roiR);

    for(int i=1;i<numImages;i++)
    {
        imageNumber = i;
        imNum = std::to_string(imageNumber);

        //read in the images
        imageL = cv::imread(stereoL + imNum + ".bmp",1);
        imageR = cv::imread(stereoR + imNum + ".bmp",1);

        //convert to grayscale
        cv::cvtColor(imageL,imageL,cv::COLOR_BGR2GRAY);
        cv::cvtColor(imageR,imageR,cv::COLOR_BGR2GRAY);

        //only process inside ROI
        imageL = imageL(roiL);
        imageR = imageR(roiR);

        //absolute difference the images
        cv::absdiff(imageL,diffImageL,imageL);
        cv::absdiff(imageR,diffImageR,imageR);

        //blur to reduce noise
        cv::GaussianBlur(imageL,imageL,cv::Size(7,7),1.5,1.5);
        cv::GaussianBlur(imageR,imageR,cv::Size(7,7),1.5,1.5);

        //threshold the images
        cv::threshold(imageL,imageL,20,255,0);
        cv::threshold(imageR,imageR,20,255,0);

//        cv::imshow("Stereo Left", roiImageL);
        cv::imshow("Stereo Right", imageR);
        cv::waitKey(200); //~60fps 17
    }




    return 0;
}
