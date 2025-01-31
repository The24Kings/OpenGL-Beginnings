#pragma once

#include "game.hpp"	
#include "turret.hpp"

class flying_enemy : public turret {
	int framecount = 0;
public:
	flying_enemy(const char* id) : turret(id) {
	}

	void animate() {
		// Move around in a circle
		for (size_t i = 0; i < data.size(); i++) {
			data[i].location = glm::vec3(10 * sin(framecount / 20.0f), data[i].location.y, (10 * cos(framecount / 20.0f)));
		}
		
		framecount++;
	}
};