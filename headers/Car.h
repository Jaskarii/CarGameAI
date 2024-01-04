#include "Shader.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "VertexBufferLayout.h"
#include<string>
#include "GLFW/glfw3.h"
#include "Road.h"
#include "Vector2.h"

class Car
{
    public:
        Car();
        ~Car();
        void Render(GLFWwindow* window, Road* road);
        Vector2 GetPosition();
        
    private:
        Vertexarray* va;
        IndexBuffer* ibo;
        VertexBuffer* vbo;
        Shader* shader;
        float* positions;
        Vector2* position;
        float speed = 0;

};
