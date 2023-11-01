#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>
using namespace glm;

#include "common/shader.hpp"

#include <vector>

// For measuring frames
double lastTime = glfwGetTime();
int nbFrames = 0;

struct DrawDetails {
    DrawDetails(GLuint v, GLuint e) {
        VAO = v;
        numElements = e;
    }
    GLuint VAO = 0;
    GLuint numElements = 0;
};

// Function to initialize GLFW
int initGLFW() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 8); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    return 0;
}

// Function to create an OpenGL window
GLFWwindow* createWindow(int width, int height, const char* title) {
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 4.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return NULL;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    return window;
}

static DrawDetails UploadMesh(const GLfloat* verts, const GLfloat* colors, const int v_count, const GLuint* elems, const int e_count)
{
    GLuint vboHandles[2];
    glGenBuffers(2, vboHandles);
    GLuint posBufferHandle = vboHandles[0];
    GLuint colorBufferHandle = vboHandles[1];
    // Populate position buffer
    glBindBuffer(GL_ARRAY_BUFFER, posBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, v_count * sizeof(GLfloat), verts, GL_STATIC_DRAW);
    // Populate colors buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);  // Bind color buffer
    glBufferData(GL_ARRAY_BUFFER, v_count * sizeof(GLfloat), colors, GL_STATIC_DRAW);
    // Create and setup vertex array object
    GLuint vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);
    // Enable them
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexBuffer(0, posBufferHandle, 0, sizeof(GLfloat) * 3);
    glBindVertexBuffer(1, colorBufferHandle, 0, sizeof(GLfloat) * 3);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0); // Map to shader

    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(1, 1);

    GLuint elemHandle;
    glGenBuffers(1, &elemHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, e_count * sizeof(GLuint), elems, GL_STATIC_DRAW);

    return DrawDetails(vaoHandle, static_cast<uint32_t>(e_count));
}

static void UnloadMesh(std::vector<DrawDetails>& details) {
    for (auto& d : details) {
        glDeleteVertexArrays(1, &d.VAO);
    }
    details.clear();
}

static void Draw(const std::vector<DrawDetails>& drawDetails) {
    for (const auto& d : drawDetails) {
        glBindVertexArray(d.VAO);
        glDrawElements(GL_TRIANGLES, d.numElements, GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}

static void UploadLineMesh(){

}

// Function for the main rendering loop
// Function for the main rendering loop
void renderLoop(GLFWwindow* window, std::vector<DrawDetails> ourDrawDetails) {
    // Set up line drawing (move this outside the loop)
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // Define the line vertices (start and end points)
    float lineVertices[] = {
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // Start point with red color
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,    // End point with green color
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);

    // Specify the format of your vertex data (position and color)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(0);  // Enable position attribute
    glEnableVertexAttribArray(1);  // Enable color attribute

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        // Measure frames
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0) {
            // Print frame time and FPS
            std::cout << "Frame time: " << 1000.0 / double(nbFrames) << " ms" << std::endl;
            std::cout << "FPS: " << double(nbFrames) << std::endl;
            nbFrames = 0;
            lastTime += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //Draw(ourDrawDetails);

        glDrawArrays(GL_LINES, 0, 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


int main() {
    if (initGLFW() == -1) {
        return -1;
    }

    GLFWwindow* window = createWindow(1024, 768, "Tutorial 01");
    if (window == NULL) {
        return -1;
    }
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
    glUseProgram(programID);

    std::vector<DrawDetails> ourDrawDetails;
    const GLfloat posData[] = {
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };
    const GLfloat colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.2f,
        0.5f, 0.5f, 0.0f,
    };
    const GLuint elems[] = { 0, 1, 2 };
    ourDrawDetails.push_back(UploadMesh(
        posData, // points
        colorData, // colors at points
        sizeof(posData) / sizeof(posData[0]), // size of array pos
        elems, // indices
        sizeof(elems) / sizeof(elems[0]))); // size of array elems
    

    // Set background color
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

    // Your rendering loop
    renderLoop(window, ourDrawDetails);

    // UnloadMesh here
    UnloadMesh(ourDrawDetails);

    glfwTerminate();
    return 0;
}
