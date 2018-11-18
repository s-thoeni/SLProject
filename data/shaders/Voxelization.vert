//############################################################################
//  File:      Voxelization.vert
//  Purpose:   GLSL vertex shader calculating world-space point used in the
//             voxelization process.
//  Author:    Stefan Th�ni
//  Date:      September 2018
//  Copyright: Stefan Th�ni
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//############################################################################
#version 430 core

layout(location = 0) in vec4  a_position;    // Vertex position attribute
layout(location = 1) in vec3  a_normal;      // Vertex normal attribute

uniform     mat4  u_mvpMatrix;   // = projection * modelView
uniform     mat4  u_mMatrix;     // model matrix

out     vec3  v_N_WS;        // Normal at P_VS in world space
out     vec3  v_P_WS;        // position of vertex in world space

//-----------------------------------------------------------------------------
void main(void)
{
  // Careful! pMatrix should be a orthagonal projection!
  v_P_WS = vec3(u_mMatrix * a_position);
  //  v_N_WS = normalize(mat3(transpose(inverse(u_mMatrix))) * a_normal);
  v_N_WS = mat3(u_mMatrix) * a_normal;
  gl_Position = u_mvpMatrix * a_position;
}
//-----------------------------------------------------------------------------
