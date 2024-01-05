#ifndef CAR_H
#define CAR_H
#include <glad/glad.h>
#include "Shader.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include <string>
#include "glfw3.h"
#include "Vector2.h"

class Car
{
public:
    Car();
    ~Car();
    void Render(GLFWwindow *window);
    Vector2 GetPosition();

private:
    Vertexarray *va;
    IndexBuffer *ibo;
    VertexBuffer *vbo;
    Shader *shader;
    float *positions;
    float dirX = 0;
    float dirY = 1;
    float speed = 0;
};
#endif // CAR_H