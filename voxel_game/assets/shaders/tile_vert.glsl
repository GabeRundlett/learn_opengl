#version 440 core

layout(location = 0) in vec2 a_pos;

out vec2 v_pos;
uniform vec2 u_player_pos;
uniform float u_scale;
uniform float u_aspect;

void main() {
    v_pos = vec2(a_pos.x * u_aspect, -a_pos.y) / u_scale - u_player_pos;
    gl_Position = vec4(a_pos, 0, 1);
}
