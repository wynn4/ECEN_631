#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>


int main()
{
    //get the path to the stereo image pair
    std::string leftImagePath = "/home/jesse/Desktop/ECEN_631/Assignment_3/stereo_images/stereoL53.bmp";
    std::string rightImagePath = "/home/jesse/Desktop/ECEN_631/Assignment_3/stereo_images/stereoR53.bmp";

    //Create vectors to hold the corner locations
    std::vector<cv::Point2f> cornersL;
    std::vector<cv::Point2f> cornersR;

    //create vectors to hold the image points
    std::vector<std::vector<cv::Point2f>> imagePointsL;
    std::vector<std::vector<cv::Point2f>> imagePointsR;

    //read in the images
    cv::Mat imageL = cv::imread(leftImagePath,1);
    cv::Mat imageR = cv::imread(rightImagePath,1);

    //get the image size and input board size
    cv::Size imageSize = imageL.size();
    cv::Size boardSize = cv::Size(10,7);

    //convert to grayscale
    cv::cvtColor(imageL,imageL,cv::COLOR_BGR2GRAY);
    cv::cvtColor(imageR,imageR,cv::COLOR_BGR2GRAY);

    //find the chessboard corners
    cv::findChessboardCorners(imageL,boardSize,cornersL,1 + 2);    //don't worry storing the returned boolean
    cv::findChessboardCorners(imageR,boardSize,cornersR,1 + 2);


    //Refine corner locations
    cv::cornerSubPix(imageL, cornersL, cv::Size(5, 5), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 60, 0.001));
    cv::cornerSubPix(imageR, cornersR, cv::Size(5, 5), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 60, 0.001));

    //Draw on the colored images so it's pretty
    cv::cvtColor(imageL,imageL,cv::COLOR_GRAY2BGR);
    cv::cvtColor(imageR,imageR,cv::COLOR_GRAY2BGR);

    //Get the four corners
    std::vector<cv::Point2f> fourCornersL = {cornersL[0],cornersL[9],cornersL[60],cornersL[69]};
    std::vector<cv::Point2f> fourCornersR = {cornersR[0],cornersR[9],cornersR[60],cornersR[69]};

    cv::drawChessboardCorners(imageL, boardSize, fourCornersL, false);
    cv::drawChessboardCorners(imageR,boardSize,fourCornersR,false);

    if(cornersL.size() == boardSize.width*boardSize.height && cornersR.size() == boardSize.width*boardSize.height)
    {
        //Initialize vector to hold ImagePoints for each calibration image
        std::vector<cv::Point2f> ImgPtsL;
        std::vector<cv::Point2f> ImgPtsR;

        //Populate the ImgPts and ObjPts Vectors
        for(int i=0;i<cornersL.size();i++)
        {
            cv::Point2f ImgPtL;
            cv::Point2f ImgPtR;

            ImgPtL.x = cornersL[i].x;
            ImgPtL.y = cornersL[i].y;

            ImgPtR.x = cornersR[i].x;
            ImgPtR.y = cornersR[i].y;

            //Push each ImgPt onto the ImgPts vector
            ImgPtsL.push_back(ImgPtL);
            ImgPtsR.push_back(ImgPtR);
        }
        //Push the ImagePts vector onto the imagePoints vector
        imagePointsL.push_back(ImgPtsL);
        imagePointsR.push_back(ImgPtsR);
    }
    else
    {
        std::cout << "Board size and corner size mismatch (all corners may not have been detected).";
    }

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

    //Stereo Rectify the two images
    //initialize output Mats
    cv::Mat R1(3,3, CV_64F);
    cv::Mat R2(3,3, CV_64F);
    cv::Mat P1(3,4, CV_64F);
    cv::Mat P2(3,4, CV_64F);
    cv::Mat Q(4,4, CV_64F);;

    cv::stereoRectify(cameraMatrixL,distCoeffL,cameraMatrixR,distCoeffR,imageSize,R,T,R1,R2,P1,P2,Q,0,-1,imageSize,0,0);

    //Undistort the four corner points
    std::vector<cv::Point2f> fourCornersLUndist;
    std::vector<cv::Point2f> fourCornersRUndist;

    cv::undistortPoints(fourCornersL,fourCornersLUndist,cameraMatrixL,distCoeffL,R1,P1);
    cv::undistortPoints(fourCornersR,fourCornersRUndist,cameraMatrixR,distCoeffR,R2,P2);

    //use cv::perspectiveTransform() to calculate 3D info of the four corner points
    //
    //see "Canonical Configuration OpenCV Function" Slide in 3D Reconstruction.pdf (also need to calculate disparity for the four corners)
    //
    //cv::perspectiveTransform();

    cv::imshow("Stereo Left",imageL);
    cv::imshow("Stereo Right", imageR);
    cv::waitKey(0);




    /*
    //ObjectPoints and ImagePoints gathered, time to calibrate
    //initialize rvecs and tvecs
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;

    //initialize Intrinsic_Matrix and Distortion_Coefficients
    cv::Mat Intrinsic_Matrix(3,3, CV_64F);
    cv::Mat Distortion_Coefficients(5,1,CV_64F);


    //calibrate using calibrateCamera function
    cv::calibrateCamera(objectPoints,imagePoints,imageSize,Intrinsic_Matrix,Distortion_Coefficients,rvecs,tvecs);



    //Write Calibration Intrinsic and Distortion parameters to .txt file

    std::string filename = "camera_R_parameters.txt";
    std::FILE* file;
    file = std::fopen(filename.c_str(),"w");
    std::fprintf(file,"Assignment3 Task_1 Right Camera Calibration File\n\n");
    std::fprintf(file,"Camera Intrinsic Parameters:\n\n");
    for(int i=0;i<Intrinsic_Matrix.rows;i++)
    {
        for(int j=0;j<Intrinsic_Matrix.cols;j++)
        {
            std::fprintf(file,"%lf ", Intrinsic_Matrix.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n");
    std::fprintf(file,"Camera Distortion Coefficients:\n\n");
    for(int i=0;i<Distortion_Coefficients.rows;i++)
    {
        std::fprintf(file,"%lf ", Distortion_Coefficients.at<double>(i));
        std::fprintf(file,"\n");
    }
    std::fclose(file);

    //Write Calibration Intrinsic and Distortion parameters to .xml file (this is the preferred method)
    cv::FileStorage fs("camera_R_parameters.xml", cv::FileStorage::WRITE);
    fs << "Camera_Matrix" << Intrinsic_Matrix;
    fs << "Distortion_Coefficients" << Distortion_Coefficients;

    //If we were to read this file it would look like:
    //cv::FileStorage fs("camera_calibration_parameters.xml", cv::FileStorage::READ);
    //fs["Camera_Matrix"] >> Intrinsic_Matrix;
    //fs["Distortion_Coefficients"] >> Distortion_Coefficients;
    */

    cv::destroyAllWindows();

    return 0;
}
