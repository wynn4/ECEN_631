#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

//function prototype
void drawEpipolarLines(cv::Mat image, std::vector<cv::Point3f> epipolarLines, cv::Scalar color);

int main()
{
    //initialize cameraMatrixL, cameraMatrixR, distCoeffsL, and distCoeffsR
    cv::Mat cameraMatrixL(3,3, CV_64F);
    cv::Mat cameraMatrixR(3,3, CV_64F);
    cv::Mat distCoeffsL(5,1,CV_64F);
    cv::Mat distCoeffsR(5,1,CV_64F);

    //Read the individual camera calibration parameters from file
    cv::FileStorage fs1("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_L_parameters.xml", cv::FileStorage::READ);
    fs1["Camera_Matrix"] >> cameraMatrixL;
    fs1["Distortion_Coefficients"] >> distCoeffsL;

    cv::FileStorage fs2("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_R_parameters.xml", cv::FileStorage::READ);
    fs2["Camera_Matrix"] >> cameraMatrixR;
    fs2["Distortion_Coefficients"] >> distCoeffsR;

    //initialize cv::Mat objects to hold the images
    cv::Mat image_L;
    cv::Mat image_R;
    cv::Mat image_L_gray;
    cv::Mat image_R_gray;

    //read in the images
//    image_L = cv::imread("/home/jesse/Desktop/ImageJae/stereoL17.bmp",1);
//    image_R = cv::imread("/home/jesse/Desktop/ImageJae/stereoR17.bmp",1);

    image_L = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_3/stereo_images/stereoL84.bmp");
    image_R = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_3/stereo_images/stereoR84.bmp");

    //convert to Grayscale
    cv::cvtColor(image_L,image_L_gray,6);
    cv::cvtColor(image_R,image_R_gray,6);

    //initialize cv::Mat objects to hold the undistorted images
    cv::Mat image_L_undist;
    cv::Mat image_R_undist;
    cv::Mat image_L_undist_color;
    cv::Mat image_R_undist_color;

    cv::undistort(image_L_gray,image_L_undist,cameraMatrixL,distCoeffsL,cv::noArray());
    cv::undistort(image_R_gray,image_R_undist,cameraMatrixR,distCoeffsR,cv::noArray());

    //convert back to color for drawing
    cv::cvtColor(image_L_undist,image_L_undist_color,cv::COLOR_GRAY2BGR);
    cv::cvtColor(image_R_undist,image_R_undist_color,cv::COLOR_GRAY2BGR);

    //draw 3 points of interest on each image
    cv::circle(image_L_undist_color,cv::Point(189,166),3,cv::Scalar(0, 255, 0),2,8);
    cv::circle(image_L_undist_color,cv::Point(394,284),3,cv::Scalar(0, 255, 0),2,8);
    cv::circle(image_L_undist_color,cv::Point(196,254),3,cv::Scalar(0, 255, 0),2,8);

    cv::circle(image_R_undist_color,cv::Point(189,187),3,cv::Scalar(0, 0, 255),2,8);
    cv::circle(image_R_undist_color,cv::Point(236,85),3,cv::Scalar(0, 0, 255),2,8);
    cv::circle(image_R_undist_color,cv::Point(301,223),3,cv::Scalar(0, 0, 255),2,8);

    //read in the fundamental matrix from stereo calibration
    cv::Mat F;
    cv::FileStorage fs3("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_2-Desktop_Qt_5_7_0_GCC_64bit-Default/stereo_calibration_parameters.xml", cv::FileStorage::READ);
    fs3["Stereo_Fundamental_Matrix"] >> F;

    //initialize vectors to hold epipolarLines
    std::vector<cv::Point3f> epipolarLines_L;
    std::vector<cv::Point3f> epipolarLines_R;

    //define points of interest
    std::vector<cv::Point2f> pts_of_interest_L = {cv::Point(189,166), cv::Point(394,284), cv::Point(196,254)};
    std::vector<cv::Point2f> pts_of_interest_R = {cv::Point(189,187), cv::Point(236,85), cv::Point(301,223)};

    //compute the epipolarLines corresponding to points of interest
    cv::computeCorrespondEpilines(pts_of_interest_L,1,F,epipolarLines_L);
    cv::computeCorrespondEpilines(pts_of_interest_R,2,F,epipolarLines_R);

    //draw the lines with custom function
    drawEpipolarLines(image_R_undist_color,epipolarLines_L,cv::Scalar(0,255,0));
    drawEpipolarLines(image_L_undist_color,epipolarLines_R,cv::Scalar(0,0,255));

    //Display the images
    cv::imshow("Left Image",image_L_undist_color);
    cv::imshow("Right Image",image_R_undist_color);

    //save to file
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_3/task_3_epipolars_left_image.bmp",image_L_undist_color);
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_3/task_3_epipolars_right_image.bmp",image_R_undist_color);

    cv::waitKey(0);

    //cleanup
    cv::destroyAllWindows();

    return 0;
}

//function definition
void drawEpipolarLines(cv::Mat image, std::vector<cv::Point3f> epipolarLines, cv::Scalar color)
{
    for(int i=0;i<epipolarLines.size();i++)
    {
        cv::Point3f ep_line = epipolarLines[i];
        //y = -(a/b)*x - (c/b)
        float y1 = -(ep_line.x/ep_line.y)*10.0 - (ep_line.z/ep_line.y);                 //y value of point on ep_line 10 pixels from left edge
        float y2 = -(ep_line.x/ep_line.y)*(image.cols - 10) - (ep_line.z/ep_line.y);    //y value of point on ep_line 10 pixels from right edge
        //draw the line
        cv::line(image,cv::Point2f(10,y1),cv::Point2f(630,y2),color,1,8);
    }
}
