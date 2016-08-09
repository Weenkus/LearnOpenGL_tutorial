#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <SOIL/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <HelperLib/Shader.h>

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

void game_loop(SDL_Window* window, HelperLib::Shader shader, GLuint VAO, GLuint texture);

bool process_input();
void render(HelperLib::Shader shader, GLuint VAO, GLuint texture, int rnd_nums[]);
void swap_buffer(SDL_Window* window);
GLuint init_cubes();
GLuint init_texture();

using namespace std;

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow(
		"LearnOpenGL",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		WIDTH,                               // width, in pixels
		HEIGHT,                               // height, in pixels
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
	HelperLib::Shader shader("Shaders/textureShading.vert", "Shaders/textureShading.frag");

	// Cubes
	glEnable(GL_DEPTH_TEST);
	GLuint VAO = init_cubes();
	GLuint texutre = init_texture();

	game_loop(window, shader, VAO, texutre);
	atexit(SDL_Quit);

	return 0;
}

void game_loop(SDL_Window* window, HelperLib::Shader shader, GLuint VAO, GLuint texture) {
	bool running = true;

	int rnd_nums[10];
	srand(time(NULL));
	for (int i{ 0 }; i < 10; i++) 
		rnd_nums[i] = rand() % 360 + 1;
		

	while (running) {
		running = process_input();
		render(shader, VAO, texture, rnd_nums);
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

void render(HelperLib::Shader shader, GLuint VAO, GLuint texture, int rnd_nums[]) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, texture);
	shader.Use();

	// Create transformations
	glm::mat4 view;
	glm::mat4 projection;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(shader.Program, "model");
	GLint viewLoc = glGetUniformLocation(shader.Program, "view");
	GLint projLoc = glGetUniformLocation(shader.Program, "projection");
	// Pass the matrices to the shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// World space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	glBindVertexArray(VAO);
	glColor3f(1.0, 1.0, 1.0);
	
	for (GLuint i = 0; i < 10; i++)
	{
		// Calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model;
		model = glm::translate(model, cubePositions[i]);
		GLfloat angle = 2.0f * i * rnd_nums[i];
		model = glm::rotate(model, angle * clock()/2000000, glm::vec3(1.0f, 0.3f, 0.5f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);

}

void swap_buffer(SDL_Window* window) {
	SDL_GL_SwapWindow(window);
}


GLuint init_cubes() {
	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO
	glBindVertexArray(VAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// TexCoord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	}
	// Unbind VAO
	glBindVertexArray(0);

	return VAO;
}

GLuint init_texture() {
	// Load and create a texture 
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("assets/textures/derpina.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	return texture;
}