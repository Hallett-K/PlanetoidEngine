#include <iostream>

#include "AssetManager.h"
#include "GLDisplay.h"
#include "Resources/Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent
{
    glm::vec2 position;
    float rotation;
    glm::vec2 size;
};

int main(int argc, char** argv)
{
    PlanetoidEngine::GLDisplay display;

    if (display.Init(1280, 720, "Planetoid Test App") == false)
        return -1;

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 1, 3
    };

    unsigned int vao, vbo, ibo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); // Tex Coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    PlanetoidEngine::AssetManager assetManager;

    PlanetoidEngine::Shader* shader = new PlanetoidEngine::Shader;
    shader->AttachShaders("rsc/shaders/shader.vert", "rsc/shaders/shader.frag");

    PlanetoidEngine::Texture* tex = assetManager.GetTexture("rsc/img/crate.png");

    TransformComponent quadTransform{ {320.0f, 240.0f}, 0.0f, { 100.0f, 100.0f } };

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::orthoLH(0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1.0f);

    while (display.IsOpen())
    {
        display.Update();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);

        glBindVertexArray(vao);

        shader->Use();
        tex->Bind(0);
        shader->SetUniformTex("uTex", 0);

        // MVP matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(quadTransform.position, 0.0f));
        model = glm::rotate(model, quadTransform.rotation, glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(quadTransform.size, 1.0f));

        glm::mat4 mvp = proj * view * model;
        shader->SetUniformMat4("u_mvp", mvp);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ESCAPE))
            break;
    }

    display.CleanUp();

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

    return 0;
}

