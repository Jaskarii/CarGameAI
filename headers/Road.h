#ifndef ROAD_H
#define ROAD_H

#include <glad/glad.h>
#include "glfw3.h"
#include "Shader.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "VertexBufferLayout.h"
#include <string>
#include "Vector2.h"

class Road
{
public:
    Road();
    ~Road();
    void Render(GLFWwindow *window);
    bool IsOffRoad(Vector2 position);
    void SetNewPath(float xPos, float yPos);

private:
    int positionCount = 0;
    int indicesCount = 0;
    int vertexCount = 0;
    float *positions;
    unsigned int *indices;
    Vertexarray *va;
    IndexBuffer *ibo;
    VertexBuffer *vbo;
    Shader *shader;
};
#endif