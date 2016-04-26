#version 330 core
#define LIGHTING_OFF 0
#define LIGHTING_DIFFUSE 1
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 color;

uniform mat4 modelViewProjectionMatrix;
out vec2 v_texcoord;
out vec4 v_color;

void main(void)
{
    gl_Position = modelViewProjectionMatrix * position;
    v_texcoord = texcoord;
    v_color = color;
}
