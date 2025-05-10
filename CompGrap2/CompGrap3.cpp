#define GLEW_DLL
#define GLFW_DLL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ShaderLoader.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"
#include <iostream>

// Параметры камеры
glm::vec3 cameraPos = glm::vec3(0.0f, -1.5f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Углы Эйлера
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512.0f / 2.0f;
float lastY = 512.0f / 2.0f;
bool firstMouse = true;
float sensitivity = 0.03f;
float cameraSpeed = 0.001f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.001f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Zauzolkov Vladislav ASUb-23-2", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not create window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);

    ShaderLoader shader("vertex_shader.glsl", "fragment_shader.glsl");

    
    Model ourModel("model.obj");

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2, 1.0, 1.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

        shader.SetUniformMatrix4fv("projection", glm::value_ptr(projection));
        shader.SetUniformMatrix4fv("view", glm::value_ptr(view));
        shader.SetUniformMatrix4fv("model", glm::value_ptr(model));

        // Настройка материала
        shader.SetUniform3f("material.ambient", 0.24725f, 0.1995f, 0.0745f);
        shader.SetUniform3f("material.diffuse", 0.75164f, 0.60648f, 0.22648f);
        shader.SetUniform3f("material.specular", 0.628281f, 0.555802f, 0.366065f);
        shader.SetUniform1f("material.shininess", 51.2f);

        // Настройка источника света
        shader.SetUniform3f("light.position", 1.2f, 1.0f, 2.0f);
        shader.SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
        shader.SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f);
        shader.SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

        // Позиция камеры
        shader.SetUniform3f("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

        ourModel.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}