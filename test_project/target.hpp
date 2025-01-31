#pragma once

fragment brick_fragments("brick");
class target : public loaded_object, public block_object {
public:
	target(const char* id) : loaded_object("bomb.obj", "bomb.png", glm::vec3(8.0f, 10.0f, 8.0f), id) {
		collision_check = true;
	}

	void add_data(glm::vec3 location, glm::quat angle = glm::quat(1, 0, 0, 0), glm::vec3 scalar = glm::vec3(1)) {
		object_data d = { location, angle, scalar };
		data.push_back(d);
	}

	void hit_index(size_t index) {
		// Make fragments
		brick_fragments.create_burst(100, data[index].location, 0.01f);
		data.erase(data.begin() + index);
	}
};