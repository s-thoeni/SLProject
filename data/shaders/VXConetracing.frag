//#############################################################################
//  File:      VXConetracing.frag
//  Purpose:   Calculated direct illumination using Blinn-Phong
//             and indirect illumination using voxel cone tracing
//  Author:    Stefan Thöni
//  Date:      September 2018
//  Copyright: Stefan Thöni
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################
#version 430 core
in vec3 o_P_WS;
in vec3 o_N_WS;

// general settings:
uniform float   s_diffuseConeAngle;
uniform bool    s_directEnabled;
uniform bool    s_diffuseEnabled;
uniform bool    s_specEnabled;
uniform bool    s_shadowsEnabled;

// camera settings:
uniform vec3 u_EyePos;

// Material & Light settings:
uniform int    u_numLightsUsed;     //!< NO. of lights used light arrays
uniform bool   u_lightIsOn[8];      //!< flag if light is on
uniform vec4   u_lightPosWS[8];     //!< position of light in world space
uniform vec4   u_lightAmbient[8];   //!< ambient light intensity (Ia)
uniform vec4   u_lightDiffuse[8];   //!< diffuse light intensity (Id)
uniform vec4   u_lightSpecular[8];  //!< specular light intensity (Is)
uniform vec3   u_lightSpotDirWS[8]; //!< spot direction in view space
uniform float  u_lightSpotCutoff[8];//!< spot cutoff angle 1-180 degrees
uniform float  u_lightSpotCosCut[8];//!< cosine of spot cutoff angle
uniform float  u_lightSpotExp[8];   //!< spot exponent
uniform vec3   u_lightAtt[8];       //!< attenuation (const,linear,quadr.)
uniform bool   u_lightDoAtt[8];     //!< flag if att. must be calc.
uniform vec4   u_globalAmbient;     //!< Global ambient scene color

uniform vec4   u_matAmbient;        //!< ambient color reflection coefficient (ka)
uniform vec4   u_matDiffuse;        //!< diffuse color reflection coefficient (kd)
uniform vec4   u_matSpecular;       //!< specular color reflection coefficient (ks)
uniform vec4   u_matEmissive;       //!< emissive color for selfshining materials
uniform float  u_matShininess;      //!< shininess exponent
uniform float  u_matKr;             //!< reflection factor (kr)

uniform sampler3D texture3D; // Voxelization texture.

out vec4 color;

// Returns true if the point p is inside the unity cube. 
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

float shadowCone(vec3 from, vec3 dir, float lightDistance){
  // offset a little...  vec3 N = ;
  from += normalize(o_N_WS) * 0.05f;
  
  float res = 0;

  float dist = 3 * 0.015625;
  // STOP 4 voxels before reaching the light source
  const float STOP = lightDistance - 4 * 0.015625;

  while(dist < STOP && res < 1){	
    vec3 c = from + dist * dir;
    if(!isInsideCube(c, 0)) break;
    c = 0.5*c + 0.5;
    float l = pow(dist, 2); // Experimenting with inverse square falloff for shadows.
    float s1 = 0.062 * textureLod(texture3D, c, 1 + 0.75 * l).a;
    float s2 = 0.135 * textureLod(texture3D, c, 4.5 * l).a;
    float s = s1 + s2;
    res += (1 - res) * s;
    dist += 0.9 * 0.015625 * (1 + 0.05 * l);
  }
  return 1 - pow(smoothstep(0, 1, res * 1.4), 1.0 / 1.4);//res;
}


//-----------------------------------------------------------------------------
void DirectLight(in    int  i,   // Light number
                 in    vec3 N,   // Normalized normal 
                 in    vec3 E,   // Normalized vector 
                 inout vec4 Id,  // Diffuse light intesity
                 inout vec4 Is)  // Specular light intesity
{  
  // We use the spot light direction as the light direction vector
  vec3 L = normalize(-u_lightSpotDirWS[i].xyz);

  // Half vector H between L and E
  vec3 H = normalize(L+E);
   
  // Calculate diffuse & specular factors
  float diffFactor = max(dot(N,L), 0.0);
  float specFactor = 0.0;
  if (diffFactor!=0.0) 
    specFactor = pow(max(dot(N,H), 0.0), u_matShininess);
   
  // accumulate directional light intesities w/o attenuation
  Id += u_lightDiffuse[i] * diffFactor;
  Is += u_lightSpecular[i] * specFactor;

    
}
//-----------------------------------------------------------------------------
void PointLight (in    int  i,      // Light number
                 in    vec3 P_WS,   // Point of illumination
                 in    vec3 N,      // Normalized normal 
                 in    vec3 E,      // Normalized eye vector
                 inout vec4 Id,     // Diffuse light intensity
                 inout vec4 Is)     // Specular light intensity
{
  // Vector from v_P_VS to the light in VS
  vec3 L = u_lightPosWS[i].xyz - P_WS;
  float lengthL = length(L);
  L = L / lengthL;

  float lightAngle = dot(N,L);

  // Shadows: 
  float shadow = 1;
  if(s_shadowsEnabled){
    shadow = shadowCone(P_WS, L, length(L));
  }

    
  // Normalized halfvector between the eye and the light vector
  vec3 H = normalize(E + L);

  // Calculate diffuse
  float diffFactor = min(shadow, max(lightAngle, 0.0));
  //& specular factors

  float specFactor = 0.0;
  if (diffFactor!=0.0) 
    specFactor = pow(max(dot(N,H), 0.0), u_matShininess);
   
  // Calculate attenuation
  float att = 1.0;
  if (u_lightDoAtt[i])
    {   vec3 att_dist;
      att_dist.x = 1.0;
      att_dist.z = lengthL;
      att_dist.y = lengthL * lengthL;
      att = 1.0 / dot(att_dist, u_lightAtt[i]);
    }

  // Accumulate light intesities   
  Id += att * u_lightDiffuse[i] * diffFactor;
  Is += att * u_lightSpecular[i] * specFactor;
}

vec4 direct(){
  vec4 Id, Is;        // Accumulated light intensities at v_P_VS
   
  Id = vec4(0.0);         // Diffuse light intesity
  Is = vec4(0.0);         // Specular light intesity
   
  vec3 N = normalize(o_N_WS);  // A varying normal has not anymore unit length
  vec3 E = normalize(u_EyePos - o_P_WS); // Vector from p to the eye

  if (u_lightIsOn[0]) {if (u_lightPosWS[0].w == 0.0) DirectLight(0, N, E, Id, Is); else PointLight(0, o_P_WS, N, E, Id, Is);}
  if (u_lightIsOn[1]) {if (u_lightPosWS[1].w == 0.0) DirectLight(1, N, E, Id, Is); else PointLight(1, o_P_WS, N, E, Id, Is);}
  if (u_lightIsOn[2]) {if (u_lightPosWS[2].w == 0.0) DirectLight(2, N, E, Id, Is); else PointLight(2, o_P_WS, N, E, Id, Is);}
  if (u_lightIsOn[3]) {if (u_lightPosWS[3].w == 0.0) DirectLight(3, N, E, Id, Is); else PointLight(3, o_P_WS, N, E, Id, Is);}
  if (u_lightIsOn[4]) {if (u_lightPosWS[4].w == 0.0) DirectLight(4, N, E, Id, Is); else PointLight(4, o_P_WS, N, E, Id, Is);}
  if (u_lightIsOn[5]) {if (u_lightPosWS[5].w == 0.0) DirectLight(5, N, E, Id, Is); else PointLight(5, o_P_WS, N, E, Id, Is);}
  if (u_lightIsOn[6]) {if (u_lightPosWS[6].w == 0.0) DirectLight(6, N, E, Id, Is); else PointLight(6, o_P_WS, N, E, Id, Is);}
  if (u_lightIsOn[7]) {if (u_lightPosWS[7].w == 0.0) DirectLight(7, N, E, Id, Is); else PointLight(7, o_P_WS, N, E, Id, Is);}

  return u_matEmissive + 
    Id * u_matDiffuse +
    Is * u_matSpecular;

}

vec4 diffuseConeTrace(vec3 from, vec3 dir, float theta){
  dir = normalize(dir);

  vec4 res = vec4(0.0f);

  // initial distance is a backwards calculation to get a cone diameter of 6 voxels:
  float dist = 3 * 0.015625 / tan(theta);

  while(dist < 1.732050 && res.a < 1) {
    // calculate voxel coordinate:
    vec3 coordinate = from + dist * dir;
    coordinate = 0.5f * coordinate + vec3(0.5f);
    //    if(!isInsideCube(coordinate, 0.0)) break;
    
    float coneDiameter = 2 * tan(theta) * dist;
    float voxelDia = coneDiameter / 0.015625;
    float mipLevel = log2(voxelDia);
    
    vec4 voxel = textureLod(texture3D, coordinate, min(6, mipLevel));
    
    res += voxel * pow(1 - voxel.a, 2);
    dist *= 2.3; // * 2 for sampling without gaps. +0.3 for underestimate 
  }
  return res;
}

vec4 specularConeTrace(vec3 from, vec3 dir, float theta){
  dir = normalize(dir);

  vec4 res = vec4(0.0f);

  float dist = 8 * 0.015625 ; /// tan(theta);

  while(dist < 1.732050 && res.a < 1) {
    // calculate voxel coordinate:
    vec3 coordinate = from + dist * dir;
    coordinate = 0.5f * coordinate + vec3(0.5f);
    //    if(!isInsideCube(coordinate, 0.0)) break;
    
    float coneDiameter = 2 * tan(theta) * dist;
    float voxelDia = coneDiameter / 0.015625;
    float mipLevel = log2(voxelDia);
    
    //vec4 voxel = textureLod(texture3D, coordinate, min(6, mipLevel));
    vec4 voxel = textureLod(texture3D, coordinate, 1);
    
    res += voxel ; //* pow(1 - voxel.a, 2);
    dist += 0.05; // * 2 for sampling without gaps. +0.3 for underestimate 
  }
  return res;
}

vec4 indirectDiffuse(){
  vec4 res = vec4(0.0);
  vec3 N = normalize(o_N_WS);

  // a vector orthogonal to N:
  vec3 ortho1 = normalize(vec3(-N.y, N.x, 0.0));

  // a vector orthogonal to N and ortho1
  vec3 ortho2 = normalize(cross(N, ortho1));

  // offset 4 voxels along the normal
  const vec3 OFFSET = N * 4 * (1/64);
  const vec3 from = o_P_WS + OFFSET;
  res += diffuseConeTrace(from , N, s_diffuseConeAngle);

  // Trace 4 side cones.

  const vec3 s1 = mix(N, ortho1, 0.5);
  const vec3 s2 = mix(N, -ortho1, 0.5);
  const vec3 s3 = mix(N, ortho2, 0.5);
  const vec3 s4 = mix(N, -ortho2, 0.5);

  float cos45 = 0.52;
  res += cos45 * diffuseConeTrace(from, s1, s_diffuseConeAngle);
  res += cos45 * diffuseConeTrace(from, s2, s_diffuseConeAngle);
  res += cos45 * diffuseConeTrace(from, s3, s_diffuseConeAngle);
  res += cos45 * diffuseConeTrace(from, s4, s_diffuseConeAngle);

  const vec3 corner1 = 0.5f * (ortho1 + ortho2);
  const vec3 corner2 = 0.5f * (ortho1 - ortho2);

  const vec3 c1 = mix(N, corner1, 0.5);
  const vec3 c2 = mix(N, -corner1, 0.5);
  const vec3 c3 = mix(N, corner2, 0.5);
  const vec3 c4 = mix(N, -corner2, 0.5);
    
  res += cos45 * diffuseConeTrace(from, c1, s_diffuseConeAngle);
  res += cos45 * diffuseConeTrace(from, c2, s_diffuseConeAngle);
  res += cos45 * diffuseConeTrace(from, c3, s_diffuseConeAngle);
  res += cos45 * diffuseConeTrace(from, c4, s_diffuseConeAngle);

  return res * u_matDiffuse;
}

vec4 indirectSpecularLight(){
  vec3 N = normalize(o_N_WS);
  vec3 E = normalize(u_EyePos - o_P_WS);
  vec3 R = normalize(reflect(-E, N));

  const vec3 OFFSET = N * 4 * (1/64);
  const vec3 from = o_P_WS + OFFSET;

  float coneAng = 0.6; // 20deg total cone
  
  vec4 speck = specularConeTrace(from, R, coneAng);
  return u_matKr * u_matSpecular * speck;
}

void main(){
  color = vec4(0, 0, 0, 1);  

  if(s_directEnabled){
    color += direct();
  }

  if(s_diffuseEnabled){
    color += indirectDiffuse();
  }

  if(s_specEnabled){
    color += indirectSpecularLight();
  }

  color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}
