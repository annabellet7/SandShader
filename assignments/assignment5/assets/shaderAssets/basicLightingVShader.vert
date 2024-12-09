/*
	Author: Annabelle Thompson
*/
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 uLightDirection;
uniform vec3 uViewPos;
uniform vec3 uUp;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 LightDirection;
out vec3 ViewPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);

    //transform normals to world space
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 tangent = normalize(normalMatrix * aTangent);
    Normal = normalMatrix * aNormal;
    vec3 bitangent = normalize(cross(Normal, tangent));

    //make and apply TBN matric
    mat3 TBN = transpose(mat3(tangent, bitangent, Normal));
    LightDirection = TBN * normalize(uLightDirection);
    ViewPos = TBN * uViewPos;
    FragPos = TBN * FragPos;

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}