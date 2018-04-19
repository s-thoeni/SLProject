//#############################################################################
//  File:      SLSkybox
//  Author:    Marcus Hudritsch
//  Date:      December 2017
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h>           // precompiled headers
#ifdef SL_MEMLEAKDETECT       // set in SL.h for debug config only
#include <debug_new.h>        // memory leak detector
#endif

#include <SLSkybox.h>
#include <SLGLTexture.h>
#include <SLGLFrameBuffer.h>
#include <SLGLRenderBuffer.h>
#include <SLMaterial.h>
#include <SLBox.h>
#include <SLCamera.h>
#include <SLSceneView.h>



// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}




//-----------------------------------------------------------------------------
//! Default constructor
SLSkybox::SLSkybox(SLstring name) : SLNode(name)
{

}
//-----------------------------------------------------------------------------
//! Cubemap Constructor with cubemap images
/*! All resources allocated are stored in the SLScene vectors for textures,
materials, programs and meshes and get deleted at scene destruction.
*/
SLSkybox::SLSkybox(SLstring cubeMapXPos,
                   SLstring cubeMapXNeg,
                   SLstring cubeMapYPos,
                   SLstring cubeMapYNeg,
                   SLstring cubeMapZPos,
                   SLstring cubeMapZNeg,
                   SLstring name) : SLNode(name)
{
    // Create texture, material and program
    SLGLTexture* cubeMap = new SLGLTexture(cubeMapXPos,cubeMapXNeg
                                          ,cubeMapYPos,cubeMapYNeg
                                          ,cubeMapZPos,cubeMapZNeg);
    SLMaterial* matCubeMap = new SLMaterial("matCubeMap");
    matCubeMap->textures().push_back(cubeMap);
    SLGLProgram* sp = new SLGLGenericProgram("SkyBox.vert", "SkyBox.frag");
    matCubeMap->program(sp);

    // Create a box with max. point at min. parameter and vice versa.
    // Like this the boxes normals will point to the inside.
    this->addMesh(new SLBox(10,10,10, -10,-10,-10, "box", matCubeMap));
}
//-----------------------------------------------------------------------------
//! Draw the skybox with a cube map with the camera in its center.
SLSkybox::SLSkybox(SLstring hdrImage,
                   SLstring name) : SLNode(name)
{
    SLGLProgram* equirectangularToCubemapShader = new SLGLGenericProgram("CubeMap.vert", "EquirectangularToCubeMap.frag");
    SLGLProgram* backgroundShader = new SLGLGenericProgram("Background.vert", "Background.frag");
    SLBox* cbox = new SLBox(-1,-1,-1,1,1,1,"cube");
    
    // setup framebuffer
    SLGLFrameBuffer*  captureFBO = new SLGLFrameBuffer();
    SLGLRenderBuffer* captureRBO = new SLGLRenderBuffer();
    
    captureFBO->generate();
    captureRBO->generate();
    
    captureFBO->bind();
    captureRBO->bind();
    captureRBO->initilizeStorage(SLGLInternalFormat::IF_depth24, 512, 512);
    captureFBO->attachRenderBuffer(captureRBO->id());
    
    // loading HDR environment map
    SLGLTexture* hdrTexture = new SLGLTexture(hdrImage,
                                           GL_LINEAR, GL_LINEAR,
                                           SLTextureType::TT_unknown,
                                           GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    // pre-allocation memory for the cube map
    SLGLTexture* envCubemap = new SLGLTexture(512, 512);
    
    SLMat4f captureProjection;
    captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);
    
    SLMat4f cubemapXPositive;
    SLMat4f cubemapXNegative;
    SLMat4f cubemapYPositive;
    SLMat4f cubemapYNegative;
    SLMat4f cubemapZPositive;
    SLMat4f cubemapZNegative;
    
    cubemapXPositive.lookAt( 0.0f,  0.0f,  0.0f,
                             1.0f,  0.0f,  0.0f,
                             0.0f, -1.0f,  0.0f);
    cubemapXNegative.lookAt( 0.0f,  0.0f,  0.0f,
                            -1.0f,  0.0f,  0.0f,
                             0.0f, -1.0f,  0.0f);
    cubemapYPositive.lookAt( 0.0f,  0.0f,  0.0f,
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f);
    cubemapYNegative.lookAt( 0.0f,  0.0f,  0.0f,
                             0.0f, -1.0f,  0.0f,
                             0.0f,  0.0f, -1.0f);
    cubemapZPositive.lookAt( 0.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,
                             0.0f, -1.0f,  0.0f);                             
    cubemapZNegative.lookAt( 0.0f,  0.0f,  0.0f,
                             0.0f,  0.0f, -1.0f,
                             0.0f, -1.0f,  0.0f);
    
    SLMat4f captureViews[] = {
        cubemapXPositive, cubemapXNegative,
        cubemapYPositive, cubemapYNegative,
        cubemapZPositive, cubemapZNegative
    };
    
    equirectangularToCubemapShader->useProgram();
    equirectangularToCubemapShader->uniform1i("u_texture0", 0);
    _stateGL->activeTexture(GL_TEXTURE0);
    _stateGL->bindTexture(GL_TEXTURE2, hdrTexture->texName());
    
    _stateGL->viewport(0, 0, 512, 512);
    captureFBO->bind();
    for (SLuint i = 0; i < 6; i++)
    {
        SLMat4f mvp = captureProjection * captureViews[i];
        equirectangularToCubemapShader->uniformMatrix4fv("u_mvpMatrix", 1, (SLfloat*)&mvp);
        captureFBO->attachTexture2D(SLGLInternalFormat::IF_colorAttachment0, (SLGLInternalFormat)(SLGLInternalFormat::IF_positiveX + i), envCubemap);
        _stateGL->clearColorDepthBuffer();
        renderCube();
    }
    captureFBO->unbind();
    //captureFBO->clear();
    //captureRBO->clear();
    
    SLMat4f projection;
    projection.perspective(0.0f, 640.0f/480.0f, 0.1f, 100.0f);
    backgroundShader->uniformMatrix4fv("projection", 1, (SLfloat*)&projection);

    SLMaterial* hdrMaterial = new SLMaterial("matCubeMap");
    hdrMaterial->textures().push_back(envCubemap);
    hdrMaterial->program(backgroundShader);
    
    _stateGL->viewport(0, 0, 640, 480);

    this->addMesh(new SLBox(1,1,1,-1,-1,-1, "box", hdrMaterial));
}

//-----------------------------------------------------------------------------
//! Draw the skybox with a cube map with the camera in its center.
void SLSkybox::drawAroundCamera(SLSceneView* sv)
{
    assert(sv && "No SceneView passed to SLSkybox::drawAroundCamera");
    
    // Set the view transform
    _stateGL->modelViewMatrix.setMatrix(_stateGL->viewMatrix);

    // Put skybox at the cameras position
    this->translation(sv->camera()->translationWS());

    // Apply world transform
    _stateGL->modelViewMatrix.multiply(this->updateAndGetWM().m());

    // Freeze depth buffer
    _stateGL->depthMask(false);

    // Draw the box
    this->drawMeshes(sv);

    // Unlock depth buffer
    _stateGL->depthMask(true);
}
//-----------------------------------------------------------------------------
//! Returns the color in the skybox at the the specified direction dir
SLCol4f SLSkybox::colorAtDir(SLVec3f dir)
{
    assert(_meshes.size() > 0);
    assert(_meshes[0]->mat()->textures().size() > 0);
    
    SLGLTexture* tex = _meshes[0]->mat()->textures()[0];
    
    return tex->getTexelf(dir);
}
//-----------------------------------------------------------------------------
