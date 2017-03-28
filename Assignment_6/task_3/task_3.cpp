#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>



int main()
{
//    int closestPoint = 66;  //parallel cube and parallel real
    int closestPoint = 71;    //turned cube and turned real

    //set which image set are we working with
    std::string imageFolder = "turned_real";  //parallel_cube parallel_real turned_cube turned real
    std::string imageName = "TurnReal";     //ParallelCube ParallelReal TurnCube TurnReal


    //Read first and last points from file (from last assignment) and store in vectors
    std::vector<cv::Point2f> firstPoints;
    std::vector<cv::Point2f> lastPoints;

    cv::FileStorage fs1("/home/jesse/Desktop/ECEN_631/Assignment_6/first_and_last_" + imageFolder + ".xml", cv::FileStorage::READ);
    fs1["first_points"] >> firstPoints;
    fs1["last_points"] >> lastPoints;

    //Read R and t from task_2
    cv::Mat R;
    cv::Mat T;
    //write to .xml file
    cv::FileStorage fs2("/home/jesse/Desktop/ECEN_631/Assignment_6/task_2/rotation_and_translation_" + imageFolder + ".xml", cv::FileStorage::READ);
    fs2["rotation"] >> R;
    fs2["t"] >> T;


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


    //Get the stereo rectification params
    //initialize output Mats
    cv::Mat R1(3,3, CV_64F);
    cv::Mat R2(3,3, CV_64F);
    cv::Mat P1(3,4, CV_64F);
    cv::Mat P2(3,4, CV_64F);
    cv::Mat Q(4,4, CV_64F);;

    cv::stereoRectify(M1,dist1,M2,dist2,cv::Size(640,480),R,T,R1,R2,P1,P2,Q,0,-1,cv::Size(640,480),0,0);

    //Undistort the points
    std::vector<cv::Point2f> firstPointsUndist;
    std::vector<cv::Point2f> lastPointsUndist;

    cv::undistortPoints(firstPoints,firstPointsUndist, M1, dist1, R1,P1);
    cv::undistortPoints(lastPoints,lastPointsUndist, M2, dist2, R2,P2);

    //Construct vectors of points [x, y, disparity] in a loop to pass into perspectiveTransform() function to get 3D info
    cv::Point3f firstWDisparity;
    cv::Point3f lastWDisparity;
    std::vector<cv::Point3f> firstWDispVec;
    std::vector<cv::Point3f> lastWDispVec;

    for(int i=0;i<firstPointsUndist.size();i++)
    {
        //calculate the disparity
        float disparity = firstPointsUndist[i].x - lastPointsUndist[i].x;

        //create the points
        firstWDisparity = cv::Point3f(firstPointsUndist[i].x, firstPointsUndist[i].y, disparity);
        lastWDisparity = cv::Point3f(lastPointsUndist[i].x, lastPointsUndist[i].y, disparity);

        //push the points onto the vectors
        firstWDispVec.push_back(firstWDisparity);
        lastWDispVec.push_back(lastWDisparity);
    }

    //Use cv::perspectiveTransform() to calculate 3D info of the points
    std::vector<cv::Point3f> first3DCoords;
    std::vector<cv::Point3f> last3DCoords;

    cv::perspectiveTransform(firstWDispVec,first3DCoords,Q);
    cv::perspectiveTransform(lastWDispVec,last3DCoords,Q);

    //The 66th point is closest to the camera find the scale factor so that it's 20" from camera
    //float scaleFactorParallel = 2.915;
    float scaleFactorTurned = 4.1163;

    float scaleFactor = scaleFactorTurned;

    //go through all the points and multiply by the scale factor
    for(int i=0;i<first3DCoords.size();i++)
    {
        first3DCoords[i].x = first3DCoords[i].x*scaleFactor;
        first3DCoords[i].y = first3DCoords[i].y*scaleFactor;
        first3DCoords[i].z = first3DCoords[i].z*scaleFactor;

        last3DCoords[i].x = last3DCoords[i].x*scaleFactor;
        last3DCoords[i].y = last3DCoords[i].y*scaleFactor;
        last3DCoords[i].z = last3DCoords[i].z*scaleFactor;
    }

    std::cout << first3DCoords[closestPoint] << std::endl;

    //multiply the T matrix by the scale factor to see how the camera moved
    T.at<double>(0,0) = T.at<double>(0,0)*scaleFactor;
    T.at<double>(1,0) = T.at<double>(1,0)*scaleFactor;
    T.at<double>(2,0) = T.at<double>(2,0)*scaleFactor;

    std::cout << T << std::endl;



    //load the image to see the points
    cv::Mat firstFrame;
    cv::Mat lastFrame;

    firstFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + "/" + imageName + "10.jpg");
    lastFrame = cv::imread("/home/jesse/Desktop/ECEN_631/Assignment_5/" + imageFolder + "/" + imageName + "15.jpg");


//    //draw green dot on all the original corner points
//    for(int i=0;i<firstPoints.size();i++)
//    {
//       cv::circle(firstFrame,cv::Point(firstPoints[i].x,firstPoints[i].y),1,cv::Scalar(0,255,0),2,8,0);

//       //put their number next to each poit
//       cv::putText(firstFrame, std::to_string(i), cv::Point(firstPoints[i].x + 2, firstPoints[i].y + 2), 1, 0.75, cv::Scalar(0,255,0));
//    }

//    //color closest point red
//    cv::circle(firstFrame,cv::Point(firstPoints[closestPoint].x,firstPoints[closestPoint].y),2,cv::Scalar(0,0,255),2,8,0);



    //pick four random points and plot their 3D information
//    int four_points[4] = {32, 18, 187, 205}; //parallel_cube
//    int four_points[4] = {291, 423, 77, 389}; //parallel_real
//    int four_points[4] = {13, 47, 113, 123}; //turned_cube
    int four_points[4] = {14, 149, 118, 147}; //turned_real

    //plot in a loop
    for(int i=0;i<4;i++)
    {
        //plot the 3D data of the four points
        std::string data = "[" + std::to_string(first3DCoords[four_points[i]].x) + "; " + std::to_string(first3DCoords[four_points[i]].y) + "; " + std::to_string(first3DCoords[four_points[i]].z) + "]";
        cv::putText(firstFrame, data, cv::Point(firstPoints[four_points[i]].x + 2, firstPoints[four_points[i]].y + 2), 1, 0.9, cv::Scalar(0,0,255));

        //plot the points
        cv::circle(firstFrame,cv::Point(firstPoints[four_points[i]].x,firstPoints[four_points[i]].y),2,cv::Scalar(0,255,0),2,8,0);
    }






    cv::imshow("first frame", firstFrame);
//    cv::imshow("last frame", lastFrame);
    cv::waitKey(0);

    //write image to file
    cv::imwrite("/home/jesse/Desktop/ECEN_631/Assignment_6/task_3/" + imageFolder + ".bmp", firstFrame);

    //write translation and scale factor to file
    std::string filename = "/home/jesse/Desktop/ECEN_631/Assignment_6/task_3/" + imageFolder + "_translation_and_scale_factor" + ".txt";
    std::FILE* file;
    file = std::fopen(filename.c_str(),"w");
    std::fprintf(file,"Assignment6 Task 3 Structure from Motion Submission File\n");
    std::fprintf(file, "Image Set: ");
    std::fprintf(file, imageFolder.c_str());
    std::fprintf(file, "\n\n");
    std::fprintf(file,"Translation Vector, T:\n\n");
    for(int i=0;i<T.rows;i++)
    {
        for(int j=0;j<T.cols;j++)
        {
            std::fprintf(file,"%lf ", T.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n\n");

    std::fprintf(file,"Scale Factor:\n");
    std::fprintf(file, "%lf", scaleFactor);

    std::fclose(file);



    return 0;
}







