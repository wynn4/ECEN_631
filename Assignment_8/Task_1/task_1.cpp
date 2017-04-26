#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
    int num_images = 702;

    //camera params
    double fx = 707.09120;
    double fy = 707.09120;
    double ox = 601.88730;
    double oy = 183.11040;

    double intrinsicParams[9] = {fx, 0, ox,
                                0, fy, oy,
                                0, 0, 1};

//    double distCoeffs[5] = {k1, k2, p1, p2, k3};

    cv::Mat M(3,3, CV_64F, intrinsicParams);
//    cv::Mat dist1(5,1, CV_64F, distCoeffs);

    //mat to hold the gray image
    cv::Mat grayImage;
    cv::Mat firstFrame;
    cv::Mat colorFrame;

    //load the sample image to get image size
    firstFrame = cv::imread("/home/jesse/Desktop/practice_vo_files/VO_Practice_Sequence/000000.png", CV_LOAD_IMAGE_GRAYSCALE);

    int width = firstFrame.cols;
    int height = firstFrame.rows;
    int widthTrim = width*0.1;
    int heightTrim = height*0.1;



    //__use goodFeaturesToTrack to get set of image points
    std::vector<cv::Point2f> initialPoints;


    //mask for goodFeaturesToTrakc
    cv::Mat mask = cv::Mat::zeros(firstFrame.size(), CV_8UC1);
    //mask away from the edge a bit
    cv::Mat roi(mask, cv::Rect(cv::Point(widthTrim,heightTrim), cv::Point(width - widthTrim, height - heightTrim)));
    roi = cv::Scalar(1);
    //unmask around the middle
    cv::Mat roi2(mask,cv::Rect(cv::Point((width/2) - widthTrim, (height/2) - heightTrim), cv::Point((width/2) + widthTrim, (height/2) + heightTrim)));
    roi2 = cv::Scalar(0);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(firstFrame,initialPoints,500,0.01,10,mask,3,false,0.04);


    //file path
    std::string imgSet = "/home/jesse/Desktop/practice_vo_files/VO_Practice_Sequence/";

    cv::Mat prevFrame;
    cv::Mat nextFrame;

    std::vector<cv::Point2f> prevPts;
    std::vector<cv::Point2f> nextPts;

    std::vector<cv::Point2f> prevPtsMatched;
    std::vector<cv::Point2f> nextPtsMatched;

    //Mat to hold fundamental matrix and the output mask
    cv::Mat F;
    cv::Mat fMask;

    //ransac tuning params
    double param1 = 0.5;      //max distance (pixels) from epipolar for non-outlier
    double param2 = 0.98;   //confidence level that match is correct

    //initialize
    prevFrame = firstFrame;
    prevPts = initialPoints;

    std::vector<uchar> status;
    std::vector<float> err;


    //loop through all images in file
    for(int i=0;i<num_images;i++)
    {
        std::stringstream ss;
        ss << imgSet << std::setw(6) << std::setfill('0') << i << ".png";
        std::string s = ss.str();

        grayImage = cv::imread(s, CV_LOAD_IMAGE_GRAYSCALE);

        if (grayImage.data == NULL)
        {
            std::cout << "Could not open image: " + s << std::endl;
            return -1;
        }

        nextFrame = grayImage;

        //got the image, now lets process...

        //calcultate optical flow
        cv::calcOpticalFlowPyrLK(prevFrame,nextFrame,prevPts,nextPts,status, err);


        //get a filter mask (fmask) to eliminate outliers using cv::findFundamentalMat()
        F = cv::findFundamentalMat(prevPts,nextPts,fMask,CV_FM_RANSAC,param1,param2);

        //go through the mask and where there's a '1', keep the point, where '0' toss it
        for(int i=0;i<fMask.size().height;i++)
        {
            if(fMask.at<bool>(0,i) == 1)
            {
                nextPtsMatched.push_back(nextPts[i]);
                prevPtsMatched.push_back(prevPts[i]);

            }
        }

        //got the good points, now lets figure out how the camera moved


        //convert back to color to display
        cv::cvtColor(grayImage,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<nextPtsMatched.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPtsMatched[i].x,prevPtsMatched[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<nextPtsMatched.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPtsMatched[i].x,prevPtsMatched[i].y),cv::Point(nextPtsMatched[i].x,nextPtsMatched[i].y),cv::Scalar(0,0,255),2,8,0);
        }


        cv::imshow("image", colorFrame);
        cv::waitKey(1);




        //reset frame and clear out the vectors
        prevFrame = grayImage;
        prevPts.clear();
        nextPtsMatched.clear();
        prevPtsMatched.clear();

        //prevPoints = findgoodfeaturs to track
        cv::goodFeaturesToTrack(prevFrame,prevPts,500,0.01,10,mask,3,false,0.04);

    }
    cv::destroyAllWindows();

    return 0;
}







