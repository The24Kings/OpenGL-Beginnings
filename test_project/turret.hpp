#pragma once
#include "projectiles.hpp"

class turret : public loaded_object {
public:
	size_t target_idx = 0;
	gameobject* current_target; // Might be nice if this was a list
	projectile* current_projectile;
	int countdown = 50;
	float rotation = 0;
	bool active = false;

	turret(const char* id) : loaded_object("cone.obj", "brick.png", glm::vec3(6, 6, 6), id) {
	}

	void add_data(glm::vec3 location, glm::quat angle = glm::quat(1, 0, 0, 0), glm::vec3 scalar = glm::vec3(3)) {
		loaded_object::add_data(location, angle, scalar);
	}

	void move(int elapsed_time) { // TODO:  Incorporate elapsed_time
		/* At some rate (every so many calls to this function)
		 * Pick a target  (How do we know what targets we have?)
		 * Launch a projectile at it (How do we launch a projectile from here?)
		 */
		if (!active)
			return;

		if (countdown > 0) {
			countdown--;
			return;
		}

		countdown = 50;

		if (!current_target->data.size())
			return;

		for (auto d : data) {
			glm::vec3 target_location = current_target->data[target_idx].location;
			current_projectile->add_projectile(d.location, 0.2f * glm::normalize(target_location - d.location), 100000);
			target_idx++;
			if (target_idx >= current_target->data.size())
				target_idx = 0;
		}
	}

	void animate() {
		if (active)
			rotation += 0.01;
	}

	std::vector<glm::mat4> create_models() override {
		std::vector<glm::mat4> models;
		data_mutex.lock();
		models.reserve(data.size());

		for (auto d : data) {
			glm::mat4 new_model = glm::mat4(1.0f);

			new_model = translate(new_model, d.location);
			new_model = rotate(new_model, rotation, glm::vec3(0, 1, 0));
			models.push_back(new_model);
		}

		data_mutex.unlock();
		return models;
	}
};

