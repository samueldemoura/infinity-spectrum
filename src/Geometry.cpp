#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

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

	// View matrix: camera at (4,3,3) looks at (0,0,0), Y is up (0, 1, 0)
	viewMatrix = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

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
	pipelineMatrixID = glGetUniformLocation(shaderProgramID[0], "MVP");
}

///
/// VAO & VBO Setup
///
void Geometry::InitGeometry()
{
	//
	// Element 0: 2d square
	//

	// VAO
	glGenVertexArrays(1, &VAO[0]);
	glBindVertexArray(VAO[0]);

	// Define vertices
	static const GLfloat vertices[] = {
		-0.5f,  0.5f, 1.0f, // Top-left
		0.5f,  0.5f, 0.0f, // Top-right
		0.5f, -0.5f, 0.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f  // Bottom-left*/
	};

	static const GLint indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
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
}

///
/// Called every frame
///
void Geometry::Draw()
{
	// Use first shader
	glUseProgram(shaderProgramID[0]);

	// Draw the square
	glBindVertexArray(VAO[0]);
	glUniformMatrix4fv(pipelineMatrixID, 1, GL_FALSE, &pipelineMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

///
/// For testing
///
void Geometry::Move(Uint32 elapsedTime, int dir)
{
	// Move
	glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(elapsedTime * 0.01f * dir, 0.f, 0.f));

	// Apply the translate to the model matrix
	modelMatrix *= translate;

	// Update pipeline matrix
	pipelineMatrix = projectionMatrix * viewMatrix * modelMatrix;
}