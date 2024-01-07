#ifndef CAR_H
#define CAR_H
#include <glad/glad.h>
#include "Shader.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include <string>
#include "glfw3.h"
#include "Vector2.h"
#include "GfxObject.h"

class Car : public GfxObject
{
public:
    Car();
    ~Car();
    void Render(glm::mat4 MVP);
    glm::vec2 GetPosition();
    void Rotate(float angle);
    void Accelerate(float acc);
    void SetCrashed(bool crashed);
    bool IsCrashed();

private:
    Vertexarray *va;
    IndexBuffer *ibo;
    VertexBuffer *vbo;
    Shader *shader;
    glm::vec2 position{0.0f, 0.0f};
    float *positions;
    float dirX = 0;
    float dirY = 1;
    float speed = 0;
    bool isCrashed = false;
};
#endif // CAR_H