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
    std::string imagePath = "/home/jesse/Desktop/ECEN_631/Assignment_5/parallel_cube/ParallelCube";
    std::string imageNumber;
    std::string pathEnd = ".jpg";
    std::string fullPath;

    cv::Mat firstFrame;
    cv::Mat frame;
    cv::Mat colorFrame;

    int slowMultiplier = 50;

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


    //Look at first image to find features to track
    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/parallel_cube/ParallelCube10.jpg",1);
    cv::cvtColor(firstFrame,firstFrame,cv::COLOR_BGR2GRAY);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(firstFrame,corners,1000,0.01,10,mask,3,false,0.04);

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
    std::vector<cv::Point2f> firstPrevPtsMatched;
    std::vector<cv::Point2f> lastNextPtsMatched;

    //vector to hold delete indicies
    std::vector<int> deleteIndices;

    //ransac tuning params
    double param1 = 3;      //max distance (pixels) from epipolar for non-outlier
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

        //filter out the outliers using cv::findFundamentalMat()
        F = cv::findFundamentalMat(prevPts,nextPts,fMask,CV_FM_RANSAC,param1,param2);

        //go through the mask and where there's a (1), keep, where (0) toss
        //std::cout << fMask.size().height << std::endl;
        for(int i=0;i<fMask.size().height;i++)
        {
            if(fMask.at<bool>(0,i) == 1)
            {
                nextPtsMatched.push_back(nextPts[i]);
                prevPtsMatched.push_back(prevPts[i]);
            }
            else
                deleteIndices.push_back(i);
        }



        if(i == 10) //first frame
        {
            firstPrevPtsMatched = prevPtsMatched;
        }
        else if(i==15)
        {
            lastNextPtsMatched = nextPtsMatched;

            //go through firstPrevPtsMatched vector and erase elements at deleteIndices locations

            for(int i=0;i<deleteIndices.size();i++)
            {
                firstPrevPtsMatched.erase(firstPrevPtsMatched.begin() + deleteIndices[i] -1);
                //std::cout << number_to_delete << std::endl;
            }
        }
        else
        {
            //go through firstPrevPtsMatched vector and erase elements at deleteIndices locations

            for(int i=0;i<deleteIndices.size();i++)
            {
                firstPrevPtsMatched.erase(firstPrevPtsMatched.begin() + deleteIndices[i] -1);
                //std::cout << number_to_delete << std::endl;
            }

        }


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
        cv::waitKey(slowMultiplier*34);    //~30 fps

        //clear and reset the vectors of points
        matchLocations.clear();
        prevPts.clear();
        prevPts = nextPtsMatched;
        nextPts.clear();
        nextPtsMatched.clear();
        prevPtsMatched.clear();
        deleteIndices.clear();
        prevFrame = nextFrame;

    }


    //write firstPrevPtsMatched and lastNextPtsMatched to file
    cv::FileStorage fs("first_and_last_points.xml", cv::FileStorage::WRITE);
    fs << "First_Points" << firstPrevPtsMatched;
    fs << "Last_Points" << lastNextPtsMatched;

    //display the first and last points on their respective images
    cv::Mat firstImage;
    cv::Mat lastImage;

    firstImage = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/parallel_cube/ParallelCube10.jpg",1);
    lastImage = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/parallel_cube/ParallelCube15.jpg",1);

    //draw green dot on all of the points that survive from the first frame on the first image
    for(int i=0;i<firstPrevPtsMatched.size();i++)
    {
       cv::circle(firstImage,cv::Point(firstPrevPtsMatched[i].x,firstPrevPtsMatched[i].y),1,cv::Scalar(0,255,0),2,8,0);
    }

    //draw green dot on all of the points that survived from the first frame on the last image
    for(int i=0;i<lastNextPtsMatched.size();i++)
    {
       cv::circle(lastImage,cv::Point(lastNextPtsMatched[i].x,lastNextPtsMatched[i].y),1,cv::Scalar(0,255,0),2,8,0);
    }

    std::cout << firstPrevPtsMatched.size() << std::endl;
    std::cout << lastNextPtsMatched.size() << std::endl;

    //display the final image pair
    cv::imshow("First Frame", firstImage);
    cv::imshow("Last Frame", lastImage);
    cv::waitKey(0);

//    //Read the individual camera calibration parameters from file
//    cv::FileStorage fs1("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_L_parameters.xml", cv::FileStorage::READ);
//    fs1["Camera_Matrix"] >> cameraMatrixL;
//    fs1["Distortion_Coefficients"] >> distCoeffsL;





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
