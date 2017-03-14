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
    cv::Mat frameTemplate;
    cv::Mat colorFrame;

    int slowMultiplier = 30;

    //create vector to hold feature corners
    std::vector<cv::Point2f> corners;
    std::vector<cv::Point2f> filteredCorners;

    //mask for goodFeaturesToTrakc
    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);

    //define a searchWindowSize
    int searchWinWidth = 40;
    int searchWinHeight = 40;
    cv::Size searchWindowSize = cv::Size(searchWinWidth,searchWinHeight);
    cv::Size templateSize = cv::Size(searchWinWidth/4,searchWinHeight/4);


    //Look at first image to find features to track and calculate pyramid
    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/optical_flow/O1.jpg",1);
    cv::cvtColor(firstFrame,firstFrame,cv::COLOR_BGR2GRAY);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(firstFrame,corners,500,0.01,10,mask,3,false,0.04);

    //go through the corners vector an toss out any that are too close to the boundary
    for(int i=1;i<corners.size();i++)
    {
        if(corners[i].x > searchWindowSize.width*4 && corners[i].x < (firstFrame.cols - searchWindowSize.width*4)
           && corners[i].y > searchWindowSize.height*4 && corners[i].y < (firstFrame.rows - searchWindowSize.height*4))
        {
            filteredCorners.push_back(corners[i]);
        }
    }

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

    prevFrame = firstFrame;

    prevPts = filteredCorners;
    nextPts = prevPts;
    //Between n and n+1
    for(int i=1;i<18;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;
        nextFrame = cv::imread(fullPath,1);
        cv::cvtColor(nextFrame,nextFrame,cv::COLOR_BGR2GRAY);

        //Define a search window and template for each point
        for(int i=0;i<prevPts.size();i++)
        {
            //Define template for each point
            templateRect = cv::Rect(prevPts[i].x,prevPts[i].y,templateSize.width,templateSize.height);

            templ = prevFrame(templateRect);

            //Define searchWindow around each point
            searchWindowRect = cv::Rect(prevPts[i].x - searchWindowSize.width/2,prevPts[i].y - searchWindowSize.height/2
                                        ,searchWindowSize.width,searchWindowSize.height);
            searchWindow = nextFrame(searchWindowRect);

            //use matchTemplate() to find where the template went
            cv::matchTemplate(searchWindow,templ,result,CV_TM_CCORR_NORMED);    //
            cv::minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);

            //get the location of the match within the searchWindow
            float maxLocX = (float)maxLoc.x;
            float maxLocY = (float)maxLoc.y;
            matchLocation = cv::Point2f(maxLocX,maxLocY);

            //convert the location to the location within the whole frame
            matchGlobalLocation = cv::Point2f(prevPts[i].x - searchWindowSize.width/2 + matchLocation.x,
                                            prevPts[i].y - searchWindowSize.height/2 + matchLocation.y);
            //std::cout << matchGlobalLocation << std::endl;

            matchLocations.push_back(matchGlobalLocation);
        }

        nextPts = matchLocations;



        //convert back to color to display
        cv::cvtColor(nextFrame,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<prevPts.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<prevPts.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),cv::Point(nextPts[i].x,nextPts[i].y),cv::Scalar(0,0,255),2,8,0);
        }

        //put text
        cv::putText(colorFrame,"Frame-to-Frame",cv::Point(30,30),2,1,cv::Scalar(0,255,0),1,8);

        //show the image
        cv::imshow("frame",colorFrame);
        cv::waitKey(slowMultiplier*34);    //~30 fps

        //clear and reset the vectors of points
        matchLocations.clear();
        prevPts.clear();
        prevPts = nextPts;
        nextPts.clear();
        prevFrame = nextFrame;

        std::cout << i << std::endl;
    }

    prevFrame = firstFrame;

    prevPts = filteredCorners;
    nextPts = prevPts;
    //Between n and n+1
    for(int i=1;i<18;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;
        nextFrame = cv::imread(fullPath,1);
        cv::cvtColor(nextFrame,nextFrame,cv::COLOR_BGR2GRAY);

        //Define a search window and template for each point
        for(int i=0;i<prevPts.size();i++)
        {
            //Define template for each point
            templateRect = cv::Rect(prevPts[i].x,prevPts[i].y,templateSize.width,templateSize.height);

            templ = prevFrame(templateRect);

            //Define searchWindow around each point
            searchWindowRect = cv::Rect(prevPts[i].x - searchWindowSize.width/2,prevPts[i].y - searchWindowSize.height/2
                                        ,searchWindowSize.width,searchWindowSize.height);
            searchWindow = nextFrame(searchWindowRect);

            //use matchTemplate() to find where the template went
            cv::matchTemplate(searchWindow,templ,result,CV_TM_CCORR_NORMED);    //
            cv::minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);

            //get the location of the match within the searchWindow
            float maxLocX = (float)maxLoc.x;
            float maxLocY = (float)maxLoc.y;
            matchLocation = cv::Point2f(maxLocX,maxLocY);

            //convert the location to the location within the whole frame
            matchGlobalLocation = cv::Point2f(nextPts[i].x - searchWindowSize.width/2 + matchLocation.x,
                                            nextPts[i].y - searchWindowSize.height/2 + matchLocation.y);
            //std::cout << matchGlobalLocation << std::endl;

            matchLocations.push_back(matchGlobalLocation);
        }

        nextPts = matchLocations;



        //convert back to color to display
        cv::cvtColor(nextFrame,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<prevPts.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<prevPts.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),cv::Point(nextPts[i].x,nextPts[i].y),cv::Scalar(0,0,255),2,8,0);
        }

        //put text
        cv::putText(colorFrame,"Skip One Frame",cv::Point(30,30),2,1,cv::Scalar(0,255,0),1,8);

        //show the image
        cv::imshow("frame",colorFrame);
        cv::waitKey(slowMultiplier*34);    //~30 fps

        //clear and reset the vectors of points
        matchLocations.clear();
        prevPts.clear();
        prevPts = nextPts;
        nextPts.clear();
        prevFrame = nextFrame;

        std::cout << i << std::endl;
        i = i+1;
    }


    prevFrame = firstFrame;

    prevPts = filteredCorners;
    nextPts = prevPts;
    //Between n and n+1
    for(int i=1;i<17;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;
        nextFrame = cv::imread(fullPath,1);
        cv::cvtColor(nextFrame,nextFrame,cv::COLOR_BGR2GRAY);

        //Define a search window and template for each point
        for(int i=0;i<prevPts.size();i++)
        {
            //Define template for each point
            templateRect = cv::Rect(prevPts[i].x,prevPts[i].y,templateSize.width,templateSize.height);

            templ = prevFrame(templateRect);

            //Define searchWindow around each point
            searchWindowRect = cv::Rect(prevPts[i].x - searchWindowSize.width/2,prevPts[i].y - searchWindowSize.height/2
                                        ,searchWindowSize.width,searchWindowSize.height);
            searchWindow = nextFrame(searchWindowRect);

            //use matchTemplate() to find where the template went
            cv::matchTemplate(searchWindow,templ,result,CV_TM_CCORR_NORMED);    //
            cv::minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);

            //get the location of the match within the searchWindow
            float maxLocX = (float)maxLoc.x;
            float maxLocY = (float)maxLoc.y;
            matchLocation = cv::Point2f(maxLocX,maxLocY);

            //convert the location to the location within the whole frame
            matchGlobalLocation = cv::Point2f(prevPts[i].x - searchWindowSize.width/2 + matchLocation.x,
                                            prevPts[i].y - searchWindowSize.height/2 + matchLocation.y);
            //std::cout << matchGlobalLocation << std::endl;

            matchLocations.push_back(matchGlobalLocation);
        }

        nextPts = matchLocations;



        //convert back to color to display
        cv::cvtColor(prevFrame,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<prevPts.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<prevPts.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPts[i].x,prevPts[i].y),cv::Point(nextPts[i].x,nextPts[i].y),cv::Scalar(0,0,255),2,8,0);
        }

        //put text
        cv::putText(colorFrame,"Skip Two Frames",cv::Point(30,30),2,1,cv::Scalar(0,255,0),1,8);

        //show the image
        cv::imshow("frame",colorFrame);
        cv::waitKey(slowMultiplier*34);    //~30 fps

        //clear and reset the vectors of points
        matchLocations.clear();
        prevPts.clear();
        prevPts = nextPts;
        nextPts.clear();
        prevFrame = nextFrame;

        std::cout << i << std::endl;
        i = i+2;
    }









    cv::destroyAllWindows();

    return 0;
}
