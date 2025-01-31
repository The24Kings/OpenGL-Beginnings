#ifndef GAME_HPP
#define GAME_HPP

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include "glm\gtx\quaternion.hpp"
#include<glm/gtc/type_ptr.hpp>
#include<vector>
#include<thread>
#include<chrono>
#include<mutex>
#include <algorithm>
#include "scolor.hpp"

#define _USE_MATH_DEFINES
#include<math.h>
#define GRAVITY 0.005f

struct key_status {
	bool forward, backward, left, right,
		arrow_up, arrow_down, arrow_left, arrow_right;
};

struct player {
	key_status key_status;
	glm::vec3 hitbox;
	glm::vec3 position;
	glm::vec2 heading;
	glm::vec3 look_at_point;
	int health, points;
	float elevation, fall_speed, height;
	bool moving_forward, on_ground, paused;
};

struct vertex { // 32 bytes: texCoords is 24 bytes offset into the struct
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	bool operator==(const vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
}; //TODO: alignment

int load_model(std::vector<vertex>& verticies, std::vector<uint32_t>& indices, const char* filename, glm::vec2 texture_scale = glm::vec2(1, 1));
unsigned int load_texture(const char* filename);GLuint make_program(const char* v_file, const char* tcs_file, const char* tes_file, const char* g_file, const char* f_file);
GLuint make_shader(const char* filename, GLenum shaderType);
float randvel(float speed);
void init_helpers();
void free_helpers();

/**
 * @param location The location of the object
 * @param angle The angle of the object
 * @param scalar The scalar of the object
 */
struct object_data {
	glm::vec3 location;
	glm::quat angle;
	glm::vec3 scalar;
};

class gameobject {
public:
	const char* identifier;
	std::mutex data_mutex;
	unsigned int mvp_uniform, v_attrib, t_attrib, program, vbuf, cbuf, ebuf, tex, models_buffer;
	bool visible = true;
	bool collision_check = false; // Consider separating out player from projectile collision here
	std::vector<object_data> data;
	glm::vec3 size; // What about non-square objects?
	virtual int init() { return 0; }
	virtual void deinit() {};
	virtual void draw(glm::mat4) {}
	virtual void add_data(glm::vec3 location, glm::quat angle = glm::quat(1, 0, 0, 0), glm::vec3 scalar = glm::vec3(1)) { data.push_back(object_data{ location, angle, scalar }); }
	virtual std::vector<glm::mat4> create_models() { return std::vector<glm::mat4>(); };
	virtual void move(int elapsed_time) {}
	virtual void animate() {}
	virtual void flip() {}
	virtual bool is_on_idx(glm::vec3 position, size_t index, float height) { return false; }
	virtual int is_on(glm::vec3 position, float height) { return -1; }
	virtual int collision_index(glm::vec3 position, float distance = 0) { return -1; }
	virtual int collision_index_with_direction(glm::vec3 position, int rotation, float distanceFront = 0, float distanceLeft = 0) { return -1; }
	virtual glm::vec3 collision_normal(glm::vec3 move_to, glm::vec3 old_position, size_t index, float distance = 0) { return glm::vec3(0, 0, 0); }
	virtual bool collision_with_index(glm::vec3 position, size_t index, float distance = 0) { return false; }
	virtual void hit_index(size_t index) {}
};

class loaded_object : virtual public gameobject {
public:
	const char* objectfile, *texturefile;
	glm::vec2 texture_scale = glm::vec2(1, 1);
	loaded_object(const char* of, const char* tf, glm::vec3 s, const char* id) : objectfile(of), texturefile(tf) { size = s; identifier = id; }

	int init() override;
	void draw(glm::mat4 vp) override;
};

class block_object : virtual public gameobject {
public:
	bool is_on_idx(glm::vec3 position, size_t index, float height) override;
	int is_on(glm::vec3 position, float height) override;
	int collision_index(glm::vec3 position, float distance = 0) override;
	glm::vec3 collision_normal(glm::vec3 move_to, glm::vec3 old_position, size_t index, float distance = 0) override;
	bool collision_with_index(glm::vec3 position, size_t index, float distance = 0) override;

	int collision_index_with_direction(glm::vec3 position, int rotation, float distanceFront = 0, float distanceLeft = 0) override;
};

class wall_block : public loaded_object, public block_object {
public:
	wall_block(const char* of, const char* tf, glm::vec3 s, const char* id) : loaded_object(of, tf, s, id) {
		collision_check = true;
	}
};

/* Global variables that are really all the way global */
#ifdef MAIN
#define EXTERN
#define INIT(X) = X
#else
#define EXTERN extern
#define INIT(x)
#endif

/* Player globals */
EXTERN struct player player;
EXTERN gameobject* player_platform INIT(0);
EXTERN size_t player_platform_index INIT(0);
EXTERN int framecount INIT(0);
EXTERN double framerate INIT(0);
EXTERN double delta_time INIT(0);
EXTERN glm::vec3 local_up INIT(glm::vec3(0, 1, 0));
EXTERN glm::vec3 local_right INIT(glm::vec3(1, 0, 0));
EXTERN glm::vec3 local_forward INIT(glm::vec3(0, 0, 1));

#endif
