#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <numbers>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

//My header files
#include <cube.hpp>
#include <camera.hpp>
#include "flash_light.hpp"

float quadratic(float distance);
float linear(float distance);
char* load_txt_file(const char* file_path);
void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;

int main(int argc, char const *argv[])
{
	enum Mode {normal, debug, lit_face_debug, line_debug};
	Mode mode = Mode::normal;

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-d") == 0){
			mode = Mode::debug;
			std::cout << "debug mode active" << std::endl;
		}

		else if (strcmp(argv[i], "-dl") == 0){
			mode = Mode::line_debug; std::cout << "line debug mode active" << std::endl; }
		else if (strcmp(argv[i], "-dlf") == 0){
			mode = Mode::lit_face_debug;
			std::cout << "lit face debug mode active" << std::endl;
		}
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_WIDTH, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (glfwRawMouseMotionSupported()){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glViewport(0, 0, 800, 800);
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//cube shaders
	unsigned int vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	char* source = load_txt_file("../../shaders/vertex.glsl");
	if (source == nullptr) {
		glfwTerminate();
		return -1;
	}

	glShaderSource(vertex_shader, 1, &source, NULL);
	glCompileShader(vertex_shader);

	int compile_status;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		std::cout << "failed to compile vertex shader" << std::endl;
		char info[512];
		glGetShaderInfoLog(vertex_shader, 512, NULL, info);
		std::cout << "failed to compile fragment shader" << std::endl;
		std::cout << "info: " << info << std::endl;
	}
	delete[] source;
	source = nullptr;

	unsigned int fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	source = load_txt_file("../../shaders/fragment.glsl");

	if (source == nullptr) {
		glfwTerminate();
		return -1;
	}

	glShaderSource(fragment_shader, 1, &source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		std::cout << "failed to compile fragment shader" << std::endl;
		char info[512];
		glGetShaderInfoLog(fragment_shader, 512, NULL, info);
		std::cout << "info: " << info << std::endl;
	}
	delete[] source;
	source = nullptr;

	unsigned int cube_shader_program;
	cube_shader_program = glCreateProgram();
	glAttachShader(cube_shader_program, vertex_shader);
	glAttachShader(cube_shader_program, fragment_shader);
	glLinkProgram(cube_shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	
	glGetProgramiv(cube_shader_program, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		std::cout << "failed to link" << std::endl;
	}

	//light shaders
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	source = load_txt_file("../../shaders/light_vertex.glsl");
	if (source == nullptr) {
		glfwTerminate();
		return -1;
	}

	glShaderSource(vertex_shader, 1, &source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		std::cout << "failed to compile vertex shader" << std::endl;
		char info[512];
		glGetShaderInfoLog(vertex_shader, 512, NULL, info);
		std::cout << "info: " << info << std::endl;
	}
	delete[] source;
	source = nullptr;

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	source = load_txt_file("../../shaders/light_fragment.glsl");
	if (source == nullptr) {
		glfwTerminate();
		return -1;
	}

	glShaderSource(fragment_shader, 1, &source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		std::cout << "failed to compile fragment shader" << std::endl;
		char info[512];
		glGetShaderInfoLog(fragment_shader, 512, NULL, info);
		std::cout << "info: " << info << std::endl;
	}
	delete[] source;
	source = nullptr;

	unsigned int light_shader_program;
	light_shader_program = glCreateProgram();
	glAttachShader(light_shader_program, vertex_shader);
	glAttachShader(light_shader_program, fragment_shader);
	glLinkProgram(light_shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	
	glGetProgramiv(light_shader_program, GL_COMPILE_STATUS, &compile_status);
	if (!compile_status) {
		std::cout << "failed to link" << std::endl;
	}

	float cube_vertices[] = {
//             |      position      |  tex coord |    normals     |
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, //Red
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, //Green
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, //Blue
		-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, //Yellow
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //Cyan
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, //Magenta
		1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	};
	//data
	float light_vertices[] = {
//             |      position      |
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,

		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
	};

	unsigned int indices[] = {
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};
	
	//buffers for cube
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//We bind the vertex array so it starts to "record" what VBO to use and what EBO to use.
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	//this stops the VAO from "recording"
	glBindVertexArray(0);

	//light buffers
	unsigned int LVBO, LVAO, LEBO;
	glGenVertexArrays(1, &LVAO);
	glGenBuffers(1, &LVBO);
	glGenBuffers(1, &LEBO);
	//We bind the vertex array so it starts to "record" what VBO to use and what EBO to use.
	glBindVertexArray(LVAO);

	glBindBuffer(GL_ARRAY_BUFFER, LVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(light_vertices), light_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	//this stops the VAO from "recording"
	glBindVertexArray(0);

	//textures
	unsigned int diffuse_texture;
	glGenTextures(1, &diffuse_texture);
	glBindTexture(GL_TEXTURE_2D, diffuse_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//loading image
	int width, height, nrChannels;
	//rotates the image correctly for opengl
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("../../textures/container2.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load the image" << std::endl;
	}

	unsigned int specular_texture;
	glGenTextures(1, &specular_texture);
	glBindTexture(GL_TEXTURE_2D, specular_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//rotates the image correctly for opengl
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("../../textures/container2_specular.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load the image" << std::endl;
	}

	//camera
	engine::Camera camera(90.0f, 0.001f, 0.01f, WINDOW_WIDTH, WINDOW_HEIGHT);
	//lights
	engine::Cube light(glm::vec3(11.0f, 4.0f, 0.0f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.0f, 0.0f, 0.0f));
	engine::Flash_light flash_light(glm::vec3(11.0f, 4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.0f, 0.0f, 0.0f));
	std::vector<engine::Cube> lights(5);
	glm::vec3 spotlight_positions [] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(7.0f, 3.0f, 0.0f),
		glm::vec3(-16.0f, 5.0f, 2.0f),
		glm::vec3(-7.0f, -9.0f, -5.0f),
		glm::vec3(8.0f, 5.0f, 9.0f),
	};

	for (int i = 0; i < lights.size(); i++) {
		lights[i].position = spotlight_positions[i];
		lights[i].scale = glm::vec3(0.25f, 0.25f, 0.25f);
		lights[i].Calculate_model_matrix();
	}

	float spotlight_strenghts[] = {
		70.0f, 95.0f, 54.5f, 100.0f, 80.0f,
	};

	glm::vec3 light_colors[] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f), 
		glm::vec3(0.0f, 0.0f, 1.0f), 
		glm::vec3(0.0f, 1.0f, 1.0f), 
		glm::vec3(1.0f, 0.0f, 1.0f)
	};

	//cubes
	std::vector<engine::Cube> cube_list(5);
	glm::vec3 positions[] = {
		glm::vec3(-1.832f, 6.539f, 2.804f),
		glm::vec3(3.000f, 6.000f, -1.353f),
		glm::vec3(-0.925f ,3.726f, 0.000f),
		glm::vec3(0.922f, 8.499f, 0.339f),
		glm::vec3(-3.627f,7.113f,-1.207f),
	};

	glm::vec3 rotations[] = {
		glm::vec3(-std::numbers::pi/3.0f, std::numbers::pi, std::numbers::pi/5.0f),
		glm::vec3(std::numbers::pi/2.5f, -std::numbers::pi/1.2f, std::numbers::pi/7.0f),
		glm::vec3(std::numbers::pi/2.9f, std::numbers::pi*2.0f, -std::numbers::pi/9.0f),
		glm::vec3(std::numbers::pi, -std::numbers::pi, std::numbers::pi/8.4f),
		glm::vec3(std::numbers::pi/4.6, std::numbers::pi, -std::numbers::pi/4.32f),
	};

	for(int i = 0; i < cube_list.size(); i++){
		cube_list[i].position = positions[i];
		cube_list[i].rotation = rotations[i];
		cube_list[i].scale = glm::vec3(0.25f, 0.25f, 0.25f);
		cube_list[i].Recalculate_matrices();
	}
	
	if(mode == Mode::line_debug)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		camera.Update_camera(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(cube_shader_program);

		for(int i = 0; i < cube_list.size(); i++){
			//set the transform
			glUniform3fv(glGetUniformLocation(cube_shader_program, "a_cam_position"), 1, glm::value_ptr(camera.position));
			//glUniform1i(glGetUniformLocation(cube_shader_program, "mode"), mode);

			for(int i = 0; i < 5; i++){
				std::string light_string = "point_lights[" + std::to_string(i) + "].";
				glUniform1i(glGetUniformLocation(cube_shader_program, (light_string + "on").c_str()), true);
				glUniform1f(glGetUniformLocation(cube_shader_program, (light_string + "linear").c_str()), linear(spotlight_strenghts[i]));
				glUniform1f(glGetUniformLocation(cube_shader_program, (light_string + "quadratic").c_str()), quadratic(spotlight_strenghts[i]));
				glUniform3fv(glGetUniformLocation(cube_shader_program, (light_string + "position").c_str()), 1, glm::value_ptr(lights[i].position));
				glUniform3fv(glGetUniformLocation(cube_shader_program, (light_string + "color").c_str()), 1, glm::value_ptr(light_colors[i]));

				glUniform3fv(glGetUniformLocation(cube_shader_program, (light_string + "diffuse").c_str()), 1, glm::value_ptr(glm::vec3(0.7f, 0.7f, 0.7f)));
				glUniform3fv(glGetUniformLocation(cube_shader_program, (light_string + "ambient").c_str()), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
				glUniform3fv(glGetUniformLocation(cube_shader_program, (light_string + "specular").c_str()), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
			}

			glUniform1i(glGetUniformLocation(cube_shader_program, "directional_lights[0].on"), true);
			glUniform3fv(glGetUniformLocation(cube_shader_program, "directional_lights[0].direction"), 1, glm::value_ptr(glm::vec3(0.0f, -1.0f, 0.0f)));
			glUniform3fv(glGetUniformLocation(cube_shader_program, "directional_lights[0].diffuse"), 1, glm::value_ptr(glm::vec3(0.7f, 0.7f, 0.7f)));
			glUniform3fv(glGetUniformLocation(cube_shader_program, "directional_lights[0].ambient"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
			glUniform3fv(glGetUniformLocation(cube_shader_program, "directional_lights[0].specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

			glUniformMatrix3fv(glGetUniformLocation(cube_shader_program, "normal_matrix"), 1, GL_FALSE, glm::value_ptr(cube_list[i].normal_matrix));
			glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "model"), 1, GL_FALSE, glm::value_ptr(cube_list[i].model));
			glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(camera.view));
			glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));
	
			//cube rendering
			//this tells OpenGL to us this texture
			glUniform1i(glGetUniformLocation(cube_shader_program, "material.diffuse"), 0);
			glUniform1i(glGetUniformLocation(cube_shader_program, "material.specular"), 1);
			glUniform1f(glGetUniformLocation(cube_shader_program, "material.shine"), 32.0f);
	
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuse_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specular_texture);
	
			glBindVertexArray(VAO);
			//no need to specify what EBO or VBO to use, VAO has it stored!
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		glUseProgram(light_shader_program);
		for(int i = 0; i < lights.size(); i++){
			glUniform3fv(glGetUniformLocation(light_shader_program, "color"), 1, glm::value_ptr(light_colors[i]));
			glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "transform"), 1, GL_FALSE, glm::value_ptr(lights[i].model));
			glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(camera.view));
			glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(camera.projection));
		
			glBindVertexArray(LVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(cube_shader_program);

	glfwTerminate();

	return 0;
}

float quadratic(float distance){
	if(distance < 10.0f)
		distance = 10.0f;
	return 82.4448f * pow(distance, -2.0192f);
}

float linear(float distance){
	if(distance < 10.0f)
		distance = 10.0f;
	return 4.6905f * pow(distance, -1.0097f);
}

char* load_txt_file(const char* file_path){
	std::ifstream file(file_path, std::ifstream::binary);
	if(!file){
		std::cout << "Problem with opening the file" << std::endl;
		std::cout << "File: " << file_path << std::endl;
		return nullptr;
	}

	file.seekg(0, file.end);
	size_t size = file.tellg();
	
	try{
		file.unget();
		char* contents;
		if(file.peek() == '\0'){
			file.seekg(0, file.beg);
			contents = new char[size];
			file.read(contents, size);
		}
		else {
			file.seekg(0, file.beg);
			contents = new char[size + 1];
			file.read(contents, size);
			contents[size] = '\0';
		}
			
		if (file) {
			std::cout << "File: " << file_path << " read successfully" << std::endl;
		}
		else {
			std::cout << "Reading the whole file failed only: " << file.gcount() << " where read" << std::endl;
			std::cout << "File: " << file_path << std::endl;
		}

		file.close();

		return contents;

	} catch (const std::bad_alloc& e){
		std::cerr << "Failed to allocate memory: " << e.what() << std::endl;
	}

	file.close();

	return nullptr;
}

void process_input(GLFWwindow *window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
	glViewport(0, 0, width, height);
}


