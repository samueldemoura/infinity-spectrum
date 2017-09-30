#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <iostream>
#include <fstream>

#include <SDL.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include <Engine.h>
#include <Geometry.h>

///
/// Startup & shutdown
///
bool Engine::Initialize(int argc, char *argv[])
{
	// Prepare log file
	logFile.open("log.txt", std::ofstream::out | std::ofstream::app);
	Log("--- LAUNCHING GAME ---");

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Log("ERROR: Failed to initialize SDL...");
		Log(SDL_GetError());
		return 0;
	}

	/// OpenGL options & SDL window creation
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	gameWindow = SDL_CreateWindow("Infinity Spectrum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	gameContext = SDL_GL_CreateContext(gameWindow);
	gameStatus = 3;

	if (!gameContext)
	{
		Log("ERROR: Failed to create GLContext...");
		Log(SDL_GetError());
		return 0;
	}

	// Pointer to keyboard state
	keystate = SDL_GetKeyboardState(NULL);

	// More OpenGL options, after context creation
	glutInit(&argc, argv);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	// Game geometry setup
	geometryHandler.InitMatrixes();
	geometryHandler.InitShaders();
	geometryHandler.InitGeometry();

	// Successfully initialized
	Log("LOG: OpenGL window initialized");
	std::cout << glGetString(GL_VERSION) << std::endl;

	return 1;
}

void Engine::Shutdown()
{
	Log("--- SHUTTING DOWN ---");
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
	Log("LOG: Entering game loop");
	SDL_Event event;
	bool keepRunning = 1;
	tickStart = SDL_GetTicks();

	while (keepRunning)
	{
		tickEnd = tickStart;
		tickStart = SDL_GetTicks();
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
		}

		// Run game logic & draw onto screen
		Update(SDL_TICKS_PASSED(tickStart, tickEnd));
		Draw();
	}

	Log("LOG: Exiting game loop");
	return 1;
}

///
/// Game logic
///
void Engine::Update(Uint32 elapsedTime)
{
	if (keystate[SDL_SCANCODE_RIGHT])
	{
		geometryHandler.Rotate(elapsedTime, 1);
	}

	if (keystate[SDL_SCANCODE_LEFT])
	{
		geometryHandler.Rotate(elapsedTime, -1);
	}

	if (keystate[SDL_SCANCODE_UP])
	{
		geometryHandler.Move(elapsedTime, 1);
	}

	if (keystate[SDL_SCANCODE_DOWN])
	{
		geometryHandler.Move(elapsedTime, -1);
	}
}

///
/// OpenGL calls
///
void Engine::Draw()
{
	// Clear the screen
	glClearColor(0.0, 0.0, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw 3d geometry
	geometryHandler.Draw();

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
