#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texture_coordinate;
layout(location = 2) in vec3 a_normal;

uniform vec3 a_cam_position;
uniform mat4 normal_transform;
uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec2 texture_coordinate;
out vec3 normal;
out vec3 frag_position;
out vec3 cam_position;

void main() {
    gl_Position = projection * view * transform * vec4(a_pos, 1.0f);
    texture_coordinate = a_texture_coordinate;
    normal = vec3(vec4(a_normal, 1.0f) * normal_transform);
    frag_position = vec3(transform * vec4(a_pos, 1.0f));
    cam_position = a_cam_position;
}
