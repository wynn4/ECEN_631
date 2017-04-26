#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
    double scale_factor = 0.8;
    std::FILE* file;
    //std::string filename = "/home/jesse/Desktop/practice_vo_files/my_vo_data_practice.txt";
    std::string filename = "/home/jesse/Desktop/practice_vo_files/my_vo_data_hall.txt";
    file = std::fopen(filename.c_str(),"w");

    int num_images = 2240; //702 practice 2240 hall

//    // practice camera params
//    double fx = 707.09120;
//    double fy = 707.09120;
//    double ox = 601.88730;
//    double oy = 183.11040;

//    double intrinsicParams[9] = {fx, 0, ox,
//                                0, fy, oy,
//                                0, 0, 1};
//    cv::Mat M(3,3, CV_64F, intrinsicParams);

    // hall camera params
    double fx = 677.41251774486568;
    double fy = 680.73800850564749;
    double ox = 323.12557438767283;
    double oy = 224.77413395670021;

    double intrinsicParams[9] = {fx, 0, ox,
                                0, fy, oy,
                                0, 0, 1};
    cv::Mat M(3,3, CV_64F, intrinsicParams);


    //mat to hold the gray image
    cv::Mat grayImage;
    cv::Mat firstFrame;
    cv::Mat colorFrame;

    //load the sample image to get image size
    //firstFrame = cv::imread("/home/jesse/Desktop/practice_vo_files/VO_Practice_Sequence/000000.png", CV_LOAD_IMAGE_GRAYSCALE);
    firstFrame = cv::imread("/home/jesse/Desktop/practice_vo_files/BYU_Hallway_Sequence/000000.png", CV_LOAD_IMAGE_GRAYSCALE);

    int width = firstFrame.cols;
    int height = firstFrame.rows;
    int widthTrim = width*0.1;
    int heightTrim = height*0.1;



    //__use goodFeaturesToTrack to get set of image points
    std::vector<cv::Point2f> initialPoints;


    //mask for goodFeaturesToTrakc
    cv::Mat mask = cv::Mat::zeros(firstFrame.size(), CV_8UC1);
    //mask away from the edge a bit
    cv::Mat roi(mask, cv::Rect(cv::Point(0,0), cv::Point(width, height)));
    roi = cv::Scalar(1);
    //unmask around the middle
    cv::Mat roi2(mask,cv::Rect(cv::Point((width/2) - widthTrim, (height/2) - heightTrim), cv::Point((width/2) + widthTrim, (height/2) + heightTrim)));
    roi2 = cv::Scalar(1);

    //use goodFeaturesToTrack to find points to track
    cv::goodFeaturesToTrack(firstFrame,initialPoints,500,0.01,10,mask,3,false,0.04);


    //file path
    //std::string imgSet = "/home/jesse/Desktop/practice_vo_files/VO_Practice_Sequence/";
    std::string imgSet = "/home/jesse/Desktop/practice_vo_files/BYU_Hallway_Sequence/";


    cv::Mat prevFrame;
    cv::Mat nextFrame;

    std::vector<cv::Point2f> prevPts;
    std::vector<cv::Point2f> nextPts;

    std::vector<cv::Point2f> prevPtsMatched;
    std::vector<cv::Point2f> nextPtsMatched;

    std::vector<cv::Point2f> nextMatchedUndist;
    std::vector<cv::Point2f> prevMatchedUndist;

    //Mat to hold fundamental matrix and the output mask
    cv::Mat F;
    cv::Mat F_new;
    cv::Mat fMask;

    //ransac tuning params
    double param1 = 0.5;      //max distance (pixels) from epipolar for non-outlier
    double param2 = 0.99;   //confidence level that match is correct

    //initialize
    prevFrame = firstFrame;
    prevPts = initialPoints;

    std::vector<uchar> status;
    std::vector<float> err;

    //initialize C_k
    double initial_C_k[16] = {1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              0, 0, 0, 1};

    cv::Mat C_k(4,4, CV_64F, initial_C_k);


    //loop through all images in file
    int count = 0;
    for(int i=0;i<num_images;i++)
    {
        std::stringstream ss;
        ss << imgSet << std::setw(6) << std::setfill('0') << i << ".png";
        std::string s = ss.str();

        grayImage = cv::imread(s, CV_LOAD_IMAGE_GRAYSCALE);

        if (grayImage.data == NULL)
        {
            std::cout << "Could not open image: " + s << std::endl;
            return -1;
        }

        nextFrame = grayImage;

        //got the image, now lets process...

        //calcultate optical flow
        cv::calcOpticalFlowPyrLK(prevFrame,nextFrame,prevPts,nextPts,status, err);


        //get a filter mask (fmask) to eliminate outliers using cv::findFundamentalMat()
        F = cv::findFundamentalMat(prevPts,nextPts,fMask,CV_FM_RANSAC,param1,param2);

        //go through the mask and where there's a '1', keep the point, where '0' toss it
        for(int i=0;i<fMask.size().height;i++)
        {
            if(fMask.at<bool>(0,i) == 1)
            {
                nextPtsMatched.push_back(nextPts[i]);
                prevPtsMatched.push_back(prevPts[i]);

            }
        }

        //_got the good points, now lets figure out how the camera moved

        //_undistort
        cv::undistortPoints(nextPtsMatched,nextMatchedUndist, M, cv::noArray(), cv::noArray(),M);
        cv::undistortPoints(prevPtsMatched,prevMatchedUndist, M, cv::noArray(), cv::noArray(),M);

        //convert points back to image frame (pixels)
        for(int i=0;i<nextMatchedUndist.size();i++)
        {
            nextMatchedUndist[i].x = nextMatchedUndist[i].x * fx + ox;
            nextMatchedUndist[i].y = nextMatchedUndist[i].y * fy + oy;

            prevMatchedUndist[i].x = prevMatchedUndist[i].x * fx + ox;
            prevMatchedUndist[i].y = prevMatchedUndist[i].y * fy + oy;
        }


        //calculate essential matrix E from F (using equation on slide 28 of 3D Reconstruction lecture slides)
        cv::Mat E;
        E = M.t()*F*M;

        //Find normalized E using SVD
        cv::Mat w;  //singular values
        cv::Mat u;
        cv::Mat vt;

        cv::SVD::compute(E,w,u,vt);

        //make a new w matrix and call it sigma
        double sigmaVals[9] = {1, 0, 0,
                               0, 1, 0,
                               0, 0, 0};

        cv::Mat sigma(3,3, CV_64F, sigmaVals);

        //std::cout << sigma << std::endl;

        //compute normalized E
        E = u*sigma*vt;

        //recover pose
        cv::Mat R;
        cv::Mat t_recover;
        cv::recoverPose(E,nextMatchedUndist,prevMatchedUndist, R, t_recover,fx, cv::Point2f(ox,oy));

        if(t_recover.at<double>(2) > 0)
        {
            std::cout << "true" << std::endl;
            t_recover = -1*t_recover;
            t_recover = scale_factor*t_recover;  //add in the scale factor
        }
        else
            std::cout << "false" << std::endl;

        //Form T_k
        cv::Mat T_and_R;
        cv::Mat T_k;
        double zero_one[4] = {0, 0, 0, 1};
        cv::Mat bott_row(1,4, CV_64F,zero_one);
        cv::hconcat(R,t_recover,T_and_R);
        cv::vconcat(T_and_R, bott_row, T_k);

        // got T_k and now I have to create C_k
        C_k = C_k*T_k.inv();

        //std::cout << "\n" << std::endl;
        //std::cout << C_k << std::endl;


        //Write C_k to file
        if(count > 0)
        {
            for(int i=0;i<C_k.rows -1;i++)
            {
                for(int j=0;j<C_k.cols;j++)
                {
                    std::fprintf(file,"%lf ", C_k.at<double>(i,j));
                }
            }
            std::fprintf(file,"\n");
        }

        //convert back to color to display
        cv::cvtColor(grayImage,colorFrame,cv::COLOR_GRAY2BGR);

        //draw green dot on all the original corner points
        for(int i=0;i<nextPtsMatched.size();i++)
        {
            cv::circle(colorFrame,cv::Point(prevPtsMatched[i].x,prevPtsMatched[i].y),1,cv::Scalar(0,255,0),2,8,0);
        }

        //draw red line between the current and previous points
        for(int i=0;i<nextPtsMatched.size();i++)
        {
            cv::line(colorFrame,cv::Point(prevPtsMatched[i].x,prevPtsMatched[i].y),cv::Point(nextPtsMatched[i].x,nextPtsMatched[i].y),cv::Scalar(0,0,255),2,8,0);
        }


        cv::imshow("image", colorFrame);
        cv::waitKey(1);




        //reset frame and clear out the vectors
        prevFrame = grayImage;
        prevPts.clear();
        nextPtsMatched.clear();
        prevPtsMatched.clear();
        nextMatchedUndist.clear();
        prevMatchedUndist.clear();

        //prevPoints = findgoodfeaturs to track
        cv::goodFeaturesToTrack(prevFrame,prevPts,500,0.01,10,mask,3,false,0.04);
        count++;
        //i = i+1;
    }
    cv::destroyAllWindows();
    std::fclose(file);

    return 0;
}







