//#############################################################################
//  File:      Voxelization.geom
//  Purpose:   GLSL geometry shader projects triangle onto main axis and
//             projects to clip space for voxelization
//  Author:    Stefan Thöni
//  Date:      September 2018
//  Copyright: Stefan Thöni
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################
#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 v_N_WS[];
in vec3 v_P_WS[];

out vec3 o_F_WS; // Fragment/Voxel world position
out vec3 o_N_WS; // Fragment/Voxel normal

//-----------------------------------------------------------------------------
void main(void)
{
  // calculate face normal:
  const vec3 p1 = v_P_WS[1] - v_P_WS[0];
  const vec3 p2 = v_P_WS[2] - v_P_WS[0];
  const vec3 faceN = abs(cross(p1, p2));

  // Main axis of the triangle:
  uint maxAxis = faceN[1] > faceN[0] ? 1 : 0;
  maxAxis = faceN[2] > faceN[maxAxis] ? 2 : maxAxis;

  // emit voxel position:
  for(uint i = 0; i < 3; ++i){
    o_F_WS = v_P_WS[i];
    o_N_WS = v_N_WS[i];

    if(maxAxis == 0){
      gl_Position = vec4(o_F_WS.z, o_F_WS.y, 0, 1);
    } else if (maxAxis == 1){
      gl_Position = vec4(o_F_WS.x, o_F_WS.z, 0, 1);
    } else {
      gl_Position = vec4(o_F_WS.x, o_F_WS.y, 0, 1);
    }
    EmitVertex();
  }
  EndPrimitive();
}
//-----------------------------------------------------------------------------
