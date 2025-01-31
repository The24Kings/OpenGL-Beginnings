#pragma once

#include "game.hpp"

class platform : public wall_block {
public:
	platform(const char* id) : wall_block("platform.obj", "brick.jpg", glm::vec3(8.0f, 2.0f, 8.0f), id) {
		collision_check = true;
	}
};