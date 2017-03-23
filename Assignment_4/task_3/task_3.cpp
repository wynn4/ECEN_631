#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <eigen3/Eigen/Dense>


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

    //convert 3D ball coordinates measured from left camera to be relative to the catcher coordinates
    int x_offset = 10;
    int y_offset = -36;
    int z_offset = -5;
    cv::Point3f leftToCatcher;
    cv::Point3f rightToCatcher;
    //std::vector<cv::Point3f> ballLeftCatcher;
    //std::vector<cv::Point3f> ballRigthCatcher;
    std::vector<cv::Point3f> allBallPoints; //L and R combined
    for(int i=0;i<leftCam3DBallCoords.size();i++)
    {
        leftToCatcher = cv::Point3f(leftCam3DBallCoords[i].x + x_offset, leftCam3DBallCoords[i].y + y_offset, leftCam3DBallCoords[i].z + z_offset);
        rightToCatcher = cv::Point3f(rightCam3DBallCoordsInLeftCoordFrame[i].x + x_offset, rightCam3DBallCoordsInLeftCoordFrame[i].y + y_offset, rightCam3DBallCoordsInLeftCoordFrame[i].z + z_offset);
        //ballLeftCatcher.push_back(leftToCatcher);
        //ballRigthCatcher.push_back(rightToCatcher);
        allBallPoints.push_back(leftToCatcher);
        allBallPoints.push_back(rightToCatcher);
    }


    //massage the data into format acceptable to class Eigen
    Eigen::VectorXf xPoints(allBallPoints.size());
    Eigen::VectorXf yPoints(allBallPoints.size());
    Eigen::VectorXf zPoints(allBallPoints.size());

    for(int i=0;i<allBallPoints.size();i++)
    {
        xPoints(i) = allBallPoints[i].x;
        yPoints(i) = allBallPoints[i].y;
        zPoints(i) = allBallPoints[i].z;
    }
    //std::cout << xPoints << std::endl;

    //make vectors and matrix in preparation for computing LS solution
    Eigen::VectorXf cubeTermZ(allBallPoints.size());
    Eigen::VectorXf squareTermZ(allBallPoints.size());
    Eigen::VectorXf singleTermZ(allBallPoints.size());
    Eigen::VectorXf constTermZ(allBallPoints.size());

    for(int i=0;i<allBallPoints.size();i++)
    {
        cubeTermZ(i) = zPoints(i)*zPoints(i)*zPoints(i);
        squareTermZ(i) = zPoints(i)*zPoints(i);
        singleTermZ(i) = zPoints(i);
        constTermZ(i) = 1;
    }

    //create the A matrix
    Eigen::MatrixXf A_yz(allBallPoints.size(),4);
    Eigen::VectorXf b_yz(allBallPoints.size());
    A_yz << cubeTermZ, squareTermZ, singleTermZ, constTermZ;
    b_yz = yPoints;

    std::cout << A_yz << std::endl;

    //solve using least squares
//    std::cout << "The solution using the QR decomposition is:\n"
//         << A_yz.colPivHouseholderQr().solve(b_yz) << std::endl;



//    //write 3D data to xml file and file to be read by MATLAB
//    cv::FileStorage fs3D("/home/jesse/Desktop/ECEN_631/Assignment_4/3DWorldBallPointsL.xml", cv::FileStorage::WRITE);
//    fs3D << "leftCam3DBallCoords" << ballLeftCatcher;
//    fs3D << "rightCam3DBallCoords" << ballRigthCatcher;

//    std::ofstream matlabfile1;
//    matlabfile1.open("/home/jesse/Desktop/ECEN_631/Assignment_4/leftCam3DBallCoords.txt");
//    matlabfile1 << cv::format(ballLeftCatcher,"MATLAB");
//    matlabfile1.close();

//    std::ofstream matlabfile2;
//    matlabfile2.open("/home/jesse/Desktop/ECEN_631/Assignment_4/rightCam3DBallCoords.txt");
//    matlabfile2 << cv::format(ballRigthCatcher,"MATLAB");
//    matlabfile2.close();

//    //convert 3D points to two sets of 2D points (x,z) and (y,z)
//    std::vector<cv::Point2f> XZPoints;
//    std::vector<cv::Point2f> YZPoints;
//    for(int i=0;i<ballLeftCatcher.size();i++)
//    {
//        float x_coord = ballLeftCatcher[i].x;
//        float y_coord = ballLeftCatcher[i].y;
//        float z_coord = ballLeftCatcher[i].z;

//        cv::Point2f xz = cv::Point2f(x_coord,z_coord);
//        cv::Point2f yz = cv::Point2f(y_coord,z_coord);

//        XZPoints.push_back(xz);
//        YZPoints.push_back(yz);
//    }

    //use approxPolyDP to fit a curve to the data
    std::vector<cv::Point2f> XZOutputCurve;
    std::vector<cv::Point2f> YZOutputCurve;
    double epsilon = 0.1;

//    cv::approxPolyDP(XZPoints,XZOutputCurve,epsilon,false);

//    Eigen::VectorXf test(4);
//    test << 0, 1, 2, 3;
//    std::cout << test << std::endl;
//    Eigen::VectorXf cubed(4);



//    std::cout << A << std::endl;

    return 0;
}
