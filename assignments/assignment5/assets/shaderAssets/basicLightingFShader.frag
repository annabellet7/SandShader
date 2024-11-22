#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 ColorShade;
in vec3 ColorSun;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D normalMap;
uniform sampler2D texture1;

uniform float uTime;

uniform vec3 uLightColor;
uniform vec3 uLightDirection;
uniform vec3 uViewPos;

uniform float uAmbientK;
uniform float uDiffuseK;
uniform float uSpecularK;
uniform int uShininess;

uniform float uSandStrength;
uniform float uRimStrength;
uniform float uRimPower;

void main()
{ 
	vec3 ambient = uAmbientK * uLightColor;

	vec3 norm = normalize(texture(normalMap, TexCoord).rgb); //grain norms
	norm = normalize((norm * 2.0 - 1.0));
	//norm = Normal;
	norm = normalize(mix(Normal, norm, uSandStrength));

	//Ripple Norms

	//diffuse shader (modified lambert)
	vec3 viewDir = normalize(uViewPos - FragPos);
	vec3 lightDir = normalize(uLightDirection);

	float yNorm = norm.y * 0.3; //squishes shadows vertically 
	float diff = clamp(4 * dot(vec3(norm.x, yNorm, norm.z), lightDir), 0.0, 1.0); //causes the normal lambert shader to have a less gradual change/change faster 
	float rim = 1.0 - clamp(dot(vec3(norm.x, yNorm, norm.z), viewDir), 0.0, 1.0); //rim lighting 
	rim = clamp(pow(rim, uRimPower) * uRimStrength, 0.0, 1.0);
	vec3 color = mix(ColorShade, ColorSun, diff) + rim; //interpolates between shade and sun as zero and one values
	vec3 diffuse = diff * uLightColor * uDiffuseK * color;

	//"ocean" specular (Blinn-Phong reflectance)
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), uShininess);
	vec3  specular = uLightColor * spec * uSpecularK;

	vec3 result = (ambient + diffuse + specular) * color;
	FragColor = vec4(result, 1.0);
}