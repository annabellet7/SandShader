/*
	Author: Annabelle Thompson
*/
#version 330 core
layout (location = 0) out vec4 FragColor;

in Surface 
{
	vec2 TexCoord;
	vec3 Normal;
}fs_in;

uniform sampler2D uHeightMap;
uniform sampler2D uShallowXH;
uniform sampler2D uSteepXH;
uniform sampler2D uShallowZH;
uniform sampler2D uSteepZH;

uniform float uGrainSize;


void main()
{ 
	//grain normals
	vec3 grain = texture(uHeightMap, fs_in.TexCoord * uGrainSize).rgb; 
	vec3 norm = grain;

	//Ripple Norms
	vec3 shallowX = texture(uShallowXH, fs_in.TexCoord).rgb;
	vec3 steepX = texture(uSteepXH, fs_in.TexCoord).rgb;
	float yAlightment = dot(vec3(0, 1, 0), fs_in.Normal);
	yAlightment = pow(yAlightment, 2.0);
	vec3 rippleX = mix(steepX, shallowX, yAlightment);

	vec3 shallowZ = texture(uShallowZH, fs_in.TexCoord).rgb;
	vec3 steepZ = texture(uSteepZH, fs_in.TexCoord).rgb;
	vec3 rippleZ = mix(steepZ, shallowZ, yAlightment);

	float xAlignment = abs(dot(vec3(1, 0, 0), fs_in.Normal)); 
	vec3 ripple = mix(rippleZ, rippleX, xAlignment);

	//combine normal maps
	mat3 basis = mat3
	(ripple.z, ripple.y, -ripple.x,
	ripple.x, ripple.z, -ripple.y,
	ripple.x, ripple.y, ripple.z);
	norm = grain.x * basis[0] + grain.y * basis[1] + grain.z * basis[2]; //rotates grain over ripples;
	
	FragColor = vec4(norm.r, norm.r, norm.r, 1.0);	
}