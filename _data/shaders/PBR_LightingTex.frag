//#############################################################################
//  File:      PBR_LightingTex.frag
//  Purpose:   GLSL fragment shader for Cook-Torrance physical based rendering
//             including diffuse irradiance and specular IBL. Based on the
//             physically based rendering (PBR) tutorial with GLSL by Joey de
//             Vries on https://learnopengl.com/#!PBR/Theory
//  Author:    Carlos Arauz, 
//             adapted from PerPixCookTorrancetex.frag by Marcus Hudritsch
//  Date:      April 2018
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifdef GL_ES
precision highp float;
#endif

varying     vec3  v_P_VS;             // Point of illumination in view space (VS)
varying     vec3  v_N_VS;             // Normal at P_VS in view space
varying     vec2  v_texCoord;         // Texture coordiante varying
varying     mat4  v_invMvMatrix;      // needs inverse MV matrix for the rotation of the reflections

uniform int       u_numLightsUsed;    //!< NO. of lights used light arrays
uniform bool      u_lightIsOn[8];     //!< flag if light is on
uniform vec4      u_lightPosVS[8];    //!< position of light in view space
uniform vec4      u_lightDiffuse[8];  //!< diffuse light intensity (Id)
uniform float     u_exposure;

uniform sampler2D u_texture0;         //! Diffuse Color map (albedo)
uniform sampler2D u_texture1;         //! Normal map
uniform sampler2D u_texture2;         //! Metallic map
uniform sampler2D u_texture3;         //! Roughness map
uniform sampler2D u_texture4;         //! Ambient Occlusion map

// IBL pre-generated textures
uniform samplerCube u_texture5;       //!< IBL irradiance convolution map
uniform samplerCube u_texture6;       //!< IBL prefilter roughness map
uniform sampler2D   u_texture7;       //!< IBL brdf integration map

const float PI = 3.14159265359;
//-----------------------------------------------------------------------------
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_texture1, v_texCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_P_VS);
    vec3 Q2  = dFdy(v_P_VS);
    vec2 st1 = dFdx(v_texCoord);
    vec2 st2 = dFdy(v_texCoord);

    vec3 N  =  normalize(v_N_VS);
    vec3 T  =  normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
//-----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
//-----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
//-----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
//-----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
//-----------------------------------------------------------------------------
void PointLight (in    int   i,         // Light number
                 in    vec3  P_VS,      // Point of illumination in VS
                 in    vec3  N,         // Normalized normal at v_P_VS
                 in    vec3  V,         // Normalized vector from v_P_VS to view in VS
                 in    vec3  F0,        // Frenel reflection at 90 deg. (0 to N)
                 in    vec3  diffuse,   // Material diffuse color
                 in    float roughness, // Material roughness
                 in    float metallic,  // Material metallic
                 inout vec3  Lo)        // reflected intensity
{
    vec3 L = u_lightPosVS[i].xyz - v_P_VS;      // Vector from v_P_VS to the light in VS
    float distance = length(L);                 // distance to light
    L /= distance;                              // normalize light vector
    vec3 H = normalize(V + L);                  // Normalized halfvector between eye and light vector
    float att = 1.0 / (distance*distance);      // quadratic light attenuation
    vec3 radiance = u_lightDiffuse[i].rgb * att;// per light radiance

     // cook-torrance brdf
     float NDF = DistributionGGX(N, H, roughness);
     float G   = GeometrySmith(N, V, L, roughness);
     vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

     vec3 kS = F;
     vec3 kD = vec3(1.0) - kS;
     kD *= 1.0 - metallic;

     vec3  nominator   = NDF * G * F;
     float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
     vec3  specular    = nominator / denominator;

     // add to outgoing radiance Lo
     float NdotL = max(dot(N, L), 0.0);

     Lo += (kD*diffuse/PI + specular) * radiance * NdotL;
}
//-----------------------------------------------------------------------------
void main()
{
    vec3 albedo = pow(texture(u_texture0, v_texCoord).rgb, vec3(2.2));
    float metallic = texture(u_texture2, v_texCoord).r;
    float roughness = texture(u_texture3, v_texCoord).r;
    float ao = texture(u_texture4, v_texCoord).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(-v_P_VS); // Vector from p to the viewer
    vec3 R = mat3(v_invMvMatrix) * reflect(-V, N);
    vec3 F0 = vec3(0.04);        // Init Frenel reflection at 90 deg. (0 to N)
    F0 = mix(F0, albedo, metallic);
    
    // Get the reflection from all lights into Lo
    vec3 Lo = vec3(0.0);
    if (u_lightIsOn[0]) {PointLight(0, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    if (u_lightIsOn[1]) {PointLight(1, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    if (u_lightIsOn[2]) {PointLight(2, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    if (u_lightIsOn[3]) {PointLight(3, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    if (u_lightIsOn[4]) {PointLight(4, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    if (u_lightIsOn[5]) {PointLight(5, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    if (u_lightIsOn[6]) {PointLight(6, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    if (u_lightIsOn[7]) {PointLight(7, v_P_VS, N, V, F0, albedo, roughness, metallic, Lo);}
    
    // ambient lighting from IBL
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 irradiance = texture(u_texture5, N).rgb;
    vec3 diffuse    = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(u_texture6, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(u_texture7, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;
    const float gamma = 2.2;
    
    // Exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * u_exposure);
    
    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));
    
    gl_FragColor = vec4(mapped, 1.0);
}
//-----------------------------------------------------------------------------
