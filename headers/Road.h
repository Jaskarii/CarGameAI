#include "Shader.h"
#include "IndexBuffer.h"
#include "Vertexarray.h"
#include "VertexBufferLayout.h"
#include<string>

class Road
{
    public:
        Road();
        ~Road();
        void Render();
    private:
        Vertexarray* va;
        IndexBuffer* ibo;
        Shader* shader;
};
