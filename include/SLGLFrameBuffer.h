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

#ifndef SLGLFRAMEBUFFER_H
#define SLGLFRAMEBUFFER_H

#include <SLGLBuffer.h>

//-----------------------------------------------------------------------------
/*!
 */
class SLGLFrameBuffer : public SLGLBuffer
{
    public:         SLGLFrameBuffer     ();
                   ~SLGLFrameBuffer     () {clear();}
    
        //! Calls delete and clears data
        void        clear               ();
    
        //! Generates the framebuffer
        void        generate            ();
    
        //! Binds the framebuffer
        void        bind                ();
    
        //! Unbinds the framebuffer
        void        unbind              ();
    
        //! Attaches a renderbuffer
        void        attachRenderBuffer  (SLuint rbo);
    
        //! Attaches texture image to framebuffer
        void        attachTexture2D     (SLGLInternalFormat attachment,
                                         SLGLInternalFormat target,
                                         SLGLTexture*       texture,
                                         SLint              level = 0);
    
            
        // Some statistics
        static  SLuint      totalBufferCount;     //! static total no. of buffers in use
        static  SLuint      totalBufferSize;      //! static total size of all buffers in bytes
};
//-----------------------------------------------------------------------------

#endif
