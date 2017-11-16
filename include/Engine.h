#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <iostream>
#include <fstream>
#include <string>

#include <SDL.h>
#include <SDL_mixer.h>
#include <GL/gl.h>

#include <Geometry.h>

class Engine
{
	private:
		SDL_Window* gameWindow;
		SDL_GLContext gameContext;
		Mix_Music *gameMusic;
		const unsigned char* keystate;
		Uint32 tickStart, tickEnd;
		std::ofstream logFile;
		Geometry geometryHandler;

		void Update(Uint32 elapsedTime);
		int Draw();

		// 0 - Main menu, 1 - Highscores, 2 - Options, 3 - Gameplay
		unsigned short int gameState; 

	public:
		bool Initialize(int argc, char *argv[]);
		void Shutdown();
		bool GameLoop();
		bool Log(std::string msg);
};

#endif // _ENGINE_H_
