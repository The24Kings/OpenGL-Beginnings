/*
   Simplified OpenGL 4.5 demo
 * Seth Long, Fall 2020
 * This is a *very* short demo which displays a triangle
 * Many of these functions can fail, and return error values
 * I doubt it can be done much shorter without leaving vertices in only main memory or some such
 */
#include "game.hpp"

#include "test_objects.hpp"
#include "projectiles.hpp"
#include "elevator.hpp"
#include "target.hpp"
#include "flipper.hpp"
#include "turret.hpp"
#include "player_marker.hpp"
#include "flying_enemy.hpp"
#include "platform.hpp"
#include "lava_floor.hpp"
#include "geometric_objects.hpp"
#include "hud.hpp"
#include "maze_runner.hpp"
#include "moon.hpp"
#include "giants.hpp"
#include "octorok.hpp"

struct player player;

std::mutex grand_mutex;

gameobject* player_platform = 0;
size_t player_platform_index = 0;

/* Global Section */
float height = 1080;
float width = 1920;

bool shutdown_engine = false;
bool run_maze = false;
bool octo_start = false;

int framecount = 0;
int time_resolution = 10; // Milliseconds
int	loopcount = 0;
int target_framerate = 60;
double framerate = 0;
double delta_time = 0;

float MOVEMENT_MULT = 0.2f;

glm::vec3 local_up;
glm::vec3 local_right;
glm::vec3 local_forward;

std::vector<gameobject*> objects;
std::vector<std::vector<std::vector<char>>> octo_waves; // Used for the octorok waves
maze_runner runner("runner", &run_maze);
projectile ice_balls("ice");
flying_enemy t("turret");
hud main_hud;

octorok target_octo("octorok.obj", "red_octorok.png", glm::vec3(8, 24, 8), "target_octo");
octorok obstacle_octo("octorok.obj", "blue_octorok.png", glm::vec3(8, 24, 8), "obstacle_octo");
wall_block maze("cube.obj", "brick.jpg", glm::vec3(2, 2, 2), "wall");

/* Function Prototypes */
void player_movement();
void object_movement();
void animation();
void collision();
void octo_game();
void flipper_toggle();
void fire(bool burst);
void make_wall(gameobject* o, glm::vec3 location, glm::vec3 size);
bool is_empty(glm::vec3 position, float distance);
bool is_empty_direction(glm::vec3 position, float distance, int rotation);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void pos_callback(GLFWwindow* window, double x, double y);
void resize_callback(GLFWwindow* window, int width, int height);
void handle_mousebutton(GLFWwindow* window, int button, int action, int mods);
void spawn_wave(std::vector<std::vector<char>> wave, octorok* target, octorok* obstacle);

int main(int argc, char** argv) {
	init_helpers();

	if (!glfwInit()) {
		puts(RED("GLFW init error, terminating\n").c_str());
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, "Platformer Demo", 0, 0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwMakeContextCurrent(window);
	glewInit();

	/* Set Callbacks */
	glfwSetMouseButtonCallback(window, handle_mousebutton);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, pos_callback);
	glfwSetFramebufferSizeCallback(window, resize_callback);

	/* Starting Point */
	player.position = glm::vec3(16, 0, 70);
	player.hitbox = glm::vec3(2, 5, 2);
	player.heading = glm::vec2(M_PI, 0);
	player.elevation = 0;
	player.fall_speed = 0;
	player.health = 3;
	player.height = 10.0;
	player.on_ground = true;
	player.paused = false;
	player.points = 0;

	/* Initialize Game Objects */
	floorbox fb;
	aimpoint main_aimpoint;

	objects.push_back(&main_aimpoint);
	objects.push_back(&main_hud);
	objects.push_back(&ice_balls); // Projectiles
	objects.push_back(&fb);

	main_hud.visible = false;

	/* Project 5 */
	octo_waves.push_back({ // Wave 1 - 5 total
		{'t', 'o', 't'},
		{'o', 't', 'o'},
		{'t', ' ', 't'}
		});

	octo_waves.push_back({ // Wave 2 - 8 total
		{' ', 't', ' '},
		{' ', 't', ' '},
		{' ', 't', ' '}
		});

	octo_waves.push_back({ // Wave 3 - 12 total
		{'t', ' ', 't'},
		{'o', ' ', 'o'},
		{'t', ' ', 't'}
		});

	octo_waves.push_back({ // Wave 4 - 12 total
		{' ', 'o', ' '},
		{'o', 'o', 'o'},
		{' ', 'o', ' '}
		});

	octo_waves.push_back({ // Wave 5 - 15 total
		{'t', 't', 't'},
		{'o', ' ', 'o'},
		{' ', ' ', ' '}
		});

	octo_waves.push_back({ // Wave 6 - 18 total
		{' ', ' ', ' '},
		{'o', 'o', 'o'},
		{'t', 't', 't'}
		});

	octo_waves.push_back({ // Wave 7 - 21 total
		{'t', 'o', 't'},
		{'o', ' ', 't'},
		{' ', ' ', ' '}
		});

	octo_waves.push_back({ // Wave 8 - 25 total
		{'t', 'o', 't'},
		{'o', ' ', 'o'},
		{'t', 'o', 't'}
		});

	octo_waves.push_back({ // Wave 9 - 29 total
		{'t', 'o', 't'},
		{' ', 'o', ' '},
		{'t', 'o', 't'}
		});

	octo_waves.push_back({ // Wave 10 - 30 total
		{' ', 't', ' '},
		{'o', ' ', 'o'},
		{'o', ' ', 'o'}
		});

	objects.push_back(&target_octo);
	objects.push_back(&obstacle_octo);

	objects.push_back(&maze);
	make_wall(&maze, glm::vec3(10, 1, 64), glm::vec3(2, 14, 14));
	make_wall(&maze, glm::vec3(10, 1, 78), glm::vec3(12, 14, 2));
	make_wall(&maze, glm::vec3(22, 1, 64), glm::vec3(2, 14, 14));
	make_wall(&maze, glm::vec3(10, 15, 64), glm::vec3(14, 2, 16));

	flat_panel walls("sh_mz_ki_01_E700.png");

	walls.addpanel(glm::vec3(16, 10, -15), glm::vec2(30, 30), glm::vec2(5, 5), glm::vec4(0, 0, 1, 1), glm::vec4(0, 1, 0, 1));
	walls.addpanel(glm::vec3(40, 10, 15), glm::vec2(30, 30), glm::vec2(5, 5), glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1));
	walls.addpanel(glm::vec3(-6, 10, 15), glm::vec2(30, 30), glm::vec2(5, 5), glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1));

	objects.push_back(&walls);

	flat_panel ceiling("sh_mz_yuka_01_59000.png");

	ceiling.addpanel(glm::vec3(16, 40, 15), glm::vec2(30, 30), glm::vec2(5, 5));
	objects.push_back(&ceiling);

	moon ball("ball");
	ball.add_data(glm::vec3(16, 8, -40), glm::quat(1, 0, 0, 0), glm::vec3(0.05f));
	objects.push_back(&ball);

	flat_panel flat_p("linus.jpg");
	flat_p.addpanel(glm::vec3(16, 0.1f, 70), glm::vec2(5, 5), glm::vec2(1, 1), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
	objects.push_back(&flat_p);

	/* Shooting turret flipper platform 
	player_marker marker(&player, "marker");
	marker.locations.push_back(player.position);
	objects.push_back(&marker);

	lava_floor lf("lava");
	objects.push_back(&lf);

	t.locations.push_back(glm::vec3(0, 30, 0));
	t.current_target = &marker;
	t.current_projectile = &ice_balls;
	objects.push_back(&t);

	// Hollow Square of flipper objects
	flipper_floor ff("flipper");

	for (int x = -60; x < 72; x += 12) {
		ff.locations.push_back(glm::vec3(x, 5, 72));
	}
	for (int z = -72; z <= 72; z += 12) {
		ff.locations.push_back(glm::vec3(72, 5, z));
	}
	for (int x = 60; x > -72; x -= 12) {
		ff.locations.push_back(glm::vec3(x, 5, -72));
	}
	for (int z = 72; z >= -72; z -= 12) {
		ff.locations.push_back(glm::vec3(-72, 5, z));
	}
	objects.push_back(&ff);

	// Starting Platform
	platform start("start");
	start.locations.push_back(glm::vec3(0, 10, 85));
	objects.push_back(&start);

	// Protective Walls
	wall_block wb("cube.obj", "brick.jpg", glm::vec3(6, 2, 6), "wall");

	for (int i = -10; i <= 10; i += 2) {
		wb.locations.push_back(glm::vec3(i, 13, 60));
		wb.locations.push_back(glm::vec3(i, 15, 60));
		wb.locations.push_back(glm::vec3(i, 17, 60));
		wb.locations.push_back(glm::vec3(i, 19, 60));
		wb.locations.push_back(glm::vec3(i, 21, 60));
	}
	objects.push_back(&wb);

	/* Maze
	// Outter walls
	make_wall(&maze, glm::vec3(0, 1, 0), glm::vec3(6, 6, 108));
	make_wall(&maze, glm::vec3(-6, 1, 102), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-96, 1, 102), glm::vec3(84, 6, 6));
	make_wall(&maze, glm::vec3(-102, 1, 0), glm::vec3(6, 6, 108));
	make_wall(&maze, glm::vec3(-96, 1, 0), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-84, 1, 0), glm::vec3(84, 6, 6));

	// Inner walles
	make_wall(&maze, glm::vec3(-24, 1, 96), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-30, 1, 90), glm::vec3(24, 6, 6));
	make_wall(&maze, glm::vec3(-96, 1, 90), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-12, 1, 78), glm::vec3(12, 6, 6));
	make_wall(&maze, glm::vec3(-18, 1, 66), glm::vec3(18, 6, 6));
	make_wall(&maze, glm::vec3(-6, 1, 54), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-6, 1, 42), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-36, 1, 30), glm::vec3(36, 6, 6));
	make_wall(&maze, glm::vec3(-30, 1, 36), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-18, 1, 36), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-30, 1, 18), glm::vec3(6, 6, 12));
	make_wall(&maze, glm::vec3(-30, 1, 6), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-66, 1, 6), glm::vec3(6, 6, 36));
	make_wall(&maze, glm::vec3(-96, 1, 48), glm::vec3(48, 6, 6));
	make_wall(&maze, glm::vec3(-54, 1, 42), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-54, 1, 60), glm::vec3(42, 6, 6));
	make_wall(&maze, glm::vec3(-54, 1, 66), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-42, 1, 72), glm::vec3(6, 6, 30));
	make_wall(&maze, glm::vec3(-30, 1, 72), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-36, 1, 78), glm::vec3(18, 6, 6));
	make_wall(&maze, glm::vec3(-42, 1, 42), glm::vec3(6, 6, 18));
	make_wall(&maze, glm::vec3(-36, 1, 48), glm::vec3(24, 6, 6));
	make_wall(&maze, glm::vec3(-60, 1, 30), glm::vec3(18, 6, 6));
	make_wall(&maze, glm::vec3(-54, 1, 12), glm::vec3(6, 6, 18));
	make_wall(&maze, glm::vec3(-48, 1, 12), glm::vec3(12, 6, 6));
	make_wall(&maze, glm::vec3(-42, 1, 18), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-72, 1, 24), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-96, 1, 12), glm::vec3(24, 6, 6));
	make_wall(&maze, glm::vec3(-84, 1, 18), glm::vec3(6, 6, 12));
	make_wall(&maze, glm::vec3(-96, 1, 24), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-90, 1, 36), glm::vec3(18, 6, 6));
	make_wall(&maze, glm::vec3(-78, 1, 42), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-78, 1, 54), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-66, 1, 54), glm::vec3(6, 6, 30));
	make_wall(&maze, glm::vec3(-72, 1, 78), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-96, 1, 78), glm::vec3(18, 6, 6));
	make_wall(&maze, glm::vec3(-90, 1, 60), glm::vec3(6, 6, 18));
	make_wall(&maze, glm::vec3(-84, 1, 66), glm::vec3(12, 6, 6));
	make_wall(&maze, glm::vec3(-84, 1, 90), glm::vec3(24, 6, 6));
	make_wall(&maze, glm::vec3(-90, 1, 66), glm::vec3(6, 6, 6));
	make_wall(&maze, glm::vec3(-54, 1, 78), glm::vec3(6, 6, 18));
	make_wall(&maze, glm::vec3(-48, 1, 84), glm::vec3(6, 6, 6));

	objects.push_back(&maze);

	loaded_object octo("octorok.obj", "blue_octorok.png", glm::vec3(2, 2, 2), "octorock");
	octo.locations.push_back(glm::vec3(-12, 4, 12));
	objects.push_back(&octo);

	runner.locations.push_back(glm::vec3(-10, 0, 104));
	objects.push_back(&runner);

	runner.spin(180); // Facing into the maze

	/* Main Testing Area
	flipper_floor ff("flipper");

	for (int x = 30; x < 162; x += 12) {
		ff.locations.push_back(glm::vec3(x, 30, 72));
	}
	for (int z = -72; z <= 72; z += 12) {
		ff.locations.push_back(glm::vec3(162, 30, z));
	}
	for (int x = 150; x > 22; x -= 12) {
		ff.locations.push_back(glm::vec3(x, 30, -72));
	}
	for (int z = 72; z >= -72; z -= 12) {
		ff.locations.push_back(glm::vec3(22, 30, z));
	}
	objects.push_back(&ff);

	// Goofing Around
	moon moon("ball");
	moon.locations.push_back(glm::vec3(0, 800, -500));
	objects.push_back(&moon);

	// Rotate moon downwards
	glm::mat4 temp_model = glm::mat4(1.0f);

	temp_model = translate(temp_model, glm::vec3(0, 800, -500));

	glm::vec3 moon_local_right = glm::vec3(temp_model[0][0], temp_model[1][0], temp_model[2][0]);
	glm::quat angl = glm::angleAxis(45.0f, moon_local_right);

	moon.angle = angl;

	giant g("giant");
	g.locations.push_back(glm::vec3(0, 0, -100));
	g.locations.push_back(glm::vec3(0, 0, -500));
	g.locations.push_back(glm::vec3(200, 0, -300));
	g.locations.push_back(glm::vec3(-200, 0, -300));

	objects.push_back(&g);

	target bombs("bomb");
	for (int i = 0; i < 10; i++) {
		bombs.locations.push_back(glm::vec3(50 + (i * 6), 6, -20));
	}
	objects.push_back(&bombs);
	objects.push_back(&brick_fragments);

	//turret t("turret");
	//t.locations.push_back(glm::vec3(0, 10, -35));
	//t.current_target = &bombs;
	//t.current_projectile = &ice_balls;
	//objects.push_back(&t);

	wall_block cube("cube.obj", "brick.jpg", glm::vec3(2, 2, 2), "stairs");
	for (int x = -30; x < 50; x += 2) {
		cube.locations.push_back(glm::vec3(x + 80, (x + 40) / 3.0, 28));
		cube.locations.push_back(glm::vec3(x + 80, (x + 40) / 3.0, 30));
		cube.locations.push_back(glm::vec3(x + 80, (x + 40) / 3.0, 32));
	}

	objects.push_back(&cube);

	elevator e("platform.obj", "brick.jpg", glm::vec3(8, 2, 8), "elevator", glm::vec3(0, 1, 0), glm::vec3(0, 60, 0), glm::vec3(133, 10, 30));
	e.locations.push_back(glm::vec3(133, 10, 30));
	objects.push_back(&e);

	face_player_panel fpp("trollface.png");
	fpp.addpanel(glm::vec3(35, 10, 0), glm::vec2(2, 2), glm::vec2(1, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	objects.push_back(&fpp);

	/* Initialize game objects */
	for (gameobject* o : objects) {
		if (o->init()) {
			puts(RED("Compile Failed, giving up!").c_str());
			return 1;
		}
	}

	main_hud.set_text("Hello World\nThis is line two\nAnd line three!");

	/* Start Threads */
	std::thread movement_thread(player_movement);
	std::thread object_thread(object_movement);
	std::thread animation_thread(animation);
	std::thread collision_thread(collision);
	std::thread shooting_gallery(octo_game);
	std::thread display_fps([]() {
		while (!shutdown_engine) {
			double loop_time = 1000.0 / loopcount;
			main_hud.lprintf(1, "FPS: %4d  Move: %lf", framecount, loop_time);
			framecount = 0;
			loopcount = 0;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		});

	/* Game Loop */
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		auto start = std::chrono::high_resolution_clock::now();
		framecount++;

		glfwPollEvents();
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Get the view and projection matrices
		glm::vec3 global_up(0, 1, 0);
		glm::vec3 look_at_point = player.position + glm::vec3(cosf(player.heading.y) * sinf(player.heading.x), sinf(player.heading.y), cosf(player.heading.y) * cosf(player.heading.x));
		glm::mat4 view = glm::lookAt(player.position, look_at_point, global_up);
		glm::mat4 projection = glm::perspective(45.0f, width / height, 0.1f, 1000.0f);
		glm::mat4 vp = projection * view;

		// Local Axis of the player
		local_up = glm::vec3(view[0][1], view[1][1], view[2][1]);
		local_right = glm::vec3(view[0][0], view[1][0], view[2][0]);
		local_forward = glm::vec3(view[0][2], view[1][2], view[2][2]);

		// Draw all objects
		for (gameobject* o : objects)
			if (o->visible)
				o->draw(vp);

		glfwSwapBuffers(window);

		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		framerate = (1000000000.0 / (double)elapsed_nanoseconds);
		delta_time = ((double)target_framerate / framerate) > 1 ? 1 : ((double)target_framerate / framerate);

		std::this_thread::sleep_for(std::chrono::milliseconds(8 - elapsed.count()));
	}

	for (gameobject* o : objects)
		o->deinit();

	/* Kill Threads */
	shutdown_engine = true;
	movement_thread.join();
	object_thread.join();
	animation_thread.join();
	collision_thread.join();
	shooting_gallery.join();
	display_fps.join();

	glfwDestroyWindow(window);
	glfwTerminate();
	free_helpers();
}

void spawn_wave(std::vector<std::vector<char>> wave, octorok* target, octorok* obstacle) {
	for (size_t i = 0; i < wave.size(); i++) {
		for (size_t j = 0; j < wave[i].size(); j++) {
			if (wave[i][j] == 't') {
				target->create_octo(glm::vec3(j * 16, -15, i * 16));
			}
			else if (wave[i][j] == 'o') {
				obstacle->create_octo(glm::vec3(j * 16, -15, i * 16));
			}
		}
	}
}

/* Make a wall out of any object */
void make_wall(gameobject* o, glm::vec3 location, glm::vec3 size) {
	glm::vec3 object_size = o->size;

	for (int x = 0; x < size.x; x += object_size.x) {
		for (int y = 0; y < size.y; y += object_size.y) {
			for (int z = 0; z < size.z; z += object_size.z) {
				object_data d = {
					glm::vec3(location.x + x, location.y + y, location.z + z),
					glm::quat(1, 0, 0, 0),
					glm::vec3(1.0f, 1.0f, 1.0f)
				};

				o->data.push_back(d);
			}
		}
	}
}

/* Function Definitions */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Activate
	if (GLFW_KEY_F1 == key && action == GLFW_PRESS) {
		octo_start = !octo_start;
	}

	// HUD
	if (GLFW_KEY_F3 == key && action == GLFW_PRESS)
		main_hud.visible = !main_hud.visible;

	// Exit
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	/* Player Movement  
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		player.key_status.forward = true;
	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		player.key_status.forward = false;

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		player.key_status.backward = true;
	if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		player.key_status.backward = false;

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		player.key_status.left = true;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		player.key_status.left = false;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		player.key_status.right = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		player.key_status.right = false;

	/* Jump 
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && player.on_ground == true) {
		player.fall_speed = 0.2f;
		player.position.y += 1.0f;

		std::thread flip_platforms([]() {
			for (gameobject* o : objects) {
				o->flip();
			}
			});

		flip_platforms.join();
	}

	/* Sprint */
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		MOVEMENT_MULT = 0.5f;
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		MOVEMENT_MULT = 0.2f;

	// Pause Mouse Movements
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		player.paused = !player.paused;

		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	// Arrow Keys
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		player.key_status.arrow_up = true;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
		player.key_status.arrow_up = false;

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		player.key_status.arrow_down = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
		player.key_status.arrow_down = false;

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		player.key_status.arrow_left = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
		player.key_status.arrow_left = false;

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		player.key_status.arrow_right = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
		player.key_status.arrow_right = false;
}

bool is_empty(glm::vec3 position, float distance) {
	for (gameobject* o : objects) {
		int collide_index = o->collision_index(position, distance);
		if (collide_index != -1)
			return false;
	}
	return true;
}

bool is_empty_direction(glm::vec3 position, float distance, int rotation) {
	for (gameobject* o : objects) {
		int collide_index = o->collision_index_with_direction(position, rotation, distance);
		if (collide_index != -1)
			return false;
	}
	return true;
}

void player_movement() {
	while (!shutdown_engine) {
		float floor_height = 0;
		auto start = std::chrono::system_clock::now();
		glm::vec3 step_to_point = player.position;

		// Movement
		local_right.y = 0;
		local_forward.y = 0;

		local_right = glm::normalize(local_right);
		local_forward = glm::normalize(local_forward);

		if (player.key_status.forward || player.key_status.backward || player.key_status.left || player.key_status.right) {
			step_to_point -= local_forward * (MOVEMENT_MULT * (player.key_status.forward - player.key_status.backward));
			step_to_point += local_right * (MOVEMENT_MULT * (player.key_status.right - player.key_status.left));
		}

		// Collision Detection
		for (gameobject* o : objects) {
			int collide_index = o->collision_index(step_to_point, 0.2f);
			if (collide_index != -1) {
				if (is_empty(glm::vec3(player.position.x, step_to_point.y, step_to_point.z), 0.2f)) {
					step_to_point.x = player.position.x;
					break;
				}
				else if (is_empty(glm::vec3(step_to_point.x, step_to_point.y, player.position.z), 0.2f)) {
					step_to_point.z = player.position.z;
					break;
				}
				else {
					step_to_point = player.position;
					break;
				}
			}
		}
		player.position = step_to_point;

		// Gravity
		if (player_platform) {
			if (!player_platform->is_on_idx(player.position, player_platform_index, player.height)) {
				player.on_ground = false;
				player_platform = 0;
			}
		}
		else {
			float floor_height = 0;

			for (gameobject* o : objects) {
				int ppi = o->is_on(player.position, player.height);

				if (ppi != -1) {
					player_platform_index = ppi;
					player_platform = o;

					floor_height = player_platform->data[player_platform_index].location.y + (player_platform->size.y / 2);

					player.fall_speed = 0;
					player.position.y = floor_height + player.height;

					/*
					if (player.position.y - player.height <= 0) {
						player.health = 3;
						player.position = glm::vec3(0, 12, 85);
					}
					*/
				}
			}

			if (player.position.y - player.height > floor_height) {
				player.on_ground = false;

				player.position.y += player.fall_speed;
				player.fall_speed -= GRAVITY;
			}
			else {
				player.on_ground = true;

				player.fall_speed = 0;
				player.position.y = floor_height + player.height;

				/*
				if (player.position.y - player.height <= 0) {
					player.health = 3;
					player.position = glm::vec3(0, 12, 85);
				}
				*/
			}
		}

		main_hud.lprintf(2, "Player Position:  (%f, %f, %f)", player.position.x, player.position.y, player.position.z);
		auto end = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::microseconds(1000) - (start - end));
	}
}

void object_movement() {
	int loop_time;
	auto last_call = std::chrono::system_clock::now();

	while (!shutdown_engine) {
		auto start = std::chrono::system_clock::now();
		loop_time = std::chrono::duration_cast<std::chrono::microseconds>(start - last_call).count();

		for (gameobject* o : objects) {
			o->move(loop_time);
		}

		auto end = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::microseconds(10000) - (start - end));
		int cpu_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		last_call = start;
		int sleep_time = 1000 - cpu_time;

		loopcount++;

		if (sleep_time > 100)
			std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
	}
}

void animation() {
	float movement_cooldown = 0;

	while (!shutdown_engine) {
		auto start = std::chrono::system_clock::now();

		for (gameobject* o : objects) {
			o->animate();
		}

		auto end = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::microseconds(10000) - (start - end));
	}
}

void collision() { //TODO:  Implement
	while (!shutdown_engine) {
		auto start = std::chrono::system_clock::now();
		ice_balls.data_mutex.lock();

		size_t nproj = ice_balls.data.size();

		for (size_t proj_index = 0; proj_index < nproj; proj_index++) {
			glm::vec3 l = ice_balls.data[proj_index].location;

			for (auto o : objects) {
				if (o->collision_check) {
					size_t index = o->collision_index(l);

					if (index != -1) {
						o->hit_index(index);

						/*
						if (o->identifier == "marker") {
							// If they hit the player, reset the player
							puts(RED("Player Hit!").c_str());
							player.health -= 1;

							ice_balls.remove_projectile(proj_index);

							if (player.health <= 0) {
								puts(RED("Game Over!").c_str());

								player.position = glm::vec3(0, 12, 85);
								player.health = 3;
							}

							break;
						}
						*/

						ice_balls.hit_index(proj_index);

						if (o->identifier == "target_octo") {
							target_octo.hit_index(index);
							player.points++;
						}

						if (o->identifier == "obstacle_octo") {
							obstacle_octo.hit_index(index);
							player.points--;
						}

						break;
					}
				}
			}
		}

		ice_balls.data_mutex.unlock();

		auto end = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::microseconds(1000) - (start - end));
	}
}

void octo_game() {
	int wave = 0;
	int wave_time = 5;
	int wave_cooldown = 6;

	bool advance = false;

	while (!shutdown_engine) {
		auto start = std::chrono::system_clock::now();

		main_hud.lprintf(0, "Score: %d Wave: %d - Timer: %d/%d", player.points, wave, wave_cooldown, wave_time);

		if (player.paused) {
			auto end = std::chrono::system_clock::now();

			std::this_thread::sleep_for(std::chrono::seconds(1) - (start - end));
			continue;
		}

		if (!octo_start) {
			auto end = std::chrono::system_clock::now();

			std::this_thread::sleep_for(std::chrono::seconds(1) - (start - end));
			continue;
		}

		if (wave_cooldown > wave_time || target_octo.data.size() == 0) {
			wave_cooldown = 0;

			target_octo.clear();
			obstacle_octo.clear();

			if (wave > octo_waves.size() - 1) {
				printf("Congratulations! You've beaten the game with a score of %d\n", player.points);
				octo_start = false;
				player.points = 0;
				wave = 0;

				printf("Game Over\n");
				continue;
			}

			// Hold for 4 second
			std::this_thread::sleep_for(std::chrono::seconds(4));

			printf("Wave %d\n", wave);
			spawn_wave(octo_waves[wave], &target_octo, &obstacle_octo);

			wave++;
		}

		wave_cooldown += 1;

		auto end = std::chrono::system_clock::now();

		std::this_thread::sleep_for(std::chrono::seconds(1) - (start - end));
	}
}

void flipper_toggle() {
	while (!shutdown_engine) {
		auto start = std::chrono::system_clock::now();

		for (gameobject* o : objects) {
			o->flip();
		}

		auto end = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::seconds(3) - (start - end));
	}
}

void fire(bool burst = false) {
	if (burst)
		ice_balls.add_projectile(player.position, player.heading.x, player.heading.y, 1.0f, 10000.0f, 5.0f, true);
	else
		ice_balls.add_projectile(player.position, player.heading.x, player.heading.y, 0.1f, 100000.0f, 5.0f, false);
}

void handle_mousebutton(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		fire();
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		fire(true);
	}
}

void pos_callback(GLFWwindow* window, double x, double y) {
	if (player.paused) {
		return;
	}

	double clamp_up = (M_PI / 2) - 0.1f;
	double clamp_down = (-M_PI / 2) + 0.1f;

	double center_x = width / 2;
	double center_y = height / 2;
	double dx = x - center_x;
	double dy = y - center_y;

	glfwSetCursorPos(window, center_x, center_y);
	player.heading.x -= dx / 1000.0;
	player.heading.y -= dy / 1000.0;

	if (player.heading.y > clamp_up)
		player.heading.y = clamp_up;

	if (player.heading.y < clamp_down)
		player.heading.y = clamp_down;
}

void resize_callback(GLFWwindow* window, int width, int height) {
	::height = height;
	::width = width;

	glViewport(0, 0, width, height);
}