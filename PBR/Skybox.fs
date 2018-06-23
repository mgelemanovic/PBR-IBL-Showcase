#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float gamma;

void main()
{    
	vec3 envColor = texture(skybox, TexCoords).rgb;
    
	// HDR tonemap and gamma correct
	envColor = envColor / (envColor + vec3(1.0));
	envColor = pow(envColor, vec3(1.0/gamma)); 
    
	FragColor = vec4(envColor, 1.0);
}