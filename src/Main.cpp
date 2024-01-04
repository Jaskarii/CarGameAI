#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<fstream>
#include<string>
#include<sstream>
#include <assert.h>
#include "VertexBuffer.h"
#include"VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "Shader.h"
#include"Renderer.h"
#include"Texture.h"
#include "Road.h"
#include "Car.h"

void checkGLError() 
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error in " << std::endl;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

int main()
{
	// GLFW initialization
	if (!glfwInit()) {
		// Handle GLFW initialization failure
		return -1;
	}

	// Create a window and OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 800, "MyOpenGL", NULL, NULL);
	if (!window) {
		// Handle window creation failure
		glfwTerminate();
		return -1;
	}

	// Make the OpenGL context associated with the window current
	glfwMakeContextCurrent(window);

	// Initialize GLEW or load OpenGL function pointers (if using GLEW/Glad)
	// glewInit(); // For GLEW initialization
	gladLoadGL();

	// Check for framebuffer size and set the viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	int windowWidth, windowHeight;
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);

	Road road;
	Car car;
	
	
	Renderer renderer;

	while (!glfwWindowShouldClose(window))
	{
		checkGLError();
		renderer.Clear();
		//shader.SetUniform4f("u_color", r, 0.8f, 1.0f, 1.f);
		road.Render();
		car.Render(window);
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}