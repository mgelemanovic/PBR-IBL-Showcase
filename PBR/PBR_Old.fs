#version 330 core

precision highp float; //set default precision in glsl es 2.0

varying vec3 varLightDir;
varying vec3 varNormal;
varying vec3 varEyeDir;

in vec2 TexCoords;

uniform int emissive_texture;
uniform sampler2D texture_diffuse1;
uniform vec3 difuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform float alpha;

void main()
{
	if (emissive_texture == 1) {
		gl_FragColor = texture(texture_diffuse1, TexCoords);
		return;
	}

    // set important material values
    float roughnessValue = 0.2; // 0 : smooth, 1: rough
    float F0 = 0.8; // fresnel reflectance at normal incidence
    float k = 0.5; // fraction of diffuse reflection (specular reflection = 1 - k)
    vec3 lightColor = vec3(0.9, 0.9, 0.9);
    
    // interpolating normals will change the length of the normal, so renormalize the normal.
    vec3 normal = normalize(varNormal);
	vec3 lightDirection = normalize(varLightDir);
    
    // do the lighting calculation for each fragment.
    float NdotL = max(dot(normal, lightDirection), 0.0);
    
    float specular = 0.0;
    if(NdotL > 0.0) {
        vec3 eyeDir = normalize(varEyeDir);

        // calculate intermediary values
        vec3 halfVector = normalize(lightDirection + eyeDir);
        float NdotH = max(dot(normal, halfVector), 0.0); 
        float NdotV = max(dot(normal, eyeDir), 0.0); // note: this could also be NdotL, which is the same value
        float VdotH = max(dot(eyeDir, halfVector), 0.0);
        float mSquared = roughnessValue * roughnessValue;
        
        // geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));
     
        // roughness (or: microfacet distribution function)
        // beckmann distribution function
        float r1 = 1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
        float roughness = r1 * exp(r2);
        
        // fresnel
        // Schlick approximation
        float fresnel = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
        
        specular = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
    }
    
    vec3 finalValue = lightColor * NdotL * (k + specular * (1.0 - k));
	gl_FragColor = texture(texture_diffuse1, TexCoords) * vec4(difuseColor, alpha) * vec4(finalValue, 1.0) + vec4(ambientColor, 0.0);
}