#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

namespace engine {
	class Cube{
		public:
		glm::vec3 position, scale;
		glm::quat rotation;
		glm::mat4 model;
		glm::mat3 normal_matrix;

		Cube(glm::vec3 _position, glm::vec3 _scale, glm::quat _rotation);
		Cube();
		void Recalculate_matrices();
		void Calculate_normal_matrix();
		void Calculate_model_matrix();
	};
}
