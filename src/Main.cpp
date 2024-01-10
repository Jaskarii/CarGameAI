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
	// Make the OpenGL context associated with the window current
	glfwMakeContextCurrent(window);

	// Initialize GLEW or load OpenGL function pointers (if using GLEW/Glad)
	// glewInit(); // For GLEW initialization
	gladLoadGL();

	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
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
	carHandler = new CarHandler();
	networks = new std::vector<NeuralNetwork>();
	std::vector<int> layers = {4, 10, 12, 12, 2};

	for (int i = 0; i < 10; ++i)
	{
		Car car(0, 0); // Create a Car object and initialize it
		car.isTraining = false;
		carHandler->AddCar(car);
		NeuralNetwork network(layers);
		networks->push_back(network);
	}

	road = new Road();
	carHandler->InitBuffers();
	road->InitBuffers();
}

void ResetControls()
{
	yMax = 0;
	frames = 0;
	// Add cars y position to networks fitness score.
	carHandler->ResetCars();
}

bool OpenGLRender()
{
	if (glfwWindowShouldClose(window))
	{
		return false;
	}

	setCameraTo(carHandler->GetCars()->at(carWithMaxY).getInputs()->position);

	checkGLError();
	renderer->Clear();
	road->Render(MVP);
	UpdateCars(carHandler->GetCars(), road);
	carHandler->Render(proj, MVP);
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
	const int numGames = 10;

	// Create game instances
	for (int i = 0; i < numGames; ++i)
	{
		games.emplace_back(600);
	}

	for (size_t i = 0; i < games.size(); i++)
	{
		games[i].index = i;
		// Subscribe to the network update event with the first handler
		games[i].networkUpdateEvent.Subscribe(
			[](NeuralNetwork *updatedNetwork)
			{
				// Handle the updated network in the first handler
				networks->at(updatedNetwork->index).CopyWeights(updatedNetwork);
				networks->at(updatedNetwork->index).SetFitness(updatedNetwork->GetFitness());
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
		carHandler->GetCars()->front().Rotate(0.03f);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		carHandler->GetCars()->front().Rotate(-0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		carHandler->GetCars()->front().Accelerate(0.03f);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		carHandler->GetCars()->front().Accelerate(-0.03f);
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

void UpdateCars(std::vector<Car> *cars, Road *road)
{
	// Initialize variables to track the maximum A value and the corresponding Car.
	float maxY = cars->at(0).getInputs()->position.y;
	bool EndRun = true;
	// Iterate through the vector to find the Car with the highest A value.
	for (int i = 0; i < cars->size(); i++)
	{
		Car &car = cars->at(i);
		if (!car.IsCrashed())
		{
			EndRun = false;
		}
		car.GetAndHandleOutPuts(&(networks->at(i)));
		car.SetCrashed(road->IsOffRoad(&car));
		if (car.getInputs()->position.y > maxY)
		{
			carWithMaxY = i;
			maxY = car.getInputs()->position.y;
		}
		car.SetCamera(false);
		car.Update();
	}
	cars->at(carWithMaxY).SetCamera(true);
}