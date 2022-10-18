#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<fstream>
#include<string>
#include<sstream>
#include <assert.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "Shader.h"

int main()
{
	glfwInit();

	//Specify OpenGL version for the GLFW
	//Using version 3.3 and Core profile

	//Create window object
	GLFWwindow* window = glfwCreateWindow(800, 800, "MyOpenGL", NULL, NULL);
	
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	float positions[] =
	{
		-0.5f,-0.5f,
		0.5f,-0.5f,
		0.5f,0.5f,
		-0.5f,0.5f,
	};

	unsigned int indices[] =
	{
		0,1,2,
		2,3,0
	};

	Vertexarray va;
	//Create buffer and bind it.
	VertexBuffer vb(positions, 4 * 2 * sizeof(float));

	VertexBufferLayout layout;
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	IndexBuffer ib(indices, 6);

	Shader shader("basic.shader");
	shader.Bind();
	shader.SetUniform4f("u_color", 0.3f, 0.8f, 1.0f, 1.f);

	va.UnBind();
	shader.UnBind();
	vb.UnBind();
	ib.UnBind();
	float r = 0.0f;
	float increment = 0.01f;

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Bind();
		shader.SetUniform4f("u_color", r, 0.8f, 1.0f, 1.f);

		va.Bind();
		ib.Bind();

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		if (r > 1.0f)
		{
			increment = -0.04f;
		}
		else if (r < 0)
		{
			increment = 0.04f;
		}

		r += increment;
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}