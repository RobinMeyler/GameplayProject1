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
		uvID,		// UV ID
		mvpID[2],		// Model View Projection ID
		// 
		x_offsetID, // X offset ID
		y_offsetID,	// Y offset ID
		z_offsetID;	// Z offset ID

GLenum	error;		// OpenGL Error Code


//Please see .//Assets//Textures// for more textures
const string filename = ".//Assets//Textures//grid_wip.tga";

int width;						// Width of texture
int height;						// Height of texture
int comp_count;					// Component of texture

unsigned char* img_data;		// image data

mat4 mvp[2], projection, 
		view(1.f), model;			// Model View Projection

GameObject player;

Font font;						// Game font

float x_offset, y_offset, z_offset; // offset on screen (Vertex Shader)

Game::Game() : 
	window(VideoMode(800, 600), 
	"Introduction to OpenGL Texturing")
{
}

Game::Game(sf::ContextSettings settings) :
	window(VideoMode(800, 600),
		"Introduction to OpenGL Texturing",
		sf::Style::Default,
		settings)
{

}

Game::~Game()
{
}


void Game::run()
{

	initialize();

	Event event;

	float rotation = 0.01f;
	float start_value = 0.0f;
	float end_value = 1.0f;

	while (isRunning){

#if (DEBUG >= 2)
		DEBUG_MSG("Game running...");
#endif

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				isRunning = false;
			}
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
				zoom = event.mouseWheelScroll.delta;
			}
	
		}
		update();
		render();
	}

#if (DEBUG >= 2)
	DEBUG_MSG("Calling Cleanup...");
#endif
	unload();

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
	// Projection Matrix 
	projection = glm::perspective(
		glm::radians(45.0f),					// Field of View 45 degrees
		4.0f / 3.0f,			// Aspect ratio
		0.1f,					// Display Range Min : 0.1f unit
		100.0f					// Display Range Max : 100.0f unit
	);

	// Camera Matrix
	view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 20.0f),	// Camera (x,y,z), in World Space
		glm::vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
		glm::vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
	);

	// Initialize Positions
	cameraPosition = {0.0f, 0.0f, 20.0f};
	m_player.model = glm::mat4(1.f);
	m_player.model = glm::translate(m_player.model, m_player.objectPosition);
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
	m_player.model = glm::scale(m_player.model, m_player.objectScale);
	for (int i = 0; i < 10; i++)
	{
		m_block[i].objectPosition = vec3{ 0.f,0.f,0.f } +vec3{ 10.f + i * 20 ,0.f,0.f };
		m_block[i].model = glm::translate(m_block[i].model, m_block[i].objectPosition);
	}


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

	// Game input
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
		if (backPosition == true)										// Psudo forward for camera
		{
			m_player.objectPosition += glm::vec3(0.01f, 0.0f, 0.0f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))		// Down
	{
		//m_player.objectPosition -= glm::vec3(0.0f, 0.01f, 0.f);			// Real down
		if (backPosition == true)
		{
			m_player.objectPosition -= glm::vec3(0.01f, 0.0f, 0.0f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))	// Right
	{
		if (backPosition != true)
		{
			m_player.objectPosition += glm::vec3(0.01f, 0.0f, 0.0f);
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))		// Left
	{
		if (backPosition != true)
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

	// Updating the player to any changes made
	m_player.model = glm::mat4(1.f);
	m_player.model = glm::translate(m_player.model, m_player.objectPosition);
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.x), glm::vec3(1.f, 0.f, 0.f));
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.y), glm::vec3(0.f, 1.0f, 0.f));
	m_player.model = glm::rotate(m_player.model, glm::radians(m_player.objectRotation.z), glm::vec3(0.f, 0.f, 1.f));
	m_player.model = glm::scale(m_player.model, m_player.objectScale);
	
	// Working camera follower --------------------------------------------------------------------------------------------------
	if (m_count > 1000)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
		{
			if (backPosition == true)
			{
				cameraPosition = glm::vec3(0.0f, 0.0f, 20.0f);
				projection = glm::perspective(
					glm::radians(45.f),					// Field of View 45 degrees
					4.0f / 3.0f,			// Aspect ratio
					0.1f,					// Display Range Min : 0.1f unit
					100.0f					// Display Range Max : 100.0f unit
				);
				backPosition = false;
			}
			else
			{
				cameraPosition = glm::vec3(-20.0f, 20.0f, 0.0f);
				projection = glm::perspective(
					glm::radians(45.0f),					// Field of View 45 degrees
					4.0f / 3.0f,			// Aspect ratio
					0.1f,					// Display Range Min : 0.1f unit
					100.0f					// Display Range Max : 100.0f unit
				);
				backPosition = true;
			}
			m_count = 0;

		}
	}
	m_count++;
	if (backPosition != true)
	{
		cameraPosition.z -= zoom;
	}
	else
	{
		cameraPosition.x += zoom;
		cameraPosition.y -= zoom;
	}
	zoom = 0;
	view = glm::lookAt(
		m_player.objectPosition + cameraPosition,	// Camera (x,y,z), in World Space
		m_player.objectPosition,		// Camera looking at origin
		glm::vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
	);

	// Non follow cam
	//view = glm::lookAt(
	//	cameraPosition,	// Camera (x,y,z), in World Space
	//	glm::vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
	//	glm::vec3(0.0f, 1.0f, 0.0f)		// 0.0f, 1.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
	//);

	//-------------------------------------------------------------------------------------------------------------
}

void Game::render()
{
	//window.clear(sf::Color(255.f, 255.f, 255.f, 255.f));		//   Reduces it to its base color (funky)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Save current OpenGL render states
	window.pushGLStates();

	// Find mouse position using sf::Mouse
	int x = Mouse::getPosition(window).x;
	int y = Mouse::getPosition(window).y;

	string hud = "Heads Up Display ["
		+ string(toString(x))
		+ "]["
		+ string(toString(y))
		+ "]";

	Text text(hud, font);

	text.setFillColor(sf::Color(255, 255, 255, 170));
	text.setPosition(50.f, 50.f);

	window.draw(text);

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

	glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_player.model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(progID, "ViewMatrix"), 1, GL_FALSE, &view[0][0]);
	
	glUniformMatrix4fv(glGetUniformLocation(progID, "ProjectionMatrix"), 1, GL_FALSE, &projection[0][0]);
	glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);

	for (int i = 0; i < 10; i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(progID, "ModelMatrix"), 1, GL_FALSE, &m_block[i].model[0][0]);
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

	// Check for OpenGL Error code
	error = glGetError();
	if (error != GL_NO_ERROR) {
		DEBUG_MSG(error);
	}
}

void Game::unload()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Cleaning up...");
#endif
	glDetachShader(progID, vsid);	// Shader could be used with more than one progID
	glDetachShader(progID, fsid);	// ..
	glDeleteShader(vsid);			// Delete Vertex Shader
	glDeleteShader(fsid);			// Delete Fragment Shader
	glDeleteProgram(progID);		// Delete Shader
	glDeleteBuffers(1, &vbo);		// Delete Vertex Buffer
	glDeleteBuffers(1, &vib);		// Delete Vertex Index Buffer
	stbi_image_free(img_data);		// Free image stbi_image_free(..)
}

