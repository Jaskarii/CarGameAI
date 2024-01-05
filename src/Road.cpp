#include "Road.h"

Road::Road()
{
	positions = new float[100];
	indices = new unsigned int[100];

	vbo = new VertexBuffer(positions, 100 * sizeof(float));
	ibo = new IndexBuffer(indices, 100);
	shader = new Shader("../shaders/Road.glsl");

	va = new Vertexarray();
	VertexBufferLayout layout;
	layout.PushFloat(2);
	va->AddBuffer(*vbo, layout);
}

Road::~Road()
{
}

void Road::Render(GLFWwindow *window)
{
	va->Bind();
	ibo->Bind();
	shader->Bind();
	shader->SetUniform1f("Width", 0.07f);
	shader->SetUniform1f("Alpha", 0.9f);
	if (indicesCount > 1)
	{
		glDrawElements(GL_LINES, indicesCount, GL_UNSIGNED_INT, nullptr);
	}
}

bool Road::IsOffRoad(Vector2 position)
{
	return false;
}

void Road::SetNewPath(float xPos, float yPos)
{
	vbo->Bind();
	ibo->Bind();

	// Update positions
	positions[positionCount] = xPos;
	positions[positionCount + 1] = yPos;
	positionCount += 2; // Increment by the size of one position (x and y)

	// Update position buffer data
	glBufferSubData(GL_ARRAY_BUFFER, 0, positionCount * sizeof(float), positions);

	int prevElementCount = indicesCount;
	// Update indices
	if (vertexCount > 1)
	{
		indices[indicesCount] = vertexCount - 1; // Assuming 0-based index
		indicesCount++;
	}

	indices[indicesCount] = vertexCount;
	indicesCount++;

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesCount * sizeof(unsigned int), indices);

	vertexCount++;
}
