#version 330 core
out vec4 frag_color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shine;
};

struct Directional_light {
    vec3 ambient, diffuse, specular, direction;
    bool on;
};

struct Point_light {
    vec3 color, diffuse, specular, position;
    float linear, quadratic;
    bool on;
};

//struct Flash_light {
//    vec3 direction, position, ambient, diffuse, specular;
//    float max_angle;
//    bool active;
//};

in vec2 texture_coordinate;
in vec3 normal;
in vec3 frag_position;
in vec3 cam_position;
in vec3 color;

uniform Material material;
#define MAX_POINT_LIGHTS 5
uniform Point_light point_lights[MAX_POINT_LIGHTS];
#define MAX_DIRECTIONAL_LIGHTS 1
uniform Directional_light directional_lights[MAX_DIRECTIONAL_LIGHTS];
//#define MAX_FLASH_LIGHTS = 1;
//uniform Flash_light flash_lights[MAX_FLASH_LIGHTS];
//uniform int mode;

//All of the different modes starts from 0
//normal, debug, lit_face_debug, line_debug

vec3 Directional_light_calc(Directional_light light, vec3 norm, vec3 view_dir);
vec3 Point_light_calc(Point_light light, vec3 frag_position, vec3 norm, vec3 view_dir);
//vec3 Flash_light_calc(Flash_light light);

void main() {
    vec3 result = vec3(0.0f);
    vec3 norm = normalize(normal);
    vec3 view_dir = normalize(cam_position - frag_position);

    for (int i = 0; i < MAX_DIRECTIONAL_LIGHTS; i++)
        result += Directional_light_calc(directional_lights[i], norm, view_dir);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
        result += Point_light_calc(point_lights[i], frag_position, norm, view_dir);

    frag_color = vec4(result, 1.0f);
}

vec3 Directional_light_calc(Directional_light light, vec3 norm, vec3 view_dir) {
    if (light.on == false) {
        return vec3(0.0f, 0.0f, 0.0f);
    }

    vec3 light_dir = normalize(-light.direction);
    float diff = max(dot(norm, light_dir), 0.0f);

    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shine);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texture_coordinate));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texture_coordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texture_coordinate));

    return (ambient + diffuse + specular);
}

vec3 Point_light_calc(Point_light light, vec3 frag_position, vec3 norm, vec3 view_dir) {
    if (light.on == false) {
        return vec3(0.0f, 0.0f, 0.0f);
    }

    vec3 light_dir = normalize(light.position - frag_position);
    float diff = max(dot(norm, light_dir), 0.0f);
    vec3 reflect_dir = reflect(-light_dir, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shine);

    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texture_coordinate));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texture_coordinate));

    float distance = length(light.position - frag_position);
    if (distance == 0)
        return vec3(0.0f);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * distance * distance);

    return (attenuation * diffuse + attenuation * specular) * light.color * 5.0f;
}

//vec3 Flash_light_calc(Flash_light light) {}
