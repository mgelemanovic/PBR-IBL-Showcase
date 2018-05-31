#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textureCoords;

// Output data; will be interpolated for each fragment.
varying vec3 varLightDir;
varying vec3 varNormal;
varying vec3 varEyeDir;

out vec2 TexCoords;

// Values that stay constant for the whole mesh.
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPosition;
uniform vec3 camPos;

void main()
{
	vec4 vVertexPosition = model * vec4(vertexPosition_modelspace, 1);
    gl_Position = projection * view * vVertexPosition;
	
	varLightDir = normalize(lightPosition - vec3(vVertexPosition));
	varNormal = normalize(vec3(model * vec4(vertexNormal, 1)));
	varEyeDir = normalize(camPos - vec3(vVertexPosition));

	TexCoords = textureCoords;
}