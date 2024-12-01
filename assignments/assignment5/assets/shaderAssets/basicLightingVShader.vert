#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform float uTime;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 uLightDirection;
uniform vec3 uViewPos;
uniform vec3 uUp;

uniform float uGrainSize;

out vec3 ColorShade;
out vec3 ColorSun;
out vec2 TexCoord;
out vec3 FragPos;
out vec3 LightDirection;
out vec3 ViewPos;
out vec3 Normal;
out vec3 Up;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = vec2(aTexCoord.x, aTexCoord.y) * uGrainSize;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 tangent = normalize(normalMatrix * aTangent);
    Normal = normalMatrix * aNormal;
    tangent = normalize(tangent - dot(tangent, Normal) * Normal); 
    vec3 bitangent = cross(Normal, tangent);

    mat3 TBN = transpose(mat3(tangent, bitangent, Normal));
    LightDirection = TBN * uLightDirection;
    ViewPos = TBN * uViewPos;
    FragPos = TBN * FragPos;

    Up = TBN * uUp;

    ColorShade = vec3(0.851, 0.631, 0.565);
    ColorSun = vec3(0.925, 0.796, 0.718);

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}