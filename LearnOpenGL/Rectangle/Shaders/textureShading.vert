#version 450
//The vertex shader operates on each vertex

layout (location = 0) in vec3 position;

out vec4 vertexColor;

void main() {
    //Set the x,y position on the screen
    gl_Position = vec4(position.x, position.y, position.z, 1.0);

	// Set the color the fragment shader will receive
	vertexColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);	// Dark red
}