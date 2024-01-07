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
#include "GfxObject.h"

class Road : public GfxObject
{
public:
    Road();
    ~Road();
    void SetNewPath(glm::vec2 pos);
    void Render(glm::mat4 MVP);
    bool IsOffRoad(glm::vec2 position);
    void SetWidth(float givenWidth)
    {
        width = givenWidth;
    }

private:
    int positionCount = 0;
    int indicesCount = 0;
    int vertexCount = 0;
    glm::vec2 *positions;
    float yOffset = 0;
    float xOffset = 0;
    unsigned int *indices;
    Vertexarray *va;
    IndexBuffer *ibo;
    bool isOffRoad = false;
    float width = 35.0f;
};
#endif