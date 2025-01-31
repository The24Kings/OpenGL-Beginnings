#pragma once

#include "game.hpp"
#include "test_objects.hpp"

class player_marker : public block_object {
	struct player* p;
	float MOVEMENT_MULT = 0.2f;

public:
	player_marker(struct player* p, const char* id) {
		identifier = id;
		collision_check = true;
		this->p = p;
		size = p->hitbox;
	}

	// Constantly update to the players location
	void move(int elapsed_time) {
		glm::vec3 step_to_point = p->position - glm::vec3(0, 3, 0);

		// Movement
		if (p->key_status.forward) {
			step_to_point += glm::vec3(sinf(p->heading.x), 0, cosf(p->heading.x)) * MOVEMENT_MULT; // step_to_point
		}
		if (p->key_status.backward) {
			step_to_point -= glm::vec3(sinf(p->heading.x), 0, cosf(p->heading.x)) * MOVEMENT_MULT; // step_to_point
		}
		if (p->key_status.left) {
			step_to_point += glm::vec3(sinf(p->heading.x + M_PI / 2), 0, cosf(p->heading.x + M_PI / 2)) * MOVEMENT_MULT; // step_to_point
		}
		if (p->key_status.right) {
			step_to_point -= glm::vec3(sinf(p->heading.x + M_PI / 2), 0, cosf(p->heading.x + M_PI / 2)) * MOVEMENT_MULT; // step_to_point
		}

		for (size_t i = 0; i < data.size(); i++) {
			data[i].location = step_to_point;
		}
	}

	bool is_on_idx(glm::vec3 position, size_t index, float height) { return false; }
	int is_on(glm::vec3 position, float height) { return -1; }
};