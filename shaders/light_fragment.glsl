#version 330 core
out vec4 FragColor;

uniform vec3 color;

void main() {
    //add a fake glow!
    FragColor = vec4(vec3(color), 1.0f);
}
