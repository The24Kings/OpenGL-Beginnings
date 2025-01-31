#pragma once

class elevator : public loaded_object, public block_object {
public:
	glm::vec3 direction;
	glm::vec3 distance;
	glm::vec3 location;
	bool positive;
	bool pos_x, pos_y, pos_z;

	elevator(const char* of, const char* tf, glm::vec3 s, const char* id, glm::vec3 d, glm::vec3 dis, glm::vec3 loc) : loaded_object(of, tf, s, id) {
		direction = d;
		distance = dis;
		location = loc;

		pos_x = direction.x > 0;
		pos_y = direction.y > 0;
		pos_z = direction.z > 0;

		if (direction.x > 0 || direction.y > 0 || direction.z > 0)
			positive = true;
		else
			positive = false;
	}

	void draw(glm::mat4 vp) {
		loaded_object::draw(vp);
	}

	void move(int elapsed_time) {
		for (int i = 0; i < data.size(); i++) {
			data[i].location += direction * 0.1f;

			// Positive direction
			if (pos_x && (data[i].location.x > (location.x + distance.x) || data[i].location.x < location.x)) {
				direction.x = -direction.x;
			}
			if (pos_y && (data[i].location.y > (location.y + distance.y) || data[i].location.y < location.y)) {
				direction.y = -direction.y;
			}
			if (pos_z && (data[i].location.z > (location.z + distance.z) || data[i].location.z < location.z)) {
				direction.z = -direction.z;
			}

			// Negative direction
			if (!pos_x && (data[i].location.x > location.x || data[i].location.x < (location.x - distance.x))) {
				direction.x = -direction.x;
			}
			if (!pos_y && (data[i].location.y > location.y || data[i].location.y < (location.y - distance.y))) {
				direction.y = -direction.y;
			}
			if (!pos_z && (data[i].location.z > location.z || data[i].location.z < (location.z - distance.z))) {
				direction.z = -direction.z;
			}
			/*/
			// If the elevator has reached the end of its relative path, reverse direction
			if (positive) {
				if (data[i].location.x > (location.x + distance.x) || data[i].location.x < location.x ||
					data[i].location.z > (location.z + distance.z) || data[i].location.z < location.z ||
					data[i].location.y > (location.y + distance.y) || data[i].location.y < location.y)
				{
					direction = -direction;
				}
			} else {
				if (data[i].location.x > location.x || data[i].location.x < (location.x - distance.x) ||
					data[i].location.z > location.z || data[i].location.z < (location.z - distance.z) ||
					data[i].location.y > location.y || data[i].location.y < (location.y - distance.y))
				{
					direction = -direction;
				}
			}*/
		}
	}
};