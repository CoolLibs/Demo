#version 450

layout(location = 0) in vec2 _uv;
layout(location = 0) out vec4 _out_color;

#if defined(COOL_VULKAN)
layout(push_constant) uniform Uniform
{
    float time;
}
u;

#else // COOL_OPENGL
struct Uniform {
    float time;
};
uniform Uniform u;

#endif

void main()
{
    _out_color = vec4(_uv, sin(u.time) * 0.5 + 0.5, 1.);
}
