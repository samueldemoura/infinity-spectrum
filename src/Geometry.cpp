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
#include <TextureLoader.h>
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
	shaderProgramID[1] = LoadShaders("res/vertexShader.vert", "res/fragmentShader2.frag");

	// Get a handle for the uniforms inside our shaders
	uniformID[0] = glGetUniformLocation(shaderProgramID[0], "camera");
	uniformID[1] = glGetUniformLocation(shaderProgramID[0], "model");
	uniformID[2] = glGetUniformLocation(shaderProgramID[0], "tex");
	uniformID[3] = glGetUniformLocation(shaderProgramID[0], "light.position");
	uniformID[4] = glGetUniformLocation(shaderProgramID[0], "light.rgb");
}

///
/// VAO & VBO Setup
///
void Geometry::InitGeometry()
{
	// Load textures
	tunnelTexture = LoadTexture("res/tunnel.bmp");
	obstacleTexture = LoadTexture("res/obstacle.bmp");

	// Setup global light
	globalLight.position = glm::vec3(0.f, 0.f, 0.f);
	globalLight.rgb = glm::vec3(1.f, 255.f, 0.f);

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
		//  X     Y     Z      U     V              Normal
		-1.0f, 0.0f, 1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f,
		 1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
		 1.0f, 0.0f,-1.0f,  1.0f, 1.0f,  0.0f,  1.0f, 0.0f,
		-1.0f, 0.0f,-1.0f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
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
	int vert = glGetAttribLocation(shaderProgramID[0], "vert");
	int vertTexCoord = glGetAttribLocation(shaderProgramID[0], "vertTexCoord");
	int vertNormal = glGetAttribLocation(shaderProgramID[0], "vertNormal");
	
	//                           index  size      type  normalize             stride                   offset pointer
	glEnableVertexAttribArray(vert);
	glVertexAttribPointer(        vert,    3, GL_FLOAT,  GL_FALSE, 8*sizeof(GLfloat),                            NULL);
	glEnableVertexAttribArray(vertTexCoord);
	glVertexAttribPointer(vertTexCoord,    2, GL_FLOAT,   GL_TRUE, 8*sizeof(GLfloat), (GLvoid*)(3 * sizeof(GL_FLOAT)) );
	glEnableVertexAttribArray(vertNormal);
	glVertexAttribPointer(  vertNormal,    3, GL_FLOAT,   GL_TRUE, 8*sizeof(GLfloat), (GLvoid*)(5 * sizeof(GL_FLOAT)) );

	//
	// Element 1: 3d cube
	//

	// VAO
	glGenVertexArrays(1, &VAO[1]);
	glBindVertexArray(VAO[1]);

	// Define vertices
	static const GLfloat verticesCube[] = {
		//  X     Y     Z       U     V               Normal
		// bottom
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		 1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,

		// top
		-1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		 1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f,

		// front
		-1.0f,-1.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f,

		// back
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f,-1.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		 1.0f, 1.0f,-1.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,

		// left
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f,-1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f,-1.0f, 1.0f,   0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f,-1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,

		// right
		 1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f,-1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 1.0f,-1.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 1.0f, 1.0f,-1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f
	};

	// Create a vertex buffer object, bind it and pass vertices to it
	glGenBuffers(1, &VBO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);
	
	// Set vertex attribute pointers
	//                           index  size      type  normalize             stride                   offset pointer
	glEnableVertexAttribArray(vert);
	glVertexAttribPointer(        vert,    3, GL_FLOAT,  GL_FALSE, 8*sizeof(GLfloat),                            NULL);
	glEnableVertexAttribArray(vertTexCoord);
	glVertexAttribPointer(vertTexCoord,    2, GL_FLOAT,   GL_TRUE, 8*sizeof(GLfloat), (GLvoid*)(3 * sizeof(GL_FLOAT)) );
	glEnableVertexAttribArray(vertNormal);
	glVertexAttribPointer(  vertNormal,    3, GL_FLOAT,   GL_TRUE, 8*sizeof(GLfloat), (GLvoid*)(5 * sizeof(GL_FLOAT)) );

	// Unbind vertex array object
	glBindVertexArray(0);
}

///
/// Called every frame
///
int Geometry::Draw(Uint32 elapsedTime)
{
	// Place light in tunnel
	float dx = cos ( tunnelRotation * (PI/180) );
	float dy = sin ( tunnelRotation * (PI/180) );

	globalLight.position = glm::vec3(dx, dy, 3.f);
	globalLight.rgb = glm::vec3(1-(tunnelRotation/360), tunnelRotation/360, 1.f);

	// Draw the tunnel
	glUseProgram(shaderProgramID[0]);
	glBindVertexArray(VAO[0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tunnelTexture);

	// Draw all 6 faces individually
	for (int i = 0; i < 6; ++i)
	{
		float j = (i * 60) + 30 - tunnelRotation;

		float dx = 1.73 * cos( j * (PI/180) );
		float dy = 1.73 * sin( j * (PI/180) );

		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 1.f, 100.f));
		modelMatrix = glm::rotate(modelMatrix, (j + 90) * ((float)PI/180), glm::vec3(0.f, 0.f, 1.f));

		pipelineMatrix = projectionMatrix * viewMatrix;

		glUniformMatrix4fv(uniformID[0], 1, GL_FALSE, &pipelineMatrix[0][0]);
		glUniformMatrix4fv(uniformID[1], 1, GL_FALSE, &modelMatrix[0][0]);
		glUniform1i(uniformID[2], 0);
		glUniform3fv(uniformID[3], 1, &globalLight.position[0]);
		glUniform3fv(uniformID[4], 1, &globalLight.rgb[0]);
		glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, 0);
	}

	// Draw the obstacles
	glUseProgram(shaderProgramID[1]);
	glBindVertexArray(VAO[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, obstacleTexture);

	for (auto it = obstacles.begin(); it != obstacles.end(); ++it)
	{
		Obstacle *o = *it;
		o->Update(elapsedTime * 0.05);

		if (o->distance < -5)
		{
			// TODO: Fix blinking when obstacle goes past the camera
			// TODO: Increment score
			obstacles.erase(it);
			delete o;
			continue;
		}

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

						// TODO: Game over screen
					}
				}

				float j = (i * 60) + 30 + (-tunnelRotation) - (60*2);

				float dx = 1.70 * cos( j * (PI/180) );
				float dy = 1.70 * sin( j * (PI/180) );

				modelMatrix = glm::mat4(1.0f);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(dx, dy, (o->distance) * -1.0) );
				modelMatrix = glm::rotate(modelMatrix, (j + 90) * ((float)PI/180), glm::vec3(0.f, 0.f, 1.f));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, .7f, .3f));
				
				pipelineMatrix = projectionMatrix * viewMatrix;

				glUniformMatrix4fv(uniformID[0], 1, GL_FALSE, &pipelineMatrix[0][0]);
				glUniformMatrix4fv(uniformID[1], 1, GL_FALSE, &modelMatrix[0][0]);
				glUniform1i(uniformID[2], 0);
				glUniform3fv(uniformID[3], 1, &globalLight.position[0]);
				glUniform3fv(uniformID[4], 1, &globalLight.rgb[0]);
				glDrawArrays(GL_TRIANGLES, 0, 6*6);
			}
		}
	}

	return 0;
}

double Geometry::GetRotation()
{
	return tunnelRotation;
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

void Geometry::Move(Uint32 elapsedTime, int dir)
{
	// Move
	glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(elapsedTime * 0.01f * -dir, 0.f, 0.f));

	// Apply the translate to the matrix
	viewMatrix *= translate;

	// Update pipeline matrix
	pipelineMatrix = projectionMatrix * viewMatrix * modelMatrix;
}
