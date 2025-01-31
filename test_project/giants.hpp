#include "game.hpp"

class giant : public loaded_object {
public:
	int loopcount = 0;
	float angle = 0.0f;

	giant(const char* id) : loaded_object("fourgiant.obj", "giant_00_0.png", glm::vec3(2, 2, 2), id) {
	}

	void move(int elapsed_time) { // Do this per model rather than per object
		//TODO: Rotate along the local up axis
	}

};