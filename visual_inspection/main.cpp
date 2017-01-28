#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

int main()
{
    //Levels of Classification
    //--Good
    //--Ugly
    //--Bad

    //The Cooke image files
    std::string nilla_bad_1 = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_bad.bmp";
    std::string nilla_none = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/Background.bmp";
    std::string nilla_edge = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_edge.bmp";
    std::string nilla_good = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_good.bmp";
    std::string nilla_moving_good = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_moving_good.bmp";
    std::string nilla_moving_bad = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_moving_really_ugly.bmp";
    std::string nilla_bad_2 = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_really_bad.bmp";
    std::string nilla_ugly1 = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_really_ugly.bmp";
    std::string nilla_ugly2 = "/home/jesse/Desktop/ECEN_631/visual_inspection/images/nilla_ugly.bmp";

    //Create a namedWindow
    cv::namedWindow("cookie", 1);

    //Define image objects
    cv::Mat image;
    cv::Mat image_gray;
    cv::Mat image_blur;
    cv::Mat image_threshold;

    //Read the image
    image = cv::imread(nilla_moving_bad, 1);

    //Apply initial processing operations to image
    cv::cvtColor(image, image_gray, 6); //convert to grayscale (COLOR_BGR2GRAY == 6)
    cv::GaussianBlur(image_gray, image_blur, cv::Size(7,7), 1.5, 1.5);  //apply blur to reduce noise
    cv::threshold(image_blur, image_threshold, 100, 255, 0);


    //ADD TOP AND BOTTOM ROI
    //cv::rectangle(image_threshold,cv::Point(150,480), cv::Point(520,400),255,0.5,0);
    //cv::rectangle(image_threshold,cv::Point(150,0), cv::Point(520,80),255,0.5,0);
    //cv::rectangle(image_threshold,cv::Point(150,80),cv::Point(520,400),255,0.5,0);
    cv::Rect bottomROI(cv::Point(150,480), cv::Point(520, 400));
    cv::Rect topROI(cv::Point(150,0), cv::Point(520,80));
    cv::Rect detectionROI(cv::Point(150,80),cv::Point(520,400));


    int bottomArea = cv::countNonZero(image_threshold(bottomROI));
    int topArea = cv::countNonZero(image_threshold(topROI));
    std::cout << bottomArea << ", " << topArea << std::endl;

    //BLOB detect
    cv::SimpleBlobDetector detector;
    std::vector<cv::KeyPoint> keypoints;
    detector.detect(image_threshold, keypoints);

    cv::Mat image_w_keypoints;

    cv::drawKeypoints(image_threshold, keypoints, image_w_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    //add the text
    cv::putText(image_w_keypoints, "Cookie!", cvPoint(175,150),2, 2.5, cvScalar(255), 1, CV_AA); //FONT_HERSHEY_DUPLEX == 2

    //show the image
    cv::imshow("cookie", image_w_keypoints);
    cv::waitKey(0);

    return 0;
}
