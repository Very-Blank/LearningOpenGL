#include <flash_light.hpp>

namespace engine {
	Flash_light::Flash_light(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _scale, glm::quat _rotation) : Cube(_position, _scale, _rotation), direction(_direction){
	}
}
