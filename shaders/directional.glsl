#version 330 core
out vec4 frag_color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shine;
};

struct Light {
    vec3 ambient, diffuse, specular, direction;
};

in vec2 texture_coordinate;
in vec3 normal;
in vec3 frag_position;
in vec3 cam_position;
in vec3 color;

//this is GL_TEXTURE0
uniform sampler2D image_texture; //if you add another one it would be GL_TEXTURE1
uniform Material material;
uniform Light light;
uniform int mode;

void main() {
    //diffuse
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(norm, light_dir), 0.0f);
    //specular
    vec3 view_dir = normalize(cam_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shine);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texture_coordinate));
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, texture_coordinate)));
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, texture_coordinate)));

    vec3 result = vec3(1.0f);

    if (mode == 1) {
        result = color;
    }
    else if (mode == 2) {
        if (diff == 0)
            result = (diffuse + specular + ambient) * 0.5f + color * 0.1f;
        else
            result = (diffuse + specular + ambient) * 0.4f + color;
    }
    else {
        result = diffuse + specular + ambient;
    }

    frag_color = vec4(result, 1.0f);
}
