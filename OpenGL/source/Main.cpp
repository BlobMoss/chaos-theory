#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

int main(void)
{
    //Initialization of GLFW
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Set window 
    window = glfwCreateWindow(800, 600, "CHAOS THEORY!", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //This limits the frame rate to 60 frames per second
    glfwSwapInterval(1);

    //Initialization of GLEW
    glewInit();

    //these brackets help with garbage collection, I think...
    { 
        //An array of vertex positions and corresponding texture coordinates
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 1.0f
        };
        //An array of indeces to indicate which vertices go together
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        //Enable blending to render transparent pixels
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        VertexArray vertexArray;
        //The vertex buffer resembles the vertices array in size
        VertexBuffer vertexBuffer(vertices, 4 * 4 * sizeof(float));

        //The layout of a vertex is set to contain two pairs of two floats (position and texture coords)
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        //This layout is applied to the buffer
        vertexArray.AddBuffer(vertexBuffer, layout);

        //The index buffer resembles the indeces array in size
        IndexBuffer indexBuffer(indices, 3 * 2);

        //Create and set uniforms of the texture shader
        Shader textureShader("content/shaders/textured.shader");
        textureShader.Bind();
        Texture texture("content/textures/sergei.png");
        texture.Bind();
        textureShader.SetUniform1i("u_Texture", 0);

        //Create and set uniforms of the chaos shader
        Shader mp_shader("content/shaders/magnetic_pendulum.shader");
        mp_shader.Bind();
        mp_shader.SetUniform4f("u_Color1", 0.980f, 0.760f, 0.031f, 1.0f);
        mp_shader.SetUniform4f("u_Color2", 0.627f, 0.184f, 0.250f, 1.0f);
        mp_shader.SetUniform4f("u_Color3", 0.220f, 0.176f, 0.263f, 1.0f);
        mp_shader.SetUniform2f("u_Pos1", 0.50f, 0.50f + sqrt(3.0f) / 20.0f);
        mp_shader.SetUniform2f("u_Pos2", 0.40f, 0.50f - sqrt(3.0f) / 20.0f);
        mp_shader.SetUniform2f("u_Pos3", 0.60f, 0.50f - sqrt(3.0f) / 20.0f);

        vertexArray.Unbind();
        vertexBuffer.Unbind();
        indexBuffer.Unbind();
        textureShader.Unbind();
        mp_shader.Unbind();

        Renderer renderer;

        //Loop while "window" should not close
        while (!glfwWindowShouldClose(window))
        {
            //Clear screen
            renderer.Clear();

            //Draw background with chaos shader
            renderer.Draw(vertexArray, indexBuffer, mp_shader);

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}