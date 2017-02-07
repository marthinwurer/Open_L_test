#version 330 core
in vec3 pos;
out vec4 color;

vec4 black = vec4(0,0,0,1);
vec4 transparent = vec4(0,0,0,0);
vec4 green = vec4(0,1,0,1);
vec4 dgreen = vec4(.2, 1, .2, 1);
vec4 brown = vec4(165.0/256.0,45.0/256.0,45.0/256.0, 1);
vec4 white = vec4(1,1,1,1);
//do something with these to avoid branching
//vec4 colorArr[4] = vec4[4](green, dgreen, brown, white)
//float colorInterpVal[4] = float[4](0, 2.0 / 3.0, 5.0 / 6.0, 1.0);

vec4 alpine_gradient(float height){
	if( height < 0.3){
		return mix(transparent, green, height/ 0.3);
	}
    else if( height < 0.5)
    {
        return mix(green, dgreen, (height)/(0.5));
    }
    else if( height < 5.0/6.0)
    {
        return mix( dgreen, brown, (height - 0.5) / (2.0/6.0));
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
	color = alpine_gradient(pos.y);

//    color = vec4(pos.y, pos.y, pos.y, 1.0f);
} 