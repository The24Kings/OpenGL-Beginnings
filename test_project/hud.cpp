#include "game.hpp"
#include "hud.hpp"
#include<stdarg.h>

int aimpoint::init() {
	float vertices[]{
		-.025, .002,  0,
		.025, .002,  0,
		.025,  -.002, 0,
		-.025, .002,  0,
		.025, -.002, 0,
		-.025, -.002, 0,

		-.002, .04,  0,
		.002, .04,  0,
		.002,  -.04, 0,
		-.002, .04,  0,
		.002, -.04, 0,
		-.002, -.04, 0,
	};

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	program = make_program("aimpoint_vertex_shader.glsl", 0, 0, 0, "aimpoint_fragment_shader.glsl");

	if (!program)
		return 1;

	v_attrib = glGetAttribLocation(program, "in_vertex");

	return 0;
}

void aimpoint::draw(glm::mat4 vp) {
	glUseProgram(program);
	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 12);
}

void hud::set_char(int col, int line, char c) {
	float position = c - 32;
	position /= 95;
	size_t cstart = (col + line * char_width) * 12;

	texcoords[cstart + 0] = position;
	texcoords[cstart + 1] = 1.0f;
	texcoords[cstart + 2] = position + 1.0f / 95;
	texcoords[cstart + 3] = 1.0f;
	texcoords[cstart + 4] = position;
	texcoords[cstart + 5] = 0.0f;
	texcoords[cstart + 6] = position + 1.0f / 95;
	texcoords[cstart + 7] = 1.0f;
	texcoords[cstart + 8] = position + 1.0f / 95;
	texcoords[cstart + 9] = 0.0f;
	texcoords[cstart + 10] = position;
	texcoords[cstart + 11] = 0.0f;
}

void hud::lprintf(int line, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char text[100];

	vsprintf_s(text, fmt, args);
	set_text_line(line, text);
}

void hud::set_text_line(int line, const char* text) {
	for (int i = line * 1200; i < (line + 1) * 1200; i++)
		texcoords[i] = 0;

	int col = 0;

	for (; *text; text++) {
		set_char(col, line, *text);
		col++;
	}
}

void hud::set_text(const char* text) {
	for (int i = 0; i < 3600; i++)
		texcoords[i] = 0;

	int col = 0, line = 0;

	for (; *text; text++)
		if (*text != '\n') {
			set_char(col, line, *text);
			col++;
		}
		else {
			col = 0;
			line++;
		}
}

int hud::init() {
	std::vector<float> vertices;
	vertices.reserve(3600);

	/* 100 character display, 3 lines */
	for (float y = y_space; y >= -y_space; y -= y_space)
		for (int i = 0; i < char_width; i++) {
			vertices.push_back(0.01f * i);
			vertices.push_back(y);

			vertices.push_back(0.01f * (i + 1));
			vertices.push_back(y);

			vertices.push_back(0.01f * i);
			vertices.push_back(y - y_space);

			vertices.push_back(0.01f * (i + 1));
			vertices.push_back(y);

			vertices.push_back(0.01f * (i + 1));
			vertices.push_back(y - y_space);

			vertices.push_back(0.01f * i);
			vertices.push_back(y - y_space);

		}

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	for (int i = 0; i < char_width * 3; i++)
		set_char(i % char_width, i / char_width, '#');

	glGenBuffers(1, &cbuf);

	tex = load_texture("letters.png");

	program = make_program("hud_vertex_shader.glsl", 0, 0, 0, "hud_fragment_shader.glsl");

	if (!program)
		return 1;

	v_attrib = glGetAttribLocation(program, "in_vertex");
	t_attrib = glGetAttribLocation(program, "in_texcoord");

	return 0;
}

void hud::draw(glm::mat4 vp) {
	glUseProgram(program);
	glEnableVertexAttribArray(v_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glVertexAttribPointer(v_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cbuf);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(t_attrib);
	glBindBuffer(GL_ARRAY_BUFFER, cbuf);
	glVertexAttribPointer(t_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glDrawArrays(GL_TRIANGLES, 0, 1800);
}