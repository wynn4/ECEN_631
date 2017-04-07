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

    //vector to hold image points
    cv::Point2f point1 = cv::Point2f(282,173);  //specific points picked from the first frame
    cv::Point2f point2 = cv::Point2f(365,175);  //specific points picked from the first frame
    std::vector<cv::Point2f> features = {point1, point2};

    //first frame
    cv::Mat firstFrame;
    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_7/impact_images/T1.jpg",1);
    cv::cvtColor(firstFrame,firstFrame,cv::COLOR_BGR2GRAY);

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

    //vector to hold distances to the object
    std::vector<float> Z_distance;

    prevFrame = firstFrame;
    prevPts = features;



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

        //undistort the points in the nextPts vector
        std::vector<cv::Point2f> nextPtsUndist;
        cv::undistortPoints(nextPts,nextPtsUndist,M1,dist1);

        //go through and convert undistorted points back to pixels
        for(int i=0;i<nextPtsUndist.size();i++)
        {
            nextPtsUndist[i] = cv::Point2f(nextPtsUndist[i].x*fx + cx, nextPtsUndist[i].y*fy + cy);
        }

        //calculate distance to the object as Z(mm) = f(pixels)*X(mm)/x(pixel)
        float Z_mm;
        float X_mm = 59;
        float x_pixel = nextPtsUndist[1].x - nextPtsUndist[0].x;

        Z_mm = fx*(X_mm/x_pixel);

        Z_distance.push_back(Z_mm);


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
        cv::waitKey(33 * 5);

        //reset
        prevPts = nextPts;
        prevFrame = image;

    }

    for(int i=0;i<Z_distance.size();i++)
    {
        std::cout << Z_distance[i] << std::endl;
    }

    cv::destroyAllWindows();

    return 0;
}







