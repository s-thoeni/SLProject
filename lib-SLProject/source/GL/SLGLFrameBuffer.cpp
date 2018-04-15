//#############################################################################
//  File:      SLGLFrameBuffer.h
//  Purpose:   Wrapper class around OpenGL Frame Buffer Objects (FBO)
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

#include <SLGLFrameBuffer.h>

//-----------------------------------------------------------------------------
SLuint SLGLFrameBuffer::totalBufferSize  = 0;
SLuint SLGLFrameBuffer::totalBufferCount = 0;
//-----------------------------------------------------------------------------
//! Constructor
SLGLFrameBuffer::SLGLFrameBuffer() : SLGLBuffer() {}
//-----------------------------------------------------------------------------
void SLGLFrameBuffer::clear()
{
    deleteGL();
    totalBufferCount--;
    totalBufferSize -= _sizeBytes;
}
//-----------------------------------------------------------------------------
void SLGLFrameBuffer::generate()
{
    glGenFramebuffers(1, &this->_id);
    totalBufferCount++;
}
//-----------------------------------------------------------------------------
void SLGLFrameBuffer::bind()
{
    assert(this->_id && "No framebuffer generated");
    glBindFramebuffer(GL_FRAMEBUFFER, this->_id);
}
//-----------------------------------------------------------------------------
/*! Attaches a render buffer as depth attachment for the frame buffer
 */
void SLGLFrameBuffer::attachRenderBuffer(SLuint rbo)
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
}
//-----------------------------------------------------------------------------
