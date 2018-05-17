//#############################################################################
//  File:      PBR_Lighting.vert
//  Purpose:   GLSL vertex shader for Cook-Torrance physical based rendering
//             including diffuse irradiance and specular IBL. Based on the
//             physically based rendering (PBR) tutorial with GLSL by Joey de
//             Vries on https://learnopengl.com/#!PBR/Theory
//  Author:    Carlos Arauz, 
//             adapted from PerPixCookTorrance.vert by Marcus Hudritsch
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

attribute   vec4  a_position;     // Vertex position attribute
attribute   vec3  a_normal;       // Vertex normal attribute
attribute   vec2  a_texCoord;     // Vertex texture coordiante attribute

uniform     mat4  u_mvMatrix;     // modelview matrix 
uniform     mat4  u_invMvMatrix;  // inverse modelview matrix
uniform     mat3  u_nMatrix;      // normal matrix=transpose(inverse(mv))
uniform     mat4  u_mvpMatrix;    // = projection * modelView

varying     vec3  v_P_VS;         // Point of illumination in view space (VS)
varying     vec3  v_N_VS;         // Normal at P_VS in view space
varying     vec2  v_texCoord;     // Texture coordiante varying
varying     mat4  v_invMvMatrix;  // inverse modelview matrix for rotation

//-----------------------------------------------------------------------------
void main()
{
    v_texCoord    = a_texCoord;
    v_P_VS        = vec3(u_mvMatrix * a_position);
    v_N_VS        = vec3(u_nMatrix * a_normal);
    v_invMvMatrix = u_invMvMatrix;
  
    gl_Position = u_mvpMatrix * a_position;
}
//-----------------------------------------------------------------------------
