#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>


int main()
{
    //Define number of stereo calibration image pairs
//    int num_images = 35;  //ImageJae
    int num_images = 100;

    //Store imageSize
    cv::Size imageSize;

    //Define board Size(width x height) of inner chessboard corners
    cv::Size board_size = cv::Size(10,7);

    //Define Chessboard square width (inches)
     float square_width = 3.88;

    //Define image objects
    cv::Mat image;
    cv::Mat image_gray;
    cv::Mat image_color;

    //Create imagePoints vectors for L and R cameras
    std::vector<std::vector<cv::Point2f>> imagePointsL;
    std::vector<std::vector<cv::Point2f>> imagePointsR;
    //Create objectPoints vector (shared between L and R cameras)
    std::vector<std::vector<cv::Point3f>> objectPoints;

    //Populate ObjPts vector for images (assumes calibration chessboard doesn't change and all corners visible)
    // *ObjPts for each image are identical
    std::vector<cv::Point3f> ObjPts;

    for(int y=0;y<board_size.height;y++)
    {
        for(int x=0;x<board_size.width;x++)
        {
            ObjPts.push_back(cv::Point3f(x*square_width,y*square_width,0));
        }
    }

    //Build imagePoints and objectPoints vectors for Left Camera
    for (int i=1;i<num_images;i++)
    {
        //Generate file path to the image (string)
        int image_number = i;
        std::string number = std::to_string(image_number);
        std::string path_begin = "/home/jesse/Desktop/new_cal/NewL";
        std::string path_end = ".bmp";
        std::string file = path_begin + number + path_end;

        //Vector to store corners
        std::vector<cv::Point2f> corners;

        //Read the image
        image = cv::imread(file, 1);
        image_color = image;

        //Get the image size
        imageSize = cv::Size(image.cols, image.rows);

        //Convert_to_Grayscale
        cv::cvtColor(image, image_gray, 6); //convert to grayscale (COLOR_BGR2GRAY == 6)

        //Find Chessboard corners
        bool corners_found = cv::findChessboardCorners(image_gray,board_size,corners,1 + 2); //1 + 2

        if(corners_found)
        {
            //Refine corner locations
            cv::cornerSubPix(image_gray, corners, cv::Size(5, 5), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 60, 0.001));

            //Draw on the colored image so it's pretty
            cv::drawChessboardCorners(image_color, board_size, corners, corners_found);

            if(corners.size() == board_size.width*board_size.height)
            {
                //Initialize vector to hold ImagePoints for each calibration image
                std::vector<cv::Point2f> ImgPts;

                //Populate the ImgPts and ObjPts Vectors
                for(int i=0;i<corners.size();i++)
                {
                    cv::Point2f ImgPt;
                    ImgPt.x = corners[i].x;
                    ImgPt.y = corners[i].y;

                    //Push each ImgPt onto the ImgPts vector
                    ImgPts.push_back(ImgPt);
                }
                //Push the ImagePts vector onto the imagePoints vector
                imagePointsL.push_back(ImgPts);
            }
            else
            {
                std::cout << "Board size and corner size mismatch (all corners may not have been detected).";
            }
        //Push the ObjPts vector onto the objectPoints vector
        objectPoints.push_back(ObjPts);
        }
        else
        {
            std::cout << "Corners not found on left image " << number << std::endl;
        }

        //show the image
        cv::imshow("Chess Board", image_color);
        cv::waitKey(20);

    }

    //Build imagePoints and objectPoints vectors for Right Camera
    for (int i=1;i<num_images;i++)
    {
        //Generate file path to the image (string)
        int image_number = i;
        std::string number = std::to_string(image_number);
        std::string path_begin = "/home/jesse/Desktop/new_cal/NewR";
        std::string path_end = ".bmp";
        std::string file = path_begin + number + path_end;

        //Vector to store corners
        std::vector<cv::Point2f> corners;

        //Read the image
        image = cv::imread(file, 1);
        image_color = image;

        //Get the image size
        imageSize = cv::Size(image.cols, image.rows);

        //Convert_to_Grayscale
        cv::cvtColor(image, image_gray, 6); //convert to grayscale (COLOR_BGR2GRAY == 6)

        //Find Chessboard corners
        bool corners_found = cv::findChessboardCorners(image_gray,board_size,corners,1 + 2);

        if(corners_found)
        {
            //Refine corner locations
            cv::cornerSubPix(image_gray, corners, cv::Size(5, 5), cv::Size(-1, -1),cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 60, 0.001));

            //Draw on the colored image so it's pretty
            cv::drawChessboardCorners(image_color, board_size, corners, corners_found);

            if(corners.size() == board_size.width*board_size.height)
            {
                //Initialize vector to hold ImagePoints for each calibration image
                std::vector<cv::Point2f> ImgPts;

                //Populate the ImgPts and ObjPts Vectors
                for(int i=0;i<corners.size();i++)
                {
                    cv::Point2f ImgPt;
                    ImgPt.x = corners[i].x;
                    ImgPt.y = corners[i].y;

                    //Push each ImgPt onto the ImgPts vector
                    ImgPts.push_back(ImgPt);
                }
                //Push the ImagePts vector onto the imagePoints vector
                imagePointsR.push_back(ImgPts);
            }
            else
            {
                std::cout << "Board size and corner size mismatch (all corners may not have been detected).";
            }
        //Push the ObjPts vector onto the objectPoints vector
        //objectPoints.push_back(ObjPts);
        }
        else
        {
            std::cout << "Corners not found on right image " << number << std::endl;
        }

        //show the image
        cv::imshow("Chess Board", image_color);
        cv::waitKey(20);

    }


    //ObjectPoints and ImagePoints gathered for L and R cameras, time to calibrate

    //initialize rotation matrix and translation vector
    cv::Mat R;
    cv::Mat T;

    //initialize essential and fundamental matrices
    cv::Mat E;
    cv::Mat F;

    //initialize cameraMatrix1, cameraMatrix2, distCoeffs1, and distCoeffs2
    cv::Mat cameraMatrixL(3,3, CV_64F);
    cv::Mat cameraMatrixR(3,3, CV_64F);
    cv::Mat distCoeffsL(5,1,CV_64F);
    cv::Mat distCoeffsR(5,1,CV_64F);

    //Read the individual camera calibration parameters from file
    cv::FileStorage fs1("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_L_parameters.xml", cv::FileStorage::READ);
    fs1["Camera_Matrix"] >> cameraMatrixL;
    fs1["Distortion_Coefficients"] >> distCoeffsL;

    cv::FileStorage fs2("/home/jesse/Desktop/ECEN_631/Assignment_3/build-task_1-Desktop_Qt_5_7_0_GCC_64bit-Default/camera_R_parameters.xml", cv::FileStorage::READ);
    fs2["Camera_Matrix"] >> cameraMatrixR;
    fs2["Distortion_Coefficients"] >> distCoeffsR;


    //calibrate using calibrateCamera function
    cv::stereoCalibrate(objectPoints,imagePointsL,imagePointsR,cameraMatrixL,distCoeffsL,cameraMatrixR,distCoeffsR,imageSize,R,T,E,F);

    //get the stereo rectification parameters
    cv::Mat R1(3,3, CV_64F);
    cv::Mat R2(3,3, CV_64F);
    cv::Mat P1(3,4, CV_64F);
    cv::Mat P2(3,4, CV_64F);
    cv::Mat Q(4,4, CV_64F);

    cv::stereoRectify(cameraMatrixL, distCoeffsL, cameraMatrixR, distCoeffsR, cv::Size(640,480),R,T,R1,R2,P1,P2,Q,0,-1,cv::Size(640,480),0,0);


    //Write stereo Calibration Parameters to .txt file
    std::string filename = "stereo_calibration_parameters.txt";
    std::FILE* file;
    file = std::fopen(filename.c_str(),"w");
    std::fprintf(file,"Assignment3 Task_2 Stereo Camera Calibration File\n\n");
    std::fprintf(file,"Stereo Intrinsic Parameters:\n\n");
    std::fprintf(file,"Rotation:\n");
    for(int i=0;i<R.rows;i++)
    {
        for(int j=0;j<R.cols;j++)
        {
            std::fprintf(file,"%lf ", R.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n");
    std::fprintf(file,"Translation:\n");
    for(int i=0;i<T.rows;i++)
    {
        for(int j=0;j<T.cols;j++)
        {
            std::fprintf(file,"%lf ", T.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n\n");
    std::fprintf(file,"Essential Matrix:\n");
    for(int i=0;i<E.rows;i++)
    {
        for(int j=0;j<E.cols;j++)
        {
            std::fprintf(file,"%lf ", E.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n\n");
    std::fprintf(file,"Fundamental Matrix:\n");
    for(int i=0;i<F.rows;i++)
    {
        for(int j=0;j<F.cols;j++)
        {
            std::fprintf(file,"%lf ", F.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fclose(file);

    //Write Calibration Intrinsic and Distortion parameters to .xml file (this is the preferred method)
    cv::FileStorage fs("stereo_calibration_parameters.xml", cv::FileStorage::WRITE);
    fs << "Stereo_Rotation_Matrix" << R;
    fs << "Stereo_Translation_Vector" << T;
    fs << "Stereo_Essential_Matirx" << E;
    fs << "Stereo_Fundamental_Matrix" << F;
    fs << "R1" << R1;
    fs << "R2" << R2;
    fs << "P1" << P1;
    fs << "P2" << P2;
    fs << "Q" << Q;

    return 0;
}
