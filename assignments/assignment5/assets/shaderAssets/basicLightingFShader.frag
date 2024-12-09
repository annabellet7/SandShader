/*
	Author: Annabelle Thompson
*/
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;
in vec3 LightDirection;
in vec3 ViewPos;

uniform sampler2D uNormalMap;
uniform sampler2D uShallowX;
uniform sampler2D uSteepX;
uniform sampler2D uShallowZ;
uniform sampler2D uSteepZ;

uniform vec3 uLightColor;
uniform vec3 uColorShade;
uniform vec3 uColorSun;
uniform vec3 uSpecColor;

uniform vec3 uLightDirection;
uniform vec3 uViewPos;

uniform float uAmbientK;
uniform float uDiffuseK;
uniform float uOceanSpecularK;
uniform float uOceanShininess;
uniform float uGrainSpecularK;
uniform float uGrainShininess;
uniform float uGrainSize;

uniform float uRimStrength;
uniform float uRimPower;

void main()
{ 
	vec3 ambient = uAmbientK * uLightColor;

	//grain normals
	vec3 grain = normalize(texture(uNormalMap, TexCoord * uGrainSize).rgb * 2.0 - 1.0); 
	vec3 norm = grain;

	//Ripple Norms
	vec3 shallowX = normalize(texture(uShallowX, TexCoord).rgb * 2.0 - 1.0);
	vec3 steepX = normalize(texture(uSteepX, TexCoord).rgb * 2.0 - 1.0);
	float yAlightment = dot(vec3(0, 1, 0), Normal);
	yAlightment = pow(yAlightment, 2.0);
	vec3 rippleX = normalize(mix(steepX, shallowX, yAlightment));

	vec3 shallowZ = normalize(texture(uShallowZ, TexCoord).rgb * 2.0 - 1.0);
	vec3 steepZ = normalize(texture(uSteepZ, TexCoord).rgb * 2.0 - 1.0);
	vec3 rippleZ = normalize(mix(steepZ, shallowZ, yAlightment));

	float xAlignment = abs(dot(vec3(1, 0, 0), Normal)); 
	vec3 ripple = normalize(mix(rippleZ, rippleX, xAlignment));

	//combine normal maps
	mat3 basis = mat3
	(ripple.z, ripple.y, -ripple.x,
	ripple.x, ripple.z, -ripple.y,
	ripple.x, ripple.y, ripple.z);
	norm = normalize(grain.x * basis[0] + grain.y * basis[1] + grain.z * basis[2]); //rotates grain over ripples

	vec3 viewDir =  normalize(ViewPos - FragPos);
	vec3 lightDir = normalize(-LightDirection);

	//diffuse shader (modified lambert)
	float yNorm = norm.y * 0.3; //squishes shadows vertically 
	float diff = clamp(4 * dot(vec3(norm.x, yNorm, norm.z), lightDir), 0.0, 1.0); //causes the normal lambert shader to have a less gradual change/change faster 
	float rim = 1.0 - clamp(dot(vec3(norm.x, yNorm, norm.z), viewDir), 0.0, 1.0); //rim lighting 
	rim = clamp(pow(rim, uRimPower) * uRimStrength, 0.0, 1.0);
	vec3 color = mix(uColorShade, uColorSun, diff) + rim; //interpolates between shade and sun as zero and one values
	vec3 diffuse = diff * uLightColor * uDiffuseK * color;

	//"ocean" specular (Blinn-Phong reflectance)
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(grain, halfwayDir), 0.0), uOceanShininess);
	vec3  oceanSpecular = uSpecColor * spec * uOceanSpecularK;

	//grain specular, shimmers when the camera moves
	spec = pow(max(dot(norm, viewDir), 0.0), uGrainShininess);
	vec3  grainSpecular = uSpecColor * spec * uGrainSpecularK;

	vec3 result = (ambient + diffuse + oceanSpecular + grainSpecular) * color;
	FragColor = vec4(result, 1.0);
}