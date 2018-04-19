//#############################################################################
//  File:      CubeMap.vert
//  Purpose:   GLSL vertex program to convert equirectangular images to cubemap
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

varying vec4 P_VS;

uniform samplerCube u_texture0;  // Equirectagular map

void main()
{
    vec3 envColor = texture(u_texture0, vec3(P_VS)).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));
    
    gl_FragColor = vec4(envColor, 1.0);
}
