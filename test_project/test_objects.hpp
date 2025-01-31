#pragma once

#include "game.hpp"

class weirdbox : public gameobject {
public:
	unsigned int mvp_uniform, anim_uniform, v_attrib, c_attrib, program, vbuf, cbuf, ebuf, acount;
	glm::mat4 anim;
	int init() override;
	void draw(glm::mat4 vp) override;
	void animate() override;
	void deinit() override;
};

class lightbox : virtual public gameobject {
public:
	unsigned int mvp_uniform, anim_uniform, v_attrib, c_attrib, program, vbuf, cbuf, ebuf;
	int init() override;
	void draw(glm::mat4 vp) override;
};

class floorbox : public gameobject {
public:
	unsigned int mvp_uniform, anim_uniform, v_attrib, c_attrib, t_attrib, program, vbuf, cbuf, ebuf, tex;
	int init() override;
	void draw(glm::mat4 vp) override;
};
