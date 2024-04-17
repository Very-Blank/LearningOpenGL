#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texture_coordinate;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_color;

uniform vec3 a_cam_position;
uniform mat3 normal_matrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_position;
out vec2 texture_coordinate;
out vec3 normal;
out vec3 cam_position;
out vec3 color;

void main() {
    frag_position = vec3(model * vec4(a_pos, 1.0f));
    texture_coordinate = a_texture_coordinate;
    normal = normal_matrix * a_normal;
    cam_position = a_cam_position;
    color = a_color;

    gl_Position = projection * view * model * vec4(a_pos, 1.0f);
}
