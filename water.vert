#version 330 core
  
layout (location = 0) in vec4 position;

out vec3 pos;

uniform mat4 model2world;
uniform mat4 view;
uniform mat4 projection;
uniform float heightscale;


void main()
{
    gl_Position = projection * view * model2world * vec4(position.x, 0.5 * heightscale, position.z, 1.0f);
    pos = position.xyz;
}