#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <iostream>
#include <fstream>

#include <SDL.h>
#include <SDL_mixer.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include <Engine.h>
#include <Geometry.h>

#define SPEED_MULT 1 //6

///
/// Startup & shutdown
///
bool Engine::Initialize(int argc, char *argv[])
{
	// Prepare log file
	logFile.open("log.txt", std::ofstream::out | std::ofstream::app);
	Log("--- LAUNCHING GAME ---");

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		Log("ERROR: Failed to initialize SDL...");
		Log(SDL_GetError());
		return 0;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
	{
		Log("SDL_mixer could not initialize! SDL_mixer Error:");
		Log(Mix_GetError());
		return 0;
	}
	
	if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
    {
    	Log("SDL_mixer could not initialize! SDL_mixer Error:");
		Log(Mix_GetError());
		return 0;
    }

	//Load media
	if(LoadMedia())
	{
		Log("Media loaded successfully!");
	}
	else
	{
		Log("Failed to load media!");
	}

	/// OpenGL options & SDL window creation
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	gameWindow = SDL_CreateWindow("Infinity Spectrum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	gameContext = SDL_GL_CreateContext(gameWindow);
	gameState = 0;

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
	geometryHandler.InitFonts();

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
	//Free sound effects
	Mix_FreeChunk(gameHit);
	Mix_FreeChunk(gameSelect);
	gameHit = nullptr;
	gameSelect = nullptr;

	//Free music
	Mix_FreeMusic(gameMusic);
	gameMusic = nullptr;
	Mix_CloseAudio();
	Mix_Quit();

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
	tickStart = glutGet(GLUT_ELAPSED_TIME); //SDL_GetTicks();

	while (keepRunning)
	{
		tickEnd = tickStart;
		tickStart = glutGet(GLUT_ELAPSED_TIME); //SDL_GetTicks();
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
				// Difficulty select
				case SDLK_1:
					if (gameState == 0)
					{
						//Play select sound effect
						Mix_PlayChannel(-1, gameSelect, 0);
						gameState = 3;
						geometryHandler.SetDifficulty(1);

						//Play music
						Mix_PlayMusic( gameMusic, -1 );
						if (Mix_PlayMusic(gameMusic, -1) == -1)
							Log(Mix_GetError());
					}
					break;

				case SDLK_2:
					if (gameState == 0)
					{
						//Play select sound effect
						Mix_PlayChannel(-1, gameSelect, 0);
						gameState = 3;
						geometryHandler.SetDifficulty(2);

						//Play music
						Mix_PlayMusic( gameMusic, -1 );
						if (Mix_PlayMusic(gameMusic, -1) == -1)
							Log(Mix_GetError());
					}
					break;

				case SDLK_3:
					if (gameState == 0)
					{
						//Play select sound effect
						Mix_PlayChannel(-1, gameSelect, 0);
						gameState = 3;
						geometryHandler.SetDifficulty(3);

						//Play music
						Mix_PlayMusic( gameMusic, -1 );
						if (Mix_PlayMusic(gameMusic, -1) == -1)
							Log(Mix_GetError());
					}
					break;

				// Game over screen
				case SDLK_RETURN:
				case SDLK_SPACE:
					if (gameState == 1)
						gameState = 0;
					break;

				// Quit
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
		Update(SDL_TICKS_PASSED(tickStart, tickEnd) * SPEED_MULT);
		if (Draw())
		{
			//Play sound effect when hitting an obstacle
			Mix_PlayChannel(-1, gameHit, 0 );
			gameState = 1;
			//Stop Music
			Mix_HaltMusic();	
		}
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

	/*if (keystate[SDL_SCANCODE_UP])
	{
		geometryHandler.Move(elapsedTime, 1);
	}

	if (keystate[SDL_SCANCODE_DOWN])
	{
		geometryHandler.Move(elapsedTime, -1);
	}*/
}

///
/// OpenGL calls
///
int Engine::Draw()
{
	// Clear the screen
	glClearColor(0.0, 0.0, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw 3d geometry
	int result = geometryHandler.Draw(SDL_TICKS_PASSED(tickStart, tickEnd) * SPEED_MULT, gameState);

	// Swap buffers
	SDL_GL_SwapWindow(gameWindow);

	return result;
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

bool Engine::LoadMedia()
{
	//Loading success flag 
	bool success = true; 

	//Load music 
	gameMusic = Mix_LoadMUS("res/music.mp3");
	if (gameMusic == nullptr) 
	{ 
		Log("Failed to load music. SDL_mixer Error: ");
		Log(Mix_GetError());
		success = false;
	}

	//Load sound effects
	gameHit = Mix_LoadWAV("res/hit.wav");
	if (gameHit == nullptr)
	{
		Log("Failed to load hit obstacle sound effect. SDL_mixer Error: ");
		Log(Mix_GetError());
		success = false;
	}

	gameSelect = Mix_LoadWAV("res/select.wav");
	if (gameSelect == nullptr)
	{
		Log("Failed to load select menu option sound effect. SDL_mixer Error: ");
		Log(Mix_GetError());
		success = false;
	}

	return success;
}

