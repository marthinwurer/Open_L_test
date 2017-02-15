#version 330 core
in vec3 pos;
in float depth;
out vec4 color;

uniform vec3 camera_pos;


void main()
{
	float fog = length( pos - camera_pos) / 2000.0;
	fog = fog * fog * .75;
	fog = clamp( fog, 0.0, 1.0);
	
	float water_trans = 1 - min(depth, 0.005)/0.005;

    color = vec4(0, 0, 0.75, 0.75 - fog - water_trans);
} 