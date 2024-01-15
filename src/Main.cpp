#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"
#include "Road.h"
#include "Car.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Main.h"
#include "NetworkManager.h"
#include "CarGame.h"
#include <thread>
#include <atomic>

GLFWwindow *window;
Renderer *renderer;
glm::mat4 proj;
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 MVP;
int frames = 0;
int carWithMaxY = 0;
std::vector<NeuralNetwork> *networks;
bool isOpenGL = false;
std::vector<CarGame> games;
std::vector<std::thread> threads;
std::atomic<bool> stopFlag(false);
CarGame *mainGame;

void setCameraTo(glm::vec2 position)
{
	// Check if either component of the position is a NaN
	if (std::isnan(position.x) || std::isnan(position.y))
	{
		std::cout << "Invalid position: NaN encountered" << std::endl;
		// Handle the NaN case, perhaps set a default position or return early
		return;
	}

	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, -position.y, 0.0f));
	MVP = proj * view; // Combine the translation with the MVP
}

void checkGLError()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error in " << std::endl;
	}
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glfwGetFramebufferSize(window, &width, &height);
	float fWidth = width / 2;
	float fHeight = height / 2;
	proj = glm::ortho(-fWidth, fWidth, -fHeight, fHeight, -1.0f, 1.0f);
	glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float fwidth = width;
		float fheight = height;
		ypos = height - ypos;
		// Convert cursor position to OpenGL normalized coordinates
		// road->SetNewPath(Vector2(xpos, ypos));
	}
}

bool InitOpenGL()
{
	if (!glfwInit())
	{
		return false;
	}
	// Create a window and OpenGL context
	window = glfwCreateWindow(800, 800, "MyOpenGL", NULL, NULL);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float fWidth = width / 2;
	float fHeight = height / 2;
	proj = glm::ortho(-fWidth, fWidth, -fHeight, fHeight, -1.0f, 1.0f);

	if (!window)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Check for framebuffer size and set the viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	int windowWidth, windowHeight;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	return true;
}
float yMax = 0;

void InitControls()
{
	Road::GenerateRandomPoints();
	renderer = new Renderer();
	std::vector<int> layers = {5, 20, 20, 20, 2};
	CarGame::InitBestNetwork(layers);
	mainGame = new CarGame(6, false, false);
	mainGame->InitBuffers();
	setCameraTo(glm::vec2(0, 0));
}

void ResetControls()
{
	yMax = 0;
	frames = 0;
	// Add cars y position to networks fitness score.
	mainGame->Reset();
}

bool OpenGLRender()
{
	if (glfwWindowShouldClose(window))
	{
		return false;
	}

	checkGLError();
	renderer->Clear();
	setCameraTo(mainGame->GetCameraPosition());
	mainGame->Render(proj, MVP);

	glfwSwapBuffers(window);
	glfwPollEvents();
	return true;
}

void OpenGLEnd()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void StartGameThreads()
{
	const int numGames = 6;

	// Create game instances
	for (int i = 0; i < numGames; ++i)
	{
		games.emplace_back(300, true, false);
	}

	for (size_t i = 0; i < games.size(); i++)
	{
		games[i].index = i;
		// Subscribe to the network update event with the first handler
		games[i].networkUpdateEvent.Subscribe(
			[](NeuralNetwork *updatedNetwork)
			{
				// Handle the updated network in the first handler

				mainGame->GetNetworks()->at(updatedNetwork->index).CopyWeights(updatedNetwork);
				mainGame->GetNetworks()->at(updatedNetwork->index).SetFitness(updatedNetwork->GetFitness());
				std::cout << updatedNetwork->GetFitness() << " " << updatedNetwork->index << std::endl;
				updatedNetwork->printWeights();
			});
	}

	// Create and start game threads
	for (int i = 0; i < numGames; ++i)
	{
		threads.emplace_back([i]()
							 { games[i].StartGame(stopFlag); });
	}
}

int main()
{
	if (!InitOpenGL())
	{
		return -1;
	}

	InitControls();
	StartGameThreads();

	while (OpenGLRender())
	{
		if (!CheckKeyPress(window))
		{
			break;
		}
		mainGame->GameLoop();
	}
	OpenGLEnd();
	stopFlag.store(true, std::memory_order_release);
	// Wait for all threads to finish by joining them
	for (std::thread &thread : threads)
	{
		thread.join(); // Properly join each thread
	}

	return 0;
}

bool CheckKeyPress(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		mainGame->GetCars()->front().Rotate(0.03f);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		mainGame->GetCars()->front().Rotate(-0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mainGame->GetCars()->front().Accelerate(0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mainGame->GetCars()->front().Accelerate(-0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		scale += 0.05;
		scale = std::min(scale, 1.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		scale -= 0.05;
		scale = std::max(scale, 0.1f);
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		return false;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		ResetControls();
	}
	return true;
}