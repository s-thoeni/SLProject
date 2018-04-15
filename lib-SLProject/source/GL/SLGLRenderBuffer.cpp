//#############################################################################
//  File:      SLGLFrameBuffer.h
//  Purpose:   Wrapper class around OpenGL Render Buffer Objects (RBO)
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include "SLGLRenderBuffer.h"

//-----------------------------------------------------------------------------
SLuint SLGLRenderBuffer::totalBufferSize  = 0;
SLuint SLGLRenderBuffer::totalBufferCount = 0;
//-----------------------------------------------------------------------------
//! Constructor
SLGLRenderBuffer::SLGLRenderBuffer() : SLGLBuffer() {}
//-----------------------------------------------------------------------------
void SLGLRenderBuffer::clear()
{
    deleteGL();
    totalBufferCount--;
    totalBufferSize -= _sizeBytes;
}
//-----------------------------------------------------------------------------
void SLGLRenderBuffer::generate()
{
    glGenRenderbuffers(1, &this->_id);
    totalBufferCount++;
}
//-----------------------------------------------------------------------------
void SLGLRenderBuffer::bind()
{
    assert(this->_id && "No framebuffer generated");
    glBindRenderbuffer(GL_RENDERBUFFER, this->_id);
}
//-----------------------------------------------------------------------------
void SLGLRenderBuffer::initilizeStorage(SLGLInternalFormat format,
                                        SLsizei width,
                                        SLsizei height)
{
    assert(this->_id && "No framebuffer generated");
    glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}
