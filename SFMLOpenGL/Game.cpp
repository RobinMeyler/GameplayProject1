// Name: Robin Meyler
// Project: Gameplay Programming I - Project 1, Cube platformer
// Date: March 2019

#include <Game.h>
#include <Cube.h>
#include <Easing.h>

// Helper to convert Number to String for HUD
template <typename T>
string toString(T number)
{
	ostringstream oss;
	oss << number;
	return oss.str();
}

GLuint	vsid,		// Vertex Shader ID
fsid,		// Fragment Shader ID
progID,		// Program ID
vao = 0,	// Vertex Array ID
vbo,		// Vertex Buffer ID
vib,		// Vertex Index Buffer
to[1];		// Texture ID
GLint	positionID,	// Position ID
colorID,	// Color ID
textureID,	// Texture ID
uvID;		// UV ID

//Please see .//Assets//Textures// for more textures
//  col2 for Textured version
const string filename = "col1.tga";

int width;						// Width of texture
int height;						// Height of texture
int comp_count;					// Component of texture

unsigned char* img_data;		// image data

mat4 projection,
view(1.f);			// View, Projection

Font font;						// Game font

Game::Game() :
	window(VideoMode(800, 600),
		"Introduction to OpenGL Texturing"),
	m_playerJumpState{ jumpState::Grounded }
{
}

Game::Game(sf::ContextSettings settings) :
	window(VideoMode(800, 600),
		"Introduction to OpenGL Texturing",
		sf::Style::Default,
		settings),
	m_playerJumpState{ jumpState::Grounded }
{

}

Game::~Game()
{
}

// Constant game loop used so it will have the rate of movement on whatever machine it runs on
void Game::run()
{
	initialize();
	sf::Clock gameClock;											// Game clock
	sf::Time timeTakenForUpdate = sf::Time::Zero;					// No lag on first input
	sf::Time timePerFrame = sf::seconds(1.f / 60.f);				// 60 frames per second
	Event event;
	while (window.isOpen())									// Loop
	{
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				isRunning = false;
			}		// Zoom function used in some of the views
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
					std::cout << "wheel type: vertical" << std::endl;
				else if (event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
					std::cout << "wheel type: horizontal" << std::endl;
				else
					std::cout << "wheel type: unknown" << std::endl;

				std::cout << "wheel movement: " << event.mouseWheelScroll.delta << std::endl;
				std::cout << "mouse x: " << event.mouseWheelScroll.x << std::endl;
				std::cout << "mouse y: " << event.mouseWheelScroll.y << std::endl;
				m_zoom = event.mouseWheelScroll.delta;
			}
		}// Check for input
		timeTakenForUpdate += gameClock.restart();					// Returns time take to do the loop
		while (timeTakenForUpdate > timePerFrame)					// Update enough times to catch up on updates missed during the lag time
		{
			timeTakenForUpdate -= timePerFrame;						// Decrement lag
			//processInput();											// More checks, the more accurate input to display will be
			update();									// Update
		}
		render();
	}
}



void Game::initialize()
{
	isRunning = true;
	GLint isCompiled = 0;
	GLint isLinked = 0;

	if (!(!glewInit())) { DEBUG_MSG("glewInit() failed"); }

	// Copy UV's to all faces
	for (int i = 1; i < 6; i++)
		memcpy(&uvs[i * 4 * 2], &uvs[0], 2 * 4 * sizeof(GLfloat));

	// Vertex Array Buffer
	glGenBuffers(1, &vbo);		// Generate Vertex Buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// Vertices (3) x,y,z , Colors (4) RGBA, UV/ST (2)
	glBufferData(GL_ARRAY_BUFFER, ((3 * VERTICES) + (4 * COLORS) + (2 * UVS)) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

	glGenBuffers(1, &vib); //Generate Vertex Index Buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);

	// Indices to be drawn
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * INDICES * sizeof(GLuint), indices, GL_STATIC_DRAW);

	// NOTE: uniforms values must be used within Shader so that they 
	// can be retreived
	const char* vs_src =
		"#version 400\n\r"
		""
		"in vec3 sv_position;"
		"in vec4 sv_color;"
		"in vec2 sv_uv;"
		""
		"out vec3 position;"
		"out vec4 color;"
		"out vec2 uv;"
		""
		"uniform mat4 sv_mvp;"
		"uniform float sv_x_offset;"
		"uniform float sv_y_offset;"
		"uniform float sv_z_offset;"
		""

		"uniform mat4 ModelMatrix;"
		"uniform mat4 ViewMatrix;"
		"uniform mat4 ProjectionMatrix;"

		"void main() {"
		"   position = vec4(ModelMatrix * vec4(sv_position, 1.f)).xyz;"
		"	color = sv_color;"
		"	uv = sv_uv;"
		"	gl_Position = ProjectionMatrix * ViewMatrix *  ModelMatrix  * vec4(sv_position, 1.f);"
		"}"; //Vertex Shader Src

	DEBUG_MSG("Setting Up Vertex Shader");

	vsid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsid, 1, (const GLchar**)&vs_src, NULL);
	glCompileShader(vsid);

	// Check is Shader Compiled
	glGetShaderiv(vsid, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_TRUE) {
		DEBUG_MSG("Vertex Shader Compiled");
		isCompiled = GL_FALSE;
	}
	else
	{
		DEBUG_MSG("ERROR: Vertex Shader Compilation Error");
	}

	const char* fs_src =
		"#version 400\n\r"
		""
		"uniform sampler2D f_texture;"
		""
		"in vec4 color;"
		"in vec2 uv;"
		""
		"out vec4 fColor;"
		""
		"void main() {"
		"	fColor = color - texture2D(f_texture, uv);"
		""
		"}"; //Fragment Shader Src

	fsid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsid, 1, (const GLchar**)&fs_src, NULL);
	glCompileShader(fsid);

	// Check is Shader Compiled
	glGetShaderiv(fsid, GL_COMPILE_STATUS, &isCompiled);

	progID = glCreateProgram();
	glAttachShader(progID, vsid);
	glAttachShader(progID, fsid);
	glLinkProgram(progID);

	// Check is Shader Linked
	glGetProgramiv(progID, GL_LINK_STATUS, &isLinked);

	// Set image data
	img_data = stbi_load(filename.c_str(), &width, &height, &comp_count, 4);

	if (img_data == NULL)
	{
		DEBUG_MSG("ERROR: Texture not loaded");
	}

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &to[0]);
	glBindTexture(GL_TEXTURE_2D, to[0]);

	// Wrap around
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind to OpenGL
	glTexImage2D(
		GL_TEXTURE_2D,			// 2D Texture Image
		0,						// Mipmapping Level 
		GL_RGBA,				// GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_BGR, GL_RGBA 
		width,					// Width
		height,					// Height
		0,						// Border
		GL_RGBA,				// Bitmap
		GL_UNSIGNED_BYTE,		// Specifies Data type of image data
		img_data				// Image Data
	);

	m_backPosition = true;
	// Projection Matrix 
	projection = glm::perspective(
		glm::radians(45.0f),					// Field of View 45 degrees
		4.0f / 3.0f,			// Aspect ratio
		0.1f,					// Display Range Min : 0.1f unit
		1500.0f					// Display Range Max : 100.0f unit
	);

	// Camera Matrix
	view = glm::lookAt(
		glm::vec3(0.0f, 10.0f, 20.0f),	// Camera (x,y,z), in World Space
		glm::vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
		glm::vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
	);

	// Initialize Positions
	m_player.objectPosition = vec3{ 0.0f, 0.0f, 0.0f };
	m_cameraPosition = glm::vec3(-20.0f, 10.0f, 10.0f);			// Behind view
	 m_jumpSpeed = 0.4f;
	 m_fallSpeed = 0.4f;
	 m_rotationSpeed = 2.f;;

	 // Setting up original transformations on the game objects-----------------------------------
	setupPlayer();

	// Score blocks
	setupGoals();

	// Game block positions
	setupObstacles();

	// Game blocks
	for (int i = 0; i < 50; i++)
	{
		m_block[i].model = glm::translate(m_block[i].model, m_block[i].objectPosition);
		m_block[i].objectRotation = { 0.f, -180.f, -180.f };
		m_block[i].model = glm::rotate(m_block[i].model, glm::radians(m_block[i].objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
		m_block[i].model = glm::rotate(m_block[i].model, glm::radians(m_block[i].objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
		m_block[i].model = glm::rotate(m_block[i].model, glm::radians(m_block[i].objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
	}
	// Ground rectangle block
	for (int i = 0; i < 4; i++)
	{
		m_ground[i].model = glm::mat4(1.f);
		m_ground[i].objectRotation = { 0.f, -180.f, -180.f };
		m_ground[i].objectPosition = vec3{ 50.f + i*200,-2.f,0.f };
		m_ground[i].m_size = 100;
		m_ground[i].model = glm::translate(m_ground[i].model, m_ground[i].objectPosition);
		m_ground[i].model = glm::rotate(m_ground[i].model, glm::radians(m_ground[i].objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
		m_ground[i].model = glm::rotate(m_ground[i].model, glm::radians(m_ground[i].objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
		m_ground[i].model = glm::rotate(m_ground[i].model, glm::radians(m_ground[i].objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
	}
	// Winning cube to tell the game when the player has reached the end of the level
	m_winCube[0].objectPosition = vec3{ 420, 0.f,0.f };
	m_winCube[1].objectPosition = vec3{ 420, 2.f,0.f };
	for (int i = 0; i < 2; i++)
	{
		m_winCube[i].objectRotation = { 90.f, -180.f, 0.f };
		m_winCube[i].model = glm::mat4(1.f);
		m_winCube[i].model = glm::translate(m_winCube[i].model, m_winCube[i].objectPosition);
		m_winCube[i].model = glm::rotate(m_winCube[i].model, glm::radians(m_winCube[i].objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
		m_winCube[i].model = glm::rotate(m_winCube[i].model, glm::radians(m_winCube[i].objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
		m_winCube[i].model = glm::rotate(m_winCube[i].model, glm::radians(m_winCube[i].objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
	}
	m_enemyCube[0].objectPosition = vec3{ 175, 0.f,0.f };
	m_enemyCube[1].objectPosition = vec3{ 175, 2.f,0.f };
	m_enemyCube[2].objectPosition = vec3{ 40, 0.f,0.f };
	m_enemyCube[3].objectPosition = vec3{ 140.f,8.f,0.f  };
	for (int i = 0; i < 4; i++)
	{
		m_enemyCube[i].objectRotation = { -90.f, 90.f, -90.f };
		m_enemyCube[i].model = glm::mat4(1.f);
		m_enemyCube[i].model = glm::translate(m_enemyCube[i].model, m_enemyCube[i].objectPosition);
		m_enemyCube[i].model = glm::rotate(m_enemyCube[i].model, glm::radians(m_enemyCube[i].objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
		m_enemyCube[i].model = glm::rotate(m_enemyCube[i].model, glm::radians(m_enemyCube[i].objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
		m_enemyCube[i].model = glm::rotate(m_enemyCube[i].model, glm::radians(m_enemyCube[i].objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
	}
	// Game time for output
	m_gameClock.restart();

	// Enable Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Load Font
	font.loadFromFile(".//Assets//Fonts//BBrick.ttf");
}

void Game::update()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Updating...");
#endif
	m_time = m_gameClock.getElapsedTime().asSeconds();

	// Controls all input for the game except camera changes
	handleMovement();

	// Constant forward movement of the player for the game
	m_player.objectPosition.x += 0.25;
	
	// Player jumping stages/states
	if (m_playerJumpState == jumpState::Grounded)
	{
		m_jumpSpeed = 0.4f;		// Reset if they have been changed
		m_fallSpeed = 0.4f;
		m_rotationSpeed = 2.f;
		m_collideExtraCheck = 0.5;
	}
	else if (m_playerJumpState == jumpState::Rising)		// Go up to the limit and rotate
	{
		if (m_player.objectPosition.y < m_jumpHeght - 1)
		{
			m_player.objectPosition.y += m_jumpSpeed;
			m_player.objectRotation.z -= m_rotationSpeed;
		}
		else if (m_player.objectPosition.y < m_jumpHeght)		// Slower rise to appear to be under gravity
		{
			m_player.objectPosition.y += m_jumpSpeed/2;
			m_player.objectRotation.z -= m_rotationSpeed;
		}
		else
		{
			m_playerJumpState = jumpState::Falling;
		}
	}
	else if (m_playerJumpState == jumpState::Falling)			// Fall and continue to rotate
	{
		if (m_player.objectPosition.y > m_jumpHeght - 1)
		{
			m_player.objectPosition.y -= m_fallSpeed/2;
			m_player.objectRotation.z -= m_rotationSpeed;
		}
		else if (m_player.objectPosition.y > m_groundLevel)
		{
			m_player.objectPosition.y -= m_fallSpeed;
			m_player.objectRotation.z -= m_rotationSpeed;
		}
		else
		{
			m_playerJumpState = jumpState::Grounded;				// Landed, do resets
			m_rotateCounter -= 90;
			m_player.objectRotation.z = m_rotateCounter;
			if (m_groundLevel == 0)
			{
				m_player.objectPosition = { m_player.objectPosition.x, 0.f, 0.f };		// No clipping
			}
		}
	}
	// Player and world collisions
	collisions();
	
	// Updates the player with any changes in the transformations
	setupPlayer();

	// Different views and projections (Press T, F)
	camera();
}

void Game::render()
{
	//window.clear(sf::Color(255.f, 255.f, 255.f, 255.f));		//   Reduces it to its base color (funky)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Save current OpenGL render states
	window.pushGLStates();
	// Sf::Text output for the HUD
	if (m_won != true)
	{
		string score = "Score: "
			+ string(toString(m_score));
		Text textScore(score, font);
		textScore.setFillColor(sf::Color(255, 255, 255, 170));
		textScore.setPosition(50.f, 20.f);
		textScore.setCharacterSize(40);
		window.draw(textScore);

		string timeS = "Time: "
			+ string(toString(m_time));
		Text timeScore(timeS, font);
		timeScore.setFillColor(sf::Color(255, 255, 255, 170));
		timeScore.setPosition(50.f, 55.f);
		timeScore.setCharacterSize(40);
		window.draw(timeScore);

		string fails = "Fails: "
			+ string(toString(m_fails));
		Text failScore(fails, font);
		failScore.setFillColor(sf::Color(255, 255, 255, 170));
		failScore.setPosition(50.f, 90.f);
		failScore.setCharacterSize(40);
		window.draw(failScore);

		// Info for the hud, moves based on the view
		if (m_backPosition != true)							// Flat view
		{
			string purp = "Purple: Enemy (Evil) ";
			Text purpT(purp, font);
			purpT.setFillColor(sf::Color(255, 0, 255, 170));
			purpT.setPosition(400.f, 20.f);
			purpT.setCharacterSize(30);
			window.draw(purpT);

			string green = "Green: Score (Good)";
			Text greenT(green, font);
			greenT.setFillColor(sf::Color(0, 255, 0, 170));
			greenT.setPosition(400.f, 50.f);
			greenT.setCharacterSize(30);
			window.draw(greenT);
		}
		else												// Behind, 3D view
		{
			string purp = "Purple: Enemy (Evil) ";
			Text purpT(purp, font);
			purpT.setFillColor(sf::Color(255, 0, 255, 170));
			purpT.setPosition(400, 470.f);
			purpT.setCharacterSize(30);
			window.draw(purpT);

			string green = "Green: Score (Good)";
			Text greenT(green, font);
			greenT.setFillColor(sf::Color(0, 255, 0, 170));
			greenT.setPosition(400.f, 500.f);
			greenT.setCharacterSize(30);
			window.draw(greenT);
		}
	}
	else if (m_won == true)		// If the end has been reached
	{
		string won = "You have won !!\nYour score: " 
			 +string(toString(m_score)) + "\nYour fails: " 
			+ string(toString(m_fails));
		Text wonT(won, font);
		wonT.setFillColor(sf::Color(255, 0, 0, 170));
		wonT.setPosition(50.f, 380.f);
		wonT.setCharacterSize(60);
		window.draw(wonT);
	}

	// Restore OpenGL render states
	window.popGLStates();

	// Rebind Buffers and then set SubData
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);

	// Use Progam on GPU
	glUseProgram(progID);

	// Find variables within the shader
	positionID = glGetAttribLocation(progID, "sv_position");
	if (positionID < 0) { DEBUG_MSG("positionID not found"); }

	colorID = glGetAttribLocation(progID, "sv_color");
	if (colorID < 0) { DEBUG_MSG("colorID not found"); }

	uvID = glGetAttribLocation(progID, "sv_uv");
	if (uvID < 0) { DEBUG_MSG("uvID not found"); }

	textureID = glGetUniformLocation(progID, "f_texture");
	if (textureID < 0) { DEBUG_MSG("textureID not found"); }

	glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), m_player.getVertex());	// They are all the same size
	glBufferSubData(GL_ARRAY_BUFFER, 3 * VERTICES * sizeof(GLfloat), 4 * COLORS * sizeof(GLfloat), colors);
	glBufferSubData(GL_ARRAY_BUFFER, ((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat), 2 * UVS * sizeof(GLfloat), uvs);

	// Set Active Texture .... 32 GL_TEXTURE0 .... GL_TEXTURE31
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(textureID, 0); // 0 .... 31

	// Set the X, Y and Z offset (this allows for multiple cubes via different shaders)
	// Experiment with these values to change screen positions
	// Set pointers for each parameter (with appropriate starting positions)
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, (VOID*)(3 * VERTICES * sizeof(GLfloat)));
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, 0, (VOID*)(((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat)));

	// Enable Arrays
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);
	glEnableVertexAttribArray(uvID);

	// View and porjection updated once
	glUniformMatrix4fv(glGetUniformLocation(progID, "ViewMatrix"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(progID, "ProjectionMatrix"), 1, GL_FALSE, &projection[0][0]);

	// Drawing player
	glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_player.model[0][0]);
	glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);

	// Drawing object blocks
	for (int i = 0; i < 50; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), m_block[i].getVertex());	// They are all the same size
		glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_block[i].model[0][0]);
		glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
	}
	// Drawing Score blocks
	for (int i = 0; i < 5; i++)
	{
		if (m_goalCube[i].isHit == false)
		{
			glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), m_goalCube[i].getVertex());	// They are all the same size
			glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_goalCube[i].model[0][0]);
			glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
		}
	}
	// Drawing End blocks to show a win
	for (int i = 0; i < 2; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), m_winCube[i].getVertex());	// They are all the same size
		glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_winCube[i].model[0][0]);
		glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
	}

	// Drawing Ground rectangles
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), m_ground[i].getVertex());	// They are all the same size
		glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_ground[i].model[0][0]);
		glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
	}
	for (int i = 0; i < 4; i++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), m_enemyCube[i].getVertex());	// They are all the same size
		glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_enemyCube[i].model[0][0]);
		glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
	}
	window.display();

	// Disable Arrays
	glDisableVertexAttribArray(positionID);
	glDisableVertexAttribArray(colorID);
	glDisableVertexAttribArray(uvID);

	// Unbind Buffers with 0 (Resets OpenGL States...important step)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Reset the Shader Program to Use
	glUseProgram(0);
}

void Game::unload()
{
	glDetachShader(progID, vsid);	// Shader could be used with more than one progID
	glDetachShader(progID, fsid);	// ..
	glDeleteShader(vsid);			// Delete Vertex Shader
	glDeleteShader(fsid);			// Delete Fragment Shader
	glDeleteProgram(progID);		// Delete Shader
	glDeleteBuffers(1, &vbo);		// Delete Vertex Buffer
	glDeleteBuffers(1, &vib);		// Delete Vertex Index Buffer
	stbi_image_free(img_data);		// Free image stbi_image_free(..)
}

void Game::handleMovement()
{
	// Game input-------------------------------------------------------------------
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		m_player.objectRotation.x += 0.1f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		m_player.objectRotation.y += 0.1f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		m_player.objectRotation.z += 0.1f;
	}
	// Translate
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))		// Up
	{
		//m_player.objectPosition += glm::vec3(0.0f, 0.01f, 0.f);			// Real up
		if (m_backPosition == true)										// Psudo forward for camera
		{
			m_player.objectPosition += glm::vec3(0.001f, 0.0f, 0.0f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))		// Down
	{
		//m_player.objectPosition -= glm::vec3(0.0f, 0.01f, 0.f);			// Real down
		if (m_backPosition == true)
		{
			m_player.objectPosition -= glm::vec3(0.001f, 0.0f, 0.0f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))	// Right
	{
		if (m_backPosition != true)
		{
			m_player.objectPosition += glm::vec3(0.01f, 0.0f, 0.0f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))		// Left
	{
		if (m_backPosition != true)
		{
			m_player.objectPosition -= glm::vec3(0.01f, 0.0f, 0.0f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))		// Into the world
	{
		//m_player.objectPosition += glm::vec3(0.0f, 0.f, 0.02f);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))		// Toward the camera
	{
		//m_player.objectPosition -= glm::vec3(0.0f, 0.f, 0.02f);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))		// Toward the camera
	{
		if (m_playerJumpState == jumpState::Grounded)
		{
			m_playerJumpState = jumpState::Rising;
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))		// Toward the camera
	{
		restart();
	}
}

void Game::collisions()
{
	// Collisions --------------------------------------------------------
	// Player and winning cube
	for (int i = 0; i < 2; i++)
	{
		bool wincollide = checkCollision(m_player, m_winCube[i]);
		if (wincollide == true)
		{
			m_winCube[i].isHit = true;
			m_won = true;
		}
	}
	// Player to Enemy block 
	for (int i = 0; i < 4; i++)
	{
		bool enemyCol = checkCollision(m_player, m_enemyCube[i]);
		if (enemyCol == true)
		{
			restart();
		}
	}
	// Player and Score cubes
	for (int i = 0; i < 5; i++)
	{
		bool goalcollide = checkCollision(m_player, m_goalCube[i]);
		if (goalcollide == true)
		{
			m_goalCube[i].isHit = true;
			m_score++;
			m_goalCube[i].objectPosition = vec3{ 92.0f, 1000.0f, 0.0f };
			m_goalCube[i].model = glm::mat4(1.f);
			m_goalCube[i].model = glm::translate(m_goalCube[i].model, m_goalCube[i].objectPosition);
		}
	}
	// Player and objects game cubes
	bool collide{ false };
	for (int i = 0; i < 50; i++)
	{
		collide = checkCollision(m_player, m_block[i]);
		if (collide == true)
		{							// If the block it collides with is under it (or close enough to) Land on it
			if ((m_player.objectPosition.y - (m_player.m_size / 2)) > (m_block[i].objectPosition.y + (m_block[i].m_size / 2) - m_collideExtraCheck))
			{
				m_groundLevel = m_block[i].objectPosition.y + 2;
				m_player.objectPosition = { m_player.objectPosition.x, m_groundLevel, m_player.objectPosition.z };
				m_jumpHeght = m_groundLevel + 6;
			}
			else
			{
 				restart();			// If its the side or bottom, restart
			}
			break;
		}
		else if (collide != true && m_playerJumpState == jumpState::Grounded)		// If you move off a block, fall
		{
			m_groundLevel = 0;					// Ground level unless it meets another cube on the way down
			m_jumpHeght = m_groundLevel + 6;
		}
	}
	// This is called outside because of the break, increase fall speed if the player is high enough
	if (collide == false && m_player.objectPosition.y > 0 && m_playerJumpState == jumpState::Grounded)
	{
		m_playerJumpState = jumpState::Falling;
		if (m_player.objectPosition.y > 3)
		{
			m_fallSpeed = 0.8;
			m_rotationSpeed = 3;
			m_collideExtraCheck = 1;
		}
	}
}

void Game::camera()
{
	// Working camera follower --------------------------------------------------------------------------------------------------
	if (m_count > 10)		// Counter so the screen doesnt swap too quick
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
		{
			if (m_backPosition == true)
			{
				m_cameraPosition = glm::vec3(0.0f, 10.0f, 30.0f);				// Side view
				projection = glm::perspective(
					glm::radians(45.f),					// Field of View 45 degrees
					4.0f / 3.0f,			// Aspect ratio
					0.1f,					// Display Range Min : 0.1f unit
					1500.0f					// Display Range Max : 100.0f unit
				);
				m_sidePosition = true;
				m_backPosition = false;
			}
			else
			{
				m_cameraPosition = glm::vec3(-20.0f, 10.0f, 10.0f);			// Behind view
				projection = glm::perspective(
					glm::radians(45.0f),					// Field of View 45 degrees
					4.0f / 3.0f,			// Aspect ratio
					0.1f,					// Display Range Min : 0.1f unit
					1500.0f					// Display Range Max : 100.0f unit
				);
				m_backPosition = true;
				m_sidePosition = false;
			}
			m_count = 0;

		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))			// Flat
		{
			if (m_sidePosition == true)							// Flat view
			{
				m_cameraPosition = glm::vec3(0.0f, 0.0f, 1400.0f);
				projection = glm::perspective(
					glm::radians(1.f),					// Field of View 1 degree
					4.0f / 3.0f,			// Aspect ratio
					0.1f,					// Display Range Min : 0.1f unit
					1500.0f					// Display Range Max : 100.0f unit
				);
				m_sidePosition = false;
				m_backPosition = false;
			}
			else if (m_sidePosition != true)							// Side view
			{
				m_cameraPosition = glm::vec3(0.0f, 10.0f, 30.0f);
				projection = glm::perspective(
					glm::radians(45.0f),					// Field of View 45 degrees
					4.0f / 3.0f,			// Aspect ratio
					0.1f,					// Display Range Min : 0.1f unit
					1500.0f					// Display Range Max : 100.0f unit
				);
				m_sidePosition = true;
				m_backPosition = false;
			}
			m_count = 0;

		}
	}
	m_count++;
	// Depending on the view, zoom the screen in and out
	if (m_backPosition != true)
	{
		m_cameraPosition.z -= m_zoom;
	}
	else
	{
		m_cameraPosition.x += m_zoom;
		m_cameraPosition.y -= m_zoom;
	}
	m_zoom = 0;

	// Apply decided View, follows the player
	view = glm::lookAt(
		m_player.objectPosition + m_cameraPosition,	// Camera (x,y,z), in World Space
		m_player.objectPosition,		// Camera looking at origin
		glm::vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
	);

	// Non follow cam  --------------------------------------------
	//view = glm::lookAt(
	//	cameraPosition,	// Camera (x,y,z), in World Space
	//	glm::vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
	//	glm::vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
	//);
}

bool Game::checkCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
	// Collision x-axis
	bool collisionX = one.objectPosition.x + one.m_size >= two.objectPosition.x &&
		two.objectPosition.x + two.m_size >= one.objectPosition.x;		// two size used here in case it is a rectangle, could be expanded
	// Collision y-axis
	bool collisionY = one.objectPosition.y + one.m_size >= two.objectPosition.y &&
		two.objectPosition.y + one.m_size >= one.objectPosition.y;
	// Collision only if on both axes
	return collisionX && collisionY;
}

// Player transformations
void Game::setupPlayer()
{
	m_player.model = glm::mat4(1.f);
	m_player.model = glm::translate(m_player.model, m_player.objectPosition);
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
	m_player.model = glm::scale(m_player.model, m_player.objectScale);
}

// Setting and resseting the score cubes
void Game::setupGoals()
{
	m_goalCube[0].objectPosition = vec3{ 92.0f, 11.0f, 0.0f };
	m_goalCube[1].objectPosition = vec3{ 232.f, 13.f,0.f };
	m_goalCube[2].objectPosition = vec3{ 321.f, 14.f,0.f };
	m_goalCube[3].objectPosition = vec3{ 275.f,0.f,0.f };
	m_goalCube[4].objectPosition = vec3{ 384.f, 23.f,0.f };
	for (int i = 0; i < 5; i++)
	{
		m_goalCube[i].isHit = false;
		m_goalCube[i].objectRotation = { 90.f, -180.f, 0.f };
		m_goalCube[i].model = glm::mat4(1.f);
		m_goalCube[i].model = glm::translate(m_goalCube[i].model, m_goalCube[i].objectPosition);
		m_goalCube[i].model = glm::rotate(m_goalCube[i].model, glm::radians(m_goalCube[i].objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
		m_goalCube[i].model = glm::rotate(m_goalCube[i].model, glm::radians(m_goalCube[i].objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
		m_goalCube[i].model = glm::rotate(m_goalCube[i].model, glm::radians(m_goalCube[i].objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
		m_goalCube[i].model = glm::scale(m_goalCube[i].model, m_goalCube[i].objectScale);
	}
}

// Game block positions
void Game::setupObstacles()
{
	m_block[0].objectPosition = vec3{ 30.f,0.f,0.f };
	m_block[1].objectPosition = vec3{ 50.f,0.f,0.f };
	m_block[2].objectPosition = vec3{ 52.f,0.f,0.f };
	m_block[3].objectPosition = vec3{ 68.f,0.f,0.f };
	m_block[4].objectPosition = vec3{ 76.f,3.f,0.f };

	m_block[5].objectPosition = vec3{ 84.f,6.f,0.f };
	m_block[6].objectPosition = vec3{ 92.f,9.f,0.f };
	m_block[17].objectPosition = vec3{ 100.f,0.f,0.f };		// Filling gaps
	m_block[18].objectPosition = vec3{ 104.f,0.f,0.f };
	m_block[7].objectPosition = vec3{ 120.f,0.f,0.f };

	m_block[8].objectPosition = vec3{ 120.f,2.f,0.f };
	m_block[9].objectPosition = vec3{ 128.f,5.f,0.f };
	m_block[10].objectPosition = vec3{ 136.f,8.f,0.f };
	m_block[19].objectPosition = vec3{ 140.f,0.f,0.f };
	m_block[11].objectPosition = vec3{ 140.f,2.f,0.f };

	m_block[12].objectPosition = vec3{ 140.f,4.f,0.f };
	m_block[13].objectPosition = vec3{ 140.f,6.f,0.f };
	m_block[14].objectPosition = vec3{ 150.f,0.f,0.f };
	m_block[15].objectPosition = vec3{ 160.f,0.f,0.f };
	m_block[16].objectPosition = vec3{ 170.f,0.f,0.f };

	m_block[20].objectPosition = vec3{ 190.f,0.f,0.f };
	m_block[21].objectPosition = vec3{ 198.f,3.f,0.f };
	m_block[22].objectPosition = vec3{ 208.f,3.f,0.f };
	m_block[23].objectPosition = vec3{ 216.f,6.f,0.f };
	m_block[24].objectPosition = vec3{ 224.f,9.f,0.f };

	m_block[25].objectPosition = vec3{ 232.f,11.f,0.f };		// Goal block
	m_block[26].objectPosition = vec3{ 220.f,0.f,0.f };
	m_block[27].objectPosition = vec3{ 220.f,2.f,0.f };
	m_block[28].objectPosition = vec3{ 220.f,4.f,0.f };
	m_block[29].objectPosition = vec3{ 280.f,0.f,0.f };

	m_block[30].objectPosition = vec3{ 280.f,2.f,0.f };
	m_block[33].objectPosition = vec3{ 270.f,0.f,0.f };
	m_block[34].objectPosition = vec3{ 270.f,2.f,0.f };
	m_block[35].objectPosition = vec3{ 285.f,0.f,0.f };		
	m_block[36].objectPosition = vec3{ 294.f,3.f,0.f };

	m_block[37].objectPosition = vec3{ 303.f,6.f,0.f }; 
	m_block[31].objectPosition = vec3{ 312.f,9.f,0.f };
	m_block[32].objectPosition = vec3{ 321.f,12.f,0.f};		// Goal
	m_block[38].objectPosition = vec3{ 320.f,0.f,0.f };
	m_block[39].objectPosition = vec3{ 320.f,0.f,0.f };

	// ---------
	m_block[40].objectPosition = vec3{ 328.f,3.f,0.f };
	m_block[41].objectPosition = vec3{ 336.f,6.f,0.f };
	m_block[42].objectPosition = vec3{ 344.f,9.f,0.f };
	m_block[43].objectPosition = vec3{ 353.f,12.f,0.f };
	m_block[44].objectPosition = vec3{ 362.f,15.f,0.f };

	m_block[45].objectPosition = vec3{ 370.f,18.f,0.f };		
	m_block[46].objectPosition = vec3{ 378.f,21.f,0.f };
	m_block[47].objectPosition = vec3{ 380.f,21.f,0.f };
	m_block[48].objectPosition = vec3{ 382.f,21.f,0.f };
	m_block[49].objectPosition = vec3{ 384.f,21.f,0.f };		// End
}

// Setart the game/ reset the game
void Game::restart()
{
	m_fails++;

	m_player.objectPosition = { 0.f, 0.f, 0.f };
	m_player.objectRotation = { 0.f, 0.f, 0.f };

	m_rotateCounter = 0;
	m_score = 0;
	m_time = 0;

	m_won = false;

	setupPlayer();
	setupGoals();
	m_gameClock.restart();
}
