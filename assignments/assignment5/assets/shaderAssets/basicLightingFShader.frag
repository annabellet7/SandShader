#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 ColorShade;
in vec3 ColorSun;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;
in vec3 LightDirection;
in vec3 ViewPos;

uniform sampler2D normalMap;
uniform sampler2D texture1;

uniform float uTime;

uniform vec3 uLightColor;
uniform vec3 uLightDirection;
uniform vec3 uViewPos;

uniform float uAmbientK;
uniform float uDiffuseK;
uniform float uOceanSpecularK;
uniform float uOceanShininess;
uniform float uGrainSpecularK;
uniform float uGrainShininess;

uniform float uSandStrength;
uniform float uRimStrength;
uniform float uRimPower;

void main()
{ 
	vec3 ambient = uAmbientK * uLightColor;

	vec3 norm = normalize(texture(normalMap, TexCoord).rgb); //grain norms
	norm = normalize((norm * 2.0 - 1.0) * 2.0);


	//Ripple Norms

	vec3 viewDir =  normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(-LightDirection);

	//diffuse shader (modified lambert)
	float yNorm = norm.y * 0.3; //squishes shadows vertically 
	float diff = clamp(4 * dot(vec3(norm.x, yNorm, norm.z), lightDir), 0.0, 1.0); //causes the normal lambert shader to have a less gradual change/change faster 
	float rim = 1.0 - clamp(dot(vec3(norm.x, yNorm, norm.z), viewDir), 0.0, 1.0); //rim lighting 
	rim = clamp(pow(rim, uRimPower) * uRimStrength, 0.0, 1.0);
	vec3 color = mix(ColorShade, ColorSun, diff) + rim; //interpolates between shade and sun as zero and one values
	vec3 diffuse = diff * uLightColor * uDiffuseK * color;

	//"ocean" specular (Blinn-Phong reflectance)
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), uOceanShininess);
	vec3  oceanSpecular = uLightColor * spec * uOceanSpecularK;

	spec = pow(max(dot(norm, viewDir), 0.0), uGrainShininess);
	vec3  grainSpecular = uLightColor * spec * uGrainSpecularK;

	vec3 result = (ambient + diffuse + oceanSpecular + grainSpecular) * color;
	FragColor = vec4(result, 1.0);
}