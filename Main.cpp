#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>

#include "glad.h"

constexpr int GLM_FORCE_RADIANS = 1;
constexpr int SCREEN_FULLSCREEN = 1;
constexpr int SCREEN_WIDTH = 960;
constexpr int SCREEN_HEIGHT = 540;

const char* VertexShaderSource = "#version 460 core\n"
	"layout (location = 0) in vec3 AttributePosition;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(AttributePosition.x, AttributePosition.y, AttributePosition.z, 1.0);\n"
	"}\n\0";

const char* FragmentShaderSource = "#version 460 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";

class Game
{
public:
	Game();
	~Game();
	void run();
private:
	void init_screen();
	void terminate(int exit_code);
	void init_triangle();
	void render_triangle();

	SDL_Window* window = {};
	SDL_GLContext maincontext = {};
	bool running;

	unsigned int ShaderProgram = 0;
	unsigned int VAO = 0, VBO = 0;
};

Game::Game() : running(true)
{

}

Game::~Game()
{
	terminate(EXIT_SUCCESS);
}
void Game::init_screen() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
		terminate(EXIT_FAILURE);
	}
	// Load OpenGl by default.
	SDL_GL_LoadLibrary(NULL);

	// Request an OpenGL 4.6 context.
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	// Optional request of a depth buffer. Can be set to 0 for both for no depth buffer.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if (SCREEN_FULLSCREEN)
	{
		window = SDL_CreateWindow("OpenGL 4.6",
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					0,
					0,
					SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);
	}
	else
	{
		window = SDL_CreateWindow("OpenGL 4.6",
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					SCREEN_WIDTH,
					SCREEN_HEIGHT,
					SDL_WINDOW_OPENGL);
	}
	if (!window)
	{
		std::cout << "Error: Failed to open window: " << SDL_GetError() << std::endl;
		terminate(EXIT_FAILURE);
	}
	// Call function to create the OpenGL context.
	maincontext = SDL_GL_CreateContext(window);
	if (maincontext == NULL)
	{
		std::cout << "Error: Failed to create OpenGL context: " << SDL_GetError() << std::endl;
		terminate(EXIT_FAILURE);
	}

	// Check OpenGL properties.
	std::cout << "OpenGL loaded" << std::endl;
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

	// Use V-Sync.
	SDL_GL_SetSwapInterval(1);

	// Disable depth test and face culling.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);

	init_triangle();
}

void Game::init_triangle()
{
	unsigned int VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
	glCompileShader(VertexShader);

	int success = 0;
	char InfoLog[512];
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(VertexShader, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\N" << InfoLog << std::endl;
	}

	unsigned int FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL);
	glCompileShader(FragmentShader);

	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(FragmentShader, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << InfoLog << std::endl;
	}

	ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);

	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ShaderProgram, 512, NULL, InfoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << InfoLog << std::endl;
	}

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);
	float vertices[] = {
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Game::render_triangle()
{
	glUseProgram(ShaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

void Game::terminate(int exit_code)
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(ShaderProgram);

	if (window)
	{
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
	exit(exit_code);
}

void Game::run()
{
	init_screen();

	SDL_Event event;

	uint32_t ticks = SDL_GetTicks(), lastticks = 0;

	while (running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render_triangle();

		ticks = SDL_GetTicks();
		if (((ticks * 10 - lastticks * 10)) < 167)
		{
			SDL_Delay((167 - ((ticks * 10 - lastticks * 10))) / 10);
		}
		lastticks = SDL_GetTicks();
		SDL_GL_SwapWindow(window);
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}
	}
}

int main(int argc, char* argv[])
{
	Game game;
	game.run();
	return 0;
}
