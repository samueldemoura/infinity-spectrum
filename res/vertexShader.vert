#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;

// Pipeline matrix stays constant for the whole mesh.
uniform mat4 MVP;

void main()
{
	// Output position of the vertex, in clipping space = MVP * position
	gl_Position =  MVP * vec4(vertexPosition, 1);
	//gl_Position = vec4(vertexPosition, 1);
}
