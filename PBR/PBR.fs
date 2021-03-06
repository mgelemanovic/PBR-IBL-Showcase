#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform int renderingMode;
#define INIT_RENDERING_MODES int currentMode = 1 
#define OUTPUT_VECTOR2(name, value) if (renderingMode == currentMode) { FragColor = vec4(value, 0.0, 1.0); return; } currentMode++; vec2 name = value
#define OUTPUT_VECTOR3(name, value) if (renderingMode == currentMode) { FragColor = vec4(value, 1.0); return; } currentMode++; vec3 name = value
#define OUTPUT_FLOAT(name, value) if (renderingMode == currentMode) { FragColor = vec4(value, value, value, 1.0); return; } currentMode++; float name = value

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// lights
uniform vec3 lightPositions[16];
uniform vec3 lightColors[16];
uniform int lightCount;

uniform vec3 camPos;

uniform float gamma;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
vec3 transformNormal(vec3 normal)
{
    vec3 tangentNormal = normal * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   
// ----------------------------------------------------------------------------
void main()
{
	INIT_RENDERING_MODES;
	// get albedo value 
	OUTPUT_VECTOR3(albedo, texture(albedoMap, TexCoords).rgb);
	albedo = pow(albedo, vec3(2.2));
	// get normal value
	OUTPUT_VECTOR3(N, texture(normalMap, TexCoords).xyz);
    N = transformNormal(N);
	// get metallic value
	OUTPUT_FLOAT(metallic, texture(metallicMap, TexCoords).r);
	// get roughness value
	OUTPUT_FLOAT(roughness, texture(roughnessMap, TexCoords).r);
	// get AO value
	OUTPUT_FLOAT(ao, texture(aoMap, TexCoords).r);

    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightCount; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
         // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }
    OUTPUT_VECTOR3(totalLight, Lo);
    
    // ambient lighting (we now use IBL as the ambient term)
	OUTPUT_VECTOR3(F, fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness));
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    OUTPUT_VECTOR3(irradiance, texture(irradianceMap, N).rgb);
    OUTPUT_VECTOR3(diffuse, irradiance * albedo);
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    OUTPUT_VECTOR3(prefilteredColor, textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb);
    OUTPUT_VECTOR2(brdf, texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg);
    OUTPUT_VECTOR3(specular, prefilteredColor * (F * brdf.x + brdf.y));

    OUTPUT_VECTOR3(ambient, (kD * diffuse + specular) * ao);
    vec3 color = ambient + totalLight;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/gamma));

    FragColor = vec4(color, 1.0);
}