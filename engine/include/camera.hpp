#pragma once
#include <GLFW/glfw3.h>
#include <numbers>
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

namespace engine {
	class Camera{
		public:
		float yawn, pitch, sensitivity, fly_speed;
		glm::vec3 position;
		glm::mat4 view, projection;
		Camera(float fov, float sensitivity, float fly_speed, const int WINDOW_WIDTH, const int WINDOW_HEIGHT);
		
		glm::vec3 Get_move_vector(GLFWwindow *window);
		void Update_view_matrix(glm::quat &rotation);
		void Update_camera(GLFWwindow *window);
	};
}
