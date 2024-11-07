#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture2;
uniform sampler2D texture3;

uniform float uTime;

uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

uniform float uAmbientK;
uniform float uDiffuseK;
uniform float uSpecularK;
uniform int uShininess;

void main()
{ 
	vec3 ambient = uAmbientK * uLightColor;

	vec4 cubeTexture = mix(texture(texture2, TexCoord), texture(texture3, TexCoord), 0.4);
	vec3 norm = normalize(Normal);

	vec3 lightDir = normalize(uLightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * uLightColor * uDiffuseK;

	vec3 viewDir = normalize(uViewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), uShininess);
	vec3  specular = uLightColor * spec * uSpecularK;

	vec3 result = (ambient + diffuse + specular) * vec3(cubeTexture.x, cubeTexture.y, cubeTexture.z);
	FragColor = vec4(result, 1.0);
}