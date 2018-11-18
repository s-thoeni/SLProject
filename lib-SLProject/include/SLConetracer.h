//#############################################################################
//  File:      SLRaytracer.h
//  Author:    Stefan Thoeni
//  Date:      Sept 2018
//  Copyright: Stefan Thoeni
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef SLCONETRACER_H
#define SLCONETRACER_H

#include <SLGLTexture.h>
#include <SLMaterial.h>
#include <SLGLTexture3D.h>
#include <SLGLFbo.h>
#include <SLRectangle.h>
#include <SLBox.h>

class SLScene;
class SLSceneView;
class SLCamera;


class SLConetracer {
public:
    SLConetracer();
    ~SLConetracer();
    void init(SLint scrW, SLint scrH);
    SLbool render(SLSceneView* sv);
    void renderSceneGraph(SLuint progId); // <-- this could ev. be private
    void voxelize();
    void setCameraOrthographic();
    void resetCamera();
    void renderNode(SLNode* node, const SLuint progid); // <-- renders a node with all its children
    void uploadLights(SLuint programId);
    void visualizeVoxelization();
    void renderConetraced();
protected:
    SLSceneView*   _sv;
    SLCamera*      _cam;
    SLMaterial*    _voxelizationMaterial;
    SLMaterial*    _worldMaterial;
    SLMaterial*    _voxelVisualizationMaterial;
    SLMaterial*    _voxelConetracingMaterial;
    SLGLTexture3D* _voxelTexture;
    SLuint         _voxelTextureSize = 64; // power of 2
    SLGLFbo*       _visualizationFbo1;
    SLGLFbo*       _visualizationFbo2;
    SLRectangle*   _quadMesh;
    SLBox*         _cubeMesh;
private:
    SLProjection _oldProjection;
    SLEyeType    _oldET;
    SLbool       _first = true;
};
#endif SLCONETRACER_H
