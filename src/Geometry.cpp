#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1
#define PI 3.14159265

#include <math.h> // pow
#include <stdlib.h> // rand
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <SDL.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gltext.h>

#include <ShaderLoader.h>
#include <TextureLoader.h>
#include <Geometry.h>
#include <Color.h>

///
/// Matrixes
///
void Geometry::InitMatrixes()
{
	// Projection matrix : 90° Field of View, 16:9 aspect ratio, display range: 0.1 unit <-> 100 units
	projectionMatrix = glm::perspective(glm::radians(90.0f), 16.0f/9.0f, 0.1f, 100.0f);

	// View matrix: camera at (0,-1.5,5) looks at (0,-1.5,0), Y is up (0, 1, 0)
	//viewMatrix = glm::lookAt(glm::vec3(0, -1.62, 5), glm::vec3(0, -1.5, 0), glm::vec3(0, 1, 0));
	viewMatrix = glm::lookAt(glm::vec3(0, 0, 6), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

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
	// Generate random seed
	srand(time_t(NULL));

	// Load textures
	menuTexture = LoadTexture("res/mainMenu.jpg");
	gameOverTexture = LoadTexture("res/gameOver.jpg");
	tunnelTexture = LoadTexture("res/tunnel.bmp");
	obstacleTexture = LoadTexture("res/obstacle.bmp");

	// Setup global light
	globalLight.position = glm::vec3(0.f, 0.f, 0.f);
	globalLight.rgb = glm::vec3(1.f, 255.f, 0.f);
	brightness = 1;

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

	//
	// Element 2: 3d spaceship
	//

	// VAO
	glGenVertexArrays(1, &VAO[2]);
	glBindVertexArray(VAO[2]);

	// Define vertices
	static const GLfloat verticesShip[] = {
		//  X     Y     Z       U     V               Normal
		// bottom
		 1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, -1.0f, 0.0f,//0
		 1.0f, 0.0f,-1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,//1
		-1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,//2
		-1.0f, 0.0f,-1.0f,   1.0f, 1.0f,   0.0f, -1.0f, 0.0f,//3
		-1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   0.0f, -1.0f, 0.0f,//2
		 1.0f, 0.0f,-1.0f,   0.0f, 1.0f,   0.0f, -1.0f, 0.0f,//1

		// right
		 1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f,//0
		 1.0f, 0.0f,-1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f,//1
		 0.0f, 3.0f, 0.0f,   0.5f, 0.5f,   1.0f, 0.0f, 0.0f,//4

		// front
		 1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,//0
		-1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,//2
		 0.0f, 3.0f, 0.0f,   0.5f, 0.5f,   0.0f, 0.0f, 1.0f,//4

		// left
		-1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,//2
		-1.0f, 0.0f,-1.0f,   1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,//3
		 0.0f, 3.0f, 0.0f,   0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,//4

		// back
		 1.0f, 0.0f,-1.0f,   0.0f, 0.0f,    0.0f, 0.0f, -1.0f,//1
		-1.0f, 0.0f,-1.0f,   1.0f, 0.0f,    0.0f, 0.0f, -1.0f,//3
		 0.0f, 3.0f, 0.0f,   0.5f, 0.5f,    0.0f, 0.0f, -1.0f,//4
	};

	/// Create a vertex buffer object, bind it and pass vertices to it
	glGenBuffers(1, &VBO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesShip), verticesShip, GL_STATIC_DRAW);

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
/// Loads fonts
///
void Geometry::InitFonts()
{
	gltInit();
	ReadHighscore();
}


// Helper function
void SetArray(bool *array, bool a, bool b, bool c, bool d, bool e, bool f)
{
	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = d;
	array[4] = e;
	array[5] = f;
}

///
/// Generates random obstacles
///
void Geometry::GenerateObstacles(unsigned int number, unsigned int offset)
{
	int type, lastType;

	for (unsigned int i = 0; i < number; ++i)
	{
		lastType = type;
		type = rand() % 11;

		// Trick to make level seem slightly more random
		if (type-3 == lastType || type == lastType || type+3 == lastType)
		{
			type = (type + 1) % 11;
		}

		int distance;

		switch (difficulty)
		{
			case 1:
				distance = ( (i+offset) * 11) + 16;
				break;
			case 2:
				distance = ( (i+offset) * 16) + 40;
				break;
			case 3:
				distance = ( (i+offset) * 28) + 58;
				break;
		}

		bool obstacleArray[6];

		switch (type)
		{
			case 0:
				SetArray(&obstacleArray[0], 1,1,1,1,1,0);
				break;
			case 1:
				SetArray(&obstacleArray[0], 1,1,1,1,0,1);
				break;
			case 2:
				SetArray(&obstacleArray[0], 1,1,1,0,1,1);
				break;
			case 3:
				SetArray(&obstacleArray[0], 1,1,0,1,1,1);
				break;
			case 4:
				SetArray(&obstacleArray[0], 1,0,1,1,1,1);
				break;
			case 5:
				SetArray(&obstacleArray[0], 0,1,1,1,1,1);
				break;
			case 6:
				SetArray(&obstacleArray[0], 1,0,1,0,1,0);
				break;
			case 7:
				SetArray(&obstacleArray[0], 0,1,0,1,0,1);
				break;
			case 8:
				SetArray(&obstacleArray[0], 1,0,1,1,1,0);
				break;
			case 9:
				SetArray(&obstacleArray[0], 0,1,1,1,0,1);
				break;
			case 10:
				SetArray(&obstacleArray[0], 1,1,1,0,1,0);
				break;
		}

		Obstacle* obstacle = new Obstacle(obstacleArray, distance);
		obstacles.push_back(obstacle);
	}
}

///
/// Called every frame
///
int Geometry::Draw(Uint32 elapsedTime, unsigned short int gameState)
{
	// Draw main menu
	if (gameState == 0)
	{
		// Ignore lighting
		glUseProgram(shaderProgramID[1]);

		// Use 2d square VAO to draw menu texture
		glBindVertexArray(VAO[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, menuTexture);

		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.85f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(9.f, 9.f, 1.f));
		modelMatrix = glm::rotate(modelMatrix, -90 * ((float)PI/180), glm::vec3(1.f, 0.f, 0.f));
		pipelineMatrix = projectionMatrix * viewMatrix;

		glUniformMatrix4fv(uniformID[0], 1, GL_FALSE, &pipelineMatrix[0][0]);
		glUniformMatrix4fv(uniformID[1], 1, GL_FALSE, &modelMatrix[0][0]);
		glUniform1i(uniformID[2], 0);
		glUniform3fv(uniformID[3], 1, &globalLight.position[0]);
		glUniform3fv(uniformID[4], 1, &globalLight.rgb[0]);
		glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, 0);

		// beautiful work of art, please do not judge
		std::stringstream highscoreText;
		for (int i = 0; i < 5; ++i)
		{
			highscoreText << "Highscore #" << i+1 << ": " << highscores[i] << '\n';
		}
		GLTtext *text = gltCreateText();
		gltSetText(text, highscoreText.str().c_str());
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltDrawText2D(text, 0, 0, 1);
	}

	// Draw the game over screen
	else if (gameState == 1)
	{
		// Ignore lighting
		glUseProgram(shaderProgramID[1]);

		// Use 2d square VAO to draw menu texture
		glBindVertexArray(VAO[0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gameOverTexture);

		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.85f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(9.f, 9.f, 1.f));
		modelMatrix = glm::rotate(modelMatrix, -90 * ((float)PI/180), glm::vec3(1.f, 0.f, 0.f));
		pipelineMatrix = projectionMatrix * viewMatrix;

		glUniformMatrix4fv(uniformID[0], 1, GL_FALSE, &pipelineMatrix[0][0]);
		glUniformMatrix4fv(uniformID[1], 1, GL_FALSE, &modelMatrix[0][0]);
		glUniform1i(uniformID[2], 0);
		glUniform3fv(uniformID[3], 1, &globalLight.position[0]);
		glUniform3fv(uniformID[4], 1, &globalLight.rgb[0]);
		glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, 0);
	}

	// Draw the game itself
	else if (gameState == 3)
	{
		// Update color
		hue = (hue + (elapsedTime*0.1) );

		if (hue > 255)
			hue = 0;

		if (brightness > 1)
			brightness -= elapsedTime * 0.04;

		HsvColor hsv;
		hsv.h = hue;
		hsv.s = 255;
		hsv.v = 255;

		RgbColor rgb;
		rgb = HsvToRgb(hsv);

		// Place light in tunnel
		float dx = cos ( tunnelRotation * (PI/180) );
		float dy = sin ( tunnelRotation * (PI/180) );
		globalLight.position = glm::vec3(dx, dy, 3.f);
		globalLight.rgb = glm::vec3(brightness * rgb.r / (float)255, brightness * rgb.g / (float)255, brightness * rgb.b / (float)255);

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
		glUseProgram(shaderProgramID[0]);
		glBindVertexArray(VAO[1]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, obstacleTexture);

		for (auto it = obstacles.begin(); it != obstacles.end(); ++it)
		{
			Obstacle *o = *it;
			o->Update(elapsedTime * 0.05 * (difficulty * 0.6) );

			// Obstacle is past camera. Generate new obstacles,
			// do blinking light effect and delete obstacle
			if (o->distance < -5.5)
			{
				GenerateObstacles(1, 49);
				brightness = 10;
				score += 100 * movementSpeed;

				obstacles.erase(it);
				delete o;
				continue;
			}

			for (int i = 0; i < 6; ++i)
			{
				if (o->side[i])
				{
					// Collision detection
					if (o->distance < -4.5)
					{
						int pos = ((((int)tunnelRotation + 30) / 60) - 0) % 6;
						if ( pos == i )
						{
							std::cout << "\nAngle: " << tunnelRotation;
							std::cout << "\nWall Pos: " << i;
							std::cout << "\nCalculated Pos: " << pos;
							std::cout << '\n';

							SaveHighscore(score);
							ReadHighscore();

							return 1;
						}
					}

					// Drawing
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

		// Draw the spaceship
		globalLight.position = glm::vec3(dx, dy, 5.f);
		globalLight.rgb = glm::vec3(1.3 * brightness * rgb.r / (float)255, brightness * rgb.g / (float)255, brightness * rgb.b / (float)255);

		glUseProgram(shaderProgramID[0]);
		glBindVertexArray(VAO[2]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, obstacleTexture);

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.5f, 4.f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -1.7f, 4.85f));
		//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f, 0.02f, 0.02f));
		modelMatrix = glm::rotate(modelMatrix, -90 * ((float)PI/180), glm::vec3(1.f, 0.f, 0.f));
		modelMatrix = glm::rotate(modelMatrix, 45 * ((float)PI/180), glm::vec3(0.f, 1.f, 0.f));
		pipelineMatrix = projectionMatrix * viewMatrix;

		glUniformMatrix4fv(uniformID[0], 1, GL_FALSE, &pipelineMatrix[0][0]);
		glUniformMatrix4fv(uniformID[1], 1, GL_FALSE, &modelMatrix[0][0]);
		glUniform1i(uniformID[2], 0);
		glUniform3fv(uniformID[3], 1, &globalLight.position[0]);
		glUniform3fv(uniformID[4], 1, &globalLight.rgb[0]);
		glDrawArrays(GL_TRIANGLES, 0, 18);

		// beautiful work of art, please do not judge
		std::stringstream scoreText;
		switch (difficulty)
		{
			case 1:
				scoreText << "Difficulty: Easy";
				break;
			case 2:
				scoreText << "Difficulty: Medium";
				break;
			case 3:
				scoreText << "Difficulty: Hard";
				break;
		}
		scoreText << "\nScore: " << score;
		GLTtext *text = gltCreateText();
		gltSetText(text, scoreText.str().c_str());
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltDrawText2D(text, 0, 0, 1);
	}

	return 0;
}

double Geometry::GetRotation()
{
	return tunnelRotation;
}

///
/// Called every time the game starts
///
void Geometry::SetDifficulty(unsigned short int d)
{
	difficulty = d;
	score = 0;

	switch (d)
	{
		case 1:
			movementSpeed = 1;
			break;
		case 2:
			movementSpeed = 1.3;
			break;
		case 3:
			movementSpeed = 1.6;
			break;
	}

	Cleanup();
	GenerateObstacles(50, 0);
}

///
/// Called every time the game ends
///
void Geometry::Cleanup()
{
	obstacles.clear();
}

///
/// Score
///
void Geometry::ReadHighscore()
{
	// Read all highscores
	std::ifstream ifFile;
	ifFile.open("highscores.txt");
	std::string line;

	getline(ifFile, line);
	highscores[0] = std::stoi(line);
	getline(ifFile, line);
	highscores[1] = std::stoi(line);
	getline(ifFile, line);
	highscores[2] = std::stoi(line);
	getline(ifFile, line);
	highscores[3] = std::stoi(line);
	getline(ifFile, line);
	highscores[4] = std::stoi(line);

	ifFile.close();
}

void Geometry::SaveHighscore(unsigned int s)
{
	std::cout << "LOG: Saving " << s << '\n';

	ReadHighscore();

	// Save highscore
	std::ofstream ofFile;
	ofFile.open("highscores.txt", std::ofstream::out | std::ofstream::trunc);

	int biggerThan = -1;
	int displace = 0;

	if (s > highscores[0])
		biggerThan = 0;
	else if (s > highscores[1])
		biggerThan = 1;
	else if (s > highscores[2])
		biggerThan = 2;
	else if (s > highscores[3])
		biggerThan = 3;
	else if (s > highscores[4])
		biggerThan = 4;

	for (int i = 0; i < 5; ++i)
	{
		if (biggerThan == i)
		{
			ofFile << s;
			displace = 1;
		}
		else
			ofFile << highscores[i-displace];

		ofFile << '\n';
	}

	ofFile.close();
}

///
/// Player movement
///
void Geometry::Rotate(Uint32 elapsedTime, int dir)
{
	// Move
	tunnelRotation += ((elapsedTime * 0.5f) * dir) * movementSpeed;
	
	if (tunnelRotation > 360)
		tunnelRotation -= 360;

	if (tunnelRotation < 0)
		tunnelRotation += 360;
}
