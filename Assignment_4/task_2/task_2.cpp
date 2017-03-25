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
    int numImages = 26; //37 total

    //define filepath strings
    std::string stereoL = "/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testL";
    std::string stereoR = "/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testR";

    //create variables for imageNumber and imNum
    int imageNumber;
    std::string imNum;
    float fontSize = 0.5;

    //Define Mat objects to hold the images
    cv::Mat imageL;
    cv::Mat imageR;
    cv::Mat imageLOriginal;
    cv::Mat imageROriginal;

    //define first image to be the image that all others are differenced from
    cv::Mat diffImageL = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testL0.bmp",1);
    cv::Mat diffImageR = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testR0.bmp",1);

    //convert to grayscale
    cv::cvtColor(diffImageL,diffImageL,cv::COLOR_BGR2GRAY);
    cv::cvtColor(diffImageR,diffImageR,cv::COLOR_BGR2GRAY);

    //define ROI Rect for each camera to avoid processing entire image
    int offsetL_x = 300;
    int offsetL_y = 50;
    int offsetR_x = 210;
    int offsetR_y = 50;
    cv::Rect roiL = cv::Rect(cv::Point(offsetL_x,offsetL_y),cv::Point(diffImageL.cols-210,diffImageL.rows-300));
    cv::Rect roiR = cv::Rect(cv::Point(offsetR_x,offsetR_y),cv::Point(diffImageR.cols-330,diffImageR.rows-290));

    //shrink differenced images to match ROI
    diffImageL = diffImageL(roiL);
    diffImageR = diffImageR(roiR);

    //setup blob detector and parameters
    cv::SimpleBlobDetector::Params params;

    // Change thresholds
    //params.minThreshold = 10;
    //params.maxThreshold = 200;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 10;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.35;


    //create detector object
    cv::SimpleBlobDetector detector(params);

    //create keypoints vector
    std::vector<cv::KeyPoint> keypointsL;
    std::vector<cv::KeyPoint> keypointsR;

    //define variables to hold ballPoint x and y components;
    float ballPointL_x;
    float ballPointL_y;
    float ballPointR_x;
    float ballPointR_y;
    cv::Point2f ballPointL;
    cv::Point2f ballPointR;

    //Create ball pixel point vector;
    std::vector<cv::Point2f> ballPointsL;
    std::vector<cv::Point2f> ballPointsR;


    for(int i=1;i<numImages;i++)
    {
        imageNumber = i;
        imNum = std::to_string(imageNumber);

        //read in the images
        imageLOriginal = cv::imread(stereoL + imNum + ".bmp",1);
        imageROriginal = cv::imread(stereoR + imNum + ".bmp",1);

        //only process inside ROI
        imageL = imageLOriginal(roiL);
        imageR = imageROriginal(roiR);

        //convert to grayscale
        cv::cvtColor(imageL,imageL,cv::COLOR_BGR2GRAY);
        cv::cvtColor(imageR,imageR,cv::COLOR_BGR2GRAY);

        //absolute difference the images
        cv::absdiff(imageL,diffImageL,imageL);
        cv::absdiff(imageR,diffImageR,imageR);

        //threshold the images
        cv::threshold(imageL,imageL,15,255,1);
        cv::threshold(imageR,imageR,15,255,1);

        //detect ball
        detector.detect(imageL,keypointsL);
        detector.detect(imageR,keypointsR);

        //draw the keypoints
        cv::drawKeypoints(imageL, keypointsL, imageL, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::drawKeypoints(imageR, keypointsR, imageR, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        //std::cout << keypointsR.size() << std::endl;

        //store the keypoint
        if(i>1 && i<numImages-1)    //we don't get keypoints on the first and last frame in both cameras
        {
            //get the keypoint coordinates and add in the offset
            ballPointL_x = keypointsL[0].pt.x + offsetL_x;
            ballPointL_y = keypointsL[0].pt.y + offsetL_y;
            ballPointR_x = keypointsR[0].pt.x + offsetR_x;
            ballPointR_y = keypointsR[0].pt.y + offsetR_y;

            ballPointL = cv::Point2f(ballPointL_x,ballPointL_y);
            ballPointR = cv::Point2f(ballPointR_x,ballPointR_y);

            ballPointsL.push_back(ballPointL);
            ballPointsR.push_back(ballPointR);
        }

        cv::imshow("Stereo Left", imageL);
        cv::imshow("Stereo Right", imageR);
        cv::waitKey(1700); //~60fps = 17

//        //write to image file on 1st, 5th, 10th, 15th, 20th frames
//        if(i == 2)
//        {
//            cv::circle(imageLOriginal,ballPointL,5,cv::Scalar(0,255,0),2,8);
//            cv::circle(imageROriginal,ballPointR,5,cv::Scalar(0,255,0),2,8);
//            cv::putText(imageLOriginal,"Left Camera Frame 1",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::putText(imageROriginal,"Right Camera Frame 1",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameL1.bmp",imageLOriginal);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameR1.bmp",imageROriginal);
//        }
//        else if(i == 6)
//        {
//            cv::circle(imageLOriginal,ballPointL,5,cv::Scalar(0,255,0),2,8);
//            cv::circle(imageROriginal,ballPointR,5,cv::Scalar(0,255,0),2,8);
//            cv::putText(imageLOriginal,"Left Camera Frame 5",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::putText(imageROriginal,"Right Camera Frame 5",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameL5.bmp",imageLOriginal);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameR5.bmp",imageROriginal);
//        }
//        else if(i==11)
//        {
//            cv::circle(imageLOriginal,ballPointL,5,cv::Scalar(0,255,0),2,8);
//            cv::circle(imageROriginal,ballPointR,5,cv::Scalar(0,255,0),2,8);
//            cv::putText(imageLOriginal,"Left Camera Frame 10",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::putText(imageROriginal,"Right Camera Frame 10",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameL10.bmp",imageLOriginal);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameR10.bmp",imageROriginal);
//        }
//        else if(i==16)
//        {
//            cv::circle(imageLOriginal,ballPointL,5,cv::Scalar(0,255,0),2,8);
//            cv::circle(imageROriginal,ballPointR,5,cv::Scalar(0,255,0),2,8);
//            cv::putText(imageLOriginal,"Left Camera Frame 15",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::putText(imageROriginal,"Right Camera Frame 15",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameL15.bmp",imageLOriginal);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameR15.bmp",imageROriginal);
//        }
//        else if(i==21)
//        {
//            cv::circle(imageLOriginal,ballPointL,5,cv::Scalar(0,255,0),2,8);
//            cv::circle(imageROriginal,ballPointR,5,cv::Scalar(0,255,0),2,8);
//            cv::putText(imageLOriginal,"Left Camera Frame 20",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::putText(imageROriginal,"Right Camera Frame 20",cv::Point(100,30),2,fontSize,cv::Scalar(0,255,0),1,8);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameL20.bmp",imageLOriginal);
//            cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/frameR20.bmp",imageROriginal);
//        }
    }

    //write ballPointsL and ballPointsR to file
    cv::FileStorage fsLpoints("/home/jesse/Desktop/ECEN_631/Assignment_4/ballPointsL.xml", cv::FileStorage::WRITE);
    fsLpoints << "ballPointsL" << ballPointsL;
    fsLpoints << "ballPointsR" << ballPointsR;

    return 0;
}
