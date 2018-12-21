//#############################################################################
//  File:      SLRaytracer.h
//  Author:    Marcus Hudritsch
//  Date:      July 2014
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLRAYTRACER_H
#define SLRAYTRACER_H

#include <SLEventHandler.h>
#include <SLGLTexture.h>

class SLScene;
class SLSceneView;
class SLRay;
class SLMaterial;
class SLCamera;

//-----------------------------------------------------------------------------
//! Ray tracing state
typedef enum
{
    rtReady,    // RT is ready to start
    rtBusy,     // RT is running
    rtFinished, // RT is finished
    rtMoveGL    // RT is finished and GL camera is moving
} SLRTState;
//-----------------------------------------------------------------------------
//! Pixel index struct used in anti aliasing in ray tracing
struct SLRTAAPixel
{
    SLRTAAPixel(SLushort X = 0, SLushort Y = 0)
    {
        x = X;
        y = Y;
    }
    SLushort x; //!< Unsigned short x-pixel index
    SLushort y; //!< Unsigned short x-pixel index
};
typedef vector<SLRTAAPixel> SLVPixel;
//-----------------------------------------------------------------------------
//! SLRaytracer hold all the methods for Whitted style Ray Tracing.
/*!      
SLRaytracer implements the methods render, eyeToPixel, trace and shade for
classic Whitted style Ray Tracing. This class is a friend class of SLScene and
can access via the pointer _s all members of SLScene. The scene traversal for
the ray intersection tests is done within the intersection method of all nodes. 
*/
class SLRaytracer : public SLGLTexture
  , public SLEventHandler
{
    public:
    SLRaytracer();
    virtual ~SLRaytracer();

    // ray tracer functions
    SLbool  renderClassic(SLSceneView* sv);
    SLbool  renderDistrib(SLSceneView* sv);
    void    renderSlices(const bool isMainThread);
    void    renderSlicesMS(const bool isMainThread);
    SLCol4f trace(SLRay* ray);
    SLCol4f shade(SLRay* ray);
    void    sampleAAPixels(const bool isMainThread);
    void    finishBeforeUpdate();

    // additional ray tracer functions
    void    setPrimaryRay(SLfloat x, SLfloat y, SLRay* primaryRay);
    void    getAAPixels();
    SLCol4f fogBlend(SLfloat z, SLCol4f color);
    void    printStats(SLfloat sec);
    void    initStats(SLint depth);

    // Setters
    void state(SLRTState state)
    {
        if (_state != rtBusy) _state = state;
    }
    void maxDepth(SLint depth)
    {
        _maxDepth = depth;
        state(rtReady);
    }
    void doDistributed(SLbool distrib) { _doDistributed = distrib; }
    void doContinuous(SLbool cont)
    {
        _doContinuous = cont;
        state(rtReady);
    }
    void doFresnel(SLbool fresnel)
    {
        _doFresnel = fresnel;
        state(rtReady);
    }
    void aaSamples(SLint samples)
    {
        _aaSamples = samples;
        state(rtReady);
    }
    void gamma(SLfloat g)
    {
        _gamma        = g;
        _oneOverGamma = 1.0f / g;
    }

    // Getters
    SLRTState state() const { return _state; }
    SLint     maxDepth() const { return _maxDepth; }
    SLbool    doDistributed() const { return _doDistributed; }
    SLbool    doContinuous() const { return _doContinuous; }
    SLbool    doFresnel() const { return _doFresnel; }
    SLint     aaSamples() const { return _aaSamples; }
    SLuint    numThreads() const { return SL::maxThreads(); }
    SLint     pcRendered() const { return _pcRendered; }
    SLfloat   aaThreshold() const { return _aaThreshold; }
    SLfloat   renderSec() const { return _renderSec; }
    SLfloat   gamma() { return _gamma; }
    SLfloat   oneOverGamma() { return _oneOverGamma; }

    // Render target image
    void prepareImage();
    void renderImage();
    void saveImage();

    protected:
    SLSceneView* _sv;            //!< Parent sceneview
    SLRTState    _state;         //!< RT state;
    SLCamera*    _cam;           //!< shortcut to the camera
    SLint        _maxDepth;      //!< Max. allowed recursion depth
    SLbool       _doContinuous;  //!< if true state goes into ready again
    SLbool       _doDistributed; //!< Flag for parallel distributed RT
    SLbool       _doFresnel;     //!< Flag for Fresnel reflection
    SLint        _pcRendered;    //!< % rendered
    SLfloat      _renderSec;     //!< Rendering time in seconds

    SLfloat     _pxSize;       //!< Pixel size
    SLVec3f     _EYE;          //!< Camera position
    SLVec3f     _LA, _LU, _LR; //!< Camera lookat, lookup, lookright
    SLVec3f     _BL;           //!< Bottom left vector
    atomic<int> _next;         //!< next index to render RT
    SLVPixel    _aaPixels;     //!< Vector for antialiasing pixels
    SLfloat     _gamma;        //!< gamma correction value
    SLfloat     _oneOverGamma; //!< one over gamma correction value

    // variables for distributed ray tracing
    SLfloat _aaThreshold; //!< threshold for anti aliasing
    SLint   _aaSamples;   //!< SQRT of uneven num. of AA samples
};
//-----------------------------------------------------------------------------
#endif
