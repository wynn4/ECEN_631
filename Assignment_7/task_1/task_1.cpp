#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
    //define number of images
    int numImages = 18;
    int slowMult = 5;

    //camera params
    double fx = 825.0900600547;
    double fy = 824.2672147458;
    double cx = 331.6538103208;
    double cy = 252.9284287373;

    double k1 = -0.2380769337;
    double k2 = 0.0931325835;
    double p1 = 0.0003242537;
    double p2 = -0.0021901930;
    double k3 = 0.4641735616;

    double intrinsicParams[9] = {fx, 0, cx,
                                0, fy, cy,
                                0, 0, 1};

    double distCoeffs[5] = {k1, k2, p1, p2, k3};

    cv::Mat M1(3,3, CV_64F, intrinsicParams);
    cv::Mat dist1(5,1, CV_64F, distCoeffs);

    //__use goodFeaturesToTrack to get set of image points
    std::vector<cv::Point2f> features;

    cv::Mat firstFrame;
    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_7/impact_images/T1.jpg",1);
    cv::cvtColor(firstFrame,firstFrame,cv::COLOR_BGR2GRAY);

    //mask for goodFeaturesToTrakc
    cv::Mat mask = cv::Mat::zeros(firstFrame.size(), CV_8UC1);
    //mask around the can
    cv::Mat roi(mask, cv::Rect(cv::Point(270,150), cv::Point(380,370)));
    roi = cv::Scalar(1);
    //unmask around can middle
    cv::Mat roi2(mask,cv::Rect(cv::Point(270,220), cv::Point(380,280)));
    roi2 = cv::Scalar(0);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(firstFrame,features,100,0.01,10,mask,3,false,0.04);

    //define Mat to hold previous and next frames
    cv::Mat prevFrame;
    cv::Mat nextFrame;

    //create vectors to hold prev and next points
    std::vector<cv::Point2f> prevPts;
    std::vector<cv::Point2f> nextPts;

    //vectors to hold optical flow status and err
    std::vector<uchar> status;
    std::vector<float> err;

    cv::Mat colorImage;

    prevFrame = firstFrame;
    prevPts = features;

    std::vector<float> y;
    std::vector<float> yPrime;
    std::vector<float> a;
    std::vector<float> tau;
    std::vector<float> timeToImpact;

    //populate y vector
    float yPoint;
    for(int i=0;i<features.size();i++)
    {
        if(features[i].y <= 240)
        {
           yPoint = 240 - features[i].y;
           y.push_back(yPoint);
        }
        else
            yPoint = features[i].y - 240;
            y.push_back(yPoint);
    }


    //loop through and process/display the images
    for(int i=1;i<numImages + 1;i++)
    {
        int imageNumber = i;
        std::string imagePath = "/home/jesse/Desktop/ECEN_631/Assignment_7/impact_images/T" + std::to_string(imageNumber) + ".jpg";

        //read in the image
        cv::Mat image = cv::imread(imagePath);

        //convert to gray
        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

        nextFrame = image;

        //calcultate optical flow
        cv::calcOpticalFlowPyrLK(prevFrame,nextFrame,prevPts,nextPts,status, err);

        //find y prime for each point in each frame
        float yPrimePoint;
        for(int i=0;i<nextPts.size();i++)
        {
            if(nextPts[i].y <= 240)
            {
                yPrimePoint = 240 - nextPts[i].y;
                yPrime.push_back(yPrimePoint);
            }
            else
                yPrimePoint = nextPts[i].y - 240;
                yPrime.push_back(yPrimePoint);
        }

        //find expansion rate, 'a' for each point in each frame
        float aPoint;
        for(int i=0;i<y.size();i++)
        {
            aPoint = yPrime[i]/y[i];
            a.push_back(aPoint);
        }

        //find time to impact, tau for each point in each frame
        float tauPoint;
        for(int i=0;i<a.size();i++)
        {
            tauPoint = a[i]/(a[i] - 1);
            tau.push_back(tauPoint);
        }

        //just take the midian value of tau and add it to time to impact vector
        float timeToImpactFrame = tau[41];
        timeToImpact.push_back(timeToImpactFrame);




        //draw the feature points
        //convert back to color to display
        cv::cvtColor(image,colorImage,cv::COLOR_GRAY2BGR);

        //draw green dot on all of the feature points
        for(int i=0;i<features.size();i++)
        {
           cv::circle(colorImage,cv::Point(nextPts[i].x,nextPts[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }


        //display the image
        cv::imshow("Frame", colorImage);
        cv::waitKey(33 * slowMult);

        //reset and clear out the vectors
        prevPts = nextPts;
        prevFrame = image;
        y.clear();
        y = yPrime;

        yPrime.clear();
        a.clear();
        tau.clear();

    }


    for(int i=0;i<timeToImpact.size();i++)
    {
        std::cout << timeToImpact[i] << std::endl;
    }

    cv::destroyAllWindows();

    return 0;
}







