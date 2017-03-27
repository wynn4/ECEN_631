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

    //get the path to the template matching images
    std::string imageFolder = "parallel_cube/";
    std::string imageFile = "ParallelCube";

    std::string imagePath = "/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + imageFile;
    std::string imageNumber;
    std::string pathEnd = ".jpg";
    std::string fullPath;

    cv::Mat firstFrame;
    cv::Mat frame;
    cv::Mat colorFrame;
    cv::Mat smallFrame;

    int slowMultiplier = 10;


    //create vector to hold feature corners
    std::vector<cv::Point2f> corners;

    //mask for goodFeaturesToTrack
    cv::Mat mask = cv::Mat::zeros(cv::Size(0,0), CV_8UC1);


    //define a searchWindowSize
    float searchWinWidth = 81;
    float searchWinHeight = 81;

    //define a template size
    float templateWidth = 31;
    float templateHeight = 31;


    //Look at first image to find features to track
    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + imageFile + "10.jpg",1);
    cv::cvtColor(firstFrame,firstFrame,cv::COLOR_BGR2GRAY);

    //define a smaller frame within firstFrame for goodFeaturesToTrack to look in
    cv::Rect roi = cv::Rect(cv::Point(40,40), cv::Point(600,440));
    smallFrame = firstFrame(roi);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(smallFrame,corners,1000,0.01,10,mask,3,false,0.04);

    //convert corner locations back into the full frame coordinates
    for(int i=0;i<corners.size();i++)
    {
        corners[i].x = corners[i].x + 40;
        corners[i].y = corners[i].y + 40;
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

    //Mat to hold fundamental matrix and the output mask
    cv::Mat F;
    cv::Mat fMask;

    //vectors to hold nextPts and previousPts matches
    std::vector<cv::Point2f> nextPtsMatched;
    std::vector<cv::Point2f> prevPtsMatched;

    //vector to hold first matching points
    std::vector<cv::Point2f> originalPrevPoints;
    std::vector<cv::Point2f> lastSurvivingNextPoints;

    //set originalPrevPoints to be corners
    originalPrevPoints = corners;


    //ransac tuning params
    double param1 = 0.01;      //max distance (pixels) from epipolar for non-outlier
    double param2 = 0.99;   //confidence level that match is correct

    prevFrame = firstFrame;

    prevPts = corners;
    nextPts = prevPts;
    //Between n and n+1
    for(int i=10;i<16;i++)
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
            cv::matchTemplate(searchWindow,templ,result,CV_TM_SQDIFF_NORMED);//CV_TM_CCORR_NORMED
            cv::minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);

            //get the location of the match within the searchWindow
            float maxLocX = (float)minLoc.x;
            float maxLocY = (float)minLoc.y;
            matchLocation = cv::Point2f(maxLocX,maxLocY);

            //get the x and y coordinates of the searchWindow's top left corner
            float searchWindowTopLeft_x = searchWindowRect.tl().x;
            float searchWindowTopLeft_y = searchWindowRect.tl().y;

//            //get the x and y coordinates of the template's top left corner
//            float templateTopLeft_x = templateRect.tl().x;
//            float templateTopLeft_y = templateRect.tl().y;


            //convert the location within the search window to a location within the whole frame
            matchGlobalLocation = cv::Point2f(matchLocation.x + searchWindowTopLeft_x + templateWidth/2, matchLocation.y + searchWindowTopLeft_y + templateHeight/2);
            //std::cout << matchGlobalLocation << std::endl;

            matchLocations.push_back(matchGlobalLocation);
        }

        //set nextPoints equal to the matchLocations
        nextPts = matchLocations;


        //get a filter mask (fmask) to eliminate outliers using cv::findFundamentalMat()
        F = cv::findFundamentalMat(prevPts,nextPts,fMask,CV_FM_RANSAC,param1,param2);


        //go through the mask and where there's a '1', keep the point, where '0' toss it
        for(int i=0;i<fMask.size().height;i++)
        {
            if(fMask.at<bool>(0,i) == 1)
            {
                nextPtsMatched.push_back(nextPts[i]);
                prevPtsMatched.push_back(prevPts[i]);

                //push the indices corresponding to good points onto the pack of originalPrevPoints vector
                originalPrevPoints.push_back(originalPrevPoints[i]);
            }
        }

        //Then delete off the old points in the beginning of the originalPrevPoints vector
        originalPrevPoints.erase(originalPrevPoints.begin(),originalPrevPoints.begin()+fMask.size().height);

        // Note: At this point, originalPrevPoints contains only the points that survived the RANSAC mask


        //last time through, populate lastNextPtsMatched with the nextPtsMatched points
        if(i==15)
        {
            lastSurvivingNextPoints = nextPtsMatched;
        }


        //__Display Stuff___

        //convert back to color to display
        cv::cvtColor(nextFrame,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<prevPtsMatched.size();i++)
        {
           cv::circle(colorFrame,cv::Point(prevPtsMatched[i].x,prevPtsMatched[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<prevPtsMatched.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPtsMatched[i].x,prevPtsMatched[i].y),cv::Point(nextPtsMatched[i].x,nextPtsMatched[i].y),cv::Scalar(0,0,255),2,8,0);
        }

        //put text
        cv::putText(colorFrame,"Frame-to-Frame",cv::Point(30,30),2,1,cv::Scalar(0,255,0),1,8);

        //show the image
        cv::imshow("frame",colorFrame);
        cv::waitKey(slowMultiplier*34);    //~30 fps divided by slow multiplier

//        //check that they are the same size
//        std::cout << prevPtsMatched.size() << std::endl;
//        std::cout << originalPrevPoints.size() << std::endl;


        //__CLEAR AND RESET for next loop
        matchLocations.clear();
        prevPts.clear();
        prevPts = nextPtsMatched;
        nextPts.clear();
        nextPtsMatched.clear();
        prevPtsMatched.clear();
        prevFrame = nextFrame;
    }



    //__Display the first and last points on their respective images
    cv::Mat firstImage;
    cv::Mat lastImage;

    firstImage = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + imageFile + "10.jpg",1);
    lastImage = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + imageFile + "15.jpg",1);

    //draw green dot on all of the points that survive from the first frame on the first image
    for(int i=0;i<originalPrevPoints.size();i++)
    {
        cv::circle(firstImage,cv::Point2f(originalPrevPoints[i].x,originalPrevPoints[i].y),1,cv::Scalar(0,255,0),2,8,0);
        cv::line(firstImage,cv::Point2f(originalPrevPoints[i].x,originalPrevPoints[i].y),cv::Point2f(lastSurvivingNextPoints[i].x,lastSurvivingNextPoints[i].y),
                 cv::Scalar(0,0,255));
    }

    //draw green dot on all of the points that survived from the first frame on the last image
    for(int i=0;i<lastSurvivingNextPoints.size();i++)
    {
        cv::circle(lastImage,cv::Point2f(lastSurvivingNextPoints[i].x,lastSurvivingNextPoints[i].y),1,cv::Scalar(0,255,0),2,8,0);
    }

    std::cout << originalPrevPoints.size() << std::endl;
    std::cout << lastSurvivingNextPoints.size() << std::endl;

    //display the final image pair
    cv::imshow("First Frame", firstImage);
    cv::imshow("Last Frame", lastImage);
    cv::waitKey(0);

//    //write images to file
//    cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_5/turned_real_first.jpg",firstImage);
//    cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_5/turned_real_last.jpg",lastImage);


//    //write points to file
//    cv::FileStorage fs("first_and_last_turned_real.xml", cv::FileStorage::WRITE);
//    fs << "first_points" << originalPrevPoints;
//    fs << "last_points" << lastSurvivingNextPoints;

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
