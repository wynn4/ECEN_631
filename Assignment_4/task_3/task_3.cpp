#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>


int main()
{
    //read in the ballPoints from task_2
    std::vector<cv::Point2f> ballPointsL;
    std::vector<cv::Point2f> ballPointsR;

    cv::FileStorage fsPoints("/home/jesse/Desktop/ECEN_631/Assignment_4/ballPointsL.xml", cv::FileStorage::READ);
    fsPoints["ballPointsL"] >> ballPointsL;
    fsPoints["ballPointsR"] >> ballPointsR;

    //read in all of the camera calibration data
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

    //Get the stereo rectification params
    //initialize output Mats
    cv::Mat R1(3,3, CV_64F);
    cv::Mat R2(3,3, CV_64F);
    cv::Mat P1(3,4, CV_64F);
    cv::Mat P2(3,4, CV_64F);
    cv::Mat Q(4,4, CV_64F);;

    cv::stereoRectify(cameraMatrixL,distCoeffL,cameraMatrixR,distCoeffR,cv::Size(640,480),R,T,R1,R2,P1,P2,Q,0,-1,cv::Size(640,480),0,0);


    //Undistort the points
    std::vector<cv::Point2f> ballPointsLUndist;
    std::vector<cv::Point2f> ballPointsRUndist;

    cv::undistortPoints(ballPointsL,ballPointsLUndist,cameraMatrixL,distCoeffL,R1,P1);
    cv::undistortPoints(ballPointsR,ballPointsRUndist,cameraMatrixR,distCoeffR,R2,P2);


    //Construct vectors of points [x, y, disparity] in a loop to pass into perspectiveTransform() function
    cv::Point3f ballPointL;
    cv::Point3f ballPointR;
    std::vector<cv::Point3f> ballPointsLVec;
    std::vector<cv::Point3f> ballPointsRVec;

    for(int i=0;i<ballPointsLUndist.size();i++)
    {
        //calculate the disparity
        float disparity = ballPointsLUndist[i].x - ballPointsRUndist[i].x;

        //create the points
        ballPointL = cv::Point3f(ballPointsLUndist[i].x, ballPointsLUndist[i].y, disparity);
        ballPointR = cv::Point3f(ballPointsRUndist[i].x, ballPointsRUndist[i].y, disparity);

        //push the points onto the vectors
        ballPointsLVec.push_back(ballPointL);
        ballPointsRVec.push_back(ballPointR);
    }

    //Use cv::perspectiveTransform() to calculate 3D info of the four corner points
    std::vector<cv::Point3f> leftCam3DBallCoords;
    std::vector<cv::Point3f> rightCam3DBallCoords;

    cv::perspectiveTransform(ballPointsLVec,leftCam3DBallCoords,Q);
    cv::perspectiveTransform(ballPointsRVec,rightCam3DBallCoords,Q);

    //convert 3D ball coordinates measured from the right camera to be measured from the left camera
    cv::Point3f rightToLeft;
    std::vector<cv::Point3f> rightCam3DBallCoordsInLeftCoordFrame;
    for(int i=0;i<rightCam3DBallCoords.size();i++)
    {
        rightToLeft = cv::Point3f(rightCam3DBallCoords[i].x - T.at<double>(0,0), rightCam3DBallCoords[i].y - T.at<double>(0,1), rightCam3DBallCoords[i].z - T.at<double>(0,2));
        rightCam3DBallCoordsInLeftCoordFrame.push_back(rightToLeft);
    }

    //write 3D data to file to be read by MATLAB
    cv::FileStorage fs3D("/home/jesse/Desktop/ECEN_631/Assignment_4/3DWorldBallPointsL.xml", cv::FileStorage::WRITE);
    fs3D << "leftCam3DBallCoords" << leftCam3DBallCoords;
    fs3D << "rightCam3DBallCoords" << rightCam3DBallCoordsInLeftCoordFrame;



    return 0;
}
