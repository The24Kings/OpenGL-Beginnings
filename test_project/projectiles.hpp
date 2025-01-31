#pragma once

#include "game.hpp"

class projectile : public loaded_object {
public:
	std::vector<glm::vec3> directions;
	std::vector<float> lifetimes;
	std::vector<bool> bursting;

	projectile(const char* id) : loaded_object("projectile.obj", "projectile.jpg", glm::vec3(0.1f, 0.1f, 0.1f), id) {
		collision_check = false;
	}

	void create_burst(float quantity, glm::vec3 origin, float speed) {
		for (size_t i = 0; i < quantity; i++) {
			add_data(origin);
			lifetimes.push_back(10000.0f);

			// One note:  This does create a cube of projectiles
			directions.push_back(glm::vec3(randvel(speed), randvel(speed), randvel(speed)));
			bursting.push_back(false);
		}
	}

	void move(int elapsed_time) {
		data_mutex.lock();
		for (size_t i = 0; i < data.size(); i++) {
			if (bursting[i])
				directions[i].y -= 0.0002 * elapsed_time / 1000.0f;

			data[i].location += directions[i] * (elapsed_time / 1000.0f);
			lifetimes[i] -= 10 * elapsed_time / 1000.0f; // TODO:  Manage time resolutions better

			if (lifetimes[i] <= 0.0f) {
				if (bursting[i]) {					
					create_burst(200, data[i].location, 0.003);
					remove_projectile(i);
				}
			}
		}
		data_mutex.unlock();
	}

	void remove_projectile(size_t index) {
		data.erase(data.begin() + index);
		directions.erase(directions.begin() + index);
		lifetimes.erase(lifetimes.begin() + index);
		bursting.erase(bursting.begin() + index);
	}

	void add_projectile(glm::vec3 location, glm::vec3 direction, float lifetime, bool burst = false) {
		data_mutex.lock();

		add_data(location);
		directions.push_back(direction);
		lifetimes.push_back(lifetime);
		bursting.push_back(burst);

		data_mutex.unlock();
	}

	void add_projectile(glm::vec3 location, float heading, float elevation, float speed, float lifetime, float offset = 0.0f, bool burst = false) {
		glm::vec3 direction;

		direction.x = cosf(elevation) * sinf(heading);
		direction.y = sinf(elevation);
		direction.z = cosf(elevation) * cosf(heading);
		location += offset * direction;

		if (!burst)
			speed *= 2;

		direction *= speed;

		local_forward = glm::normalize(direction);
		location += local_forward * 8.0f;

		add_projectile(location, direction, lifetime, burst);
	}

	void hit_index(size_t index) override {
		directions[index] = glm::vec3(0, 0, 0);
		remove_projectile(index);
	}
};

class fragment : public loaded_object {
public:
	std::vector<float> life_counts;
	std::vector<glm::vec3> trajectories;

	fragment(const char* id) : loaded_object("projectile.obj", "brick.jpg", glm::vec3(1.0f, 1.0f, 1.0f), id) {
		collision_check = false;
	}

	void create_burst(float quantity, glm::vec3 origin, float speed) {
		for (size_t i = 0; i < quantity; i++) {
			add_data(origin);
			life_counts.push_back(1000.0f);

			// One note:  This does create a cube of projectiles
			trajectories.push_back(glm::vec3(randvel(speed), randvel(speed), randvel(speed)) * 0.2f);
		}
	}

	void move(int elapsed_time) override {
		for (size_t i = 0; i < data.size(); i++) {
			life_counts[i] -= 0.01f;
			data[i].location += trajectories[i] * (elapsed_time / 1000.0f);
			// Is it on the ground?
			// Import player fall code to make this more elaborate and probably buggy
			if (data[i].location.y <= 0.0) {
				trajectories[i].y = fabs(trajectories[i].y);

				if (fabs(trajectories[i].x) < 0.02)
					trajectories[i].x = 0.0f;
				else
					trajectories[i].x *= 0.95f;

				if (trajectories[i].y < 0.2)
					trajectories[i].y = 0.0f;
				else
					trajectories[i].y *= 0.8f;

				if (fabs(trajectories[i].z) < 0.02)
					trajectories[i].z = 0.0f;
				else
					trajectories[i].z *= 0.95f;
			}
			else {
				trajectories[i].y -= 0.001;
			}
		}
	}

	void draw(glm::mat4 vp) override {
		glUseProgram(program);
		std::vector<glm::mat4> models;
		models.reserve(data.size());

		for (size_t i = 0; i < data.size(); i++) {
			glm::mat4 new_model = glm::mat4(1.0f);
			new_model = translate(new_model, data[i].location);

			if (fabs(trajectories[i].x) > 0.0f || fabs(trajectories[i].z) > 0.0f)
				new_model = rotate(new_model, life_counts[i], glm::vec3(-trajectories[i].z, 0, trajectories[i].x));

			models.push_back(new_model);
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, models_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, models_buffer);

		glEnableVertexAttribArray(v_attrib);
		glBindBuffer(GL_ARRAY_BUFFER, vbuf);
		glVertexAttribPointer(v_attrib, 3, GL_FLOAT, GL_FALSE, 32, 0);

		glEnableVertexAttribArray(t_attrib);
		glVertexAttribPointer(t_attrib, 2, GL_FLOAT, GL_FALSE, 32, (const void*)24);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		int size;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

		glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

		glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0, models.size());
	}

};

