#pragma once

class maze_runner : public loaded_object {
	bool* active = nullptr;

public:
	maze_runner(const char* id, bool* active) : loaded_object("octorok.obj", "red_octorok.png", glm::vec3(2, 2, 2), id) {
		this->active = active;
	};

	void add_data(glm::vec3 location, glm::quat angle = glm::quat(1, 0, 0, 0), glm::vec3 scalar = glm::vec3(0.1)) {
		object_data d = { location, angle, scalar };
		data.push_back(d);
	};

	void animate() {/*
		if (run_maze) {
			// Maze Runner
			float speed = 0.3f;
			int front_c_index = -1;
			int left_c_index = -1;
			glm::vec3 runner_step_to_point = runner.locations[0];

			main_hud.lprintf(2, "Runner Position:  (%f, %f, %f) Facing: %d", runner_step_to_point.x, runner_step_to_point.y, runner_step_to_point.z, runner.rotation);

			if (rotation == 0) {
				runner_step_to_point.z += speed;
			}
			else if (rotation == 90) {
				runner_step_to_point.x += speed;
			}
			else if (rotation == 180) {
				runner_step_to_point.z -= speed;
			}
			else if (rotation == 270) {
				runner_step_to_point.x -= speed;
			}

			// Add the current movement to the cooldown
			movement_cooldown += speed;

			// Check to the left if there is an open space for the runner to turn into
			front_c_index = maze.collision_index_with_direction(runner_step_to_point, rotation, 1.5f);
			left_c_index = maze.collision_index_with_direction(runner_step_to_point, (rotation + 90) % 360, 4.5f, 1.3f);

			// If there is no wall to the left, turn left
			if (left_c_index == -1 && movement_cooldown > 4.0f) {
				spin((int)((rotation + 90) % 360));

				runner_step_to_point = locations[0];
				movement_cooldown = 0;
			}
			else {
				// If there is a wall in front of the runner, follow the left wall
				if (front_c_index != -1) {

					spin((int)((rotation + 270) % 360));

					runner_step_to_point = locations[0];
					movement_cooldown = 0;
				}
				else {
					// Update Runner Position
					locations[0] = runner_step_to_point;
				}
			}
		}
		*/
	}
};