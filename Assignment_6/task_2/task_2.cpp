#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
    //set which image set are we working with
    std::string imageFolder = "turned_real";  //parallel_cube parallel_real turned_cube turned real
    std::string imageName = "TurnReal";     //ParallelCube ParallelReal TurnCube TurnReal


    //Read first and last points from file (from last assignment) and store in vectors
    std::vector<cv::Point2f> firstPoints;
    std::vector<cv::Point2f> lastPoints;

    cv::FileStorage fs("/home/jesse/Desktop/ECEN_631/Assignment_6/first_and_last_" + imageFolder + ".xml", cv::FileStorage::READ);
    fs["first_points"] >> firstPoints;
    fs["last_points"] >> lastPoints;


    //Use first and last points to estimate fundamental matrix F
    cv::Mat F;
    F = cv::findFundamentalMat(firstPoints,lastPoints);


    //known camera intrinsic parameters
    double fx = 825.0900600547;
    double fy = 824.2672147458;
    double cx = 331.6538103208;
    double cy = 252.9284287373;

    double k1 = -0.2380769337;
    double k2 = 0.0931325835;
    double p1 = 0.0003242537;
    double p2 = -0.0021901930;
    double k3 = 0.4641735616;

    double intrinsicMatrix[9] = {fx, 0, cx,
                                0, fy, cy,
                                0, 0, 1};

    double distMatrix[5] = {k1, k2, p1, p2, k3};


    cv::Mat M1(3,3, CV_64F, intrinsicMatrix);
    cv::Mat M2(3,3, CV_64F, intrinsicMatrix);

    cv::Mat dist1(5,1, CV_64F, distMatrix);
    cv::Mat dist2(5,1, CV_64F, distMatrix);

    //use undistortPoints()
    std::vector<cv::Point2f> firstPointsUndist;
    std::vector<cv::Point2f> lastPointsUndist;

    cv::undistortPoints(firstPoints,firstPointsUndist,M1,dist1,cv::noArray(),cv::noArray());
    cv::undistortPoints(lastPoints,lastPointsUndist,M2,dist2,cv::noArray(),cv::noArray());

    //convert points back to image frame (pixels)
    for(int i=0;i<firstPointsUndist.size();i++)
    {
        firstPointsUndist[i].x = firstPointsUndist[i].x * fx + cx;
        firstPointsUndist[i].y = firstPointsUndist[i].y * fy + cy;

        lastPointsUndist[i].x = lastPointsUndist[i].x * fx + cx;
        lastPointsUndist[i].y = lastPointsUndist[i].y * fy + cy;
    }


    //calculate essential matrix E from F (using equation on slide 28 of 3D Reconstruction lecture slides)
    cv::Mat E;
    E = M1.t()*F*M2;

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

    //compute normalized E
    E = u*sigma*vt;

    //decompose E into Rotation and translation matrix
    cv::Mat R1;
    cv::Mat R2;
    cv::Mat t;

    //decomposeEssentialMat returns one of four possible translation and rotation paris
    cv::decomposeEssentialMat(E,R1,R2,t);

    //recover pose figures out the correct rotation and translation for you
    cv::Mat R;
    cv::Mat t_recover;
    cv::recoverPose(E,firstPointsUndist,lastPointsUndist, R, t_recover,fx, cv::Point2f(cx,cy));

    //std::cout << R << "\n" << t_recover << std::endl;
    std::cout << R1 << "\n" << R2 << "\n" << t << std::endl;


    //Write R, t_recover, E and F to file
    std::string filename = "/home/jesse/Desktop/ECEN_631/Assignment_6/task_2/" + imageFolder + "_data" + ".txt";
    std::FILE* file;
    file = std::fopen(filename.c_str(),"w");
    std::fprintf(file,"Assignment6 Task 2 Structure from Motion Submission File\n");
    std::fprintf(file, imageFolder.c_str());
    std::fprintf(file, ":\n");
    std::fprintf(file,"Rotation Matrix, R:\n\n");
    for(int i=0;i<R.rows;i++)
    {
        for(int j=0;j<R.cols;j++)
        {
            std::fprintf(file,"%lf ", R.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n");

    std::fprintf(file,"Translation Matrix, T:\n\n");
    for(int i=0;i<t_recover.rows;i++)
    {
        for(int j=0;j<t_recover.cols;j++)
        {
            std::fprintf(file,"%lf ", t_recover.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n");

    std::fprintf(file,"Essential Matrix, E:\n\n");
    for(int i=0;i<E.rows;i++)
    {
        for(int j=0;j<E.cols;j++)
        {
            std::fprintf(file,"%lf ", E.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n");

    std::fprintf(file,"Fundamental Matrix, F:\n\n");
    for(int i=0;i<F.rows;i++)
    {
        for(int j=0;j<F.cols;j++)
        {
            std::fprintf(file,"%lf ", F.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n");

    std::fclose(file);



    return 0;
}







