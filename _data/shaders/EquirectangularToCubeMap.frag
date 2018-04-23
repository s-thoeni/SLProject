//#############################################################################
//  File:      CubeMap.vert
//  Purpose:   GLSL vertex program to convert equirectangular images to cubemap
//  Author:    Carlos Arauz
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

varying vec3 P_VS;

uniform sampler2D u_texture0;  // Equirectagular map

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(P_VS));
    vec3 color = texture(u_texture0, uv).rgb;
    
    gl_FragColor = vec4(color, 1.0);
}
