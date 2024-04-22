#pragma once
#include <cube.hpp>

namespace engine {
	class Flash_light : public Cube{
		public:
		glm::vec3 direction;
		Flash_light(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _scale, glm::quat _rotation);
	};
}
