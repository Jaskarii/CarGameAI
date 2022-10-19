#pragma once
#include<glad/glad.h>
#include"Vertexarray.h"
#include"IndexBuffer.h"
#include"Shader.h"

class Renderer
{
public:
	void Draw(const Vertexarray& va, const IndexBuffer& ib, const Shader& shader) const;

	void Clear();
};

