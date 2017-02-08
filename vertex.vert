#version 330 core
  
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;

out vec4 pos;
out vec4 norm;

uniform mat4 model2world;
uniform mat4 view;
uniform mat4 projection;
uniform float heightscale;


void main()
{
    gl_Position = projection * view * model2world * vec4(position.x, position.y * 100.0f, position.z, 1.0f);
    pos = model2world * position;
    norm = normal;
}