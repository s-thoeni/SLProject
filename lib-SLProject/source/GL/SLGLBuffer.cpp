//#############################################################################
//  File:      SLGLBuffer.h
//  Purpose:   Abstract class around OpenGL Buffer Objects
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <SLGLBuffer.h>

//-----------------------------------------------------------------------------
//! Constructor initializing with default values
SLGLBuffer::SLGLBuffer()
{
    _id = 0;
    _sizeBytes = 0;
}
//-----------------------------------------------------------------------------
/*! Deletes the OpenGL objects for the frame buffer.
 */
void SLGLBuffer::deleteGL()
{
    if (_id)
    {   glDeleteBuffers(1, &_id);
        _id = 0;
    }
}
//-----------------------------------------------------------------------------
void SLGLBuffer::clearBuffers()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//-----------------------------------------------------------------------------
