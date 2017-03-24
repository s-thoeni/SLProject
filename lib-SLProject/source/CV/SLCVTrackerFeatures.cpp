//#############################################################################
//  File:      SLCVTrackerAruco.cpp
//  Author:    Michael Goettlicher, Marcus Hudritsch
//  Date:      Winter 2016
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch, Michael Goettlicher
//             This softwareis provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h>         // precompiled headers

/*
The OpenCV library version 3.1 with extra module must be present.
If the application captures the live video stream with OpenCV you have
to define in addition the constant SL_USES_CVCAPTURE.
All classes that use OpenCV begin with SLCV.
See also the class docs for SLCVCapture, SLCVCalibration and SLCVTracker
for a good top down information.
*/
#include <SLSceneView.h>
#include <SLCVTrackerFeatures.h>
#include <SLCVCapture.h>
#include <SLCVRaulMurOrb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace cv;

#define DEBUG 1
#define SL_VIDEO_DEBUG 0
#define HOFF_EXAMPLE 0
#define SAVE_SNAPSHOTS_OUTPUT "/tmp/cv_tracking/"

#define FLANN_BASED 0

// Matching configuration
const float minRatio = 1.0f;

// RANSAC configuration
const int iterations = 500;
const float reprojectionError = 2.0;
const double confidence = 0.85;

#if(SL_VIDEO_DEBUG || defined SAVE_SNAPSHOTS_OUTPUT)
int frame_count;
#endif
#if SL_VIDEO_DEBUG
float low_detection_milis = 1000.0f;
float avg_detection_milis;
float high_detection_milis;

float low_compute_milis = 1000.0f;
float avg_compute_milis;
float high_compute_milis;
#endif
//-----------------------------------------------------------------------------
SLCVTrackerFeatures::SLCVTrackerFeatures(SLNode *node) :
        SLCVTracker(node) {

    #if FLANN_BASED
    _matcher = new FlannBasedMatcher();
    #else
    _matcher =  BFMatcher::create(BFMatcher::BRUTEFORCE_HAMMING, false);
    #endif

    #ifdef SAVE_SNAPSHOTS_OUTPUT
    #if defined(unix)
    mkdir(SAVE_SNAPSHOTS_OUTPUT, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    #else
    mkdir(SAVE_SNAPSHOTS_OUTPUT);
    #endif
    #endif
}


//------------------------------------------------------------------------------
void SLCVTrackerFeatures::loadModelPoints() {
    // Read marker
    Mat planartracking = imread("../_data/images/textures/planartracking.jpg");
    cvtColor(planartracking, _map.frameGray, CV_RGB2GRAY);

    // Detect and compute features in marker image
    SLScene::current->_detector->detect(_map.frameGray, _map.keypoints);
    SLScene::current->_descriptor->compute(_map.frameGray, _map.keypoints, _map.descriptors);

    // Calculates proprtion of MM and Pixel (sample measuring)
    const SLfloat lengthMM = 297.0;
    const SLfloat lengthPX = 2 * _calib->cx();
    float pixelPerMM = lengthPX / lengthMM;

    // Calculate 3D-Points based on the detected features
    for (unsigned int i = 0; i< _map.keypoints.size(); i++) {
        Point2f refImageKeypoint = _map.keypoints[i].pt; // 2D location in image
        refImageKeypoint /= pixelPerMM;                  // Point scaling
        float Z = 0;                                     // Here we can use 0 because we expect a planar object
        _map.model.push_back(Point3f(refImageKeypoint.x, refImageKeypoint.y, Z));
    }
}

//------------------------------------------------------------------------------
SLbool SLCVTrackerFeatures::track(SLCVMat imageGray,
                                  SLCVMat image,
                                  SLCVCalibration *calib,
                                  SLSceneView *sv) {
    #if SL_VIDEO_DEBUG
    if (frame_count == 700){
           ofstream myfile;
           myfile.open ("/tmp/tracker_stats.txt");
           myfile << "Min Detection Time (Ms) " << low_detection_milis << "\n";
           myfile << "Avg Detection Time (Ms) " << avg_detection_milis << "\n";
           myfile << "High Detection Time (Ms) " << high_detection_milis << "\n";

           myfile << "Min Compute Time (Ms) " << low_compute_milis << "\n";
           myfile << "Avg Compute Time (Ms) " << avg_compute_milis << "\n";
           myfile << "High Compute Time (Ms) " << high_compute_milis << "\n";
           myfile.close();
    }
    #endif
    assert(!image.empty() && "Image is empty");
    assert(!calib->cameraMat().empty() && "Calibration is empty");
    assert(_node && "Node pointer is null");
    assert(sv && "No sceneview pointer passed");
    assert(sv->camera() && "No active camera in sceneview");

    if (_map.model.empty()) {
        _calib = calib;
        loadModelPoints();
    }

    // Detect keypoints ####################################################
    SLCVVKeyPoint keypoints = getFeatures(imageGray);
    // #####################################################################


    // Extract descriptors from keypoints ##################################
    Mat descriptors = getDescriptors(imageGray , keypoints);
    // #####################################################################


    // Feature matching ####################################################
    vector<DMatch> matches = matchFeatures(descriptors);
    // #####################################################################


    // POSE calculation ####################################################
    Mat rvec = cv::Mat::zeros(3, 3, CV_64FC1);      // rotation matrix
    Mat tvec = cv::Mat::zeros(3, 1, CV_64FC1);      // translation matrix
    vector<DMatch> inliers;
    bool foundPose = calculatePose(keypoints, matches, inliers, rvec, tvec);

    #ifdef SAVE_SNAPSHOTS_OUTPUT
    if (foundPose) {
        Mat imgMatches;
        drawMatches(image, keypoints, _map.frameGray, _map.keypoints, inliers, imgMatches);
        imwrite(SAVE_SNAPSHOTS_OUTPUT + to_string(frame_count) + ".png", imgMatches);

        Mat out; Rodrigues(rvec, out);
        cout << "Found pose. Rotation=" << out << "\nTranslation=" << tvec << endl;
    }
    #endif
    // #####################################################################


    // Feature tracking ####################################################
    //trackPose()
    // #####################################################################


    // Update camera object matrix  ########################################
    if (foundPose) {
        // Converts calulated extrinsic camera components (translation & rotation) to OpenGL camera matrix
        _pose = createGLMatrix(tvec, rvec);

        // Update Scene Graph camera to display model correctly (positioning cam relative to world coordinates)
        sv->camera()->om(_pose.inverse());
    }
    // #####################################################################

    #if(SL_VIDEO_DEBUG || defined SAVE_SNAPSHOTS_OUTPUT)
    frame_count++;
    #endif

    _prev.image = image;
    _prev.imageGray = imageGray;
    return false;
}

//-----------------------------------------------------------------------------
inline SLCVVKeyPoint SLCVTrackerFeatures::getFeatures(const Mat &imageGray) {
    SLCVVKeyPoint keypoints;
    SLfloat detectTimeMillis = SLScene::current->timeMilliSec();
    SLScene *scene = SLScene::current;
    scene->_detector->detect(imageGray, keypoints);

    #if SL_VIDEO_DEBUG
    SLfloat time = SLScene::current->timeMilliSec() - detectTimeMillis;
    if (time != 0){
        if (time < low_detection_milis){
            low_detection_milis = time;
        }
        else if (time > high_detection_milis){
            high_detection_milis = time;
        }
        if (frame_count > 0)
        avg_detection_milis = (frame_count*avg_detection_milis + time)/(1+frame_count);
    }
    #endif
    SLScene::current->setDetectionTimesMS(SLScene::current->timeMilliSec() - detectTimeMillis);
    return keypoints;
}

//-----------------------------------------------------------------------------
inline Mat SLCVTrackerFeatures::getDescriptors(const Mat &imageGray, SLCVVKeyPoint &keypoints) {
    Mat descriptors;
    SLfloat computeTimeMillis = SLScene::current->timeMilliSec();
    SLScene *scene = SLScene::current;
    scene->_descriptor->compute(imageGray, keypoints, descriptors);
    #if SL_VIDEO_DEBUG
    SLfloat time = SLScene::current->timeMilliSec() - computeTimeMillis;
    if (time != 0.0f){
        if (time < low_compute_milis){
            low_compute_milis = time;
        }
        else if (time > high_compute_milis){
            high_compute_milis = time;
        }
        if (frame_count > 0){
            avg_compute_milis = (avg_compute_milis*frame_count + time)/(1+frame_count);
        }
        else {
            avg_compute_milis = time;
        }
    }
    #endif
    SLScene::current->setFeatureTimesMS(SLScene::current->timeMilliSec() - computeTimeMillis);
    return descriptors;
}

//-----------------------------------------------------------------------------
inline vector<DMatch> SLCVTrackerFeatures::matchFeatures(const Mat &descriptors) {
    SLfloat matchTimeMillis = SLScene::current->timeMilliSec();

    // 1. Get matches with FLANN or KNN algorithm ######################################################################################
    #if FLANN_BASED
    if(descriptors.type() !=CV_32F ) descriptors.convertTo(descriptors, CV_32F);
    if(_lastFrameDescriptors.type() != CV_32F) _lastFrameDescriptors.convertTo(_lastFrameDescriptors, CV_32F);

    vector< DMatch > matches;
    _matcher->match(descriptors, _lastFrameDescriptors, matches);
    #else
    int k = 2;
    vector<vector<DMatch>> matches;
    _matcher->knnMatch(descriptors, _map.descriptors, matches, k);

    /* Perform ratio test which determines if k matches from the knn matcher are not too similar.
     *  If the ratio of the the distance of the two matches is toward 1, the matches are near identically.
     */
    vector<DMatch> goodMatches;
    for(size_t i = 0; i < matches.size(); i++) {
        const DMatch &match1 = matches[i][0];
        const DMatch &match2 = matches[i][1];
        if (match2.distance == 0.0f || ( match1.distance / match2.distance) < minRatio)
            goodMatches.push_back(match1);
    }
    #endif

    SLScene::current->setMatchTimesMS(SLScene::current->timeMilliSec() - matchTimeMillis);
    return goodMatches;
}

//-----------------------------------------------------------------------------
inline bool SLCVTrackerFeatures::calculatePose(const SLCVVKeyPoint &keypoints, const vector<DMatch> &matches, vector<DMatch> &inliers, Mat &rvec, Mat &tvec) {

    // RANSAC crashes if 0 points are given
    if (matches.size() == 0) return 0;
    bool foundPose = 0;

    /* Find 2D/3D correspondences
     *
     *  At the moment we are using only the two correspondences like this:
     *  KeypointsOriginal <-> KeypointsActualscene
     *
     *  Train index --> "Point" in the model
     *  Query index --> "Point" in the actual frame
     */
    vector<Point3f> modelPoints(matches.size());
    vector<Point2f> framePoints(matches.size());
    for (size_t i = 0; i < matches.size(); i++) {
        modelPoints[i] = _map.model[matches[i].trainIdx];
        framePoints[i] =  keypoints[matches[i].queryIdx].pt;
    }

    /* We execute first RANSAC to eliminate wrong feature correspondences (outliers) and only use
     * the correct ones (inliers) for PnP solving.
     *
     * RANSAC --------------------------
     * The RANdom Sample Consensus algorithm is called to remove "wrong" point correspondences
     *  which makes the solvePnP more robust. The so called inliers are used for calculation,
     *  wrong correspondences (outliers) will be ignored. Therefore the method below will first
     *  run a solvePnP with the EPNP method and returns the reprojection error.
     *
     * PnP ----------------------------- (https://en.wikipedia.org/wiki/Perspective-n-Point)
     * General problem: We have a calibrated cam and sets of corresponding 2D/3D points.
     *  We will calculate the rotation and translation in respect to world coordinates.
     *
     * Methods
     *
     * P3P: If we have 3 Points given, we have the minimal form of the PnP problem. We can
     *  treat the points as a triangle definition ABC. We have 3 corner points and 3 angles.
     *  Because we get many soulutions for the equation, there will be a fourth point which
     *  removes the ambiguity. Therefore the OpenCV implementation requires 4 points to use
     *  this method.
     *
     * EPNP: This method is used if there are n >= 4 points. The reference points are expressed
     *  as 4 virtual control points. The coordinates of these points are the unknowns for the
     *  equtation.
     *
     * ITERATIVE: Calculates pose using the DLT (Direct Linear Transform) method. If there is
     *  a homography will be much easier and no DLT will be used. Otherwise we are using the DLT
     *  and make a Levenberg-Marquardt optimization. The latter helps to decrease the reprojection
     *  error which is the sum of the squared distances between the image and object points.
     *
     */

    //TODO: Split up
    vector<unsigned char> inliersMask(modelPoints.size());
    foundPose = cv::solvePnPRansac(modelPoints,
                       framePoints,
                       _calib->cameraMat(),
                       _calib->distortion(),
                       rvec, tvec,
                       false,        // Use extrinsic guess: We don't know the pose at the moment.
                                     // But it's possible to give the known pose to the method as
                                     // rvec and tvec. This will be used for tracking.
                       iterations,
                       reprojectionError,
                       confidence,
                       inliersMask,
                       cv::SOLVEPNP_ITERATIVE);

    /* TODO: Levenberg-Marquardt optimization
     *
     */



    #if DEBUG
    /*
     * Convert inliers back to points. The inliersIndex matrix contais the frame location point
     */
    for (int i = 0; i < inliersMask.size(); i++) {
        inliers.push_back(matches[inliersMask[i]]);
    }
    #endif

    return foundPose;
}
