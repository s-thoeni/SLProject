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

#ifndef SLGLBUFFER_H
#define SLGLBUFFER_H

#include <stdafx.h>
#include <SLGLEnums.h>

//-----------------------------------------------------------------------------
/*!
 */
class SLGLBuffer
{
    public:                 SLGLBuffer     ();
        virtual            ~SLGLBuffer     () {};
    
        // Virtual functions
        virtual void        clear               () = 0;
        virtual void        generate            () = 0;
        virtual void        bind                () = 0;
        
                void        deleteGL            ();
    
        // Getters
                SLuint      id                  () {return _id;}
    
    protected:
                SLuint      _id;
                SLuint      _sizeBytes;
};
//-----------------------------------------------------------------------------

#endif
