#include <iostream>

#include <SDL/SDL.h>
#include <GL/glew.h>

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		640,                               // width, in pixels
		480,                               // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
		);

	if (window == nullptr) {
		// In the case that the window could not be made...
		std::cerr << "Could not create window: %s\n", SDL_GetError();
		return 1;
	}

	// Save the OpenGL context to the game engine window
	SDL_GLContext glContex = SDL_GL_CreateContext(window);
	if (glContex == nullptr) {
		std::cerr << "SDL_GL contect could not be created!";
	}

	// Initialise the GLEW library
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		std::cerr << "GLEW could not be initialise!";
	}

	// Check the OpenGL version
	std::printf("*** OpenGL Version: %s   ***\n", glGetString(GL_VERSION));

	// Set the background to blue
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// Turn on VSync (1) or turn off (0)
	SDL_GL_SetSwapInterval(0);

	// Enable alfa blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	std::cout << "Hello!";
	getchar();

	return 0;
}