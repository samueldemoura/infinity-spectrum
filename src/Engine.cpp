#include <iostream>
#include <fstream>

#include <SDL.h>
#include <GL/glut.h>

#include <Engine.h>

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

	gameWindow = SDL_CreateWindow("Infinity Spectrum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	gameStatus = 3;

	// OpenGL options
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	gameContext = SDL_GL_CreateContext(gameWindow);

	if (!gameContext)
	{
		this->Log("ERROR: Failed to create GLContext...");
		this->Log(SDL_GetError());
		return 0;
	}

	glutInit(&argc, argv);

	// Successfully initialized
	this->Log("LOG: OpenGL window initialized");
	return 1;
}

void Engine::Shutdown()
{
	logFile << "Shutting down...\n";
	logFile.close();

	// Release resources
	SDL_GL_DeleteContext(gameContext);
	SDL_DestroyWindow(gameWindow);
	SDL_Quit();
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
    glClearColor(0.0, 0.5, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
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
	return 1;
}