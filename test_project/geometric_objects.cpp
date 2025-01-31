#include "game.hpp"
#include "geometric_objects.hpp"

int flat_panel::init() {
	// Initialization part
	float vertices[] = {
		-1.0, 1.0,
		1.0, 1.0,
		1.0, -1.0,
		-1.0, 1.0,
		1.0, -1.0,
		-1.0, -1.0,
	};
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	program = make_program("normal_panel_vertex_shader.glsl", 0, 0, 0, "loaded_object_fragment_shader.glsl");
	if (!program)
		return 1;

	tex = load_texture(texturefile);

	glGenBuffers(1, &models_buffer);
	glGenBuffers(1, &scales_buffer);
	glGenBuffers(1, &normals_buffer);
	glGenBuffers(1, &ups_buffer);
	glGenBuffers(1, &object_scales_buffer);

	v_attrib = glGetAttribLocation(program, "in_vertex");
	mvp_uniform = glGetUniformLocation(program, "vp");
	return 0;
}

void flat_panel::addpanel(glm::vec3 location, glm::vec2 object_scale, glm::vec2 texture_scale, glm::vec3 normal, glm::vec3 up) {
	add_data(location);
	object_scales.push_back(object_scale);
	texture_scales.push_back(texture_scale);
	normals.push_back(glm::vec4(glm::normalize(normal), 1.0f));
	ups.push_back(glm::vec4(glm::normalize(up), 1.0f));
}

void flat_panel::rotate(size_t index, float angle, glm::vec3 axis) {

}

void flat_panel::set_normal(size_t index, glm::vec3 normal) {
	normals[index] = glm::vec4(glm::normalize(normal), 1.0f);
}

void flat_panel::set_up(size_t index, glm::vec3 up) {
	ups[index] = glm::vec4(glm::normalize(up), 1.0f);
}

std::vector<glm::mat4> flat_panel::create_models() {
	std::vector<glm::mat4> models;
	models.reserve(data.size());
	for (size_t i = 0; i < data.size(); i++) {
		glm::mat4 new_model = glm::mat4(1.0f);

		new_model = glm::translate(new_model, data[i].location);
		models.push_back(new_model);
	}
	return models;
}

void flat_panel::draw(glm::mat4 vp) {
	glUseProgram(program);
	data_mutex.lock();
	std::vector<glm::mat4> models = create_models();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, models_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, models.size() * sizeof(glm::mat4), models.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, models_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, scales_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, texture_scales.size() * sizeof(glm::vec2), texture_scales.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, scales_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, normals_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, normals.size() * sizeof(normals[0]), normals.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, normals_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ups_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, ups.size() * sizeof(ups[0]), ups.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ups_buffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, object_scales_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, object_scales.size() * sizeof(object_scales[0]), object_scales.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, object_scales_buffer);
	data_mutex.unlock();

	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, models.size());
}