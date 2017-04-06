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

    //loop through and process/display the images
    for(int i=1;i<numImages + 1;i++)
    {
        int imageNumber = i;
        std::string imagePath = "/home/jesse/Desktop/ECEN_631/Assignment_7/impact_images/T" + std::to_string(imageNumber) + ".jpg";

        //read in the image
        cv::Mat image = cv::imread(imagePath);

        //display the image
        cv::imshow("Frame", image);
        cv::waitKey(33 * slowMult);
    }



    cv::destroyAllWindows();

    return 0;
}







