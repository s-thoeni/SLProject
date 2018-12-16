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
    void     init(SLint scrW, SLint scrH);
    SLbool   render(SLSceneView* sv);
    void     renderSceneGraph(SLuint progId); // <-- this could ev. be private
    void     voxelize();
    void     renderNode(SLNode* node, const SLuint progid); // <-- renders a node with all its children
    void     uploadLights(SLuint programId);
    void     visualizeVoxelization();
    void     renderConetraced();
    SLfloat  diffuseConeAngle() {return _diffuseConeAngle; };
    void     diffuseConeAngle(SLfloat angle) { _diffuseConeAngle = angle; };
    SLfloat  specularConeAngle() {return _specularConeAngle; };
    void     specularConeAngle(SLfloat angle) { _specularConeAngle = angle; };

    void     setCameraOrthographic();
    SLfloat  lightMeshSize() {return _lightMeshSize; };
    void     lightMeshSize(SLfloat size) { _lightMeshSize = size; };
    SLfloat  shadowConeAngle() {return _shadowConeAngle; };
    void     shadowConeAngle(SLfloat angle) { _shadowConeAngle = angle; };
    void     toggleVoxelvisualization() {_voxelVisualization = !_voxelVisualization;}
    SLbool   voxelVisualization() { return this->_voxelVisualization;}
    void     toggleDirectIllumination() {_directIllumination = !_directIllumination;}
    SLbool   directIllumination() { return this->_directIllumination;}
    void     toggleDiffuseIllumination() {_diffuseIllumination = !_diffuseIllumination;}
    SLbool   diffuseIllumination() { return this->_diffuseIllumination;}
    void     toggleSpecIllumination() {_specIllumination = !_specIllumination;}
    SLbool   specIllumination() { return this->_specIllumination;}
    void     toggleShadows() {_shadows = !_shadows;}
    SLbool   shadows() { return this->_shadows;}
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
    void         uploadRenderSettings(SLuint progId);
    void         calcWsToVoxelSpaceTransformation();
    void         voxelSpaceTransform(const SLfloat l, const SLfloat r, const SLfloat b,
                                       const SLfloat t, const SLfloat n, const SLfloat f);
    SLProjection _oldProjection;
    SLEyeType    _oldET;
    SLbool       _first = true;
    SLfloat      _diffuseConeAngle = 0.5f;
    SLfloat      _specularConeAngle = 0.01f;
    SLfloat      _shadowConeAngle = 0.f;
    SLfloat      _lightMeshSize = 2.7f;

    SLbool       _voxelVisualization = false;
    SLbool       _directIllumination = true;
    SLbool       _diffuseIllumination = true;
    SLbool       _specIllumination = true;
    SLbool       _shadows = true;

    SLMat4f*     _wsToVoxelSpace = new SLMat4f();
};
#endif SLCONETRACER_H
