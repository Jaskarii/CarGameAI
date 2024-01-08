#include "Road.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Road::Road()
{
	int numVertices = 500000; // Assuming each vertex is a Vector2 (x, y)
	positions = new glm::vec2[numVertices];
	indices = new unsigned int[1000000];

	const float min_val = -500.0f;
	const float max_val = 500.0f;
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	SetNewPath(glm::vec2(0.0f, -100.0f));
	SetNewPath(glm::vec2(0.0f, 100.0f));
	float yPos = 300.0f;
	for (size_t i = 0; i < 1000; i++)
	{
		int random_int = std::rand();
		// Convert to a float between 0 and 1 and scale it to the desired range
		float random_float = static_cast<float>(random_int) / RAND_MAX;	 // Between 0 and 1
		random_float = min_val + random_float * (max_val - min_val);	 // Scale to the desired range
		float random_floaty = static_cast<float>(random_int) / RAND_MAX; // Between 0 and 1
		random_floaty = 50.0f + random_floaty * (500.0f - 50.0f);		 // Scale to the desired range
		yPos += random_floaty;
		SetNewPath(glm::vec2(random_float, yPos));
	}

	vbo = new VertexBuffer(positions, 1000000 * sizeof(float));
	ibo = new IndexBuffer(indices, 1000000);
	shader = new Shader("../shaders/Road.glsl");
	shader->Bind();
	shader->SetUniform1f("Width", width);

	va = new Vertexarray();
	VertexBufferLayout layout;
	layout.PushFloat(2);
	va->AddBuffer(*vbo, layout);
}

Road::~Road()
{
}

void Road::Render(glm::mat4 MVP)
{
	va->Bind();
	ibo->Bind();
	shader->Bind();
	shader->SetUniformMat4f("MVP", MVP);

	if (indicesCount > 1)
	{
		glDrawElements(GL_LINES, indicesCount, GL_UNSIGNED_INT, nullptr);
	}
}

int positionIndex = 1;

bool Road::IsOffRoad(Car* car)
{
	glm::vec2 position = car->GetPosition();
	float distance1 = DistanceToLineSegment(position, positions[positionIndex], positions[positionIndex - 1]);
	if (distance1 < width)
	{
		return false;
	}
	float distance2 = DistanceToLineSegment(position, positions[positionIndex], positions[positionIndex + 1]);

	if (distance1 > distance2)
	{
		positionIndex++;
	}

	if (std::min(distance1, distance2) > width)
	{
		return true;
	}

	return false;
}

void Road::SetNewPath(glm::vec2 pos)
{
	// Update positions
	positions[positionCount] = pos;
	positionCount++; // Increment by the size of one position (x and y)

	// Update position buffer data
	// glBufferSubData(GL_ARRAY_BUFFER, 0, positionCount * 2 * sizeof(float), positions);

	int prevElementCount = indicesCount;
	// Update indices
	if (vertexCount > 1)
	{
		indices[indicesCount] = vertexCount - 1; // Assuming 0-based index
		indicesCount++;
	}

	indices[indicesCount] = vertexCount;
	indicesCount++;

	// glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesCount * sizeof(unsigned int), indices);

	vertexCount++;
}
