#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

//Used to stop the program if an if statement is not fulfilled
#define ASSERT(x) if (!(x)) __debugbreak();

class Renderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Shader& shader) const;
};