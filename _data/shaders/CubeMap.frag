//#############################################################################
//  File:      CubeMap.vert
//  Purpose:   GLSL vertex program for unlit skybox with a cube map
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

uniform samplerCube u_texture0;

varying vec4 P_VS;

void main()
{
    vec3 uv = vec3(P_VS.x,P_VS.y,P_VS.z);
    gl_FragColor = vec4(texture(u_texture0, uv).rgb, 1.0);
}
