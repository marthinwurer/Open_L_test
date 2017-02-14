#version 330 core
in vec3 pos;
in vec4 norm;
out vec4 color;

uniform vec3 camera_pos;


void main()
{
	float fog = length( pos - camera_pos) / 2000.0;
	fog = fog * fog * .75;
	fog = min( fog, 1.0);
	fog = max( 0.0, fog);

    color = vec4(0, 0, 0.75, 0.75 - fog);
} 