#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#define MAX_GEOM 2
#define MAX_SHADERS 2

#include <vector>

#include <SDL.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

#include <Obstacle.h>

struct Light
{
    glm::vec3 position;
    glm::vec3 rgb;
};

class Geometry
{
	private:
		// Matrixes
		glm::mat4 modelMatrix, viewMatrix, projectionMatrix, pipelineMatrix;

		// VAOs, VBOs & EBOs
		GLuint VAO[MAX_GEOM], VBO[MAX_GEOM], EBO[MAX_GEOM];

		// Compiled shader program IDs
		GLuint shaderProgramID[MAX_SHADERS];

		// Holds reference for uniforms inside
		// the vertex shader.
		GLuint uniformID[MAX_SHADERS*5];

		// Level obstacles
		std::vector<Obstacle*> obstacles;
		
		// Global light
		Light globalLight;

		// Light effects
		double hue, brightness;

		// Textures
		GLuint menuTexture, gameOverTexture, tunnelTexture, obstacleTexture;

		// In degrees
		double tunnelRotation;

		// Game difficulty
		unsigned short int difficulty;
		double movementSpeed;

		// Score
		unsigned int score;
		unsigned int highscores[5];

	public:
		void InitMatrixes();
		void InitShaders();
		void InitGeometry();
		void InitFonts();
		void GenerateObstacles(unsigned int number, unsigned int offset);
		void Cleanup();
		void SetDifficulty(unsigned short int d);
		void SaveHighscore(unsigned int s);
		void ReadHighscore();
		int Draw(Uint32 elapsedTime, unsigned short int gameState);

		void Rotate(Uint32 elapsedTime, int dir);

		double GetRotation();
};

#endif
