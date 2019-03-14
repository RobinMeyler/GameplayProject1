#ifndef GAME_H
#define GAME_H

// Name: Robin Meyler
// Project: Gameplay Programming I - Project 1, Cube platformer
// Date: March 2019


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

	// Functions to keep the code readable and easy to navigate
	void handleMovement();
	void collisions();
	void camera();
	void setupPlayer();
	void setupGoals();
	void setupObstacles();
	void restart();

	// Checks AABB colisions for the cubes
	bool checkCollision(GameObject &one, GameObject &two);

	// Game objects ------------------------
	// Value decides shape, 1 = Cube, 2 = Rectangle
	GameObject m_player{ 1 };
	GameObject m_block[50]{ {1},{1},{1},{1},{1}, {1},{1},{1},{1},{1},
	{1},{1},{1},{1},{1}, {1},{1},{1},{1},{1} ,
	{1},{1},{1},{1},{1}, {1},{1},{1},{1},{1} ,
	{1},{1},{1},{1},{1}, {1},{1},{1},{1},{1} ,
	{1},{1},{1},{1},{1}, {1},{1},{1},{1},{1} };
	GameObject m_ground[4]{ 2,2,2,2 };
	GameObject m_goalCube[5]{ 1,1,1,1,1 };
	GameObject m_enemyCube[4]{ 1,1,1,1 };
	GameObject m_winCube[2]{ 1,1 };

	// Camera
	vec3 m_cameraPosition{ 0.0f, 0.0f, 20.0f };

	// For the jumping
	enum class jumpState
	{
		Grounded,
		Rising,
		Peak,
		Falling
	}m_playerJumpState;

	// output time
	sf::Clock m_gameClock;

	// Variables used through the game
	int m_time{ 0 };
	int m_fails{ 0 };
	int m_count{ 0 };
	int m_score{ 0 };
	int restartCount{ 0 };
	float m_groundLevel{ 0 };
	float m_jumpHeght{ 6 };
	float m_jumpSpeed;
	float m_fallSpeed;
	float m_rotationSpeed;
	float m_collideExtraCheck{ 0.5 };
	float m_zoom{ 0.f };
	float m_rotationAngle = 0.0f;
	float m_rotateCounter{ 0 };

	bool m_backPosition{ false };
	bool m_sidePosition{ true };
	bool m_won{ false };
};

#endif  // ! GAME_H