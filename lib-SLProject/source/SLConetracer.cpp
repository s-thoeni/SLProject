//#############################################################################
//  File:      SLConetracer.cpp
//  Author:    Stefan Thöni
//  Date:      September 2018
//  Copyright: Stefan Thöni
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h> // Must be the 1st include followed by  an empty line

#include <SLConetracer.h>
#include <SLGLProgram.h>
#include <SLGLGenericProgram.h>
#include <SLGLTexture3D.h>
#include <SLSceneView.h>
#include <SLApplication.h>


SLConetracer::SLConetracer() {
    SL_LOG("Constructor      : SLConetracer\n");
}

void SLConetracer::init(SLint scrW, SLint scrH){
    SL_LOG("initializing      : SLConetracer\n");
    SL_LOG("Viewport Width    : %d\n", scrW);
    SL_LOG("Viewport Height   : %d\n", scrH);

    // Initialize voxel 3D-Texture:
    const std::vector<GLfloat> texture3D(4 * _voxelTextureSize * _voxelTextureSize * _voxelTextureSize, 0.0f);
    _voxelTexture = new SLGLTexture3D(texture3D, _voxelTextureSize, _voxelTextureSize, _voxelTextureSize, true);
    GET_GL_ERROR;

    // Initialize voxelization:
    SLGLProgram* voxelizationShader = new SLGLGenericProgram("Voxelization.vert", "Voxelization.frag","Voxelization.geom");
    voxelizationShader->initRaw();
    _voxelizationMaterial = new SLMaterial("Voxelization-Material", voxelizationShader);
    GET_GL_ERROR;

    // initialize voxel visualization:
    SLGLProgram* worldPosProg = new SLGLGenericProgram("VXWorldpos.vert", "VXWorldpos.frag");
    worldPosProg->initRaw();
    _worldMaterial = new SLMaterial("World-Material", worldPosProg);

    // initialize voxel visualization:
    SLGLProgram* vxvisualizatonShader = new SLGLGenericProgram("VXVisualize.vert", "VXVisualize.frag");
    vxvisualizatonShader->initRaw();
    _voxelVisualizationMaterial = new SLMaterial("World-Material", vxvisualizatonShader);

    // initialize voxel conetracing material:
    SLGLProgram* ctShader = new SLGLGenericProgram("VXConetracing.vert", "VXConetracing.frag");
    ctShader->initRaw();
    _voxelConetracingMaterial = new SLMaterial("Voxel Cone Tracing Material", ctShader);

    // FBOs.
    // read current viewport
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport );

    _visualizationFbo1 = new SLGLFbo(scrW, scrH);
    _visualizationFbo2 = new SLGLFbo(scrW, scrH);

    _quadMesh = new SLRectangle(SLVec2f(-1, -1), SLVec2f(1, 1), 1, 1);
    _cubeMesh = new SLBox(-1, -1, -1);
}

void SLConetracer::visualizeVoxelization(){
    // store viewport
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport );

    glViewport(0, 0, _voxelTextureSize, _voxelTextureSize);

    glUseProgram(_worldMaterial->program()->progid());
    GET_GL_ERROR;
    // Settings.
    SLGLState* stateGL   = SLGLState::getInstance();
    stateGL->clearColor(_sv->_camera->background().colors()[0]);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    GET_GL_ERROR;

    // Back.
    glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, _visualizationFbo1->frameBuffer);
    glViewport(0, 0, _visualizationFbo1->width, _visualizationFbo1->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderNode(new SLNode( _cubeMesh), _worldMaterial->program()->progid());
    GET_GL_ERROR;

    // Front.
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, _visualizationFbo2->frameBuffer);
    glViewport(0, 0, _visualizationFbo2->width, _visualizationFbo2->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderNode(new SLNode(_cubeMesh), _worldMaterial->program()->progid());
    GET_GL_ERROR;
    // Render 3D Texture to screen
    glUseProgram(_voxelVisualizationMaterial->program()->progid());

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GET_GL_ERROR;
    _visualizationFbo1->activateAsTexture(_voxelVisualizationMaterial->program()->progid(), "textureBack", 0);
    _visualizationFbo2->activateAsTexture(_voxelVisualizationMaterial->program()->progid(), "textureFront", 1);
    _voxelTexture->activate(_voxelVisualizationMaterial->program()->progid(), "texture3D", 2);
    GET_GL_ERROR;
    // Render.
    // restore viewport:
    glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint loc = glGetUniformLocation(_voxelVisualizationMaterial->program()->progid(), "cameraPosition");
    SLVec3f pos = _sv->camera()->translationWS();
    glUniform3fv(loc, 1, (float*) &pos);

    renderNode(new SLNode(_quadMesh), _voxelVisualizationMaterial->program()->progid());
}

SLbool SLConetracer::render(SLSceneView* sv){
    _sv = sv;

    voxelize();

    //visualizeVoxelization();
    renderConetraced();
    GET_GL_ERROR;
    // reset vp after voxelization:
    // GL Settings.

    // SL_LOG("I can render!      : SLConetracer\n");
    return true;
}

void SLConetracer::renderConetraced(){
    SLuint progId = _voxelConetracingMaterial->program()->progid();
    GET_GL_ERROR;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GET_GL_ERROR;

    glUseProgram(progId);

    renderSceneGraph(progId);
}

void SLConetracer::uploadLights(SLuint progId) {
    SLGLState *stateGL = SLGLState::getInstance();
    // no ambient color needed. :-)
    //glUniform1f(glGetUniformLocation(program, "u_matShininess"), this->_shininess);
    glUniform1i(glGetUniformLocation(progId, "u_numLightsUsed"), stateGL->numLightsUsed);

    if (stateGL->numLightsUsed > 0) {
        SLint nL = SL_MAX_LIGHTS;
        glUniform1iv(glGetUniformLocation(progId, "u_lightIsOn"), nL, (SLint *) stateGL->lightIsOn);
        glUniform4fv(glGetUniformLocation(progId, "u_lightPosWS"), nL, (SLfloat *) stateGL->lightPosWS);
        glUniform4fv(glGetUniformLocation(progId, "u_lightDiffuse"), nL, (SLfloat *) stateGL->lightDiffuse);
        glUniform4fv(glGetUniformLocation(progId, "u_lightSpecular"), nL, (SLfloat *) stateGL->lightSpecular);
        glUniform3fv(glGetUniformLocation(progId, "u_lightSpotDirWS"), nL, (SLfloat *) stateGL->lightSpotDirWS);
        glUniform1fv(glGetUniformLocation(progId, "u_lightSpotCutoff"), nL, (SLfloat *) stateGL->lightSpotCutoff);
        glUniform1fv(glGetUniformLocation(progId, "u_lightSpotCosCut"), nL, (SLfloat *) stateGL->lightSpotCosCut);
        glUniform1fv(glGetUniformLocation(progId, "u_lightSpotExp"), nL, (SLfloat *) stateGL->lightSpotExp);
        glUniform3fv(glGetUniformLocation(progId, "u_lightAtt"), nL, (SLfloat *) stateGL->lightAtt);
        glUniform1iv(glGetUniformLocation(progId, "u_lightDoAtt"), nL, (SLint *) stateGL->lightDoAtt);
    }
}

// Renders scene using a given Program
void SLConetracer::renderSceneGraph(SLuint progId){
    // upload light settings:
    GET_GL_ERROR;
    this->uploadLights(progId);
    GET_GL_ERROR;
    // upload camera position:
    SLVec3f camPos = _sv->camera()->translationWS();
    glUniform3fv(glGetUniformLocation(progId, "u_EyePos"),1,  (SLfloat *) &camPos);
    GET_GL_ERROR;
    SLScene* s = SLApplication::scene;
    this->renderNode(s->root3D(), progId);
}

void SLConetracer::setCameraOrthographic(){
    // _sv->camera()->projection(P_monoOrthographic);
    GET_GL_ERROR;
    _sv->camera()->setProjection(_sv, ET_center);
    _sv->camera()->setView(_sv, ET_center);
    GET_GL_ERROR;
}

void SLConetracer::resetCamera(){
    SLCamera* cam = _sv->camera();
    cam->projection(_oldProjection);
    cam->setProjection(_sv, _oldET);
}


void SLConetracer::renderNode(SLNode* node, const SLuint progId){
    GET_GL_ERROR;
    assert(node);

    GET_GL_ERROR;
    SLGLState* stateGL   = SLGLState::getInstance();

    // set view transform:
    stateGL->modelViewMatrix.setMatrix(stateGL->viewMatrix);

    // apply world transform for this node:
    stateGL->modelViewMatrix.multiply(node->updateAndGetWM().m());

    // print mvp matrix
    //stateGL->mvpMatrix()->print("mvp matrix: ");

    GLint loc = glGetUniformLocation(progId, "u_mvpMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, (SLfloat*) stateGL->mvpMatrix());

    node->draw(progId);

    GET_GL_ERROR;

    for(auto child : node->children()){
        renderNode(child, progId);
    }
}

void SLConetracer::voxelize(){
    this->_voxelTexture->clear(SLVec4f(0.0f, 0.0f, 0.0f, 0.0f));
    SLGLProgram* prog = this->_voxelizationMaterial->program();

    // store viewport
    GLint m_viewport[4];
    glGetIntegerv( GL_VIEWPORT, m_viewport );


    glUseProgram(prog->progid());
    GET_GL_ERROR;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GET_GL_ERROR;

    glViewport(0, 0, this->_voxelTextureSize, this->_voxelTextureSize);
    GET_GL_ERROR;
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    GET_GL_ERROR;
    this->_voxelTexture->activate(prog->progid(), "texture3D", 0);
    GET_GL_ERROR;

    // Bind texture where we want to write to:
    glBindImageTexture(0, this->_voxelTexture->textureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

    GET_GL_ERROR;
    // Set the camera into orthographic mode:
    this->setCameraOrthographic();
    this->renderSceneGraph(prog->progid());
    //this->resetCamera();

    // restore viewport:
    glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

    glGenerateMipmap(GL_TEXTURE_3D);

    // reset color mask
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    GET_GL_ERROR;
}

SLConetracer::~SLConetracer() {
    SL_LOG("Destructor      : ~SLConetracer\n");
}