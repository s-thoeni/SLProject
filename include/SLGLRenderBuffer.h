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

#ifndef SLGLRENDERBUFFER_H
#define SLGLRENDERBUFFER_H

#include <SLGLBuffer.h>

//-----------------------------------------------------------------------------
/*!
 */
class SLGLRenderBuffer : public SLGLBuffer
{
    public:         SLGLRenderBuffer    ();
                   ~SLGLRenderBuffer    () {clear();}
        
        //! Calls delete and clears data
        void        clear               ();
    
        //! Generates the renderbuffer
        void        generate            ();
    
        //! Binds the renderbuffer
        void        bind                ();
        
        //! Initializes the storage of a renderbuffer
        void        initilizeStorage    (SLGLInternalFormat format,
                                         SLsizei width,
                                         SLsizei height);
                                             
        // Some statistics
        static  SLuint      totalBufferCount;     //! static total no. of buffers in use
        static  SLuint      totalBufferSize;      //! static total size of all buffers in bytes
};
//-----------------------------------------------------------------------------

#endif
