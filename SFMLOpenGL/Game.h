#ifndef GAME_H
#define GAME_H

#include <string>
#include <sstream>

#include <iostream>
#include <GL/glew.h>
#include <GL/wglew.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <Debug.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GameObject.h>

using namespace std;
using namespace sf;
using namespace glm;

class Game
{
public:
	Game();
	Game(sf::ContextSettings settings);
	~Game();
	void run();

private:
	RenderWindow window;
	Clock clock;
	Time time;
	bool animate = false;
	vec3 animation = vec3(0.0f);
	float rotation = 0.0f;
	bool isRunning = false;
	void initialize();
	void update();
	void render();
	void unload();
	void handleMovement();
	void camera();
	void drawGround();

	GameObject m_player;
	GameObject m_block[10];
	GameObject m_ground[200];

	enum class jumpState
	{
		Grounded,
		Rising,
		Peak,
		Falling
	}m_playerJumpState;



	bool backPosition{ false };
	bool sidePosition{ true };
	int m_count{ 0 };
	vec3 cameraPosition{ 0.0f, 0.0f, 20.0f };
	float zoom{ 0.f };
	float rotationAngle = 0.0f;
	float rotateCounter{ 0 };

};

#endif  // ! GAME_H