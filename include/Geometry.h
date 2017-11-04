#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#define MAX_GEOM 2
#define MAX_SHADERS 3

#include <vector>

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
		// the vertex shader. Responsible
		// for perspective distortion.
		GLuint pipelineMatrixID[MAX_SHADERS];

		// Level obstacles
		std::vector<Obstacle*> obstacles;
		
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
