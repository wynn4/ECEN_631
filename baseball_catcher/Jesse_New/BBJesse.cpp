#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

//tuning params
int imageThreshold = 20;

int NumImages = 38;
string directory = "/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/";
string filename = "Ball_test";

//__instantiate cv::Mats to hold calibration data__

//intrinsic
Mat cameraMatrixL;
Mat cameraMatrixR;
Mat distCoeffL;
Mat distCoeffR;

//extrinsic
Mat R;
Mat T;
Mat E;
Mat F;

//__define ROI rectangles
Point ROI_center_L = Point(358,120);
Point ROI_center_R = Point(284,122);

int widthL = 100;
int heightL = 100;
int widthR = 100;
int heightR = 100;

Point ROI_tl_L = Point(ROI_center_L.x - widthL/2, ROI_center_L.y - heightL/2);
Point ROI_br_L = Point(ROI_center_L.x + widthL/2, ROI_center_L.y + heightL/2);

Point ROI_tl_R = Point(ROI_center_R.x - widthR/2, ROI_center_R.y - heightR/2);
Point ROI_br_R = Point(ROI_center_R.x + widthR/2, ROI_center_R.y + heightR/2);

Rect ROI_L = Rect(ROI_tl_L, ROI_br_L);
Rect ROI_R = Rect(ROI_tl_R, ROI_br_R);

//__define Mats to hold the different frames

Mat firstFrameL = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testL0.bmp", CV_LOAD_IMAGE_GRAYSCALE);
Mat firstFrameR = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testR0.bmp", CV_LOAD_IMAGE_GRAYSCALE);
Mat firstFrameROI_L = firstFrameL(ROI_L);
Mat firstFrameROI_R = firstFrameR(ROI_R);

Mat frameL;
Mat frameR;

//__stuff to hold the contours
vector<vector<Point> > contoursL;
vector<Vec4i> hierarchyL;

vector<vector<Point> > contoursR;
vector<Vec4i> hierarchyR;

//__stuff for canny
Mat edgesL;
Mat edgesR;

//stuff for circles
vector<Vec3f> circles;


//function prototypes
void readCalibrationData();

int main()
{
    //__STEP__1: read in all of the camera calibration data
    readCalibrationData();


    //__STEP__2: get initial ROI(s) and store them
    //   ***done outside loop***

    //__STEP__3: begin looping through the images
    for(int i=0;i<NumImages;i++)
    {
        int imageNumber = i;
        frameL = cv::imread(directory + filename + "L" + std::to_string(imageNumber) + ".bmp", CV_LOAD_IMAGE_GRAYSCALE); //1
        frameL = frameL(ROI_L);

        frameR = cv::imread(directory + filename + "R" + std::to_string(imageNumber) + ".bmp", CV_LOAD_IMAGE_GRAYSCALE);
        frameR = frameR(ROI_R);

        //absolute differnce each frame from the first frame
        cv::absdiff(frameL,firstFrameROI_L,frameL);
        cv::absdiff(frameR,firstFrameROI_R,frameR);

        //gaussian blur to reduce noise
        cv::GaussianBlur(frameL,frameL, cv::Size(11,11), 0,0);
        cv::GaussianBlur(frameR,frameR, cv::Size(11,11), 0,0);

        //threshold
        cv::threshold(frameL,frameL,imageThreshold,255,1);
        cv::threshold(frameR,frameR,imageThreshold,255,1);


        //try circles
        //HoughCircles(frameL, circles, CV_HOUGH_GRADIENT, 1, frameL.rows/8, 10, 100, 0, 0);
        //cout << circles.size() << endl;


        //canny lines to get ball
        //Canny(frameL,edgesL,100,255,3);
        //cout << edgesL.size() << endl;

        //findContours
        findContours(frameL, contoursL, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
        cout << contoursL.size() << endl;

        cv::imshow("Left ROI", frameL);
        //cv::imshow("Right ROI", frameR);
        cv::waitKey(200);
    }



//    //_draw the ROI
//    cv::rectangle(firstFrameL, ROI_L, cv::Scalar(255),1);
//    cv::rectangle(firstFrameR, ROI_R, cv::Scalar(255),1);

//    cv::imshow("Left", firstFrameL);
//    cv::imshow("Right", firstFrameR);
//    //cv::imshow("Left ROI", firstFrameROI_L);
//    //cv::imshow("Right ROI", firstFrameROI_R);
//    cv::waitKey(4000);


//    //define filepath strings
//    std::string stereoL = "/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testL";
//    std::string stereoR = "/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testR";

//    //create variables for imageNumber and imNum
//    int imageNumber;
//    std::string imNum;
//    float fontSize = 0.5;

//    //Define Mat objects to hold the images
//    cv::Mat imageL;
//    cv::Mat imageR;
//    cv::Mat imageLOriginal;
//    cv::Mat imageROriginal;

//    //define first image to be the image that all others are differenced from
//    cv::Mat diffImageL = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testL0.bmp",1);
//    cv::Mat diffImageR = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_4/newBall/Ball_testR0.bmp",1);

//    //convert to grayscale
//    cv::cvtColor(diffImageL,diffImageL,cv::COLOR_BGR2GRAY);
//    cv::cvtColor(diffImageR,diffImageR,cv::COLOR_BGR2GRAY);

//    //define ROI Rect for each camera to avoid processing entire image
//    int offsetL_x = 300;
//    int offsetL_y = 50;
//    int offsetR_x = 210;
//    int offsetR_y = 50;
//    cv::Rect roiL = cv::Rect(cv::Point(offsetL_x,offsetL_y),cv::Point(diffImageL.cols-210,diffImageL.rows-300));
//    cv::Rect roiR = cv::Rect(cv::Point(offsetR_x,offsetR_y),cv::Point(diffImageR.cols-330,diffImageR.rows-290));

//    //shrink differenced images to match ROI
//    diffImageL = diffImageL(roiL);
//    diffImageR = diffImageR(roiR);

//    //setup blob detector and parameters
//    cv::SimpleBlobDetector::Params params;

//    // Change thresholds
//    //params.minThreshold = 10;
//    //params.maxThreshold = 200;

//    // Filter by Area.
//    params.filterByArea = true;
//    params.minArea = 10;

//    // Filter by Circularity
//    params.filterByCircularity = true;
//    params.minCircularity = 0.1;

//    // Filter by Convexity
//    params.filterByConvexity = true;
//    params.minConvexity = 0.87;

//    // Filter by Inertia
//    params.filterByInertia = true;
//    params.minInertiaRatio = 0.35;


//    //create detector object
//    cv::SimpleBlobDetector detector(params);

//    //create keypoints vector
//    std::vector<cv::KeyPoint> keypointsL;
//    std::vector<cv::KeyPoint> keypointsR;

//    //define variables to hold ballPoint x and y components;
//    float ballPointL_x;
//    float ballPointL_y;
//    float ballPointR_x;
//    float ballPointR_y;
//    cv::Point2f ballPointL;
//    cv::Point2f ballPointR;

//    //Create ball pixel point vector;
//    std::vector<cv::Point2f> ballPointsL;
//    std::vector<cv::Point2f> ballPointsR;


//    for(int i=1;i<numImages;i++)
//    {
//        imageNumber = i;
//        imNum = std::to_string(imageNumber);

//        //read in the images
//        imageLOriginal = cv::imread(stereoL + imNum + ".bmp",1);
//        imageROriginal = cv::imread(stereoR + imNum + ".bmp",1);

//        //only process inside ROI
//        imageL = imageLOriginal(roiL);
//        imageR = imageROriginal(roiR);

//        //convert to grayscale
//        cv::cvtColor(imageL,imageL,cv::COLOR_BGR2GRAY);
//        cv::cvtColor(imageR,imageR,cv::COLOR_BGR2GRAY);

//        //absolute difference the images
//        cv::absdiff(imageL,diffImageL,imageL);
//        cv::absdiff(imageR,diffImageR,imageR);

//        //threshold the images
//        cv::threshold(imageL,imageL,15,255,1);
//        cv::threshold(imageR,imageR,15,255,1);

//        //detect ball
//        detector.detect(imageL,keypointsL);
//        detector.detect(imageR,keypointsR);

//        //draw the keypoints
//        cv::drawKeypoints(imageL, keypointsL, imageL, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//        cv::drawKeypoints(imageR, keypointsR, imageR, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
//        //std::cout << keypointsR.size() << std::endl;

//        //store the keypoint
//        if(i>1 && i<numImages-1)    //we don't get keypoints on the first and last frame in both cameras
//        {
//            //get the keypoint coordinates and add in the offset
//            ballPointL_x = keypointsL[0].pt.x + offsetL_x;
//            ballPointL_y = keypointsL[0].pt.y + offsetL_y;
//            ballPointR_x = keypointsR[0].pt.x + offsetR_x;
//            ballPointR_y = keypointsR[0].pt.y + offsetR_y;

//            ballPointL = cv::Point2f(ballPointL_x,ballPointL_y);
//            ballPointR = cv::Point2f(ballPointR_x,ballPointR_y);

//            ballPointsL.push_back(ballPointL);
//            ballPointsR.push_back(ballPointR);
//        }

//        cv::imshow("Stereo Left", imageL);
//        cv::imshow("Stereo Right", imageR);
//        cv::waitKey(1700); //~60fps = 17


//    //write ballPointsL and ballPointsR to file
//    cv::FileStorage fsLpoints("/home/jesse/Desktop/ECEN_631/Assignment_4/ballPointsL.xml", cv::FileStorage::WRITE);
//    fsLpoints << "ballPointsL" << ballPointsL;
//    fsLpoints << "ballPointsR" << ballPointsR;

    return 0;
}

//function definitions
void readCalibrationData()
{
    cv::FileStorage fsL("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_L_parameters.xml", cv::FileStorage::READ);
    fsL["Camera_Matrix"] >> cameraMatrixL;
    fsL["Distortion_Coefficients"] >> distCoeffL;

    cv::FileStorage fsR("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_R_parameters.xml", cv::FileStorage::READ);
    fsR["Camera_Matrix"] >> cameraMatrixR;
    fsR["Distortion_Coefficients"] >> distCoeffR;

    cv::FileStorage fsS("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_2-Desktop_Qt_5_7_0_GCC_64bit-Default/stereo_calibration_parameters.xml", cv::FileStorage::READ);
    fsS["Stereo_Rotation_Matrix"] >> R;
    fsS["Stereo_Translation_Vector"] >> T;
    fsS["Stereo_Essential_Matirx"] >> E;
    fsS["Stereo_Fundamental_Matirx"] >> F;
}
