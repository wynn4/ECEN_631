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
    cv::Mat colorFrame;

    int slowMultiplier = 10;

    //create vector to hold feature corners
    std::vector<cv::Point2f> corners;

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


    prevFrame = firstFrame;

    prevPts = corners;
    nextPts = prevPts;
    //Between n and n+1
    for(int i=1;i<18;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;
        nextFrame = cv::imread(fullPath,1);
        cv::cvtColor(nextFrame,nextFrame,cv::COLOR_BGR2GRAY);

        //Define a template and searchwindow for each point
        for(int i=0;i<prevPts.size();i++)
        {
            //Define template for each point
            templateRect = trimRect(cv::Point2f(prevPts[i].x, prevPts[i].y), templateWidth, templateHeight, cv::Size(640,480));

            templ = prevFrame(templateRect);

            //Define searchWindow around each point
            searchWindowRect = trimRect(cv::Point2f(prevPts[i].x, prevPts[i].y), searchWinWidth, searchWinHeight, cv::Size(640,480));

            searchWindow = nextFrame(searchWindowRect);

            //use matchTemplate() to look for the template from the previous frame inside of the searchWindow inside of the nextFrame
            cv::matchTemplate(searchWindow,templ,result,CV_TM_CCORR_NORMED);
            cv::minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);

            //get the location of the match within the searchWindow
            float maxLocX = (float)maxLoc.x;
            float maxLocY = (float)maxLoc.y;
            matchLocation = cv::Point2f(maxLocX,maxLocY);

            //get the x and y coordinates of the searchWindow's top left corner
            float searchWindowTopLeft_x = searchWindowRect.tl().x;
            float searchWindowTopLeft_y = searchWindowRect.tl().y;


            //convert the location within the search window to a location within the whole frame
            matchGlobalLocation = cv::Point2f(matchLocation.x + searchWindowTopLeft_x + templateWidth/2, matchLocation.y + searchWindowTopLeft_y + templateHeight/2);
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

    prevPts = corners;
    nextPts = prevPts;
    //Between n and n+2
    for(int i=1;i<18;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;
        nextFrame = cv::imread(fullPath,1);
        cv::cvtColor(nextFrame,nextFrame,cv::COLOR_BGR2GRAY);

        //Define a template and searchwindow for each point
        for(int i=0;i<prevPts.size();i++)
        {
            //Define template for each point
            templateRect = trimRect(cv::Point2f(prevPts[i].x, prevPts[i].y), templateWidth, templateHeight, cv::Size(640,480));

            templ = prevFrame(templateRect);

            //Define searchWindow around each point
            searchWindowRect = trimRect(cv::Point2f(prevPts[i].x, prevPts[i].y), searchWinWidth, searchWinHeight, cv::Size(640,480));

            searchWindow = nextFrame(searchWindowRect);

            //use matchTemplate() to look for the template from the previous frame inside of the searchWindow inside of the nextFrame
            cv::matchTemplate(searchWindow,templ,result,CV_TM_CCORR_NORMED);
            cv::minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);

            //get the location of the match within the searchWindow
            float maxLocX = (float)maxLoc.x;
            float maxLocY = (float)maxLoc.y;
            matchLocation = cv::Point2f(maxLocX,maxLocY);

            //get the x and y coordinates of the searchWindow's top left corner
            float searchWindowTopLeft_x = searchWindowRect.tl().x;
            float searchWindowTopLeft_y = searchWindowRect.tl().y;


            //convert the location within the search window to a location within the whole frame
            matchGlobalLocation = cv::Point2f(matchLocation.x + searchWindowTopLeft_x + templateWidth/2, matchLocation.y + searchWindowTopLeft_y + templateHeight/2);
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

    prevPts = corners;
    nextPts = prevPts;
    //Between n and n+3
    for(int i=1;i<18;i++)
    {
        imageNumber = std::to_string(i);
        fullPath = imagePath + imageNumber + pathEnd;
        nextFrame = cv::imread(fullPath,1);
        cv::cvtColor(nextFrame,nextFrame,cv::COLOR_BGR2GRAY);

        //Define a template and searchwindow for each point
        for(int i=0;i<prevPts.size();i++)
        {
            //Define template for each point
            templateRect = trimRect(cv::Point2f(prevPts[i].x, prevPts[i].y), templateWidth, templateHeight, cv::Size(640,480));

            templ = prevFrame(templateRect);

            //Define searchWindow around each point
            searchWindowRect = trimRect(cv::Point2f(prevPts[i].x, prevPts[i].y), searchWinWidth, searchWinHeight, cv::Size(640,480));

            searchWindow = nextFrame(searchWindowRect);

            //use matchTemplate() to look for the template from the previous frame inside of the searchWindow inside of the nextFrame
            cv::matchTemplate(searchWindow,templ,result,CV_TM_CCORR_NORMED);
            cv::minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);

            //get the location of the match within the searchWindow
            float maxLocX = (float)maxLoc.x;
            float maxLocY = (float)maxLoc.y;
            matchLocation = cv::Point2f(maxLocX,maxLocY);

            //get the x and y coordinates of the searchWindow's top left corner
            float searchWindowTopLeft_x = searchWindowRect.tl().x;
            float searchWindowTopLeft_y = searchWindowRect.tl().y;


            //convert the location within the search window to a location within the whole frame
            matchGlobalLocation = cv::Point2f(matchLocation.x + searchWindowTopLeft_x + templateWidth/2, matchLocation.y + searchWindowTopLeft_y + templateHeight/2);
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
