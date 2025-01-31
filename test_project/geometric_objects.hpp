#pragma once

/* TODO:
 * 	Collision detection (can't move through panel)
 */
class flat_panel : public gameobject {
public:
	std::vector<glm::vec2> object_scales, texture_scales;
	std::vector<glm::vec4> normals, ups;
	unsigned scales_buffer, normals_buffer, ups_buffer, object_scales_buffer;
	const char* texturefile;
	flat_panel(const char* tf) : texturefile(tf) {}
	int init() override;
	void draw(glm::mat4 vp) override;
	std::vector<glm::mat4> create_models();
	void addpanel(glm::vec3 location, glm::vec2 object_scale, glm::vec2 texture_scale, glm::vec3 normal = glm::vec3(0, 1, 0), glm::vec3 up = glm::vec3(0, 0, 1));
	void rotate(size_t index, float angle, glm::vec3 axis);
	void set_normal(size_t index, glm::vec3 normal);
	void set_up(size_t index, glm::vec3 normal);
};

class rotating_panel : public flat_panel {
public:
	float angle = 0.0f;
	rotating_panel(const char* tf) : flat_panel(tf) {}
	void move(int elapsed_time) {
		angle = elapsed_time * 0.000003f;
		for (size_t i = 0; i < normals.size(); i++) {
			normals[i] = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(ups[i].x, ups[i].y, ups[i].z)) * normals[i];
		}
	}
};

class y_rotating_panel : public flat_panel {
public:
	float angle = 0.0f;
	y_rotating_panel(const char* tf) : flat_panel(tf) {}
	void move(int elapsed_time) {
		angle = elapsed_time * 0.000003f;
		for (size_t i = 0; i < normals.size(); i++) {
			normals[i] = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * normals[i];
			ups[i] = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * ups[i];
		}
	}
};

class face_player_panel : public flat_panel {
public:
	face_player_panel(const char* tf) : flat_panel(tf) {}
	void move(int elapsed_time) {
		for (size_t i = 0; i < data.size(); i++)
			set_normal(i, (data[i].location - player.position));
	}
};
