#include <iostream>
#include <fstream>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <SDL.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <Engine.h>
#include <ShaderLoader.h>

///
/// Startup & shutdown
///
bool Engine::Initialize(int argc, char *argv[])
{
	// Prepare log file
	logFile.open("log.txt", std::ofstream::out | std::ofstream::app);
	this->Log("--- LAUNCHING GAME ---");

	// Initialize OpenGL with SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		this->Log("ERROR: Failed to initialize SDL...");
		this->Log(SDL_GetError());
		return 0;
	}

	///
	/// OpenGL options
	///
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	gameWindow = SDL_CreateWindow("Infinity Spectrum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	gameContext = SDL_GL_CreateContext(gameWindow);
	gameStatus = 3;

	if (!gameContext)
	{
		this->Log("ERROR: Failed to create GLContext...");
		this->Log(SDL_GetError());
		return 0;
	}

	// VAO, VBO, matrix & shader setup
	glutInit(&argc, argv);
	SceneSetup();

	// Successfully initialized
	this->Log("LOG: OpenGL window initialized");
	std::cout << glGetString(GL_VERSION);
	return 1;
}

void Engine::Shutdown()
{
	this->Log("--- SHUTTING DOWN ---");
	logFile.close();

	// Release resources
	SDL_GL_DeleteContext(gameContext);
	SDL_DestroyWindow(gameWindow);
	SDL_Quit();
}

///
/// VAO, VBO, matrix & shader setup
///
void Engine::SceneSetup()
{
	///
	/// Matrixes
	///

	// Projection matrix : 90° Field of View, 16:9 aspect ratio, display range: 0.1 unit <-> 100 units
	projectionMatrix = glm::perspective(glm::radians(90.0f), 16.0f/9.0f, 0.1f, 100.0f);

	// View matrix: camera at (4,3,3) looks at (0,0,0), Y is up (0, 1, 0)
	viewMatrix = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	// Model matrix: identity matrix
	modelMatrix = glm::mat4(1.0f);

	// Pipeline matrix: all of the previous, put together
	pipelineMatrix = projectionMatrix * viewMatrix * modelMatrix;

	///
	/// Shaders
	///

	// Compile shaders
	mainShaderProgramID = LoadShaders("res/vertexShader.vert", "res/fragmentShader.frag");

	// Get a handle for the "MVP" uniform inside our shader
	pipelineMatrixID = glGetUniformLocation(mainShaderProgramID, "MVP");

	///
	/// VAO & VBO Setup
	///
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create our triangle's polygons
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	// Generate a vertex buffer and put the identifier in vertexBufferID
	glGenBuffers(1, &vertexBufferID);

	// The following commands will talk about our 'vertexBufferID' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

	// Pass our vertices into the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Send our transformation to the currently
	// bound shader, in the "MVP" uniform
	glUniformMatrix4fv(pipelineMatrixID, 1, GL_FALSE, &pipelineMatrix[0][0]);

	// Linking vertex attributes	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

///
/// Continuously receives events & keeps window open
///
bool Engine::GameLoop()
{
	this->Log("LOG: Entering game loop");
	SDL_Event event;
	bool keepRunning = 1;

	while (keepRunning)
	{
		while (SDL_PollEvent(&event))
		{
			// Exit game loop
			if (event.type == SDL_QUIT)
				keepRunning = 0;

			// Handle keyboard input
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_q:
				case SDLK_ESCAPE:
					keepRunning = 0;
					break;
				default:
					break;
				}
			}

			// Run game logic & draw onto screen
			this->Update();
			this->Draw();
		}
	}

	this->Log("LOG: Exiting game loop");
	return 1;
}

///
/// Game logic
///
void Engine::Update()
{
	// TODO
}

///
/// OpenGL calls
///
void Engine::Draw()
{
	// Clear the screen
	glClearColor(0.0, 0.0, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(mainShaderProgramID);

	// Use our previously defined VAO
	glBindVertexArray(vertexArrayID);

	// Send pipeline matrix to shader
	glUniformMatrix4fv(pipelineMatrixID, 1, GL_FALSE, &pipelineMatrix[0][0]);

	// Draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Swap buffers
	SDL_GL_SwapWindow(gameWindow);
}

///
/// Basic logging support
///
bool Engine::Log(std::string msg)
{
	if (!logFile)
		return 0;

	logFile << msg << '\n';
	std::cout << msg << std::endl;

	return 1;
}
