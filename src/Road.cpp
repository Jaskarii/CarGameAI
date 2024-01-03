#include "Road.h"

Road::Road()
{
    float positions[] =
	{
		0.5f,0.5f,
		0.8f,0.8f,
		-0.2f,0.8f,
	};

	unsigned int indices[] =
	{
		0,1,2
	};

	//Create buffer and bind it.
	VertexBuffer vb(positions, 2 * 3*sizeof(float));
	vb.UnBind();
	layout.PushFloat(2);

	va.AddBuffer(vb, layout);
    ibo = IndexBuffer(indices, 3);
	ibo.UnBind();
}

Road::~Road()
{
}

void Road::Render()
{

}
