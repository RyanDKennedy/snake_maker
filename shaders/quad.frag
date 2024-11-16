#version 450

out vec4 fragment_color;

in vec2 v_tex_coord;

uniform sampler2D u_tex;

void main()
{
    vec4 col = texture(u_tex, v_tex_coord);
    fragment_color = vec4(col.xyz, 1.0);
}
