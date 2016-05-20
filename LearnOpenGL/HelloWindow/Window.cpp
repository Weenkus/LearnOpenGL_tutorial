#include <iostream>

#include <SDL/SDL.h>
#include <GL/glew.h>

void game_loop(SDL_Window* window);

bool process_input();
void render();
void swap_buffer(SDL_Window* window);


int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow(
		"LearnOpenGL",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		800,                               // width, in pixels
		600,                               // height, in pixels
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


	// Turn on VSync (1) or turn off (0)
	SDL_GL_SetSwapInterval(0);

	glViewport(0, 0, 800, 600);

	// Enable alfa blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	game_loop(window);
	atexit(SDL_Quit);

	return 0;
}

void game_loop(SDL_Window* window) {
	bool running = true;

	while (running) {

		running = process_input();
		render();
		swap_buffer(window);
	}
}

bool process_input() {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {

		switch (event.type) {
		case SDL_KEYDOWN: if (event.key.keysym.sym == SDLK_ESCAPE)
			return false;
		}
	}
	return true;
}

void render() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void swap_buffer(SDL_Window* window) {
	SDL_GL_SwapWindow(window);
}

