#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

//function prototype
cv::Rect trimRect(cv::Point2f point, float desWidth, float desHeight, cv::Size frameSize);

int main()
{

    //get the path to the optical flow images
    std::string imagePath = "/home/jesse/Desktop/ECEN_631/Assignment_5/optical_flow/O";
    std::string imageNumber;
    std::string pathEnd = ".jpg";
    std::string fullPath;

    cv::Mat firstFrame;
    cv::Mat frame;
    cv::Mat frameTemplate;
    cv::Mat colorFrame;

    int slowMultiplier = 10;

    //create vector to hold feature corners
    std::vector<cv::Point2f> corners;
    std::vector<cv::Point2f> filteredCorners;

    //mask for goodFeaturesToTrakc
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);

    //define a searchWindowSize
    float searchWinWidth = 61;
    float searchWinHeight = 61;

    //define a template size
    float templateWidth = 31;
    float templateHeight = 31;


    //Look at first image to find features to track and calculate pyramid
    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/optical_flow/O1.jpg",1);
    cv::cvtColor(firstFrame,firstFrame,cv::COLOR_BGR2GRAY);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(firstFrame,corners,500,0.01,10,mask,3,false,0.04);


    //create Mat for prev and Next frame
    cv::Mat prevFrame;
    cv::Mat nextFrame;

    //create vectors to hold prev and next points
    std::vector<cv::Point2f> prevPts;
    std::vector<cv::Point2f> nextPts;

    //initailize rectangle objects for the search window and template
    cv::Mat searchWindow;
    cv::Rect searchWindowRect;
    cv::Mat templ;
    cv::Rect templateRect;

    //create Mat to hold the result of matchTemplate()
    cv::Mat result;

    //initialize variables to hold parts of the result
    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;

    //define a vector to hold the match locations
    std::vector<cv::Point2f> matchLocations;
    cv::Point2f matchLocation;
    cv::Point2f matchGlobalLocation;

    //vector to hold template rects
    std::vector<cv::Rect> templateRects;
    std::vector<cv::Rect> searchWindowRects;

    for(int i=0;i<corners.size();i++)
    {
        templateRect = trimRect(cv::Point2f(corners[i].x, corners[i].y), templateWidth, templateHeight, cv::Size(640,480));
        templateRects.push_back(templateRect);

        searchWindowRect = trimRect(cv::Point2f(corners[i].x, corners[i].y), searchWinWidth, searchWinHeight, cv::Size(640,480));
        searchWindowRects.push_back(searchWindowRect);


    }


    //__draw all of the points and rectangles

    //convert back to color to display
    cv::cvtColor(firstFrame,colorFrame,cv::COLOR_GRAY2BGR);

    //draw green dot on all the original corner points
    for(int i=0;i<corners.size();i++)
    {
       cv::circle(colorFrame,cv::Point(corners[i].x,corners[i].y),0.5,cv::Scalar(0,255,0),2,8,0);
    }

    //draw a rectangle to represent each template
    for(int i=0;i<templateRects.size();i++)
    {
        cv::rectangle(colorFrame,templateRects[i],cv::Scalar(0,0,255),1,8,0);
    }

    //draw a rectangle to represent each search window
    for(int i=0;i<searchWindowRects.size();i++)
    {
        cv::rectangle(colorFrame,searchWindowRects[i],cv::Scalar(255,0,0),1,8,0);
    }


    cv::imshow("frame",colorFrame);
    cv::waitKey(0);

    cv::destroyAllWindows();

    return 0;
}


//function definition
cv::Rect trimRect(cv::Point2f point, float desWidth, float desHeight, cv::Size frameSize)
{
    cv::Point2f upperLeft;
    cv::Point2f lowerRight;

    //case 1
    if(point.x <= desWidth/2 && point.y <= desHeight/2)
    {
        upperLeft = cv::Point2f(0,0);
        lowerRight = cv::Point2f(point.x + desWidth/2, point.y + desHeight/2);
    }
    //case 2
    else if(point.x >= desWidth/2 && point.x <= (float)frameSize.width - desWidth/2 && point.y <= desHeight/2)
    {
        upperLeft = cv::Point2f(point.x - desWidth/2,0);
        lowerRight = cv::Point2f(point.x + desWidth/2, point.y + desHeight/2);
    }
    //case 3
    else if(point.x >= (float)frameSize.width - desWidth/2 && point.y <= desHeight/2)
    {
        upperLeft = cv::Point2f(point.x - desWidth/2, 0);
        lowerRight = cv::Point2f((float)frameSize.width,point.y+desHeight/2);
    }
    //case 4
    else if(point.x >= (float)frameSize.width - desWidth/2 && point.y >= desHeight/2 && point.y <= (float)frameSize.height - desHeight/2)
    {
        upperLeft = cv::Point2f(point.x - desWidth/2, point.y - desHeight/2);
        lowerRight = cv::Point2f((float)frameSize.width, point.y + desHeight/2);
    }
    //case 5
    else if(point.x >= (float)frameSize.width - desWidth/2 && point.y >= (float)frameSize.height - desHeight/2)
    {
        upperLeft = cv::Point2f(point.x - desWidth/2, point.y - desHeight/2);
        lowerRight = cv::Point2f((float)frameSize.width, (float)frameSize.height);
    }
    //case 6
    else if(point.x >= desWidth/2 && point.x <= (float)frameSize.width - desWidth/2 && point.y >= (float)frameSize.height - desHeight/2)
    {
        upperLeft = cv::Point2f(point.x - desWidth/2, point.y - desHeight/2);
        lowerRight = cv::Point2f(point.x + desWidth/2, (float)frameSize.height);
    }
    //case 7
    else if(point.x <= desWidth/2 && point.y >= (float)frameSize.height - desHeight/2)
    {
        upperLeft = cv::Point2f(0, point.y - desHeight/2);
        lowerRight = cv::Point2f(point.x + desWidth/2, (float)frameSize.height);
    }
    //case 8
    else if(point.x <= desWidth/2 && point.y >= desHeight/2 && point.y <= (float)frameSize.height - desHeight/2)
    {
        upperLeft = cv::Point2f(0, point.y - desHeight/2);
        lowerRight = cv::Point2f(point.x + desWidth/2, point.y + desHeight/2);
    }
    //anything else
    else
    {
        upperLeft = cv::Point2f(point.x - desWidth/2, point.y - desHeight/2);
        lowerRight = cv::Point2f(point.x + desWidth/2, point.y + desHeight/2);
    }

    return cv::Rect(upperLeft,lowerRight);


}
