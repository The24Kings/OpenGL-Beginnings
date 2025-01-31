#pragma once

#include "game.hpp"

class moon : public loaded_object {
public:
	float rotation = 0.1f;
	float speed = 0.5f;
	glm::quat target_angle = glm::quat(1, 0, 0, 0);

	moon(const char* id) : loaded_object("moon.obj", "moon_00_0.png", glm::vec3(3, 3, 3), id) {
	}

	void move(int elapsed_time) {
		for (size_t i = 0; i < data.size(); i++) {
			// Set the look ahead to the current position
			glm::vec3 moon_move_to = data[i].location;

			// Ignore y axis
			local_right.y = 0;
			local_forward.y = 0;

			// Normalize
			local_right = glm::normalize(local_right);
			local_forward = glm::normalize(local_forward);

			// Moon movement inspired by raymond mataka's lecture on local axis and quaternion rotation
			// Movement
			if (player.key_status.arrow_up) {
				moon_move_to -= local_forward * speed; // Move in the direction of the local forward axis
				target_angle = glm::angleAxis(-rotation, local_right) * target_angle; // Rotate along the local right axis
			}
			if (player.key_status.arrow_down) {
				moon_move_to += local_forward * speed;
				target_angle = glm::angleAxis(rotation, local_right) * target_angle;
			}
			if (player.key_status.arrow_left) {
				moon_move_to -= local_right * speed;
				target_angle = glm::angleAxis(rotation, local_forward) * target_angle;
			}
			if (player.key_status.arrow_right) {
				moon_move_to += local_right * speed;
				target_angle = glm::angleAxis(-rotation, local_forward) * target_angle;
			}

			target_angle = glm::normalize(target_angle); // Normalize the target angle or shit goes wrong

			// Update the moon's position and angle of rotation
			data[i].location = moon_move_to;
			data[i].angle = target_angle;
		}
	}
};