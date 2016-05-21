#version 450
//The fragment shader operates on each pixel in a given polygon

//This is the 3 component float vector that gets outputted to the screen
//for each pixel.
out vec4 color;

void main()
{
	color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}