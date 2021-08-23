#version 450

layout(location = 0) in vec2 _uv;
layout(location = 0) out vec4 _out_color;

layout(push_constant) uniform Uniform
{
    float time;
}
u;

void main()
{
    _out_color = vec4(_uv, sin(u.time) * 0.5 + 0.5, 1.);
}
