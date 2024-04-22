#include <camera.hpp>
namespace engine {

	Camera::Camera(float fov, float sensitivity, float fly_speed, const int WINDOW_WIDTH, const int WINDOW_HEIGHT) : sensitivity(sensitivity), fly_speed(fly_speed) {
		projection = glm::perspective(glm::radians(fov) , 800.0f/800.0f, 0.01f, 1000.0f);
		view = glm::mat4(1.0f);
		yawn = 0.0f;
		pitch = 0.0f;
		position = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	void Camera::Update_camera(GLFWwindow *window){
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);
		glfwSetCursorPos(window, 0.0f, 0.0f);

		yawn += static_cast<float>(x_pos) * sensitivity;
		pitch = glm::clamp(pitch + (static_cast<float>(y_pos) * sensitivity), -static_cast<float>(std::numbers::pi)/2.0f, static_cast<float>(std::numbers::pi)/2.0f);

		glm::quat q_pitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
		glm::quat q_yawn = glm::angleAxis(yawn, glm::vec3(0, 1, 0));
		glm::quat camera_rotation = q_pitch * q_yawn;
		camera_rotation = glm::normalize(camera_rotation);

		glm::vec3 camera_move_vector = Get_move_vector(window);
		position += glm::vec3(camera_move_vector.x, 0.0f, camera_move_vector.z) * q_yawn * fly_speed;
		position.y += camera_move_vector.y * fly_speed;

		Update_view_matrix(camera_rotation);
	}

	glm::vec3 Camera::Get_move_vector(GLFWwindow *window){
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

		//down
		if(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS){
			axis.y += -1.0f;
		}
	
		return axis;
	}

	void Camera::Update_view_matrix(glm::quat &rotation){
		view = glm::mat4(1.0f);
		view *= glm::toMat4(rotation);
		view = glm::translate(view, -position);
	}
}
