#version 330 core
in vec4 FragPos;
in vec3 Normal;
in vec3 ObjectColor;
out vec4 FragColor;

uniform vec3 lightPos;  // Position of the light source
uniform vec3 viewPos;   // Position of the camera/viewer
uniform vec3 lightColor;
uniform float ambientStrength; // Ambient 강도 조절

void main()
{
vec3 ambient = ambientStrength * lightColor;

vec3 normalVector = normalize (Normal);
vec3 lightDir = normalize (lightPos - FragPos.xyz);
float diffuseLight = max (dot (normalVector, lightDir), 0.0);
vec3 diffuse = diffuseLight * lightColor;

float shininess = 32.0;
vec3 viewDir = normalize (viewPos - FragPos.xyz);
vec3 reflectDir = reflect(-lightDir, normalVector);
float specularLight = max (dot (viewDir, reflectDir), 0.0);
specularLight = pow(specularLight, shininess);
vec3 specular = specularLight * lightColor;

vec3 result = (ambient + diffuse + specular) * ObjectColor;

FragColor = vec4 (result, 1.0);
}