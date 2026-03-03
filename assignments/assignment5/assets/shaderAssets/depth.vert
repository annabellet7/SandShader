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

out Surface 
{
	vec2 TexCoord;
	vec3 Normal;
}vs_out;

void main()
{
    vs_out.TexCoord = aTexCoord;
    //transform normals to world space
    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    vs_out.Normal = normalMatrix * aNormal;

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
}

//#version 450
//
//layout (location = 1) in vec3 aNormal;
//
//uniform mat4 uModel;
//
//out Surface 
//{
//	vec2 TexCoord;
//	vec3 Normal;
//}vs_out;
//
//vec4 vertices[3] =
//{
//	vec4(-1.0, -1.0, 0.0, 0.0),
//	vec4(3.0, -1.0, 2.0, 0.0),
//	vec4(-1.0, 3.0, 0.0, 2.0)
//};	
//
//void main()
//{
//	mat3 normalMatrix = mat3(transpose(inverse(uModel)));
//    vs_out.Normal = normalMatrix * aNormal;
//
//	gl_Position = vec4(vertices[gl_VertexID].xy,0,1);
//	vs_out.TexCoord = vertices[gl_VertexID].zw;
//}