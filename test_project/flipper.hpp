#pragma once

class flipper_floor : public wall_block {
public:
	std::vector<bool> flipper_states;
	std::vector<glm::vec3> flipper_locations; // Contains the locations of all the flippers

	flipper_floor(const char* id) : wall_block("platform.obj", "brick.jpg", glm::vec3(8.0f, 2.0f, 8.0f), id) {
		collision_check = true;
	}

	int init() override {
		if (loaded_object::init())
			return 1;

		// Initialize the flipper states in an alternating fashion
		for (int i = 0; i < data.size(); i++) {
			if (i % 2 == 0)
				flipper_states.push_back(false);
			else
				flipper_states.push_back(true);

			flipper_locations.push_back(data[i].location);
		}

		flip();

		return 0;
	}

	void flip() {
		// Flip the state of the flippers
		for (int i = 0; i < flipper_locations.size(); i++) {
			flipper_states[i] = !flipper_states[i];
		}

		// Reset the locations vector
		data.erase(data.begin(), data.end());

		// If the state is true, move the flipper to the hidden location
		for (int i = 0; i < flipper_locations.size(); i++) {
			if (flipper_states[i]) {
				object_data d = {
					flipper_locations[i],
					glm::quat(1, 0, 0, 0),
					glm::vec3(1.0f, 1.0f, 1.0f)
				};

				data.push_back(d);
			}
		}
	}
};