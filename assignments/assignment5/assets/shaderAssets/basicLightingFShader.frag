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
in vec3 Z;

uniform sampler2D uNormalMap;
uniform sampler2D uShallowX;
uniform sampler2D uSteepX;
uniform sampler2D uShallowZ;
uniform sampler2D uSteepZ;

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
uniform float uGrainSize;

uniform float uSandStrength;
uniform float uRimStrength;
uniform float uRimPower;
uniform float uSteepnessStrength;

void main()
{ 
	vec3 ambient = uAmbientK * uLightColor;

	//grain normals
	vec3 grain = normalize(texture(uNormalMap, TexCoord * uGrainSize).rgb * 2.0 - 1.0); //grain norms

	//Ripple Norms
	vec3 shallowX = normalize(texture(uShallowX, TexCoord).rgb * 2.0 - 1.0);
	vec3 steepX = normalize(texture(uSteepX, TexCoord).rgb * 2.0 - 1.0);
	float steepnessX = dot(Up, Normal);
	steepnessX = pow(steepnessX, 2.0);
	vec3 rippleX = normalize(mix(shallowX, steepX, steepnessX));

	vec3 shallowZ = normalize(texture(uShallowZ, TexCoord).rgb * 2.0 - 1.0);
	vec3 steepZ = normalize(texture(uSteepZ, TexCoord).rgb * 2.0 - 1.0);
	float steepnessZ = dot(Z, Normal);
	steepnessZ = pow(steepnessZ, 2.0);
	vec3 rippleZ = normalize(mix(shallowZ, steepZ, steepnessZ));

	vec3 ripple = normalize(mix(rippleX, rippleZ, 0.5));

	//rotating ripples over grain normal offset
	float phi = atan(grain.z, grain.x);
	float theta = atan(grain.x, grain.y);
	mat3 rotateZ= mat3(1);
	rotateZ[0][0] = cos(phi);
	rotateZ[1][0] = -sin(phi);
	rotateZ[0][1] = sin(phi);
	rotateZ[1][1] = cos(phi);

	mat3 rotateY = mat3(1);
	rotateY[0][0] = cos(theta);
	rotateY[2][0] = sin(theta);
	rotateY[0][2] = -sin(theta);
	rotateY[2][2] = cos(theta);

	ripple = ripple * -rotateZ;
	ripple = ripple * rotateY;
	ripple = ripple * rotateZ;
	
	//rotating grain over ripple normal offset
	phi = atan(ripple.z, ripple.x);
	theta = atan(ripple.x, ripple.y);
	rotateZ= mat3(1);
	rotateZ[0][0] = cos(phi);
	rotateZ[1][0] = -sin(phi);
	rotateZ[0][1] = sin(phi);
	rotateZ[1][1] = cos(phi);

	rotateY = mat3(1);
	rotateY[0][0] = cos(theta);
	rotateY[2][0] = sin(theta);
	rotateY[0][2] = -sin(theta);
	rotateY[2][2] = cos(theta);

	grain = grain * -rotateZ;
	grain = grain * rotateY;
	grain = grain * rotateZ;

	vec3 norm = grain;

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