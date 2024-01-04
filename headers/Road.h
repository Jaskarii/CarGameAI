#include "Shader.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "VertexBufferLayout.h"
#include<string>
#include "Car.h"

class Road
{
    public:
        Road();
        ~Road();
        void Render();
        bool IsOffRoad(Car* car);
    private:
        Vertexarray* va;
        IndexBuffer* ibo;
        VertexBuffer* vbo;
        Shader* shader;
};
