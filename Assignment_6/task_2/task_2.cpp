#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
    //set which image set are we working with
    std::string imageFolder = "parallel_cube";  //parallel_cube parallel_real turned_cube turned real
    std::string imageName = "ParallelCube";     //ParallelCube ParallelReal TurnCube TurnReal


    //Read first and last points from file (from last assignment) and store in vectors
    std::vector<cv::Point2f> firstPoints;
    std::vector<cv::Point2f> lastPoints;

    cv::FileStorage fs("/home/jesse/Desktop/ECEN_631/Assignment_6/first_and_last_" + imageFolder + ".xml", cv::FileStorage::READ);
    fs["first_points"] >> firstPoints;
    fs["last_points"] >> lastPoints;


    //Use first and last points to estimate fundamental matrix F
    cv::Mat F;
    F = cv::findFundamentalMat(firstPoints,lastPoints);


    //known camera intrinsic parameters
    double fx = 825.0900600547;
    double fy = 824.2672147458;
    double cx = 331.6538103208;
    double cy = 252.9284287373;

    double k1 = -0.2380769337;
    double k2 = 0.0931325835;
    double p1 = 0.0003242537;
    double p2 = -0.0021901930;
    double k3 = 0.4641735616;

    double intrinsicMatrix[9] = {fx, 0, cx,
                                0, fy, cy,
                                0, 0, 1};

    double distMatrix[5] = {k1, k2, p1, p2, k3};


    cv::Mat M1(3,3, CV_64F, intrinsicMatrix);
    cv::Mat M2(3,3, CV_64F, intrinsicMatrix);

    cv::Mat dist1(5,1, CV_64F, distMatrix);
    cv::Mat dist2(5,1, CV_64F, distMatrix);

    //use undistortPoints()
    std::vector<cv::Point2f> firstPointsUndist;
    std::vector<cv::Point2f> lastPointsUndist;

    cv::undistortPoints(firstPoints,firstPointsUndist,M1,dist1,cv::noArray(),cv::noArray());
    cv::undistortPoints(lastPoints,lastPointsUndist,M2,dist2,cv::noArray(),cv::noArray());

    //convert points back to image frame (pixels)
    for(int i=0;i<firstPointsUndist.size();i++)
    {
        firstPointsUndist[i].x = firstPointsUndist[i].x * fx + cx;
        firstPointsUndist[i].y = firstPointsUndist[i].y * fy + cy;

        lastPointsUndist[i].x = lastPointsUndist[i].x * fx + cx;
        lastPointsUndist[i].y = lastPointsUndist[i].y * fy + cy;
    }


    //calculate essential matrix E from F (using equation on slide 28 of 3D Reconstruction lecture slides)
    cv::Mat E;
    E = M1.t()*F*M2;

    //normalize E









//    //Calculate R1 and R2
//    cv::Mat R1 = M1.inv()*H1*M1;
//    cv::Mat R2 = M2.inv()*H2*M2;

//    //get the undistortRectifyMap
//    cv::Mat map1First;
//    cv::Mat map2First;
//    cv::Mat map1Last;
//    cv::Mat map2Last;

//    cv::initUndistortRectifyMap(M1,dist1,R1,M1,cv::Size(640,480),CV_32FC1,map1First,map2First);
//    cv::initUndistortRectifyMap(M2,dist2,R2,M2,cv::Size(640,480),CV_32FC1,map1Last,map2Last);


//    //load images from file and remap()
//    cv::Mat firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + "/" + imageName + "10.jpg", 1);
//    cv::Mat lastFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + "/" + imageName + "15.jpg", 1);

//    cv::remap(firstFrame,firstFrame,map1First,map2First,cv::INTER_NEAREST,cv::BORDER_CONSTANT);
//    cv::remap(lastFrame,lastFrame,map1Last,map2Last,cv::INTER_NEAREST,cv::BORDER_CONSTANT);

//    //draw some lines to see how well it rectified
////    //parallel_cube
////    int topLineY = 53;
////    int midLineY = 246;
////    int botLineY = 457;

////    //parallel_real
////    int topLineY = 63;
////    int midLineY = 258;
////    int botLineY = 415;

////    //turned_cube
////    int topLineY = 63;
////    int midLineY = 234;
////    int botLineY = 375;

//    //turned_real
//    int topLineY = 53;
//    int midLineY = 246;
//    int botLineY = 457;

//    cv::line(firstFrame,cv::Point(0,topLineY),cv::Point(640,topLineY),cv::Scalar(0,255,0));
//    cv::line(firstFrame,cv::Point(0,midLineY),cv::Point(640,midLineY),cv::Scalar(0,255,0));
//    cv::line(firstFrame,cv::Point(0,botLineY),cv::Point(640,botLineY),cv::Scalar(0,255,0));

//    cv::line(lastFrame,cv::Point(0,topLineY),cv::Point(640,topLineY),cv::Scalar(0,255,0));
//    cv::line(lastFrame,cv::Point(0,midLineY),cv::Point(640,midLineY),cv::Scalar(0,255,0));
//    cv::line(lastFrame,cv::Point(0,botLineY),cv::Point(640,botLineY),cv::Scalar(0,255,0));


//    //show the rectified images
//    cv::imshow("First Frame", firstFrame);
//    cv::imshow("Last Frame", lastFrame);
//    cv::waitKey(0);


//    //write images to file
//    cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_6/task_1/" + imageName + "First.jpg",firstFrame);
//    cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_6/task_1/" + imageName + "Last.jpg",lastFrame);




    cv::destroyAllWindows();

    return 0;
}







