#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

float catcher_x_offset = -10;
float catcher_y_offset = -20;

//calibration objects
cv::Mat cameraMatrixL;
cv::Mat cameraMatrixR;
cv::Mat distCoeffL;
cv::Mat distCoeffR;

cv::Mat R;
cv::Mat T;
cv::Mat E;
cv::Mat F;
cv::Mat R1;
cv::Mat R2;
cv::Mat P1;
cv::Mat P2;
cv::Mat Q;

//Define Mat objects to hold the images
cv::Mat imageL;
cv::Mat imageR;
cv::Mat imageLOriginal;
cv::Mat imageROriginal;

//define ROI Rect for each camera to avoid processing entire image
int offsetL_x = 300;
int offsetL_y = 50;
int offsetR_x = 210;
int offsetR_y = 50;
cv::Rect roiL = cv::Rect(cv::Point(offsetL_x,offsetL_y),cv::Point(640-210,480-300));
cv::Rect roiR = cv::Rect(cv::Point(offsetR_x,offsetR_y),cv::Point(640-330,480-290));

int number_good_frames = 0;

std::vector<cv::Point2f> ballPointsLUndist;
std::vector<cv::Point2f> ballPointsRUndist;

cv::Point3f ballPointDispL;
cv::Point3f ballPointDispR;
std::vector<cv::Point3f> ballPointsDispL;
std::vector<cv::Point3f> ballPointsDispR;

std::vector<cv::Point3f> my3DBallCoordsL;
std::vector<cv::Point3f> my3DBallCoordsR;
std::vector<cv::Point3f> my3DBallCoordsALL;



cv::Mat A_inv;
cv::Mat x_yz(4,1, CV_32F);
cv::Mat x_xz(4,1, CV_32F);




//function prototypes
void readCalibrationData();

int main()
{
    //_STEP_1: read in all of the camera calibration data
    //Read in the individual camera calibration intrinsic parameters from calibration in Assignment 3

    readCalibrationData();


    //_STEP_2: loop through the stereo images
    //define number of stereo image pairs
    int numImages = 38; //37 total

    //define filepath strings
    std::string stereoL = "/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testL";
    std::string stereoR = "/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testR";

    //create variables for imageNumber and imNum
    int imageNumber;
    std::string imNum;


    //define first image to be the image that all others are differenced from
    cv::Mat diffImageL = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testL0.bmp",CV_LOAD_IMAGE_GRAYSCALE);
    cv::Mat diffImageR = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testR0.bmp",CV_LOAD_IMAGE_GRAYSCALE);


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



        //check to make sure that each camera got a keypoint
        if(keypointsL.size() == 1 && keypointsR.size() == 1)
        {

            //draw the keypoints
            cv::drawKeypoints(imageL, keypointsL, imageL, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            cv::drawKeypoints(imageR, keypointsR, imageR, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
            //std::cout << keypointsR.size() << std::endl;

            //get the keypoint coordinates and add in the offset
            ballPointL_x = keypointsL[0].pt.x + offsetL_x;
            ballPointL_y = keypointsL[0].pt.y + offsetL_y;
            ballPointR_x = keypointsR[0].pt.x + offsetR_x;
            ballPointR_y = keypointsR[0].pt.y + offsetR_y;

            ballPointL = cv::Point2f(ballPointL_x,ballPointL_y);  //zero is a place-holder for disparity (which we'll add in a sec)
            ballPointR = cv::Point2f(ballPointR_x,ballPointR_y);

            ballPointsL.push_back(ballPointL);
            ballPointsR.push_back(ballPointR);

            number_good_frames++;

        }


        //after we've got 10 frames, calculate the coordinates and move catcher, then move it every time after that
        if(number_good_frames >= 15)
        {
            //undistort the points
            cv::undistortPoints(ballPointsL,ballPointsLUndist,cameraMatrixL,distCoeffL,R1,P1);
            cv::undistortPoints(ballPointsR,ballPointsRUndist,cameraMatrixR,distCoeffR,R2,P2);



            //create ballPointsVector with disparity
            for(int i=0;i<ballPointsLUndist.size();i++)
            {
                //calculate the disparity
                float disparity = ballPointsLUndist[i].x - ballPointsRUndist[i].x;

                //create the points
                ballPointDispL = cv::Point3f(ballPointsLUndist[i].x, ballPointsLUndist[i].y, disparity);
                ballPointDispR = cv::Point3f(ballPointsRUndist[i].x, ballPointsRUndist[i].y, disparity);

                //push the points onto the vectors
                ballPointsDispL.push_back(ballPointDispL);  //____needs to be reset each frame
                ballPointsDispR.push_back(ballPointDispR);  //____needs to be reset each frame
            }

            //std::cout << ballPointsDispL.size() << std::endl;

            //calculate ball 3D coordinates relative to each camera
            cv::perspectiveTransform(ballPointsDispL,my3DBallCoordsL,Q);
            cv::perspectiveTransform(ballPointsDispR,my3DBallCoordsR,Q);

            //std::cout << my3DBallCoordsL.size() << std::endl;

            //convert my3DBallCoordsR to be with respect to the left camera
            for(int i=0;i<my3DBallCoordsR.size();i++)
            {
                my3DBallCoordsR[i].x = my3DBallCoordsR[i].x - T.at<double>(0,0);
                my3DBallCoordsR[i].y = my3DBallCoordsR[i].y - T.at<double>(0,1);
                my3DBallCoordsR[i].z = my3DBallCoordsR[i].z - T.at<double>(0,2);
            }

            //add my3DBallCoords(R and L) to the my3DBallCoordsALL vector
            for(int i=0;i<my3DBallCoordsL.size();i++)
            {
                my3DBallCoordsALL.push_back(my3DBallCoordsL[i]);  //____needs to be reset each frame
                my3DBallCoordsALL.push_back(my3DBallCoordsR[i]);  //____needs to be reset each frame
            }

            //std::cout << my3DBallCoordsALL.size() << std::endl;


            //____________OPENCV_METHOD for Least Squares _____________
            //y = Ax^3 + Bx^2 + Cx + D

            //create A_y Mat and b_z Mat
            cv::Mat A(my3DBallCoordsALL.size(),4, CV_32F);
            cv::Mat b_yz(my3DBallCoordsALL.size(),1, CV_32F);
            cv::Mat b_xz(my3DBallCoordsALL.size(),1, CV_32F);

            //populate the A matrix and the b matrix for the yz data
            for(int i=0;i<my3DBallCoordsALL.size();i++)
            {
                //y = Ax^3 + Bx^2 + Cx + D
                A.at<float>(i,0) = my3DBallCoordsALL[i].z * my3DBallCoordsALL[i].z * my3DBallCoordsALL[i].z;
                A.at<float>(i,1) = my3DBallCoordsALL[i].z * my3DBallCoordsALL[i].z;
                A.at<float>(i,2) = my3DBallCoordsALL[i].z;
                A.at<float>(i,3) = 1;

                b_yz.at<float>(i,0) = my3DBallCoordsALL[i].y;
                b_xz.at<float>(i,0) = my3DBallCoordsALL[i].x;
            }

            //use Mat::inv to calculate inv(A)

            A_inv = A.inv(cv::DECOMP_SVD);

            x_yz = A_inv*b_yz;
            x_xz = A_inv*b_xz;


            //get the ball's final xy position relative to the cameras
            float x_final_camera = x_xz.at<float>(3,0);
            float y_final_camera = x_yz.at<float>(3,0);

            //convert to catcher coordinates and generate catcher commands
            float catcherX_cmd = x_final_camera + catcher_x_offset;
            float catcherY_cmd = y_final_camera + catcher_y_offset;

            std::cout << catcherX_cmd << "\t" << catcherY_cmd << std::endl;

            //reset vectors
            ballPointsDispL.clear();
            ballPointsDispR.clear();
            my3DBallCoordsALL.clear();
        }



        cv::imshow("Stereo Left", imageL);
        cv::imshow("Stereo Right", imageR);
        cv::waitKey(17); //~60fps = 17
    }



    return 0;
}

//function definitions
void readCalibrationData()
{
    cv::FileStorage fsL("/home/jesse/Desktop/BBCalibration/camera_L_parameters.xml", cv::FileStorage::READ);
    fsL["Camera_Matrix"] >> cameraMatrixL;
    fsL["Distortion_Coefficients"] >> distCoeffL;

    cv::FileStorage fsR("/home/jesse/Desktop/BBCalibration/camera_R_parameters.xml", cv::FileStorage::READ);
    fsR["Camera_Matrix"] >> cameraMatrixR;
    fsR["Distortion_Coefficients"] >> distCoeffR;

    cv::FileStorage fsS("/home/jesse/Desktop/BBCalibration/stereo_calibration_parameters.xml", cv::FileStorage::READ);
    fsS["Stereo_Rotation_Matrix"] >> R;
    fsS["Stereo_Translation_Vector"] >> T;
    fsS["Stereo_Essential_Matirx"] >> E;
    fsS["Stereo_Fundamental_Matirx"] >> F;
    fsS["R1"] >> R1;
    fsS["R2"] >> R2;
    fsS["P1"] >> P1;
    fsS["P2"] >> P2;
    fsS["Q"] >> Q;

}
