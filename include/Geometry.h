#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#define MAX_GEOM 2
#define MAX_SHADERS 2

#include <queue>

#include <SDL.h>
#include <GL/gl.h>
#include <glm/mat4x4.hpp>

#include <Obstacle.h>

class Geometry
{
	private:
		// Matrixes
		glm::mat4 modelMatrix, viewMatrix, projectionMatrix, pipelineMatrix;

		// VAOs, VBOs & EBOs
		GLuint VAO[MAX_GEOM], VBO[MAX_GEOM], EBO[MAX_GEOM];

		// Compiled shader program IDs
		GLuint shaderProgramID[MAX_SHADERS];

		// Holds reference for "MVP" uniform inside
		// the generic vertex shader. Responsible
		// for perspective distortion.
		GLuint pipelineMatrixID;

		// Level obstacles
		std::queue<Obstacle> obstacles;

	public:
		void InitMatrixes();
		void InitShaders();
		void InitGeometry();
		void Draw();

		void Move(Uint32 elapsedTime, int dir);
};

#endif
