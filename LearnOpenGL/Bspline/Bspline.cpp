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
#include <glm/gtx/string_cast.hpp>

#include <HelperLib/Shader.h>
#include <HelperLib/InputManager.h>
#include <HelperLib/FileParser.h>

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLfloat fov = 45.0f;

// Movement
int current_movment_point{ 0 };
int num_of_movement_points;
int curve_points;
double movment_granularity{ 0.0005 };
std::vector<glm::vec3> movement;

GLfloat path_thickness{ 3.0f };
GLuint VBO_path, VAO_path;

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

void game_loop(SDL_Window* window, HelperLib::Shader shader, GLuint VAO, GLuint texture, HelperLib::InputManager& inputManager);

bool process_input(HelperLib::InputManager& inputManager);
void render(HelperLib::Shader shader, GLuint VAO, GLuint texture, int rnd_nums[], double t);
void swap_buffer(SDL_Window* window);
GLuint init_cubes();
GLuint init_texture();
glm::vec3 b_spline(double t, glm::vec3 r0, glm::vec3 r1, glm::vec3 r2, glm::vec3 r3);
glm::vec3 b_spline_direction(double t, glm::vec3 r0, glm::vec3 r1, glm::vec3 r2, glm::vec3 r3);

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

	// Read movement curve points
	movement = FileParser::extract_points("assets/movement/path.txt");
	num_of_movement_points = movement.size();

	// Cubes
	glEnable(GL_DEPTH_TEST);
	GLuint VAO = init_cubes();
	GLuint texutre = init_texture();

	glLineWidth(path_thickness);
	HelperLib::InputManager inputManager;

	game_loop(window, shader, VAO, texutre, inputManager);
	atexit(SDL_Quit);

	return 0;
}

void game_loop(SDL_Window* window, HelperLib::Shader shader, GLuint VAO, GLuint texture, HelperLib::InputManager& inputManager) {
	bool running = true;

	int rnd_nums[10];
	srand(time(NULL));
	for (int i{ 0 }; i < 10; i++)
		rnd_nums[i] = rand() % 360 + 1;

	double t = 0;
	while (running) {
		GLfloat currentFrame = clock();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		running = process_input(inputManager);

		if (t > 1) {
			t = 0;
			current_movment_point = (current_movment_point + 1) % num_of_movement_points;
		} else {
			t += movment_granularity;
		}

		render(shader, VAO, texture, rnd_nums, t);
		swap_buffer(window);
	}
}

bool process_input(HelperLib::InputManager& inputManager) {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
			return false;

		switch (event.type) {
		case SDL_KEYDOWN:
			inputManager.pressKey(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			inputManager.releaseKey(event.key.keysym.sym);
			break;
		}

	}


	GLfloat cameraSpeed = 0.01f * deltaTime;
	GLfloat turnSpeed = 0.005f * deltaTime;
	if (inputManager.isKeyDown(SDLK_w))
		cameraPos += cameraSpeed * cameraFront;
	if (inputManager.isKeyDown(SDLK_s))
		cameraPos -= cameraSpeed * cameraFront;
	if (inputManager.isKeyDown(SDLK_a))
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (inputManager.isKeyDown(SDLK_d))
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (inputManager.isKeyDown(SDLK_q))
		cameraFront.x -= turnSpeed;
	if (inputManager.isKeyDown(SDLK_e))
		cameraFront.x += turnSpeed;

	return true;
}

void render(HelperLib::Shader shader, GLuint VAO, GLuint texture, int rnd_nums[], double t) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, texture);
	shader.Use();

	// Create transformations
	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 projection;
	projection = glm::perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
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

	//double t = (clock() % 100) / static_cast<double>(10);

	glm::vec3 a = b_spline(
		t,
		movement[current_movment_point % num_of_movement_points],
		movement[(current_movment_point + 1) % num_of_movement_points],
		movement[(current_movment_point + 2) % num_of_movement_points],
		movement[(current_movment_point + 3) % num_of_movement_points]
	);

	//cout << current_movment_point << endl;
	//cout << glm::to_string(a) << endl;

	glBindVertexArray(VAO);
	{
		glColor3f(1.0, 1.0, 1.0);
		glm::vec3 direction = b_spline_direction(
			t,
			movement[current_movment_point % num_of_movement_points],
			movement[(current_movment_point + 1) % num_of_movement_points],
			movement[(current_movment_point + 2) % num_of_movement_points],
			movement[(current_movment_point + 3) % num_of_movement_points]
		);

		for (GLuint i{ 9 }; i < 10; ++i)
		{
			// Calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model;
			GLfloat angle = 2.0f * i * rnd_nums[i];
			model = glm::translate(model, a);
			model = glm::rotate(model, 0.5f, direction);

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	glBindVertexArray(0);

	glBindVertexArray(VAO_path);
	{
		glm::mat4 model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_LINE_LOOP, 0, curve_points);
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

	glGenVertexArrays(1, &VAO_path);
	glGenBuffers(1, &VBO_path);

	// Generate curve
	std::vector<glm::vec3> curve;
	for (int i{ 0 }; i < num_of_movement_points; ++i) {
		for (double t{ 0 }; t <= 1.0; t += movment_granularity) {
			glm::vec3 curve_point = b_spline(
				t,
				movement[i % num_of_movement_points],
				movement[(i + 1) % num_of_movement_points],
				movement[(i + 2) % num_of_movement_points],
				movement[(i + 3) % num_of_movement_points]
			);

			curve.push_back(curve_point);
		}
	}
	curve_points = curve.size();

	// Bind VAO
	glBindVertexArray(VAO_path);
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO_path);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * curve.size(), &curve.front(), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
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
	unsigned char* image = SOIL_load_image("assets/textures/ghosty.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	return texture;
}

glm::vec3 b_spline(double t, glm::vec3 r0, glm::vec3 r1, glm::vec3 r2, glm::vec3 r3) {
	glm::vec4 T = glm::vec4(glm::pow(t, 3), glm::pow(t, 2), t, 1);

	double B_mat_data[16] = {
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0
	};
	glm::mat4 B = glm::make_mat4(B_mat_data);

	glm::vec4 core = float(1) / 6 * B * T;
	//std::cout << glm::to_string(core);
	return (r0*core.x) + (r1*core.y) + (r2*core.z) + (r3*core.a);
}

glm::vec3 b_spline_direction(double t, glm::vec3 r0, glm::vec3 r1, glm::vec3 r2, glm::vec3 r3) {
	glm::vec3 T = glm::vec3(glm::pow(t, 2), t, 1);

	double B_mat_data[12] = {
		-1, 3, -3, 1,
		2, -4, 2, 0,
		-1, 0, 1, 0,
	};
	glm::mat3x4 B = glm::make_mat3x4(B_mat_data);

	glm::vec4 core = float(1) / 6 * B * T;
	//std::cout << glm::to_string(core);
	return (r0*core.x) + (r1*core.y) + (r2*core.z) + (r3*core.a);
}
