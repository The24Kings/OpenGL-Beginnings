#include "game.hpp"

int loaded_object::init() {
	// Initialization part
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices; // Consider unified terminology

	load_model(vertices, indices, objectfile, texture_scale);

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// TODO:  Remember to explain the layout later

	glGenBuffers(1, &ebuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &models_buffer);

	tex = load_texture(texturefile);

	program = make_program("loaded_object_vertex_shader.glsl", 0, 0, 0, "loaded_object_fragment_shader.glsl");
	if (!program)
		return 1;

	v_attrib = glGetAttribLocation(program, "in_vertex");
	t_attrib = glGetAttribLocation(program, "in_texcoord");
	mvp_uniform = glGetUniformLocation(program, "vp");
	return 0;
}

void loaded_object::draw(glm::mat4 vp) {
	if (!tex) {
		return;
	}
	
	glUseProgram(program);
	std::vector<glm::mat4> models;
	data_mutex.lock();
	models.reserve(data.size());

	for (size_t i = 0; i < data.size(); i++) {
		glm::mat4 new_model = glm::mat4(1.0f);

		new_model = translate(new_model, data[i].location);
		new_model = new_model * glm::toMat4(data[i].angle);
		new_model = glm::scale(new_model, data[i].scalar);

		models.push_back(new_model);
	}

	data_mutex.unlock();

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

bool block_object::is_on_idx(glm::vec3 position, size_t index, float height) {
	return (0.0f < (position.y - data[index].location.y) &&
		1.0f > (position.y - height) - (data[index].location.y + size.y / 2) &&
		size.x / 2 > fabs(position.x - data[index].location.x) &&
		size.z / 2 > fabs(position.z - data[index].location.z));

}

int block_object::is_on(glm::vec3 position, float height) {
	for (size_t i = 0; i < data.size(); i++)
		if (is_on_idx(position, i, height))
			return i;
	return -1;
}

int block_object::collision_index(glm::vec3 position, float distance) {
	for (size_t i = 0; i < data.size(); i++) {
		glm::vec3 l = data[i].location; // This'll get optimized out

		if (size.x / 2.0f + distance > abs(l.x - position.x) &&
			size.y / 2.0f + distance > abs(l.y - position.y) &&
			size.z / 2.0f + distance > abs(l.z - position.z)) {
			return i;
		}
	}
	return -1;
}

int block_object::collision_index_with_direction(glm::vec3 position, int rotation, float distanceFront, float distanceLeft) {
	// Calculate the direction vector based on the runner's rotation
	glm::vec3 direction = glm::vec3(0, 0, 0);

	if (rotation == 0) {
		direction = glm::vec3((distanceLeft), 0, distanceFront - 1.0f); // Facing forward
	}
	else if (rotation == 90) {
		direction = glm::vec3((distanceFront + 1.8f), 0, -distanceLeft); // Facing left
	}
	else if (rotation == 180) {
		direction = glm::vec3(-distanceLeft, 0, -(distanceFront - 1.0f)); // Facing backward
	}
	else if (rotation == 270) {
		direction = glm::vec3(-(distanceFront + 1.8f), 0, distanceLeft); // Facing right
	}

	// Calculate the position in front of the runner
	glm::vec3 check_pos = position + direction;

	//printf("\nPosition: ((% f, % f, % f)\n", position.x, position.y, position.z);
	//printf("Direction: (%f, %f, %f)\n", direction.x, direction.y, direction.z);
	//printf("Check position: (%f, %f, %f)\n", check_pos.x, check_pos.y, check_pos.z);

	// Iterate through the locations and check if any object is in front of the runner
	for (size_t i = 0; i < data.size(); i++) {
		glm::vec3 l = data[i].location;

		if (rotation == 0 && l.z > check_pos.z) {
			// Limit the check to the immediate direction in front of the runner
			if ((size.x / 2.0f) + distanceLeft > abs(l.x - check_pos.x) &&
				(size.z / 2.0f) + distanceFront > abs(l.z - check_pos.z)) {

				//locations.erase(locations.begin() + i);
				return i;
			}
		}
		else if (rotation == 90 && l.x < check_pos.x) {
			if ((size.x / 2.0f) + distanceFront > abs(l.x - check_pos.x) &&
				(size.z / 2.0f) + distanceLeft > abs(l.z - check_pos.z)) {

				//locations.erase(locations.begin() + i);
				return i;
			}
		}
		else if (rotation == 180 && l.z < check_pos.z) {
			if ((size.x / 2.0f) + distanceLeft > abs(l.x - check_pos.x) &&
				(size.z / 2.0f) + distanceFront > abs(l.z - check_pos.z)) {

				//locations.erase(locations.begin() + i);
				return i;
			}
		}
		else if (rotation == 270 && l.x > check_pos.x) {
			if ((size.x / 2.0f) + distanceFront > abs(l.x - check_pos.x) &&
				(size.z / 2.0f) + distanceLeft > abs(l.z - check_pos.z)) {

				//locations.erase(locations.begin() + i);
				return i;
			}
		}
	}

	return -1; // No object found in the immediate direction in front of the runner
}

/* Leaving y for later, so we finish today */
glm::vec3 block_object::collision_normal(glm::vec3 move_to, glm::vec3 old_position, size_t index, float distance) {
	glm::vec3 l = data[index].location; // This'll get optimized out
	if (old_position.z > l.z + (size.z / 2 + distance) &&
		old_position.x >= l.x - (size.x / 2 + distance) &&
		old_position.x <= l.x + (size.x / 2 + distance)) {
		return glm::vec3(0, 0, 1);
	}
	if (old_position.z < l.z - (size.z / 2 + distance) &&
		old_position.x >= l.x - (size.x / 2 + distance) &&
		old_position.x <= l.x + (size.x / 2 + distance)) {
		return glm::vec3(0, 0, -1);
	}
	if (old_position.x < l.x - (size.x / 2 + distance) &&
		old_position.z >= l.z - (size.z / 2 + distance) &&
		old_position.z <= l.z + (size.z / 2 + distance)) {
		return glm::vec3(1, 0, 0);
	}
	if (old_position.x > l.x + (size.x / 2 + distance) &&
		old_position.z >= l.z - (size.z / 2 + distance) &&
		old_position.z <= l.z + (size.z / 2 + distance)) {
		return glm::vec3(-1, 0, 0);
	}
	puts("Ended collision normal without returning");
	return glm::vec3(0, 0, 0);
}

bool block_object::collision_with_index(glm::vec3 position, size_t index, float distance) {
	glm::vec3 l = data[index].location; // This variable will get optimized out
	if (size.x / 2.0f + distance > abs(l.x - position.x) &&
		size.y / 2.0f + distance > abs(l.y - position.y) &&
		size.z / 2.0f + distance > abs(l.z - position.z)) {
		return true;
	}
	return false;
}