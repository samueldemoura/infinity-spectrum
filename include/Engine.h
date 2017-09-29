#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <iostream>
#include <fstream>
#include <string>

#include <SDL.h>
#include <GL/gl.h>

#include <glm/mat4x4.hpp> // glm::mat4

class Engine
{
	private:
		SDL_Window* gameWindow;
		SDL_GLContext gameContext;
		std::ofstream logFile;

		GLuint mainShaderProgramID, vertexArrayID, vertexBufferID, pipelineMatrixID;

		glm::mat4 modelMatrix, viewMatrix, projectionMatrix, pipelineMatrix;

		void SceneSetup();
		void Update();
		void Draw();

		// 0 - Main menu, 1 - Highscores, 2 - Options, 3 - Gameplay
		unsigned short int gameStatus; 

	public:
		bool Initialize(int argc, char *argv[]);
		void Shutdown();
		bool GameLoop();
		bool Log(std::string msg);
};

#endif // _ENGINE_H_
