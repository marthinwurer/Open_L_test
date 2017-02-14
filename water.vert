#version 330 core
  
layout (location = 0) in vec4 position;

out vec3 pos;

uniform mat4 model2world;
uniform mat4 view;
uniform mat4 projection;
uniform float heightscale;
uniform float sensitivity;


void main()
{
	float current = position.w + position.y - sensitivity; 
    gl_Position = projection * view * model2world * vec4(position.x, current * heightscale, position.z, 1.0f);
    pos = position.xyz;
}