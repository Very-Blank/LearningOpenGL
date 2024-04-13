#include <fstream>
#include <iostream>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <numbers>
#include <ostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <cube.hpp>
#include <cstdlib>
#include <ctime>

glm::mat4 update_cam_transform(GLFWwindow *window, float &yaw, float &pitch, glm::vec3 &position, float &movement_speed);
float available_rotation(float current, float input);
glm::mat4 calculate_new_matrix(glm::vec3 position, glm::quat rotation);
glm::vec3 move_axis(GLFWwindow *window);
char* load_txt_file(const char* file_path);
void process_input(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main(int argc, char const *argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow *window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
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
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // Red face
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		1.0f,  1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // Green face
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
		1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f,

		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // Blue face
		-1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f,  1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,

		1.0f, -1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // Yellow face
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f,  1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,

		-1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Cyan face
		1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
		-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // Magneta face
		1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
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

	//model transform stuff
	glm::vec3 cube_position(0.0f, 0.0f, 5.0f);
	glm::vec3 cube_scale(0.25f, 0.25f, 0.25f);
	glm::quat cube_rotation = glm::quat(glm::vec3(0.0f, std::numbers::pi, 0.0f));
	glm::mat4 cube_transform(1.0f);

	glm::vec3 flip_scale(1.0f);
	if(cube_scale.x < 0)
		flip_scale.x = -1.0f;
	if(cube_scale.y < 0)
		flip_scale.y = -1.0f;
	if(cube_scale.z < 0)
		flip_scale.z = -1.0f;

	glm::mat4 normal_transform(1.0f);
	normal_transform = glm::scale(normal_transform, flip_scale);
	normal_transform *= glm::toMat4(cube_rotation);

	//combining everything into a matrix
	cube_transform = glm::scale(cube_transform, cube_scale);
	cube_transform *= glm::toMat4(cube_rotation);
	cube_transform = glm::translate(cube_transform, cube_position);
	
	//light transform stuff
	glm::vec3 light_position(-5.0f, 4.0f, 2.0f);
	glm::vec3 light_scale(0.25f, 0.25f, 0.25f);
	glm::vec3 light_color(1.0f, 1.0f, 1.0f);
	float ambient_light_strength = 0.5f;
	glm::quat light_rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 light_transform(1.0f);

	//combining everything into a matrix
	light_transform = glm::scale(light_transform, light_scale);
	light_transform *= glm::toMat4(light_rotation);
	light_transform = glm::translate(light_transform, light_position);

	//camera
	float cam_move_speed = 0.02f;
	glm::vec3 cam_position(0.0f, 0.0f, 5.0f);
	float cam_yaw, cam_pitch = 0.0f;
	glm::mat4 view(1.0f);
	//view *= glm::toMat4(rotation);
	view = glm::translate(view, -cam_position);
	glm::mat4 projection = glm::perspective(glm::radians(90.0f) , 800.0f/800.0f, 0.01f, 1000.0f);
	

	//buffers for cube
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//We bind the vertex array so it starts to "record" what VBO to use and what EBO to use.
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

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

	//line mode for debugging!
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	std::vector<engine::Cube> cube_list(5);
	std::cout << cube_list.size() << std::endl;
	for(int i = 0; i < cube_list.size(); i++){
		srand(time(0) + i);
		cube_list[i].position = glm::vec3((float)(rand()) / (float)(rand()), (float)(rand()) / (float)(rand()), (float)(rand()) / (float)(rand()));
		std::cout << cube_list[i].position.x << " " << cube_list[i].position.y << " " <<  cube_list[i].position.z << std::endl;	
		srand(time(0) + i);
		cube_list[i].rotation = glm::quat(glm::vec3((float)(rand()) / (float)(rand()), (float)(rand()) / (float)(rand()), (float)(rand()) / (float)(rand())));
		cube_list[i].scale = glm::vec3(0.25f, 0.25f, 0.25f);
		cube_list[i].Recalculate_transform();
	}
	
	
	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		view = update_cam_transform(window, cam_yaw, cam_pitch, cam_position, cam_move_speed); 

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for(int i = 0; i < cube_list.size(); i++){
			//set the transform
			glUseProgram(cube_shader_program);
			glUniform3fv(glGetUniformLocation(cube_shader_program, "a_cam_position"), 1, glm::value_ptr(cam_position));
			glUniform3fv(glGetUniformLocation(cube_shader_program, "light.position"), 1, glm::value_ptr(light_position));
			glUniform3fv(glGetUniformLocation(cube_shader_program, "light.ambient"), 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, 0.6f)));
			glUniform3fv(glGetUniformLocation(cube_shader_program, "light.diffuse"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
			glUniform3fv(glGetUniformLocation(cube_shader_program, "light.specular"), 1, glm::value_ptr(glm::vec3(4.0f, 4.0f, 4.0f)));
	
			glUniform1f(glGetUniformLocation(cube_shader_program, "material.shine"), 32.0f);
	
	
			glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "normal_transform"), 1, GL_FALSE, glm::value_ptr(cube_list[i].normal_transform));
			glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "transform"), 1, GL_FALSE, glm::value_ptr(cube_list[i].transform));
			glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(cube_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	
			//cube rendering
			//this tells OpenGL to us this texture
			glUniform1i(glGetUniformLocation(cube_shader_program, "material.diffuse"), 0);
			glUniform1i(glGetUniformLocation(cube_shader_program, "material.specular"), 1);
	
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuse_texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specular_texture);
	
			glBindVertexArray(VAO);
			//no need to specify what EBO or VBO to use, VAO has it stored!
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		//light rendering
		glUseProgram(light_shader_program);
		glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "transform"), 1, GL_FALSE, glm::value_ptr(light_transform));
		glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(light_shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(LVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(cube_shader_program);

	glfwTerminate();

	return 0;
}

glm::mat4 update_cam_transform(GLFWwindow *window, float &yaw, float &pitch, glm::vec3 &position, float &movement_speed){
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);
		glfwSetCursorPos(window, 0.0f, 0.0f);

		yaw += x_pos * 0.001f;
		pitch = glm::clamp(pitch + (static_cast<float>(y_pos) * 0.001f), -static_cast<float>(std::numbers::pi)/2.0f, static_cast<float>(std::numbers::pi)/2.0f);
		glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
		glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));

		glm::quat rotation = qPitch * qYaw;
		rotation = glm::normalize(rotation);
		position += move_axis(window) * rotation * movement_speed;
		return calculate_new_matrix(position, rotation);
}

char* load_txt_file(const char* file_path){
	std::ifstream file(file_path, std::ifstream::binary);
	if(!file){
		std::cout << "Problem with opening the file" << std::endl;
		std::cout << "Path: " << file_path << std::endl;
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
			std::cout << "All charecters where read succefully" << std::endl;
		}
		else {
			std::cout << "Reading the whole file failed only: " << file.gcount() << " where read" << std::endl;
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

glm::mat4 calculate_new_matrix(glm::vec3 position, glm::quat rotation){
	glm::mat4 matrix(1.0f);
	matrix *= glm::toMat4(rotation);
	matrix = glm::translate(matrix, -position);
	return matrix;
}

glm::vec3 move_axis(GLFWwindow *window){
	glm::vec3 axis(0.0f);
	//axis
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		axis.z -= 1.0f;
	}

	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		axis.z += 1.0f;
	}

	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		axis.x += 1.0f;
	}

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		axis.x -= 1.0f;
	}

	//up
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		axis.y += 1.0f;
	}

	return axis;
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height){
	glViewport(0, 0, width, height);
}


