/*
	Author: Annabelle Thompson
*/
#version 330 core
out vec4 FragColor;

in Surface 
{
	vec3 FragPos;
	vec2 TexCoord;
	vec3 Normal;
    vec3 ViewPos;
    vec3 LightDirection;
}fs_in;

uniform sampler2D uNormalMap;
uniform sampler2D uShallowX;
uniform sampler2D uSteepX;
uniform sampler2D uShallowZ;
uniform sampler2D uSteepZ;

uniform sampler2D uHeightMap;
uniform sampler2D uShallowXH;
uniform sampler2D uSteepXH;
uniform sampler2D uShallowZH;
uniform sampler2D uSteepZH;

uniform float uHeightScale = 0.1;

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

float getDepth(vec2 texCoords);

float getDepth(vec2 texCoords)
{
	vec3 grain = texture(uHeightMap, texCoords * uGrainSize).rgb; 
	vec3 depth = grain;

	//Ripple Norms
	vec3 shallowX = texture(uShallowXH, texCoords).rgb;
	vec3 steepX = texture(uSteepXH, texCoords).rgb;
	float yAlightment = dot(vec3(0, 1, 0), fs_in.Normal);
	yAlightment = pow(yAlightment, 2.0);
	vec3 rippleX = mix(steepX, shallowX, yAlightment);

	vec3 shallowZ = texture(uShallowZH, texCoords).rgb;
	vec3 steepZ = texture(uSteepZH, texCoords).rgb;
	vec3 rippleZ = mix(steepZ, shallowZ, yAlightment);

	float xAlignment = abs(dot(vec3(1, 0, 0), fs_in.Normal)); 
	vec3 ripple = mix(rippleZ, rippleX, xAlignment);

	//combine normal maps
	mat3 basis = mat3
	(ripple.z, ripple.y, -ripple.x,
	ripple.x, ripple.z, -ripple.y,
	ripple.x, ripple.y, ripple.z);
	depth = grain.x * basis[0] + grain.y * basis[1] + grain.z * basis[2]; //rotates grain over ripples
	return depth.r;
}

vec2 parallaxMapping(vec3 viewDir);

vec2 parallaxMapping(vec3 viewDir)
{
	const int minLayers = 8;
    const int maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
	numLayers = 32;
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * uHeightScale / viewDir.z; 
    vec2 deltaTexCoords = P / numLayers;
	int i = 0;

    vec2  currentTexCoords = fs_in.TexCoord;
    float currentDepth = getDepth(currentTexCoords);

//	for (int i = 0; i < 100; i++)
//	{
//		if (currentLayerDepth < currentDepth)
//		{
//			break;
//		}
//
//		//lastDepth = currentDepth;
//		//lastTexCoords = currentTexCoords;
//        // shift texture coordinates along direction of P
//        currentTexCoords -= deltaTexCoords;
//        // get depthmap value at current texture coordinates
//        currentDepth = getDepth(currentTexCoords);
//        // get depth of next layer
//        currentLayerDepth += layerDepth;  
//	}

	while (currentLayerDepth < currentDepth)
	{
		currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepth = getDepth(currentTexCoords);
        // get depth of next layer
        currentLayerDepth += layerDepth;  
	}

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepth - currentLayerDepth;
    float beforeDepth = getDepth(prevTexCoords) - currentLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{ 
	vec3 ambient = uAmbientK * uLightColor;

	vec3 viewDir =  normalize(fs_in.ViewPos - fs_in.FragPos);
	vec3 lightDir = normalize(-fs_in.LightDirection);

	//vec2 newCoords = parallaxMapping(viewDir);
	vec2 newCoords = fs_in.TexCoord;
	if(newCoords.x > 1.0 || newCoords.y > 1.0 || newCoords.x < 0.0 || newCoords.y < 0.0)
        discard;
	
//	
//	//parallaxMapping
//	vec2 newTexCoord = parallaxMapping(viewDir);
//
	//grain normals
	vec3 grain = normalize(texture(uNormalMap, newCoords * uGrainSize).rgb * 2.0 - 1.0); 
	vec3 norm = grain;

	//Ripple Norms
	vec3 shallowX = normalize(texture(uShallowX, newCoords).rgb * 2.0 - 1.0);
	vec3 steepX = normalize(texture(uSteepX, newCoords).rgb * 2.0 - 1.0);
	float yAlightment = dot(vec3(0, 1, 0), fs_in.Normal);
	yAlightment = pow(yAlightment, 2.0);
	vec3 rippleX = normalize(mix(steepX, shallowX, yAlightment));

	vec3 shallowZ = normalize(texture(uShallowZ, newCoords).rgb * 2.0 - 1.0);
	vec3 steepZ = normalize(texture(uSteepZ, newCoords).rgb * 2.0 - 1.0);
	vec3 rippleZ = normalize(mix(steepZ, shallowZ, yAlightment));

	float xAlignment = abs(dot(vec3(1, 0, 0), fs_in.Normal)); 
	vec3 ripple = normalize(mix(rippleZ, rippleX, xAlignment));

	//combine normal maps
	mat3 basis = mat3
	(ripple.z, ripple.y, -ripple.x,
	ripple.x, ripple.z, -ripple.y,
	ripple.x, ripple.y, ripple.z);
	norm = normalize(grain.x * basis[0] + grain.y * basis[1] + grain.z * basis[2]); //rotates grain over ripples

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

	//vec2 otherCoords = newCoords + x;

	FragColor = vec4(norm, 1.0);
	
	
	
	
	
	//FragColor = vec4(newTexCoord, 0.0, 1.0);
	//FragColor = vec4(x, 0.0, 1.0);
	//FragColor = vec4(norm.r, 0.0, 0.0, 1.0);
}