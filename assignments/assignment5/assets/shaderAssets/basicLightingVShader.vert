/*
	Author: Annabelle Thompson
*/
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform vec3 uLightDirection;
uniform vec3 uViewPos;
uniform vec3 uUp;

out Surface 
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
    vec3 ViewPos;
    vec3 LightDirection;
}vs_out;

void main()
{
    vs_out.FragPos = vec3(uModel * vec4(aPos, 1.0));
    vs_out.TexCoord = aTexCoord;

    //transform normals to world space
    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    vec3 tangent = normalize(normalMatrix * aTangent);
    vs_out.Normal = normalMatrix * aNormal;
    vec3 bitangent = normalize(cross(vs_out.Normal, tangent));

    //make and apply TBN matric
    mat3 TBN = transpose(mat3(tangent, bitangent, vs_out.Normal));
    vs_out.LightDirection = TBN * normalize(uLightDirection);
    vs_out.ViewPos = TBN * uViewPos;
    vs_out.FragPos = TBN * vs_out.FragPos;

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}