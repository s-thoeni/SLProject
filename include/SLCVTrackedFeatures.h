//#############################################################################
//  File:      SLCVTrackedFeatures.h
//  Author:    Pascal Zingg, Timon Tschanz
//  Date:      Spring 2017
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch, Michael Goettlicher
//             This softwareis provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLCVTRACKERFEATURES_H
#define SLCVTRACKERFEATURES_H

/*
The OpenCV library version 3.1 with extra module must be present.
If the application captures the live video stream with OpenCV you have
to define in addition the constant SL_USES_CVCAPTURE.
All classes that use OpenCV begin with SLCV.
See also the class docs for SLCVCapture, SLCVCalibration and SLCVTracked
for a good top down information.
*/
#include <SLCV.h>
#include <SLCVTracked.h>
#include <SLNode.h>
#include <SLCVRaulMurOrb.h>

using namespace cv;

#define SL_SPLIT_DETECT_COMPUTE 0
#define SL_DO_FEATURE_BENCHMARKING 1

// Settings for drawing things into current camera frame
#define SL_DRAW_REPROJECTION_POINTS 0
#define SL_DRAW_REPROJECTION_ERROR 0
#define SL_DRAW_PATCHES 0


// Set stones Tracker as default reference image
#ifndef SL_MARKER_IMAGE_NAME
    #define SL_MARKER_IMAGE_NAME "features_stones.png"
#endif

#ifdef SL_SAVE_DEBUG_OUTPUT
    #if defined(SL_OS_LINUX) || defined(SL_OS_MACOS) || defined(SL_OS_MACIOS)
    #define SL_SAVE_SNAPSHOTS_OUTPUT "/tmp/cv_tracking/"
    #elif defined(SL_OS_WINDOWS)
    #define SL_SAVE_SNAPSHOTS_OUTPUT "cv_tracking/"
    #endif
#endif

// Feature detection and extraction
const int nFeatures = 2000;
const float minRatio = 0.7f;

// RANSAC parameters
const int iterations = 500;
const float reprojection_error = 2.0f;
const double confidence = 0.95;

// Repose patch size
const int reposeFrequency = 10;
const int initialPatchSize = 2;
const int maxPatchSize = 60;

//-----------------------------------------------------------------------------
//! SLCVTrackedFeatures is the main part of the AR Christoffelturm scene
/*! The implementation tries to find a valid pose based on feature points in
realtime. The feature matching algorithm checks the points of the current camera
frame with against a reference. There are two important parts of this procedure:
The relocalisation, which will be called if we have to find the pose with no hint
where the camera could be. The other one is called feature tracking: If a pose
was found, the implementation tries to track them and update the pose respectively.
*/
class SLCVTrackedFeatures : public SLCVTracked
{
    public:
                            SLCVTrackedFeatures (SLNode* node);
                           ~SLCVTrackedFeatures ();
        SLbool              track               (SLCVMat imageGray,
                                                 SLCVMat image,
                                                 SLCVCalibration* calib,
                                                 SLSceneView* sv);
        // Getters
        SLbool              forceRelocation     () {return _forceRelocation;}

        // Setters
        void                forceRelocation     (SLbool fR) {_forceRelocation = fR;}

    private:
        void                initializeMarker    (string markerFilename);
        void                relocate            ();
        void                tracking            ();
        void                drawDebugInformation();
        void                updateSceneCamera   (SLSceneView* sv);
        void                transferFrameData   ();
        void                detectKeypointsAndDescriptors();
        SLCVVDMatch         getFeatureMatches   ();
        bool                calculatePose       ();
        void                optimizeMatches     ();
        bool                trackWithOptFlow    (SLCVMat rvec, SLCVMat tvec);

        Ptr<DescriptorMatcher>  _matcher;       //!< Descriptor matching algorithm
        SLCVCalibration*        _calib;         //!< Current calibration in use
        SLint                   _frameCount=0;  //!< NO. of frames since process start
        bool                    _isTracking;    //!< True if tracking

        //! Data of a 2D marker image
        struct SLFeatureMarker2D
        {   SLCVMat         imageGray;          //!< Grayscale image of the marker
            SLCVMat         imageDrawing;       //!< Color debug image
            SLCVVKeyPoint   keypoints2D;        //!< 2D keypoints in pixels
            SLCVVPoint3f    keypoints3D;        //!< 3D feature points in mm
            SLCVMat         descriptors;        //!< Descriptors of the 2D keypoints
            SLCVVKeyPoint   bboxKeypoints2D;    //!< bounding boxes of 2D keypoints
        };

        //! Feature date for a video frame
        struct SLFrameData
        {   SLCVMat         image;              //!< Reference to color video frame
            SLCVMat         imageGray;          //!< Reference to grayscale video frame
            SLCVVPoint2f    inlierPoints2D;     //!< Inlier 2D points after RANSAC
            SLCVVPoint3f    inlierPoints3D;     //!< Inlier 3D points after RANSAC on the marker
            SLCVVKeyPoint   keypoints;          //!< 2D keypoints detected in video frame
            SLCVMat         descriptors;        //!< Descriptors of keypoints
            SLCVVDMatch     matches;            //!< matches between video decriptors and marker descriptors
            SLCVVDMatch     inlierMatches;      //!< matches that lead to correct transform
            SLCVMat         rvec;               //!< Rotation of the camera pose
            SLCVMat         tvec;               //!< Translation of the camera pose
            SLbool          foundPose;          //!< True if pose was found
            SLfloat         reprojectionError;  //!< Reprojection error of the pose
            SLbool          useExtrinsicGuess;  //!< flag if extrinsic gues should be used
        };

        SLFeatureMarker2D   _marker;            //!< 2D marker data
        SLFrameData         _currentFrame;      //!< The current video frame data
        SLFrameData         _prevFrame;         //!< The previous video frame data
        SLbool              _forceRelocation;   //!< Force relocation every frame (no opt. flow tracking)
};
//-----------------------------------------------------------------------------
#endif // SLCVTrackedFeatures_H