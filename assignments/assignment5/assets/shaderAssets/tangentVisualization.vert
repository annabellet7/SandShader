#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aTangent;

out VS_OUT {
    vec3 tangent;
} vs_out;

uniform mat4 view;
uniform mat4 model;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.tangent = vec3(vec4(normalMatrix * aTangent, 0.0));
    gl_Position = view * model * vec4(aPos, 1.0); 
}