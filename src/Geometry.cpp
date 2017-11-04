#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#define PI 3.14159265

#include <math.h> // pow
#include <stdlib.h> // rand
#include <iostream>

#include <SDL.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ShaderLoader.h>
#include <Geometry.h>

///
/// Matrixes
///
void Geometry::InitMatrixes()
{
	// Projection matrix : 90° Field of View, 16:9 aspect ratio, display range: 0.1 unit <-> 100 units
	projectionMatrix = glm::perspective(glm::radians(90.0f), 16.0f/9.0f, 0.1f, 100.0f);

	// View matrix: camera at (2,2,5) looks at (0,0,0), Y is up (0, 1, 0)
	viewMatrix = glm::lookAt(glm::vec3(0, -1.5, 5), glm::vec3(0, -1.5, 0), glm::vec3(0, 1, 0));

	// Model matrix: identity matrix
	modelMatrix = glm::mat4(1.0f);

	// Pipeline matrix: all of the previous, put together
	pipelineMatrix = projectionMatrix * viewMatrix * modelMatrix;
}

///
/// Shaders
///
void Geometry::InitShaders()
{
	// Compile shaders
	shaderProgramID[0] = LoadShaders("res/vertexShader.vert", "res/fragmentShader.frag");

	// Get a handle for the "MVP" uniform inside our shader
	pipelineMatrixID[0] = glGetUniformLocation(shaderProgramID[0], "MVP");

	// Compile shaders
	shaderProgramID[1] = LoadShaders("res/vertexShader.vert", "res/fragmentShader2.frag");

	// Get a handle for the "MVP" uniform inside our shader
	pipelineMatrixID[1] = glGetUniformLocation(shaderProgramID[1], "MVP");

	// Compile shaders
	shaderProgramID[2] = LoadShaders("res/vertexShader.vert", "res/fragmentShader3.frag");

	// Get a handle for the "MVP" uniform inside our shader
	pipelineMatrixID[2] = glGetUniformLocation(shaderProgramID[2], "MVP");
}

///
/// VAO & VBO Setup
///
void Geometry::InitGeometry()
{
	// Test obstacles
	srand(time_t(NULL));
	for (int i = 0; i < 100; ++i)
	{
		//bool obstacleArray[6] = {0, 0, 1, 0, 0, 0};
		bool obstacleArray[6] = {rand() % 3, rand() % 2, rand() % 4, rand() % 2, rand() % 3, rand() % 2};
		Obstacle* obstacleTest = new Obstacle(obstacleArray, (i * 13) + 50);
		obstacles.push_back(obstacleTest);
	}

	//
	// Element 0: 2d square on the floor
	//

	// VAO
	glGenVertexArrays(1, &VAO[0]);
	glBindVertexArray(VAO[0]);

	// Define vertices
	static const GLfloat vertices[] = {
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, -1.0f
	};

	static const GLint indices[] = {
		0, 1, 2,
		0, 3, 2
	};

	// Create a vertex buffer object, bind it and pass vertices to it
	glGenBuffers(1, &VBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Link vertex attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Create & fill element buffer
	glGenBuffers(1, &EBO[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//
	// Element 1: 3d cube
	//

	// VAO
	glGenVertexArrays(1, &VAO[1]);
	glBindVertexArray(VAO[1]);

	// Define vertices
	static const GLfloat verticesCube[] = {
		-1.0f, -1.0f, -1.0f, //0 back bottom left
		-1.0f, 1.0f, -1.0f, //1 back top left
		1.0f, -1.0f,-1.0f, //2 back bottom right
		1.0f, 1.0f, -1.0f, //3 back top right
		-1.0f, -1.0f, 1.0f, //4 front bottom left
		-1.0f, 1.0f, 1.0f, //5 front top left
		1.0f, -1.0f, 1.0f, //6 front bottom right
		1.0f, 1.0f, 1.0f, //7 front top right
	};

	static const GLint indicesCube[] = {
		0, 1, 3, //back
		0, 2, 3, //back
		0, 1, 5, //left
		0, 4, 5, //left
		2, 3, 7, //right
		2, 6, 7, //right
		1, 3, 7, //top
		1, 5, 7, //top
		4, 5, 7, //front
		4, 6, 7, //front
		0, 2, 6, //bottom
		0, 4, 6, //bottom
	};

	// Create a vertex buffer object, bind it and pass vertices to it
	glGenBuffers(1, &VBO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

	// Create & fill element buffer
	glGenBuffers(1, &EBO[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesCube), indicesCube, GL_STATIC_DRAW); 

	// Set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

///
/// Called every frame
///
int Geometry::Draw(Uint32 elapsedTime)
{
	// Use first shader
	glUseProgram(shaderProgramID[0]);

	// Draw the tunnel
	glBindVertexArray(VAO[0]);

	// Draw all 6 faces individually
	for (int i = 0; i < 6; ++i)
	{
		float j = (i * 60) + 30 - tunnelRotation;

		float dx = 1.73 * cos( j * (PI/180) );
		float dy = 1.73 * sin( j * (PI/180) );

		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 1.f, 100.f));
		modelMatrix = glm::rotate(modelMatrix, (j + 90) * ((float)PI/180), glm::vec3(0.f, 0.f, 1.f));
		pipelineMatrix = projectionMatrix * viewMatrix * modelMatrix;

		glUseProgram(shaderProgramID[(i % 2 == 0)]);
		glUniformMatrix4fv(pipelineMatrixID[(i % 2 == 0)], 1, GL_FALSE, &pipelineMatrix[0][0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	// TODO: iterate through obstacles and draw them here
	for (auto it = obstacles.begin(); it != obstacles.end(); ++it)
	{
		Obstacle *o = *it;
		o->Update(elapsedTime * 0.05);

		for (int i = 0; i < 6; ++i)
		{
			if (o->side[i])
			{
				if (o->distance < -5)
				{
					int pos = ((((int)tunnelRotation + 30) / 60) - 0) % 6;
					if ( pos == i )
					{
						std::cout << "\nAngle: " << tunnelRotation;
						std::cout << "\nWall Pos: " << i;
						std::cout << "\nCalculated Pos: " << pos;
						std::cout << '\n';
						return 1;
					}
				}

				float j = (i * 60) + 30 + (-tunnelRotation) - (60*2);

				float dx = 1.70 * cos( j * (PI/180) );
				float dy = 1.70 * sin( j * (PI/180) );

				modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(dx, dy, (o->distance) * -1.0 ));
				modelMatrix = glm::rotate(modelMatrix, (j + 90) * ((float)PI/180), glm::vec3(0.f, 0.f, 1.f));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, .7f, .3f));
				
				pipelineMatrix = projectionMatrix * viewMatrix * modelMatrix;

				glUseProgram(shaderProgramID[2]);
				glBindVertexArray(VAO[1]);
				glUniformMatrix4fv(pipelineMatrixID[2], 1, GL_FALSE, &pipelineMatrix[0][0]);
				glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, 0);
			}
		}

		if (o->distance < -5)
		{
			obstacles.erase(it);
			delete o;
		}
	}

	return 0;
}

///
/// For testing
///
void Geometry::Rotate(Uint32 elapsedTime, int dir)
{
	// Move
	tunnelRotation += ((elapsedTime * 0.5f) * dir);
	if (tunnelRotation > 360)
		tunnelRotation -= 360;

	if (tunnelRotation < 0)
		tunnelRotation += 360;
}

double Geometry::GetRotation()
{
	return tunnelRotation;
}

void Geometry::Move(Uint32 elapsedTime, int dir)
{
	// Move
	glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(elapsedTime * 0.01f * -dir, 0.f, 0.f));

	// Apply the translate to the matrix
	viewMatrix *= translate;

	// Update pipeline matrix
	pipelineMatrix = projectionMatrix * viewMatrix * modelMatrix;
}