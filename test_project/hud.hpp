#pragma once

class aimpoint : public gameobject {
public:
	int init();
	void draw(glm::mat4 vp);
};

class hud : public gameobject {
public:
	float texcoords[3600];
	int char_width = 100;
	float y_space = 0.02f;

	int init();
	void draw(glm::mat4 vp);
	void set_char(int col, int line, char c);
	void lprintf(int line, const char* fmt, ...);
	void set_text_line(int line, const char* text);
	void set_text(const char* text);
};