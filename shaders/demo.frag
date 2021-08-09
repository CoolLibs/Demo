#version 450

layout(location = 0) in vec2 _uv;
layout(location = 0) out vec4 _out_color;

void main()
{
    _out_color = vec4(_uv, 1., _uv.x);
}
