//#############################################################################
//  File:      PBR_CubeMap.frag
//  Purpose:   GLSL fragment program for rendering cube maps
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

uniform samplerCube u_texture0;       // cube map texture

varying vec4        P_VS;             // sample direction

void main()
{
    vec3 uv = vec3(P_VS.x,P_VS.y,P_VS.z);
    gl_FragColor = vec4(texture(u_texture0, uv).rgb, 1.0);
}
