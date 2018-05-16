//#############################################################################
//  File:      SLGLTextureGenerated.cpp
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h>           // precompiled headers
#ifdef SL_MEMLEAKDETECT       // set in SL.h for debug config only
#include <debug_new.h>        // memory leak detector
#endif

#include <SLApplication.h>
#include <SLScene.h>
#include <SLGLTextureGenerated.h>

//-----------------------------------------------------------------------------
//! ctor for generated textures from hdr textures
SLGLTextureGenerated::SLGLTextureGenerated(SLGLTexture*      texture,
                                           SLGLFrameBuffer*  fbo,
                                           SLTextureType     type,
                                           SLenum            target,
                                           SLint             min_filter,
                                           SLint             mag_filter,
                                           SLint             wrapS,
                                           SLint             wrapT)
{
    if (texture != nullptr)
    {   assert(texture->texType()>=TT_hdr);
        assert(type>TT_hdr);
    }
    
    _stateGL = SLGLState::getInstance();
    
    _sourceTexture  = texture;
    _captureFBO     = fbo;
    _texType        = type;
    _min_filter     = min_filter;
    _mag_filter     = mag_filter;
    _wrap_s         = wrapS;
    _wrap_t         = wrapT;
    _target         = target;
    _texName        = 0;
    _bumpScale      = 1.0f;
    _resizeToPow2   = false;
    _autoCalcTM3D   = false;
    _needsUpdate    = false;
    _bytesOnGPU     = 0;
    _shaderProgram  = this->getProgramFromType(type);
    _captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);
    
    _captureViews.push_back(SLMat4f::sLookAt(0.0f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f));
    _captureViews.push_back(SLMat4f::sLookAt(0.0f, 0.0f, 0.0f,-1.0f,  0.0f,  0.0f, 0.0f, -1.0f,  0.0f));
    _captureViews.push_back(SLMat4f::sLookAt(0.0f, 0.0f, 0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  0.0f,  1.0f));
    _captureViews.push_back(SLMat4f::sLookAt(0.0f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f,  0.0f, -1.0f));
    _captureViews.push_back(SLMat4f::sLookAt(0.0f, 0.0f, 0.0f, 0.0f,  0.0f,  1.0f, 0.0f, -1.0f,  0.0f));
    _captureViews.push_back(SLMat4f::sLookAt(0.0f, 0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f, -1.0f,  0.0f));
    
    if (this->_sourceTexture != nullptr) this->_sourceTexture->bindActive();
    this->build();
    
    SLApplication::scene->textures().push_back(this);
}
//-----------------------------------------------------------------------------
SLGLTextureGenerated::~SLGLTextureGenerated()
{
    clearData();
}
//-----------------------------------------------------------------------------
void SLGLTextureGenerated::clearData()
{
    glDeleteTextures(1, &_texName);

    numBytesInTextures -= _bytesOnGPU;

    for (SLint i=0; i<_images.size(); ++i)
    {   delete _images[i];
        _images[i] = 0;
    }
    _images.clear();

    _texName = 0;
    _bytesOnGPU = 0;
    _vaoSprite.clearAttribs();
}
//-----------------------------------------------------------------------------
void SLGLTextureGenerated::build(SLint texID)
{
    if (this->_captureFBO->id() && this->_captureFBO->rbo())
    {
        this->_captureFBO->bind();
        this->_captureFBO->bindRenderBuffer();
        
        glGenTextures(1, &this->_texName);
        glBindTexture(this->_target, this->_texName);
        
        if (this->_texType == TT_environment || this->_texType == TT_irradiance)
        {   for (SLuint i = 0; i < 6; i++)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
                             this->_captureFBO->rboWidth(), this->_captureFBO->rboHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
            }
        
            glTexParameteri(this->_target, GL_TEXTURE_WRAP_S, this->_wrap_s);
            glTexParameteri(this->_target, GL_TEXTURE_WRAP_T, this->_wrap_t);
            glTexParameteri(this->_target, GL_TEXTURE_WRAP_R, this->_wrap_t);
            glTexParameteri(this->_target, GL_TEXTURE_MIN_FILTER, this->_min_filter);
            glTexParameteri(this->_target, GL_TEXTURE_MAG_FILTER, this->_mag_filter);
            
            _shaderProgram->useProgram();
            _shaderProgram->uniform1i("u_texture0", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(this->_sourceTexture->target(), this->_sourceTexture->texName());
            
            glViewport(0, 0, this->_captureFBO->rboWidth(), this->_captureFBO->rboHeight());
            this->_captureFBO->bind();
            
            for (SLuint i = 0; i < 6; i++)
            {
                SLMat4f mvp = _captureProjection * _captureViews[i];
                _shaderProgram->uniformMatrix4fv("u_mvpMatrix", 1, mvp.m());
                this->_captureFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                this->renderCube();
            }
            
            this->_captureFBO->unbind();
            
            if (this->_texType==TT_environment)
            {   glBindTexture(GL_TEXTURE_CUBE_MAP, this->_texName);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            }
        } else
        if (this->_texType == TT_prefilter)
        {
            assert(this->_sourceTexture->texType()==TT_environment && "the source texture is not an environment map");
            this->_captureFBO->unbind();
            
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            
            _shaderProgram->useProgram();
            _shaderProgram->uniform1i("u_texture0", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(this->_sourceTexture->target(), this->_sourceTexture->texName());
            
            this->_captureFBO->bindRenderBuffer();
            SLuint maxMipLevels = 5;
            for (SLuint mip = 0; mip < maxMipLevels; ++mip)
            {
                // resize framebuffer according to mip-level size
                SLuint mipWidth  = 128 * pow(0.5, mip);
                SLuint mipHeight = 128 * pow(0.5, mip);
                this->_captureFBO->bufferStorage(mipWidth, mipHeight);
                glViewport(0, 0, mipWidth, mipHeight);
                
                SLfloat roughness = (SLfloat)mip / (SLfloat)(maxMipLevels - 1);
                _shaderProgram->uniform1f("u_roughness", roughness);
                for (SLuint i = 0; i < 6; ++i)
                {
                    SLMat4f mvp = _captureProjection * _captureViews[i];
                    _shaderProgram->uniformMatrix4fv("u_mvpMatrix", 1, mvp.m());
                    this->_captureFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this, mip);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    
                    this->renderCube();
                }
                
                this->_captureFBO->unbind();
            }
        } else
        if (this->_texType == TT_lut)
        {
            this->_captureFBO->unbind();
            
            SLsizei lutWidth  = 512;
            SLsizei lutHeight = 512;
            
            glTexImage2D(this->_target, 0, GL_RG16F, lutWidth, lutHeight, 0, GL_RG, GL_FLOAT, 0);
            glTexParameteri(this->_target, GL_TEXTURE_WRAP_S, this->_wrap_s);
            glTexParameteri(this->_target, GL_TEXTURE_WRAP_T, this->_wrap_t);
            glTexParameteri(this->_target, GL_TEXTURE_MIN_FILTER, this->_min_filter);
            glTexParameteri(this->_target, GL_TEXTURE_MAG_FILTER, this->_mag_filter);
            
            this->_captureFBO->bufferStorage(lutWidth, lutHeight);
            this->_captureFBO->attachTexture2D(GL_COLOR_ATTACHMENT0, this->_target, this);
            
            glViewport(0, 0, lutWidth, lutHeight);
            this->_shaderProgram->useProgram();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            this->renderQuad();
            this->_captureFBO->unbind();
        }
    }
}
//-----------------------------------------------------------------------------
//! renders 1x1 cube
void SLGLTextureGenerated::renderCube()
{
    // initialize (if necessary)
    if (_cubeVAO == 0)
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
        glGenVertexArrays(1, &_cubeVAO);
        glGenBuffers(1, &_cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, _cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(_cubeVAO);
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
    glBindVertexArray(_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
//-----------------------------------------------------------------------------
//! renders a 1x1 XY quad
void SLGLTextureGenerated::renderQuad()
{
    if (this->_quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &this->_quadVAO);
        glGenBuffers(1, &this->_quadVBO);
        glBindVertexArray(this->_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(this->_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

SLGLProgram* SLGLTextureGenerated::getProgramFromType(SLTextureType type)
{
    if (type==TT_environment) return new SLGLGenericProgram("CubeMap.vert", "EquirectangularToCubeMap.frag");
    if (type==TT_irradiance)  return new SLGLGenericProgram("CubeMap.vert", "IrradianceConvolution.frag");
    if (type==TT_prefilter)   return new SLGLGenericProgram("CubeMap.vert", "Prefilter.frag");
    if (type==TT_lut)         return new SLGLGenericProgram("BRDF.vert", "BRDF.frag");
    return nullptr;
}













