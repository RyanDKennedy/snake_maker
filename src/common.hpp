#pragma once

#include <cstdio>

#if !defined NDEBUG && !defined WIN32
#define SNAKE_MSG(text, ...) fprintf(stderr, text __VA_OPT__(,) __VA_ARGS__);
#else
#define SNAKE_MSG(text, ...)
#endif

inline const char *g_map_dir = "../share/maps/";
inline const char *g_map_file_extension = ".snake";
inline const char *g_tile_dir = "../share/tiles/";
inline const char *g_tile_file_extension = ".tile";
inline const char *g_settings_file = "../share/settings";
inline const char *g_skin_dir = "../share/skins/";
inline const char *g_skin_file_extension = ".skin";
inline const char *g_background_dir = "../share/backgrounds/";
inline const char *g_resources_dir = "../share/resources/";

inline const char *g_shader_vertex_code =
"#version 330\n\
\n\
layout (location = 0) in vec3 a_pos;\n\
layout (location = 1) in vec2 a_tex_coord;\n\
uniform mat4 u_model;\n\
uniform mat4 u_vp;\n\
\n\
out vec2 v_tex_coord;\n\
\n\
void main()\n\
{\n\
    v_tex_coord = a_tex_coord;\n\
    gl_Position = u_vp * u_model * vec4(a_pos, 1.0f);\n\
}";

inline const char *g_shader_fragment_code =
"#version 330\n\
\n\
out vec4 fragment_color;\n\
\n\
in vec2 v_tex_coord;\n\
\n\
uniform sampler2D u_tex;\n\
\n\
void main()\n\
{\n\
    fragment_color = texture(u_tex, v_tex_coord);\n\
}";


