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

//    //plot the undistorted points
//    cv::circle(imageL,fourCornersLUndist[0],5,cv::Scalar(0,255,0),2,8,0);
//    cv::circle(imageL,fourCornersLUndist[1],5,cv::Scalar(0,255,0),2,8,0);
//    cv::circle(imageL,fourCornersLUndist[2],5,cv::Scalar(0,255,0),2,8,0);
//    cv::circle(imageL,fourCornersLUndist[3],5,cv::Scalar(0,255,0),2,8,0);

//    cv::circle(imageR,fourCornersRUndist[0],5,cv::Scalar(0,255,0),2,8,0);
//    cv::circle(imageR,fourCornersRUndist[1],5,cv::Scalar(0,255,0),2,8,0);
//    cv::circle(imageR,fourCornersRUndist[2],5,cv::Scalar(0,255,0),2,8,0);
//    cv::circle(imageR,fourCornersRUndist[3],5,cv::Scalar(0,255,0),2,8,0);

//    //Check that the y coords have been rectified
//    std::cout << fourCornersLUndist << std::endl;
//    std::cout << fourCornersRUndist << std::endl;

//    //Get the Disparity between corresponding points
//    float corner1Disp = fourCornersLUndist[0].x - fourCornersRUndist[0].x;
//    float corner2Disp = fourCornersLUndist[1].x - fourCornersRUndist[1].x;
//    float corner3Disp = fourCornersLUndist[2].x - fourCornersRUndist[2].x;
//    float corner4Disp = fourCornersLUndist[3].x - fourCornersRUndist[3].x;

//    //Construct vectors of 3D points [x, y, disparity] to pass into perspectiveTransform() function
//    cv::Point3f corner1L = cv::Point3f(fourCornersLUndist[0].x, fourCornersLUndist[0].y, corner1Disp);
//    cv::Point3f corner2L = cv::Point3f(fourCornersLUndist[1].x, fourCornersLUndist[1].y, corner2Disp);
//    cv::Point3f corner3L = cv::Point3f(fourCornersLUndist[2].x, fourCornersLUndist[2].y, corner3Disp);
//    cv::Point3f corner4L = cv::Point3f(fourCornersLUndist[3].x, fourCornersLUndist[3].y, corner4Disp);

//    cv::Point3f corner1R = cv::Point3f(fourCornersRUndist[0].x, fourCornersRUndist[0].y, corner1Disp);
//    cv::Point3f corner2R = cv::Point3f(fourCornersRUndist[1].x, fourCornersRUndist[1].y, corner2Disp);
//    cv::Point3f corner3R = cv::Point3f(fourCornersRUndist[2].x, fourCornersRUndist[2].y, corner3Disp);
//    cv::Point3f corner4R = cv::Point3f(fourCornersRUndist[3].x, fourCornersRUndist[3].y, corner4Disp);

//    std::vector<cv::Point3f> leftVec = {corner1L,corner2L,corner3L,corner4L};
//    std::vector<cv::Point3f> rightVec = {corner1R,corner2R,corner3R,corner4R};


//    //Use cv::perspectiveTransform() to calculate 3D info of the four corner points
//    std::vector<cv::Point3f> leftCam3DCoords;
//    std::vector<cv::Point3f> rightCam3DCoords;

//    cv::perspectiveTransform(leftVec,leftCam3DCoords,Q);
//    cv::perspectiveTransform(rightVec,rightCam3DCoords,Q);

//    //cout to see and check if the 3D points make sense
//    std::cout << leftCam3DCoords << std::endl;
//    std::cout << rightCam3DCoords << std::endl;

//    //set up everything to draw the points on the images
//    std::string corner1LStr = "[" + std::to_string(leftCam3DCoords[0].x) + ", " + std::to_string(leftCam3DCoords[0].y) + ", " + std::to_string(leftCam3DCoords[0].z) + "]";
//    std::string corner2LStr = "[" + std::to_string(leftCam3DCoords[1].x) + ", " + std::to_string(leftCam3DCoords[1].y) + ", " + std::to_string(leftCam3DCoords[1].z) + "]";
//    std::string corner3LStr = "[" + std::to_string(leftCam3DCoords[2].x) + ", " + std::to_string(leftCam3DCoords[2].y) + ", " + std::to_string(leftCam3DCoords[2].z) + "]";
//    std::string corner4LStr = "[" + std::to_string(leftCam3DCoords[3].x) + ", " + std::to_string(leftCam3DCoords[3].y) + ", " + std::to_string(leftCam3DCoords[3].z) + "]";

//    std::string corner1RStr = "[" + std::to_string(rightCam3DCoords[0].x) + ", " + std::to_string(rightCam3DCoords[0].y) + ", " + std::to_string(rightCam3DCoords[0].z) + "]";
//    std::string corner2RStr = "[" + std::to_string(rightCam3DCoords[1].x) + ", " + std::to_string(rightCam3DCoords[1].y) + ", " + std::to_string(rightCam3DCoords[1].z) + "]";
//    std::string corner3RStr = "[" + std::to_string(rightCam3DCoords[2].x) + ", " + std::to_string(rightCam3DCoords[2].y) + ", " +  std::to_string(rightCam3DCoords[2].z) + "]";
//    std::string corner4RStr = "[" + std::to_string(rightCam3DCoords[3].x) + ", " + std::to_string(rightCam3DCoords[3].y) + ", " + std::to_string(rightCam3DCoords[3].z) + "]";

//    float fontSize = 0.5;

//    cv::putText(imageL,corner1LStr,cv::Point(160,128),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageL,corner2LStr,cv::Point(324,181),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageL,corner3LStr,cv::Point(123,251),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageL,corner4LStr,cv::Point(306,302),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageL,"Red = Original Points",cv::Point(10,15),2,fontSize,cv::Scalar(0,0,255),1,8);
//    cv::putText(imageL,"Green = Undistorted and Rectified Points",cv::Point(10,30),2,fontSize,cv::Scalar(0,255,0),1,8);

//    cv::putText(imageR,corner1RStr,cv::Point(49,128),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageR,corner2RStr,cv::Point(234,182),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageR,corner3RStr,cv::Point(14,252),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageR,corner4RStr,cv::Point(198,302),2,fontSize,cv::Scalar(0,255,0),1,8);
//    cv::putText(imageR,"Red = Original Points",cv::Point(10,15),2,fontSize,cv::Scalar(0,0,255),1,8);
//    cv::putText(imageR,"Green = Undistorted and Rectified Points",cv::Point(10,30),2,fontSize,cv::Scalar(0,255,0),1,8);

//    //display the images
//    cv::imshow("Stereo Left",imageL);
//    cv::imshow("Stereo Right", imageR);
//    cv::waitKey(0);

    //write images to file
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/3DpointsL.bmp",imageL);
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_4/3DpointsR.bmp",imageR);

    cv::destroyAllWindows();

    return 0;
}
