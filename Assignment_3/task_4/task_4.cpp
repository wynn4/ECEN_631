#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>


int main()
{
    //initialize cameraMatrixL, cameraMatrixR, distCoeffsL,distCoeffsR, Rotattion,Translation,FundamentalMatrix
    cv::Mat cameraMatrixL(3,3, CV_64F);
    cv::Mat cameraMatrixR(3,3, CV_64F);
    cv::Mat distCoeffsL(5,1,CV_64F);
    cv::Mat distCoeffsR(5,1,CV_64F);
    cv::Mat stereoRotation(3,3, CV_64F);
    cv::Mat stereoTranslation(3,1,CV_64F);
    cv::Mat F;

    //read the individual camera calibration parameters from file
    cv::FileStorage fs1("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_L_parameters.xml", cv::FileStorage::READ);
    fs1["Camera_Matrix"] >> cameraMatrixL;
    fs1["Distortion_Coefficients"] >> distCoeffsL;

    cv::FileStorage fs2("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_R_parameters.xml", cv::FileStorage::READ);
    fs2["Camera_Matrix"] >> cameraMatrixR;
    fs2["Distortion_Coefficients"] >> distCoeffsR;

    //read in the fundamental matrix from stereo calibration as well as Rotation and Translation
    cv::FileStorage fs3("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_2-Desktop_Qt_5_7_0_GCC_64bit-Default/stereo_calibration_parameters.xml", cv::FileStorage::READ);
    fs3["Stereo_Fundamental_Matrix"] >> F;
    fs3["Stereo_Rotation_Matrix"] >> stereoRotation;
    fs3["Stereo_Translation_Vector"] >> stereoTranslation;

    //initialize cv::Mat objects to hold the images
    cv::Mat image_L;
    cv::Mat image_R;
    cv::Mat image_L_gray;
    cv::Mat image_R_gray;

    //read in the images
    //image_L = cv::imread("/home/jesse/Desktop/ImageJae/stereoL17.bmp",1);
    //image_R = cv::imread("/home/jesse/Desktop/ImageJae/stereoR17.bmp",1);

    image_L = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_3/stereo_images/stereoL84.bmp");
    image_R = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_3/stereo_images/stereoR84.bmp");

    //convert to Grayscale
    cv::cvtColor(image_L,image_L_gray,6);
    cv::cvtColor(image_R,image_R_gray,6);


    //stereo rectify the image using stereoRectify()
    //initialize output matrices for stereoRectify()
    cv::Mat R1(3,3, CV_64F);
    cv::Mat R2(3,3, CV_64F);
    cv::Mat P1(3,4, CV_64F);
    cv::Mat P2(3,4, CV_64F);
    cv::Mat Q(4,4, CV_64F);
    //stereoRectify
    cv::stereoRectify(cameraMatrixL,distCoeffsL,cameraMatrixR,distCoeffsR,image_L_gray.size(),stereoRotation,stereoTranslation,R1,R2,P1,P2,Q,0,-1,image_L_gray.size(),0,0);


    //compute undistortion and rectification transformation map for L and R using initUndistortRectifyMap()
    //initialize output matrices for initUndistortRectifyMap()
    cv::Mat map1_L;
    cv::Mat map2_L;
    cv::Mat map1_R;
    cv::Mat map2_R;
    //left camera
    cv::initUndistortRectifyMap(cameraMatrixL,distCoeffsL,R1,P1,image_L.size(),CV_32FC1,map1_L,map2_L);
    //right camera
    cv::initUndistortRectifyMap(cameraMatrixR,distCoeffsR,R2,P2,image_R.size(),CV_32FC1,map1_R,map2_R);


    //remap the images using cv::remap()
    //initialize output Mat for each image
    cv::Mat image_L_rectified;
    cv::Mat image_R_rectified;
    //call remap() on L and R images
    cv::remap(image_L_gray,image_L_rectified,map1_L,map2_L,cv::INTER_LINEAR,cv::BORDER_CONSTANT,0);
    cv::remap(image_R_gray,image_R_rectified,map1_R,map2_R,cv::INTER_LINEAR,cv::BORDER_CONSTANT,0);


    //compute the absolute difference between the original and rectified images
    cv::Mat image_L_diff;
    cv::Mat image_R_diff;
    cv::absdiff(image_L_gray,image_L_rectified,image_L_diff);
    cv::absdiff(image_R_gray,image_R_rectified,image_R_diff);

    //convert back to color for drawing lines
    cv::Mat image_L_rectified_color;
    cv::Mat image_R_rectified_color;
    cv::cvtColor(image_L_rectified,image_L_rectified_color,cv::COLOR_GRAY2BGR);
    cv::cvtColor(image_R_rectified,image_R_rectified_color,cv::COLOR_GRAY2BGR);

    //draw some horizontal lines on each image
    int horz_1_y = 60;
    int horz_2_y = 120;
    int horz_3_y = 180;
    int horz_4_y = 240;
    int horz_5_y = 300;
    int horz_6_y = 360;
    int horz_7_y = 420;
    cv::line(image_L_rectified_color,cv::Point2f(1,horz_1_y),cv::Point2f((image_L_rectified_color.cols -1),horz_1_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_L_rectified_color,cv::Point2f(1,horz_2_y),cv::Point2f((image_L_rectified_color.cols -1),horz_2_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_L_rectified_color,cv::Point2f(1,horz_3_y),cv::Point2f((image_L_rectified_color.cols -1),horz_3_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_L_rectified_color,cv::Point2f(1,horz_4_y),cv::Point2f((image_L_rectified_color.cols -1),horz_4_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_L_rectified_color,cv::Point2f(1,horz_5_y),cv::Point2f((image_L_rectified_color.cols -1),horz_5_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_L_rectified_color,cv::Point2f(1,horz_6_y),cv::Point2f((image_L_rectified_color.cols -1),horz_6_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_L_rectified_color,cv::Point2f(1,horz_7_y),cv::Point2f((image_L_rectified_color.cols -1),horz_7_y),cv::Scalar(0,0,255),1,8);

    cv::line(image_R_rectified_color,cv::Point2f(1,horz_1_y),cv::Point2f((image_L_rectified_color.cols -1),horz_1_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_R_rectified_color,cv::Point2f(1,horz_2_y),cv::Point2f((image_L_rectified_color.cols -1),horz_2_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_R_rectified_color,cv::Point2f(1,horz_3_y),cv::Point2f((image_L_rectified_color.cols -1),horz_3_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_R_rectified_color,cv::Point2f(1,horz_4_y),cv::Point2f((image_L_rectified_color.cols -1),horz_4_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_R_rectified_color,cv::Point2f(1,horz_5_y),cv::Point2f((image_L_rectified_color.cols -1),horz_5_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_R_rectified_color,cv::Point2f(1,horz_6_y),cv::Point2f((image_L_rectified_color.cols -1),horz_6_y),cv::Scalar(0,0,255),1,8);
    cv::line(image_R_rectified_color,cv::Point2f(1,horz_7_y),cv::Point2f((image_L_rectified_color.cols -1),horz_7_y),cv::Scalar(0,0,255),1,8);
    //Display the images
    cv::imshow("Left Image",image_L_rectified_color);
    cv::imshow("Right Image",image_R_rectified_color);
    cv::imshow("Left Absolute Difference",image_L_diff);
    cv::imshow("Right Absolute Difference",image_R_diff);

    //save to file
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_3/task_4_rectified_L.bmp",image_L_rectified_color);
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_3/task_4_rectified_R.bmp",image_R_rectified_color);
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_3/task_4_absdiff_L.bmp",image_L_diff);
    //cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_3/task_4_absdiff_R.bmp",image_R_diff);


    cv::waitKey(0);

    //cleanup
    cv::destroyAllWindows();

    return 0;
}
