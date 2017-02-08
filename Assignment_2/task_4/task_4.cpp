#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>

void tokenize(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiters);

int main()
{   
    //Get the path to the object data file
    std::string data_file = "/home/jesse/Desktop/ECEN_631/Assignment_2/task_4/data_points.txt";
    std::string line;

    //define a vector of vectors to hold all of the data (tokens) in the file
    std::vector<std::vector<std::string>> tokens;
    std::string delimiters(", ;\t");

    //Build the vector of token vectors
    std::ifstream infile1(data_file.c_str());
    while (std::getline(infile1, line, '\n'))
    {
        if(line == "") continue;            //skip blank lines
        std::vector<std::string> temp;      //temp string vector to pass tokenize
        tokenize(line, temp, delimiters);
        tokens.push_back(temp);             //pushback the new line of tokens back onto the tokens vector
    }


    //___Tokenized the file, now let's assign the data___


    //Initialize vectors to hold ImagePoints and ObjectPoints
    std::vector<cv::Point2f> ImagePoints;
    std::vector<cv::Point3f> ObjectPoints;

    //Populate the ImagePoints Vector
    for(int i=0;i<20;i++)
    {
        cv::Point2f ImgPt;
        ImgPt.x = std::stof(tokens[i][0]);
        ImgPt.y = std::stof(tokens[i][1]);

        //Push each ImgPt onto the ImgPts vector
        ImagePoints.push_back(ImgPt);
    }

    //Populate the ObjectPoints vector
    for(int i=20;i<40;i++)
    {
        cv::Point3f ObjPt;
        ObjPt.x = std::stof(tokens[i][0]);
        ObjPt.y = std::stof(tokens[i][1]);
        ObjPt.z = std::stof(tokens[i][2]);

        //Push each ImgPt onto the ImgPts vector
        ObjectPoints.push_back(ObjPt);
    }

    //___Bring in the Camera Calibration data___

    //Get the path to the camera calibration file
    std::string file_path = "/home/jesse/Desktop/ECEN_631/Assignment_2/build-task_2-Desktop_Qt_5_7_0_GCC_64bit-Debug/task_2_camera_parameters.txt";
    std::string lines;

    //define a vector of vectors to hold all of the data (tokens2) in the file
    std::vector<std::vector<std::string>> tokens2;

    //Build the vector of token2 vectors
    std::ifstream infile2(file_path.c_str());
    while (std::getline(infile2, lines, '\n'))
    {
        if(lines == "") continue;            //skip blank lines
        std::vector<std::string> temp;      //temp string vector to pass tokenize
        tokenize(lines, temp, delimiters);
        tokens2.push_back(temp);             //pushback the new line of tokens back onto the tokens vector
    }

    //Read the Camera Calibration Data

    //Read Camera Intrinsic Parameters
    double fx = std::stod(tokens2[2][0]);
    double fy = std::stod(tokens2[3][1]);
    double cx = std::stod(tokens2[2][2]);
    double cy = std::stod(tokens2[3][2]);

    //Read Camera Distortion Parameters
    double k1 = std::stod(tokens2[6][0]);
    double k2 = std::stod(tokens2[7][0]);
    double p1 = std::stod(tokens2[8][0]);
    double p2 = std::stod(tokens2[9][0]);
    double k3 = std::stod(tokens2[10][0]);

    //Set up the Camera Intrinsic Matrix
    cv::Mat Camera_Matrix(3, 3, CV_64F);
    Camera_Matrix = cv::Mat::eye(3, 3, CV_64F);
    Camera_Matrix.at<double>(0,0) = fx;
    Camera_Matrix.at<double>(0,2) = cx;
    Camera_Matrix.at<double>(1,1) = fy;
    Camera_Matrix.at<double>(1,2) = cy;

    //Set up the Camera Distortion Vector
    cv::Mat Distortion_Vector(5, 1, CV_64F);
    Distortion_Vector = cv::Mat::zeros(5, 1, CV_64F);
    Distortion_Vector.at<double>(0,0) = k1;
    Distortion_Vector.at<double>(1,0) = k2;
    Distortion_Vector.at<double>(2,0) = p1;
    Distortion_Vector.at<double>(3,0) = p2;
    Distortion_Vector.at<double>(4,0) = k3;

    //___Use solvePNP() to estimate object pose___

    //initialize rvec and tvec
    cv::Mat rotation_vector;
    cv::Mat translation_vector;

    cv::solvePnP(ObjectPoints,ImagePoints,Camera_Matrix,Distortion_Vector,rotation_vector,translation_vector,false,CV_ITERATIVE);

    //__Create the 3x3 Rotation Matrix

    //create a Mat to store the Rotation Matrix
    cv::Mat rotation_matrix(3, 3, CV_64F);

    //Use cv::Rodrigues() to convert rotation_vector to rotation_matrix
    cv::Rodrigues(rotation_vector,rotation_matrix,cv::noArray());


    //__get the data out of the rotation_matrix and translation_vector and write to .txt file

    std::string filename = "task_4_pose_estimation.txt";
    std::FILE* file;
    file = std::fopen(filename.c_str(),"w");
    std::fprintf(file,"Assignment2 Task_4 Pose Estimation File\n\n");
    std::fprintf(file,"Rotation Matrix (3x3):\n\n");
    for(int i=0;i<rotation_matrix.rows;i++)
    {
        for(int j=0;j<rotation_matrix.cols;j++)
        {
            std::fprintf(file,"%lf ", rotation_matrix.at<double>(i,j));
        }
        std::fprintf(file,"\n");
    }
    std::fprintf(file,"\n");
    std::fprintf(file,"Translation Vector (3x1):\n\n");
    for(int i=0;i<translation_vector.rows;i++)
    {
        std::fprintf(file,"%lf ", translation_vector.at<double>(i));
        std::fprintf(file,"\n");
    }
    std::fclose(file);







    return 0;
}

//Definition tokenize function
void tokenize(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }

}
