#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#define MAX_GEOM 2
#define MAX_SHADERS 3

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

		// Texutres
		GLuint obstacleTexture;

		// In degrees
		double tunnelRotation;

	public:
		void InitMatrixes();
		void InitShaders();
		void InitGeometry();
		int Draw(Uint32 elapsedTime);

		void Rotate(Uint32 elapsedTime, int dir);
		void Move(Uint32 elapsedTime, int dir);

		double GetRotation();
};

#endif
