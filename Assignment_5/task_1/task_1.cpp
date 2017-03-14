#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>


int main()
{

    //get the path to the optical flow images
    std::string imagePath = "/home/jesse/Desktop/ECEN_631/Assignment_5/optical_flow/O";
    std::string imageNumber;
    std::string pathEnd = ".jpg";
    std::string fullPath;

    cv::Mat firstFrame;
    cv::Mat frame;
    cv::Mat colorFrame;

    int slowMultiplier = 30;

    //create vector to hold pyramid points
    std::vector<cv::Point2f> pyramid;

    //create vector to hold feature corners
    std::vector<cv::Point2f> corners;

    //mask for goodFeaturesToTrakc
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);

    //variable to hold pyramid window size
    cv::Size pyramid_win_size = cv::Size(100,100);


    //Look at first image to find features to track and calculate pyramid
    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/optical_flow/O1.jpg",1);
    cv::cvtColor(firstFrame,firstFrame,cv::COLOR_BGR2GRAY);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(firstFrame,corners,500,0.01,10,mask,3,false,0.04);
    //cv::buildOpticalFlowPyramid(firstFrame,pyramid,pyramid_win_size,5,true,cv::BORDER_REFLECT_101,cv::BORDER_CONSTANT,true);

    //create Mat for prev and Next frame
    cv::Mat prevFrame;
    cv::Mat nextFrame;

    //create vectors to hold prev and next points
    std::vector<cv::Point2f> prevPts;
    std::vector<cv::Point2f> nextPts;

    prevFrame = firstFrame;
    prevPts = corners;

    std::vector<uchar> status;
    std::vector<float> err;



    //Between n and n+1
    for(int i=1;i<18;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;

        frame = cv::imread(fullPath,1);
        cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);

        nextFrame = frame;

        cv::calcOpticalFlowPyrLK(prevFrame,nextFrame,prevPts,nextPts,status,err);

        prevFrame = frame;


        //convert back to color to display
        cv::cvtColor(frame,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<corners.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<corners.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),cv::Point(nextPts[i].x,nextPts[i].y),cv::Scalar(0,0,255),2,8,0);
        }

        //put text
        cv::putText(colorFrame,"Frame-to-Frame",cv::Point(30,30),2,1,cv::Scalar(0,255,0),1,8);

        //show the image
        cv::imshow("frame",colorFrame);
        cv::waitKey(slowMultiplier*34);    //~30 fps
    }

    //Between n and n+2
    prevPts.clear();
    nextPts.clear();
    prevFrame = firstFrame;
    prevPts = corners;
    for(int i=1;i<17;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;

        frame = cv::imread(fullPath,1);
        cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);

        nextFrame = frame;

        cv::calcOpticalFlowPyrLK(prevFrame,nextFrame,prevPts,nextPts,status,err);

        prevFrame = frame;


        //convert back to color to display
        cv::cvtColor(frame,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<corners.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<corners.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),cv::Point(nextPts[i].x,nextPts[i].y),cv::Scalar(0,0,255),2,8,0);
        }

        //put text
        cv::putText(colorFrame,"Skip One Frame",cv::Point(30,30),2,1,cv::Scalar(0,255,0),1,8);

        //show the image
        cv::imshow("frame",colorFrame);
        cv::waitKey(slowMultiplier*34);    //~30 fps
        i = i+1;
    }

    //Between n and n+3
    prevPts.clear();
    nextPts.clear();
    prevFrame = firstFrame;
    prevPts = corners;
    for(int i=1;i<16;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;

        frame = cv::imread(fullPath,1);
        cv::cvtColor(frame,frame,cv::COLOR_BGR2GRAY);

        nextFrame = frame;

        cv::calcOpticalFlowPyrLK(prevFrame,nextFrame,prevPts,nextPts,status,err);

        prevFrame = frame;


        //convert back to color to display
        cv::cvtColor(frame,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<corners.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<corners.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),cv::Point(nextPts[i].x,nextPts[i].y),cv::Scalar(0,0,255),2,8,0);
        }

        //put text
        cv::putText(colorFrame,"Skip Two Frames",cv::Point(30,30),2,1,cv::Scalar(0,255,0),1,8);

        //show the image
        cv::imshow("frame",colorFrame);
        cv::waitKey(slowMultiplier*34);    //~30 fps
        i = i+2;
    }






    cv::destroyAllWindows();

    return 0;
}
