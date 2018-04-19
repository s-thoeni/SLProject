//#############################################################################
//  File:      CubeMap.vert
//  Purpose:   GLSL vertex program for unlit skybox with a cube map
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

attribute   vec4 a_position;     // Vertex position attribute

uniform     mat4 u_mvMatrix;    // modelview matrix

varying vec4 P_VS;

//-----------------------------------------------------------------------------
void main ()
{
    P_VS = a_position;
    gl_Position = u_mvMatrix * P_VS;
}
//-----------------------------------------------------------------------------
