#include "Road.h"

Road::Road()
{
    float positions[] =
	{
		0.0f,-0.7f,
		-0.7f,-0.7f,
		-0.7f,0.7f,
		0.9f,0.7f,
		0.9f,-0.7f,
	};

	unsigned int indices[] =
	{
		0,1,1,2,2,3,3,4,4,0
	};

	vbo = new VertexBuffer(positions, 3*2*sizeof(float));
	ibo = new IndexBuffer(indices, 10);
	shader = new Shader("shaders/Road.shader");

	va = new Vertexarray();
	VertexBufferLayout layout;
	layout.PushFloat(2);
	va->AddBuffer(*vbo, layout);
}

Road::~Road()
{
}

void Road::Render()
{
	va->Bind();
	ibo->Bind();
	shader->Bind();
	shader->SetUniform1f("Width", 0.1);
	shader->SetUniform1f("Alpha", 0.9);

	glDrawElements(GL_LINES, ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
}

bool Road::IsOffRoad(Car *car)
{
    return false;
}
