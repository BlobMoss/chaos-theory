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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

const unsigned int windowWidth = 600;
const unsigned int windowHeight = 450;

glm::vec3* selectedMagnet;

//Places magnets in an eqilateral triangle with the side of 2 * radius
const float triangleRadius = 30.0f;
glm::vec3 magnets[3] = {
    glm::vec3(windowWidth / 2.0f + triangleRadius, windowHeight / 2.0f - sqrt(3.0f) * triangleRadius / 2.0f, 0.0f),
    glm::vec3(windowWidth / 2.0f                 , windowHeight / 2.0f + sqrt(3.0f) * triangleRadius / 2.0f, 0.0f),
    glm::vec3(windowWidth / 2.0f - triangleRadius, windowHeight / 2.0f - sqrt(3.0f) * triangleRadius / 2.0f, 0.0f)
};

const float magnetRadius = 12.5f;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        //The length of an array can be calculated by dividing the size of the entire array by the size of a single element
        for (unsigned int i = 0; i < sizeof(magnets) / sizeof(magnets[0]); i++)
        {
            if (glm::distance(glm::vec3(xpos, windowHeight - ypos, 0.0f), magnets[i]) < 15.0f)
            {
                std::cout << "Click!" << std::endl;
                selectedMagnet = &magnets[i];
            }
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        selectedMagnet = nullptr;
    }
}

int main(void)
{
    //Initialization of GLFW
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    //Set window 
    window = glfwCreateWindow(windowWidth, windowHeight, "CHAOS THEORY!", NULL, NULL);
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

    //Enable blending to render transparent pixels properly
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    //This scope helps with garbage collection, I think...
    {
        //An array of vertex positions with corresponding texture coordinates
        float backgroundVertices[] = {
            0.0f       , 0.0f        , 0.0f, 0.0f,
            windowWidth, 0.0f        , 1.0f, 0.0f,
            windowWidth, windowHeight, 1.0f, 1.0f,
            0.0f       , windowHeight, 0.0f, 1.0f
        };

        float magnetVertices[] = {
            -magnetRadius, -magnetRadius, 0.0f, 0.0f,
             magnetRadius, -magnetRadius, 1.0f, 0.0f,
             magnetRadius,  magnetRadius, 1.0f, 1.0f,
            -magnetRadius,  magnetRadius, 0.0f, 1.0f
        };
        //An array of indeces to indicate which vertices go together
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        VertexArray backgroundVertexArray;
        VertexArray magnetVertexArray;
        //The vertex buffer resembles the vertices array in size
        VertexBuffer backgroundVertexBuffer(backgroundVertices, 4 * 4 * sizeof(float));
        VertexBuffer magnetVertexBuffer(magnetVertices, 4 * 4 * sizeof(float));

        //The layout of a vertex is set to contain two pairs of two floats (position and texture coords)
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        //This layout is applied to the buffer
        backgroundVertexArray.AddBuffer(backgroundVertexBuffer, layout);
        magnetVertexArray.AddBuffer(magnetVertexBuffer, layout);

        //The index buffer resembles the indices array in size
        IndexBuffer indexBuffer(indices, 3 * 2);

        glm::vec4 colors[3] = {
            glm::vec4(0.980f, 0.760f, 0.031f, 1.0f),
            glm::vec4(0.627f, 0.184f, 0.250f, 1.0f),
            glm::vec4(0.220f, 0.176f, 0.263f, 1.0f)
        };

        //Glm contains everything which I did myself in Löve3D.
        
        //Creates an mvp for the background
        glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); //This application does not implement camera movement at the moment
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        //MVP: Model, View, Projection matrix.
        //(multiplied right to left in OpenGL)
        glm::mat4 mvp = projection * view * model;

        //Create and set uniforms of the texture shader
        Shader textureShader("content/shaders/textured.shader");
        textureShader.Bind();
        textureShader.SetUniformMat4f("u_MVP", mvp);
        //Create a texture for the shader
        Texture texture("content/textures/magnet.png");
        texture.Bind();
        textureShader.SetUniform1i("u_Texture", 0);

        //Create and set uniforms of the magnetic pendulum shader
        Shader chaosShader("content/shaders/magnetic_pendulum.shader");
        chaosShader.Bind();
        chaosShader.SetUniformMat4f("u_MVP", mvp);
        //Set color uniforms
        chaosShader.SetUniform4f("u_Color1", colors[0].x, colors[0].y, colors[0].z, colors[0].w);
        chaosShader.SetUniform4f("u_Color2", colors[1].x, colors[1].y, colors[1].z, colors[1].w);
        chaosShader.SetUniform4f("u_Color3", colors[2].x, colors[2].y, colors[2].z, colors[2].w);

        backgroundVertexArray.Unbind();
        magnetVertexArray.Unbind();
        backgroundVertexBuffer.Unbind();
        magnetVertexBuffer.Unbind();
        indexBuffer.Unbind();
        textureShader.Unbind();
        chaosShader.Unbind();

        Renderer renderer;

        //Loop while "window" should not close
        while (!glfwWindowShouldClose(window))
        {
            //Clear screen
            renderer.Clear();

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            if (selectedMagnet != nullptr)
            {
                *selectedMagnet = glm::vec3(xpos, windowHeight - ypos, 0.0f);
            }

            //Draw:

            //Draw the backround with the chaos shader:
            chaosShader.Bind();
            //Set magnet position uniforms
            chaosShader.SetUniform2f("u_Pos1", magnets[0].x / windowWidth, magnets[0].y / windowHeight);
            chaosShader.SetUniform2f("u_Pos2", magnets[1].x / windowWidth, magnets[1].y / windowHeight);
            chaosShader.SetUniform2f("u_Pos3", magnets[2].x / windowWidth, magnets[2].y / windowHeight);

            renderer.Draw(backgroundVertexArray, indexBuffer, chaosShader);

            //Draw every magnet:
            textureShader.Bind();
            //The length of an array can be calculated by dividing the size of the entire array by the size of a single element
            for (unsigned int i = 0; i < sizeof(magnets) / sizeof(magnets[0]); i++)
            {
                //Draw magnet 
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(magnets[i].x, magnets[i].y, 0));
                glm::mat4 mvp = projection * view * model;
                textureShader.SetUniformMat4f("u_MVP", mvp);
                textureShader.SetUniform4f("u_Color", colors[i].x, colors[i].y, colors[i].z, colors[i].w);

                renderer.Draw(magnetVertexArray, indexBuffer, textureShader);
            }

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}