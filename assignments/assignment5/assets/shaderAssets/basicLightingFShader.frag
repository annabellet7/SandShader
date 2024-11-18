#version 330 core
out vec4 FragColor;

in vec3 ColorShade;
in vec3 ColorSun;
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

	//vec4 cubeTexture = mix(texture(texture2, TexCoord), texture(texture3, TexCoord), 0.4);
	vec3 norm = normalize(Normal);

	//Ripple Norms

	//Grain Norms

	//diffuse shader
	vec3 lightDir = normalize(uLightPos - FragPos);
	//shadows be more vertical?? - ask about
	float yNorm = norm.y * 0.3;
	//causes the normal lambert shader to have a less gradual change/change faster 
	float diff = clamp(4 * dot(vec3(norm.x, yNorm, norm.z), lightDir), 0.0, 1.0); 
	vec3 diffuse = diff * uLightColor * uDiffuseK;

	//interpolates between shade and sun as zero and one values
	vec3 color = mix(ColorShade, ColorSun, diff);

	vec3 viewDir = normalize(uViewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), uShininess);
	vec3  specular = uLightColor * spec * uSpecularK;

	vec3 result = (ambient + diffuse + specular) * color;
	FragColor = vec4(result, 1.0);
}