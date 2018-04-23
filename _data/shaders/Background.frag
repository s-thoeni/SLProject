//#############################################################################
//  File:      SkyBox.frag
//  Purpose:   GLSL vertex program for unlit skybox with a cube map
//  Author:    Marcus Hudritsch
//  Date:      October 2017
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifdef GL_ES
precision mediump float;
#endif

uniform float       u_exposure;
uniform samplerCube u_texture0; // cube map texture
varying vec3        v_texCoord; // Interpol. 3D texture coordinate

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = textureCube(u_texture0, v_texCoord).rgb;
    
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * u_exposure);
    
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));
    
    gl_FragColor = vec4(mapped, 1.0);
}
