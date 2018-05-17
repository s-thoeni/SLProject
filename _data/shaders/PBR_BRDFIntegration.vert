//#############################################################################
//  File:      PBR_BRDFIntegration.vert
//  Purpose:   GLSL vertex program for generating a BRDF integration map, which
//             is the second part of the specular integral.
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

attribute   vec3 a_position;       // Vertex position attribute
attribute   vec2 a_texCoord;       // Vertex texture coord. attribute

varying vec2 v_texCoord;

//-----------------------------------------------------------------------------
void main()
{
    v_texCoord  = a_texCoord;
    gl_Position = vec4(a_position, 1.0);
}
//-----------------------------------------------------------------------------
