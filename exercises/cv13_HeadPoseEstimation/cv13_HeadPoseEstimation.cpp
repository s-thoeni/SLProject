//#############################################################################
//  File:      cv13_HeadPoseEstimation.cpp
//  Purpose:   Minimal OpenCV app for head pose estimation
//  Taken from Satya Mallic on:
//  http://www.learnopencv.com/head-pose-estimation-using-opencv-and-dlib/
//  Date:      Authumn 2017
//#############################################################################

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//-----------------------------------------------------------------------------
int
main()
{
    // Read input image
    // Note for Visual Studio: You must set the Working Directory to $(TargetDir)
    // with: Right Click on Project > Properties > Debugging
    Mat image = imread("../_data/images/textures/headPose.jpg");
    if (image.empty())
    {
        cout << "Could not load image. Is the working dir correct?" << endl;
        return -1;
    }

    // 2D image points. If you change the image, you need to change vector
    std::vector<Point2d> image_points;
    image_points.push_back(Point2d(359, 391)); // Nose tip
    image_points.push_back(Point2d(399, 561)); // Chin
    image_points.push_back(Point2d(337, 297)); // Left eye left corner
    image_points.push_back(Point2d(513, 301)); // Right eye right corner
    image_points.push_back(Point2d(345, 465)); // Left Mouth corner
    image_points.push_back(Point2d(453, 469)); // Right mouth corner

    // 3D model points with a unknown scale factor in it
    vector<Point3d> model_points;
    model_points.push_back(Point3d(0.0f, 0.0f, 0.0f));          // Nose tip
    model_points.push_back(Point3d(0.0f, -330.0f, -65.0f));     // Chin
    model_points.push_back(Point3d(-225.0f, 170.0f, -135.0f));  // Left eye left corner
    model_points.push_back(Point3d(225.0f, 170.0f, -135.0f));   // Right eye right corner
    model_points.push_back(Point3d(-150.0f, -150.0f, -125.0f)); // Left Mouth corner
    model_points.push_back(Point3d(150.0f, -150.0f, -125.0f));  // Right mouth corner

    // Camera intrinsic matrix
    double  f             = image.cols;                              // Approximate focal length of fov of 60 deg.
    Point2d c             = Point2d(image.cols / 2, image.rows / 2); // Approximate optical center = image center
    Mat     camera_matrix = (Mat_<double>(3, 3) << f, 0, c.x, 0, f, c.y, 0, 0, 1);

    // Create empty distortion coefficient with k1, k2, p1 & k3 all zero
    Mat dist_coeffs = Mat::zeros(4, 1, DataType<double>::type);

    cout << "Camera Matrix " << endl
         << camera_matrix << endl;
    cout << "Distortion coefficients:" << dist_coeffs << endl;

    // Output rotation and translation
    Mat rotation_vector; // Rotation in axis-angle form
    Mat translation_vector;

    // Solve for pose
    solvePnP(model_points,
             image_points,
             camera_matrix,
             dist_coeffs,
             rotation_vector,
             translation_vector,
             false, // No extrinsic guess
             SOLVEPNP_ITERATIVE);

    // Project a 3D point (0, 0, 1000.0) onto the image plane.
    // We use this to draw a line sticking out of the nose
    vector<Point3d> nose_end_point3D;
    vector<Point2d> nose_end_point2D;
    nose_end_point3D.push_back(Point3d(0, 0, 1000.0));

    projectPoints(nose_end_point3D,
                  rotation_vector,
                  translation_vector,
                  camera_matrix,
                  dist_coeffs,
                  nose_end_point2D);

    // Draw red dots on all image points
    for (int i = 0; i < image_points.size(); i++)
        circle(image, image_points[i], 3, Scalar(0, 0, 255), -1);

    // Draw blue nose line
    line(image,
         image_points[0],
         nose_end_point2D[0],
         Scalar(255, 0, 0),
         2);

    cout << "Rotation Vector " << endl
         << rotation_vector << endl;
    cout << "Translation Vector" << endl
         << translation_vector << endl;
    cout << nose_end_point2D << endl;

    // Display image.
    imshow("Output", image);

    // Wait until user presses some key
    waitKey(0);
    return 0;
}
//-----------------------------------------------------------------------------
