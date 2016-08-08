#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>

#include <SDL/SDL.h>
#include <GL/glew.h>

void game_loop(SDL_Window* window, GLuint shader, GLuint VAO);

bool process_input();
void render(GLuint shader, GLuint VAO);
void swap_buffer(SDL_Window* window);
void color_triangle(GLuint shader);
GLuint init_shaders();
GLuint init_triangle_VAO();

using namespace std;

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

	// Shaders
	GLuint shader = init_shaders();

	// Triangle
	GLuint VAO = init_triangle_VAO();

	game_loop(window, shader, VAO);
	atexit(SDL_Quit);

	return 0;
}

void color_triangle(GLuint shader) {
	time_t timeValue = time(0);
	GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
	GLint vertexColorLocation = glGetUniformLocation(shader, "vertexColor");
	glUseProgram(shader);
	glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
}

void game_loop(SDL_Window* window, GLuint shader, GLuint VAO) {
	bool running = true;

	while (running) {
		color_triangle(shader);
		running = process_input();
		render(shader, VAO);
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

void render(GLuint shader, GLuint VAO) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

}

void swap_buffer(SDL_Window* window) {
	SDL_GL_SwapWindow(window);
}

GLuint init_shaders() {
	ifstream vert("Shaders/textureShading.vert");
	string str_vert(static_cast<stringstream const&>(stringstream() << vert.rdbuf()).str());
	const GLchar* vertexShaderSource = str_vert.c_str();
	vert.close();

	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ifstream frag("Shaders/textureShading.frag");
	string str_frag(static_cast<stringstream const&>(stringstream() << frag.rdbuf()).str());
	const GLchar* fragmentShaderSource = str_frag.c_str();
	frag.close();

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

GLuint init_triangle_VAO() {
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	// Bind VAO
	glBindVertexArray(VAO);
	{
		// Initialise the vertex buffer object
		GLuint VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// 0. Copy our vertices array in a buffer for OpenGL to use
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// 1. Then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
	}
	// Unbind VAO
	glBindVertexArray(0);

	return VAO;
}
