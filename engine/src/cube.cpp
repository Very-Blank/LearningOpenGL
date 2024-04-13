#include "glm/fwd.hpp"
#include <cube.hpp>
namespace engine {
		Cube::Cube(glm::vec3 _position, glm::vec3 _scale, glm::quat _rotation) : position(_position), scale(_scale), rotation(_rotation){
			transform = glm::mat4(1.0f);
			transform = glm::scale(transform, scale);
			transform *= glm::toMat4(rotation);
			transform = glm::translate(transform, position);
			glm::vec3 flip_scale(1.0f);
			//if(scale.x < 0)
			//	flip_scale.x = -1.0f;
			//if(scale.y < 0)
			//	flip_scale.y = -1.0f;
			//if(scale.z < 0)
			//	flip_scale.z = -1.0f;
		
			normal_transform = glm::mat4(1.0f);
			normal_transform = glm::scale(normal_transform, flip_scale);
			normal_transform *= glm::toMat4(rotation);
		}

		Cube::Cube(){
			position = glm::vec3(0.0f, 0.0f, 0.0f);
			scale = glm::vec3(1.0f, 1.0f, 1.0f);
			rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
			transform = glm::mat4(1.0f);
		
			normal_transform = glm::mat4(1.0f);
			normal_transform = glm::scale(normal_transform, scale);
			normal_transform *= glm::toMat4(rotation);
		}

		void Cube::Recalculate_transform(){
			transform = glm::mat4(1.0f);
			transform = glm::scale(transform, scale);
			transform *= glm::toMat4(rotation);
			transform = glm::translate(transform, position);

			glm::vec3 flip_scale(1.0f);
			if(scale.x < 0)
				flip_scale.x = -1.0f;
			if(scale.y < 0)
				flip_scale.y = -1.0f;
			if(scale.z < 0)
				flip_scale.z = -1.0f;
		
			normal_transform = glm::mat4(1.0f);
			normal_transform = glm::scale(normal_transform, flip_scale);
			normal_transform *= glm::toMat4(rotation);
		}
}
