#version 330 core
in vec4 pos;
in vec4 norm;
out vec4 color;

vec4 black = vec4(0,0,0,1);
vec4 transparent = vec4(0,0,0,0);
vec4 green = vec4(0,1,0,1);
vec4 dgreen = vec4(56.0/256.0,124.0/256.0,68.0/256.0, 1);
vec4 brown = vec4(111.0/256.0,78.0/256.0,55.0/256.0, 1);
vec4 white = vec4(1,1,1,1);
//do something with these to avoid branching
//vec4 colorArr[4] = vec4[4](green, dgreen, brown, white)
//float colorInterpVal[4] = float[4](0, 2.0 / 3.0, 5.0 / 6.0, 1.0);

/*
const SDL_Color black = {0, 0, 0, 255};
const SDL_Color blue = {0, 0, 255, 255};
const SDL_Color green = {0, 255, 0, 255};
const SDL_Color dgreen = {56, 124, 68, 255};
const SDL_Color brown = {111, 78, 55, 255};
const SDL_Color white = {255, 255, 255, 255};
*/


/*#version 430
sam's lighting shader

in vec3 fragPos;
in vec3 fragNorm;
in vec2 fragTexUV;
uniform sampler2D sampler;
out vec4 fragColor;

void main()
{
	vec3 lightDir = normalize(vec3(1,0,1));
	float lamb = max(dot(lightDir, fragNorm), 0);
	vec3 viewDir = normalize(-fragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float spcAngl = max(dot(halfDir, fragNorm), 0);
	float specular = pow(spcAngl, 16.0);
	float light = .1 + .6 * lamb + .3 * specular;
	fragColor = texture(sampler, fragTexUV) * light;
}


this is slightly hacked
but this is a light shining in the direction of the positive z axis
given by lightdir
lamb (lambda) is the dot of the light direction with the normal
then some specular lighting stuff

the actual amount of light on a given pixel emitted from the shader is a base of .1
(so things are not completely black when no light is shining on it), then .6 defined
 by the light direction, then .3 defined by the specular reflection amount
for you I may want to use a light value of like .3 + .7 * dotOfLightDirAndNormal

learnopengl.com



*/

vec4 alpine_gradient(float height){
	if( height < 0.3)
	{
		return mix(transparent, dgreen, height/ 0.3);
	}
    else if( height < 2.0/3.0)
    {
        return mix(green, dgreen, (height)/(2.0/3.0));
    }
    else if( height < 5.0/6.0)
    {
        return mix( dgreen, brown, (height -(2.0/3.0) ) / (1.0/6.0));
    }
    else if(height <= 1.0)
    {
        return mix( brown, white, (height - 5.0/6.0) / (1.0/6.0));
    }
    else
    {
        return white;
    }
}

void main()
{
	vec4 objectcolor = alpine_gradient(pos.y);
	vec3 lightDir = vec3(-1.0, 0.0, 0.0);
	vec3 normal = normalize(norm.xyz);
	
	float ambientStrength = 0.7f;
    float ambient = ambientStrength;
    float diffuse = max(dot(normal, lightDir), -1.0) * 0.9;

//    color = vec4(pos.y, pos.y, pos.y, 1.0f);

//color = vec4(normal, 1);
	color = vec4(objectcolor.xyz * (ambient + diffuse), objectcolor.w);
} 