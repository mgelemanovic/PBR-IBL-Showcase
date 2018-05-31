#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float exposure;

void main()
{    
	vec3 envColor = texture(skybox, TexCoords).rgb;
    
	// HDR tonemap and gamma correct
	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0/exposure)); 
    
	FragColor = vec4(envColor, 1.0);
}