#include <cube.hpp>
namespace engine {
		Cube::Cube(glm::vec3 _position, glm::vec3 _scale, glm::quat _rotation) : position(_position), scale(_scale), rotation(_rotation){
			Recalculate_matrices();
		}

		Cube::Cube(){
			position = glm::vec3(0.0f, 0.0f, 0.0f);
			scale = glm::vec3(1.0f, 1.0f, 1.0f);
			rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
			Recalculate_matrices();
		}

		void Cube::Recalculate_matrices(){
			Calculate_model_matrix();
			Calculate_normal_matrix();
		}

		void Cube::Calculate_normal_matrix(){
			normal_matrix = glm::mat3(model);
			normal_matrix = glm::inverse(normal_matrix);
			normal_matrix = glm::transpose(normal_matrix);
		}

		void Cube::Calculate_model_matrix(){
			model = glm::mat4(1.0f);
			model = glm::scale(model, scale);
			model *= glm::toMat4(rotation);
			model = glm::translate(model, position);
		}
}
