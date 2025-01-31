#ifndef HELPERS_CPP
#define HELPERS_CPP

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <stdexcept>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>

#include "game.hpp"
#include "tiny_obj_loader.h"
#include "stb_image.h"
#include "scolor.hpp"

#define GBLEN 100000
char* general_buffer;

void init_helpers() {
	general_buffer = (char*)malloc(GBLEN);
}

void free_helpers() {
	free(general_buffer);
}

float randvel(float speed) {
	long min = -100;
	long max = 100;
	return speed * (min + rand() % (max + 1 - min));
}

namespace std {
	template<> struct hash<vertex> {
		size_t operator()(vertex const& v) const {
			return ((hash<glm::vec3>()(v.pos) ^ (hash<glm::vec3>()(v.color) << 1)) >> 1) ^ (hash<glm::vec2>()(v.texCoord) << 1);
		}
	};
}

// This loads straight into the GPU
unsigned load_texture(const char* filename) {
	int texwidth, texheight;
	unsigned tex = NULL;
	unsigned char* image_data = stbi_load(filename, &texwidth, &texheight, 0, STBI_rgb_alpha);

	if (!image_data) {
		printf("Failed to load texture %s\n", filename);
		return tex;
	}

	printf("Loaded texture %d by %d\n", texwidth, texheight);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	free(image_data);

	return tex;
}

int load_model(std::vector<vertex>& vertices, std::vector<uint32_t>& indices, const char* filename, glm::vec2 texture_scale) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename)) {
		throw std::runtime_error(warn + err);
		return 1;
	}

	std::unordered_map<vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.texCoord *= texture_scale;
			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = (uint32_t)vertices.size();
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	return 0;
}

GLuint make_shader(const char* filename, GLenum shaderType) {
	FILE* fd;

	fopen_s (&fd, filename, "r");

	if (fd == 0) {
		printf("File not found:  %s\n", filename);
		return 0;
	}
	size_t readlen = fread(general_buffer, 1, GBLEN, fd);
	fclose(fd);
	if (readlen == GBLEN) {
		printf(RED("Buffer Length of %d bytes Inadequate for File %s\n").c_str(), GBLEN, filename);
		return 0;
	}
	if (readlen == 0) {
		puts(RED("File read problem, read 0 bytes").c_str());
		return 0;
	}
	general_buffer[readlen] = 0;
	printf(DGREEN("Read shader in file %s (%d bytes)\n").c_str(), filename, readlen);
	puts(general_buffer);
	unsigned int s_reference = glCreateShader(shaderType);
	glShaderSource(s_reference, 1, (const char**)&general_buffer, 0);
	glCompileShader(s_reference);
	glGetShaderInfoLog(s_reference, GBLEN, NULL, general_buffer);
	puts(general_buffer);
	GLint compile_ok;
	glGetShaderiv(s_reference, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok) {
		puts(GREEN("Compile Success").c_str());
		return s_reference;
	}
	puts(RED("Compile Failed\n").c_str());
	return 0;
}

GLuint make_program(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file) {
	unsigned int vs_reference = make_shader(v_file, GL_VERTEX_SHADER);
	unsigned int tcs_reference = 0, tes_reference = 0;
	if (tcs_file)
		if (!(tcs_reference = make_shader(tcs_file, GL_TESS_CONTROL_SHADER)))
			return 0;
	if (tes_file)
		if (!(tes_reference = make_shader(tes_file, GL_TESS_EVALUATION_SHADER)))
			return 0;
	unsigned int gs_reference = 0;
	if (g_file)
		gs_reference = make_shader(g_file, GL_GEOMETRY_SHADER);
	unsigned int fs_reference = make_shader(f_file, GL_FRAGMENT_SHADER);
	if (!(vs_reference && fs_reference))
		return 0;
	if (g_file && !gs_reference)
		return 0;


	unsigned int program = glCreateProgram();
	glAttachShader(program, vs_reference);
	if (g_file)
		glAttachShader(program, gs_reference);
	if (tcs_file)
		glAttachShader(program, tcs_reference);
	if (tes_file)
		glAttachShader(program, tes_reference);
	glAttachShader(program, fs_reference);
	glLinkProgram(program);
	GLint link_ok;
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		glGetProgramInfoLog(program, GBLEN, NULL, general_buffer);
		puts(general_buffer);
		puts(RED("Link Failed").c_str());
		return 0;
	}

	return program;
}

#endif