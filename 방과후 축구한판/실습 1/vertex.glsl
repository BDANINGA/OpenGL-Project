#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 vTexCoord;
 
out vec4 FragPos;
out vec3 Normal;
out vec3 ObjectColor;
out vec2 TexCoord;

uniform mat4 modelTransform;
uniform mat4 viewTransform;
uniform mat4 projectionTransform;

void main()
{
gl_Position = projectionTransform * viewTransform * modelTransform * vec4(vPos, 1.0);
 
FragPos = modelTransform * vec4(vPos, 1.0);
 
Normal = mat3(transpose(inverse(modelTransform))) * vNormal;

ObjectColor = vColor;

TexCoord = vTexCoord;
}