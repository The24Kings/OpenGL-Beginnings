#pragma once

#include "game.hpp"

struct key_frame {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	bool initialized;
	int frame;
	int time;
};

struct anim_status {
	bool entering, hit, transit;
};

class octorok : public loaded_object, public block_object {
public:
	const char* id;
	std::vector<key_frame> enter_anim;
	std::vector<key_frame> hit_anim;
	std::vector<key_frame> transit_anim;

	std::vector<double> anim_time;
	std::vector<anim_status> status;

	std::vector<object_data> start_data;

	glm::vec3 scale = glm::vec3(0.5f, 0.6f, 0.5f);
	glm::vec3 global_right = glm::vec3(1, 0, 0);
	glm::vec3 global_forward = glm::vec3(0, 0, 1);
	glm::vec3 global_up = glm::vec3(0, 1, 0);

	bool can_score = true;

	octorok(const char* obj, const char* tex, glm::vec3 size, const char* id) : loaded_object(obj, tex, size, id) {
		this->id = id;
		collision_check = true;
	}

	int init() override {
		if (loaded_object::init())
			return 1;

		// Entering
		// Jump up
		int f_count = 0;
		int time = 0;
		glm::vec3 new_location = glm::vec3(0, 0, 0);
		glm::quat new_angle = glm::quat(1, 0, 0, 0);
		glm::vec3 new_scale = glm::vec3(1, 1, 1);

		while (new_location.y <= 25.0f) {
			new_location.y += 0.8f;
			new_scale.x -= 0.005f;
			new_scale.y += 0.01f;
			new_scale.x -= 0.005f;

			create_anim(new_location, new_angle, new_scale, f_count, time, enter_anim);

			f_count++;
			time++;
		}

		// Come back to the ground
		while (new_location.y >= 16) { // Floating point error
			new_location.y -= 0.5f;

			if (new_scale.y > 1) {
				new_scale.y -= 0.05f;
			}

			if (new_scale.x < 1 || new_scale.z < 1) {
				new_scale.x += 0.05f;
				new_scale.z += 0.05f;
			}

			create_anim(new_location, new_angle, new_scale, f_count, time, enter_anim);

			f_count++;
			time++;
		}

		// Hit
		f_count = 0;
		time = 0;

		new_location = glm::vec3(0, 0, 0);
		new_angle = glm::quat(1, 0, 0, 0);
		new_scale = glm::vec3(1, 1, 1);

		// Flies up
		while (new_location.y < 12.0f) {
			new_location.y += 0.8f;

			create_anim(new_location, new_angle, new_scale, f_count, time, hit_anim);

			f_count++;
			time++;
		}

		// Flips around the local right axis twice
		while (new_location.y >= 0.0001) {
			new_angle = glm::angleAxis(-0.2f, global_right) * new_angle;

			new_location.y -= 0.4f;

			create_anim(new_location, new_angle, new_scale, f_count, time, hit_anim);

			f_count++;
			time++;
		}

		// Lands face up
		new_angle = glm::angleAxis(-89.5f, global_right) * new_angle;

		f_count++;
		time++;

		create_anim(new_location, new_angle, new_scale, f_count, time, hit_anim);

		// Scale down along the Y axis
		while (new_scale.z > 0.2f) {
			new_scale.z -= 0.025f;
			new_location.y -= 0.025f;

			f_count++;
			time++;

			create_anim(new_location, new_angle, new_scale, f_count, time, hit_anim);
		}

		// Transit
		f_count = 0;
		time = 0;
		new_location = glm::vec3(0, 0, 0);
		new_angle = glm::quat(1, 0, 0, 0);
		new_scale = glm::vec3(1, 1, 1);

		// Jumps up and goes into ground
		while (new_location.y < 10.0f) {
			new_location.y += 0.8f;
			new_scale.x -= 0.008f;
			new_scale.y += 0.035f;
			new_scale.z -= 0.008f;

			create_anim(new_location, new_angle, new_scale, f_count, time, transit_anim);

			f_count++;
			time++;
		}

		// Goes into the ground
		while (new_location.y > -15.9999) {
			new_location.y -= 0.8f;

			if (new_scale.y > 1) {
				new_scale.y -= 0.05f;
			}

			create_anim(new_location, new_angle, new_scale, f_count, time, transit_anim);

			f_count++;
			time++;
		}

		printf(GREEN("Animation Initialization Complete\n").c_str());
		printf("Entering Animation: %zu frames\n", enter_anim.size());
		printf("Hit Animation: %zu frames\n", hit_anim.size());
		printf("Transit Animation: %zu frames\n", transit_anim.size());

		return 0;
	}

	key_frame get_last_frame(std::vector<key_frame>& anim, double time) {
		if (time > anim.size() - 1) { return anim[anim.size() - 1]; }

		for (int i = (int)time; i != -1; --i) {
			if (!anim[i].initialized) {
				return anim[i];
			}
		}

		return anim[0];
	}

	key_frame get_next_frame(std::vector<key_frame>& anim, double time) {
		if (time > anim.size() - 1) { return anim[anim.size() - 1]; }

		for (int i = (int)time + 1; i < anim.size(); ++i) {
			if (!anim[i].initialized) {
				return anim[i];
			}
		}

		return anim[0];
	}

	float inverse_lerp(float min, float max, float center) {
		float size = (max - min);
		center = center - min;

		printf("\tMin: %f - Max: %f\n", min, max);
		printf("\tCenter: %f - Size: %f\n", center, size);

		return (size == 0) ? 0 : (center / size);
	}

	void create_anim(glm::vec3 end_location, glm::quat end_rotation, glm::vec3 end_scale, int frame, int time, std::vector<key_frame>& anim) {
		key_frame k = { end_location, end_rotation, end_scale, false, frame, time };
		anim.push_back(k);
	}

	void hit_index(size_t index) {
		status[index].hit = true;
	}

	void create_octo(glm::vec3 location) {
		add_data(location, glm::quat(1, 0, 0, 0), scale);
		start_data.push_back(data[data.size() - 1]); // Save the starting data for the animation offset

		status.push_back(
			anim_status {
				true, 
				false,
				false
			}
		);
		anim_time.push_back(0);
	}

	void remove_octo(size_t index) {
		data.erase(data.begin() + index);
		start_data.erase(start_data.begin() + index);
		status.erase(status.begin() + index);
		anim_time.erase(anim_time.begin() + index);
	}

	void face_player(size_t index) {
		if (status[index].hit || status[index].entering || status[index].transit) {
			return;
		}

		glm::vec3 l = data[index].location;
		glm::vec3 target = glm::normalize(player.position - l);

		target.y = 0; // Ignore y axis

		glm::quat target_angle = glm::rotation(global_forward, target);

		data[index].angle = glm::slerp(data[index].angle, target_angle, 0.2f);
	}

	void move(int elapsed_time) {
		for (size_t i = 0; i < data.size(); i++) {
			face_player(i);
		}
	}

	void clear() {
		for (size_t i = 0; i < data.size(); i++) {
			status[i].transit = true;
		}
	}

	void animate() { 
		if (enter_anim.size() == 0 || hit_anim.size() == 0 || transit_anim.size() == 0) {
			return;
		}

		if (data.size() == 0) {
			return;
		}

		for (size_t i = 0; i < data.size(); i++) {
			if (status[i].entering) {
				if (run_anim(enter_anim, i)) {
					printf(BLUE("Completed Enterance!").c_str());
					status[i].entering = false;
				}

			} else if (status[i].hit) {
				if (run_anim(hit_anim, i)) {
					printf(RED("Completed Hit!\n").c_str());
					remove_octo(i);

					continue;
				}

			} else if (status[i].transit) {
				if (run_anim(transit_anim, i)) {
					printf(RED("Goodbye!\n").c_str());
					remove_octo(i);

					continue;
				}
			}
		}
	}

	bool run_anim(std::vector<key_frame> &anim, size_t i) {
		key_frame final_frame = anim[anim.size() - 1];
		double current_time = anim_time[i];

		if (current_time == 0) {
			printf(YELLOW("Starting Animation\n").c_str());

			// Update for when a previous animation may have moved it
			start_data[i].location = data[i].location;
			start_data[i].angle = data[i].angle;
			start_data[i].scalar = data[i].scalar;
		}

		if (current_time >= final_frame.time) {
			printf(GREEN("Animation Complete\n").c_str());
			anim_time[i] = 0;

			return true;
		}

		printf("\nAnimating %s : %zu\n", id, i);

		key_frame last_frame = get_last_frame(anim, current_time);
		key_frame next_frame = get_next_frame(anim, current_time);

		// Adjust the last and next frame to the current object data
		last_frame.position += start_data[i].location;
		last_frame.rotation = start_data[i].angle * last_frame.rotation;
		last_frame.scale *= start_data[i].scalar;

		next_frame.position += start_data[i].location;
		next_frame.rotation = start_data[i].angle * next_frame.rotation;
		next_frame.scale *= start_data[i].scalar;

		printf("Last Frame: (%f, %f, %f) at %d\n", last_frame.position.x, last_frame.position.y, last_frame.position.z, last_frame.frame);
		printf("Next Frame: (%f, %f, %f) at %d\n\n", next_frame.position.x, next_frame.position.y, next_frame.position.z, next_frame.frame);

		printf("\tCurrent Location: (%f, %f, %f)\n", data[i].location.x, data[i].location.y, data[i].location.z);

		float lerped = inverse_lerp(last_frame.frame, next_frame.frame, current_time);

		printf("\tLerping to: %f\%\n", lerped);

		data[i].location = glm::mix(last_frame.position, next_frame.position, lerped);
		data[i].angle = glm::slerp(glm::normalize(last_frame.rotation), glm::normalize(next_frame.rotation), lerped);
		data[i].scalar = glm::mix(last_frame.scale, next_frame.scale, lerped);

		printf("\tNew Location: (%f, %f, %f)\n", data[i].location.x, data[i].location.y, data[i].location.z);

		anim_time[i] += delta_time * 10.0f;

		printf("Delta Time: %f - Anim_time: %f\n", delta_time, anim_time[i]);

		return false;
	}
};