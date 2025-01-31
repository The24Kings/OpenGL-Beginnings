#pragma once

#include "game.hpp"

class lava_floor : public gameobject {
public:
	lava_floor(const char* id) {
		identifier = id;
	}

	unsigned int mvp_uniform, anim_uniform, v_attrib, c_attrib, t_attrib, program, vbuf, cbuf, ebuf, tex;

	int init() {
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

		tex = load_texture("lava.png");

		program = make_program("fb_vertex_shader.glsl", 0, 0, 0, "fb_fragment_shader.glsl");
		if (!program)
			return 1;

		v_attrib = glGetAttribLocation(program, "in_vertex");
		t_attrib = glGetAttribLocation(program, "in_texcoord");
		mvp_uniform = glGetUniformLocation(program, "mvp");
		return 0;
	}

	void draw(glm::mat4 vp) {
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

};