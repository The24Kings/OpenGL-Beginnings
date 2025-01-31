#include "game.hpp"
#include "test_objects.hpp"

int weirdbox::init() {
	// Initialization part
	float vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
	};
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	float colors[] = {
		// front colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		// back colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
	};
	glGenBuffers(1, &cbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	glGenBuffers(1, &ebuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	program = make_program("vertex_shader.glsl", 0, 0, 0, "fragment_shader.glsl");
	if (!program)
		return 1;

	v_attrib = glGetAttribLocation(program, "in_vertex");
	c_attrib = glGetAttribLocation(program, "in_color");
	mvp_uniform = glGetUniformLocation(program, "mvp");
	anim_uniform = glGetUniformLocation(program, "animation");
	return 0;
}
void weirdbox::draw(glm::mat4 vp) {
	glUseProgram(program);

	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(c_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, cbuf);
	glVertexAttribPointer(c_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);


	glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));
	glUniformMatrix4fv(anim_uniform, 1, 0, (float*)&anim);

	glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0, 10);
}
void weirdbox::animate() {
	acount++;
	anim = glm::rotate(glm::mat4(1.0f), acount / 20.0f, glm::vec3(0, 1, 0));
	anim = glm::scale(anim, glm::vec3(1 - .5f * sinf(acount / 10.0f), 3 + 2 * sinf(acount / 10.0f), 1 - 0.5 * sinf(acount / 10.0f)));
}

void weirdbox::deinit() {
	glDeleteProgram(program);
	// And the vertex, color, and element buffers
	// And the shaders	
}

int lightbox::init() {
	// Initialization part
	float vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
	};
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	float colors[] = {
		// front colors
		1.0, 0.0, 1.0,
		1.0, 0.5, 1.0,
		1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
		// back colors
		0.5, 1.0, 0.5,
		0.5, 1.0, 0.5,
		1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
	};
	glGenBuffers(1, &cbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	glGenBuffers(1, &ebuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	program = make_program("other_vertex_shader.glsl", 0, 0, 0, "fragment_shader.glsl");
	if (!program)
		return 1;

	v_attrib = glGetAttribLocation(program, "in_vertex");
	c_attrib = glGetAttribLocation(program, "in_color");
	mvp_uniform = glGetUniformLocation(program, "mvp");
	return 0;
}

void lightbox::draw(glm::mat4 vp) {
	glUseProgram(program);

	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(c_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, cbuf);
	glVertexAttribPointer(c_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	int size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

	glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0, 1);
}

int floorbox::init() {
	// Initialization part
	float vertices[] = {
		1.0, -1.0,  1.0,
		1.0, -1.0,  -1.0,
		-1.0, -1.0,  -1.0,
		-1.0, -1.0,  1.0,
	};

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLushort floor_elements[] = {
		0, 1, 2, 2, 3, 0,

	};

	glGenBuffers(1, &ebuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_elements), floor_elements, GL_STATIC_DRAW);

	tex = load_texture("oak_planks.png");

	program = make_program("fb_vertex_shader.glsl", 0, 0, 0, "fb_fragment_shader.glsl");
	if (!program)
		return 1;

	v_attrib = glGetAttribLocation(program, "in_vertex");
	t_attrib = glGetAttribLocation(program, "in_texcoord");
	mvp_uniform = glGetUniformLocation(program, "mvp");
	return 0;
}

void floorbox::draw(glm::mat4 vp) {
	glUseProgram(program);

	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(t_attrib);
	glVertexAttribPointer(t_attrib, 2, GL_FLOAT, GL_FALSE, 32, (const void*)24);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	int size;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUniformMatrix4fv(mvp_uniform, 1, 0, glm::value_ptr(vp));

	glDrawElementsInstanced(GL_TRIANGLES, size / sizeof(uint16_t), GL_UNSIGNED_SHORT, 0, 10000);
}