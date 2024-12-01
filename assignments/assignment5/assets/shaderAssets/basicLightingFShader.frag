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
in vec3 Up;

uniform sampler2D uNormalMap;
uniform sampler2D uShallowX;
uniform sampler2D uSteepX;

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
uniform float uSteepnessStrength;

void main()
{ 
	vec3 ambient = uAmbientK * uLightColor;

	vec3 norm = normalize(texture(uNormalMap, TexCoord).rgb); //grain norms
	norm = normalize((norm * 2.0 - 1.0) * 2.0);

	//Ripple Norms
//	vec3 shallowX = normalize(texture(uShallowX, TexCoord).rgb);
//	vec3 steepX = normalize(texture(uSteepX, TexCoord).rgb);
//	float steepness = dot(Up, Normal);
//	steepness = pow(steepness, uSteepnessStrength);
//	vec3 rippleX = normalize(mix(shallowX, steepX, steepness));
	

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