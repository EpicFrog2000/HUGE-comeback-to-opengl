#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp>
using namespace glm;

#include "common/shader.hpp"
#include "setup/setup.cpp" //functions for setting up window initializing glew and so on
#include "mesh_and_drawing/mesh.cpp"

#include <vector>
#include <cmath>

// For measuring frames
double lastTime = glfwGetTime();
int nbFrames = 0;

// Function for the main rendering loop
void renderLoop(GLFWwindow* window,
std::vector<DrawDetails> ourDrawDetails,
std::vector<DrawDetails> ourRayDrawDetails,
std::vector<RaysData> MyRays,
std::vector<WallsData> wallsData) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    GLfloat x = static_cast<GLfloat>((2.0 * xpos) / 1000.0 - 1.0);  // Transform to the range [-1, 1] for X
    GLfloat y = static_cast<GLfloat>(1.0 - (2.0 * ypos) / 1000.0);  // Transform to the range [-1, 1] for Y
    GLfloat lastX = 0.0f;
    GLfloat lastY = 0.0f;
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
        
        
        // Rotate points by 10 degrees on LMB
        static int oldState = GLFW_RELEASE;
        rotateRays(window, oldState, MyRays[0].LineposData);
        moveRays(window, MyRays[0].LineposData, lastX, lastY);
        ourRayDrawDetails.clear();
        ourRayDrawDetails.push_back(UploadRayMesh(
            MyRays[0].LineposData, // points
            MyRays[0].LinecolorData, // colors at points
            MyRays[0].LineElems // indices
        ));
        // here i can calculate and change lenght of rays
        //for each ray:
        


        Draw(ourDrawDetails);
        DrawLines(ourRayDrawDetails);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}


int main() {
    if (initGLFW() == -1) {
        return -1;
    }

    GLFWwindow* window = createWindow(1000, 1000, "Tutorial 01");
    if (window == NULL) {
        return -1;
    }
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
    glUseProgram(programID);


    //Setting up walls data
    std::vector<DrawDetails> ourDrawDetails;
    GLfloat posData[] = {
        0.1f, -0.1f, 0.0f,
        -0.1f, -0.1f, 0.0f,
        0.0f, 0.1f, 0.0f,
    };
    GLfloat colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.2f,
        0.5f, 0.5f, 0.0f,
    };
    GLuint elems[] = { 0, 1, 2 };
    std::vector<WallsData> wallsData;
    wallsData.push_back(WallsData(posData, colorData, elems, 
    sizeof(posData) / sizeof(posData[0]), 
    sizeof(colorData) / sizeof(colorData[0]), 
    sizeof(elems) / sizeof(elems[0])));

    ourDrawDetails.push_back(UploadMesh(
        wallsData[0].posData, // points
        wallsData[0].colorData, // colors at points
        sizeof(posData) / sizeof(posData[0]), // size of array pos
        wallsData[0].elems, // indices
        sizeof(elems) / sizeof(elems[0]))); // size of array elems






    //Setting up lines data
    std::vector<DrawDetails> ourRayDrawDetails;
    GLfloat x = 0.0f;
    GLfloat y = 0.0f;
    // Set up base rays
    int rayCount = 30;
    GLfloat baseRayLenght = 0.5f;  // Desired line length
    // Initialize to store the combined vertices
    std::vector<GLfloat> LineposData = {
        x,y, // stis is starting point of all rays
    };
    std::vector<GLfloat>  LinecolorData = {
        1.0f, 0.0f, 0.0f,
    };
    std::vector<GLuint>  LineElems = {};//indices for keeping it optimized
    for (int i = 0; i <= rayCount; i ++) {
        GLfloat nextX = x + baseRayLenght * cos(1*90/rayCount * i * 3.14159265359f / 180);  // Calculate the x-coordinate
        GLfloat nextY = y + baseRayLenght * sin(1*90/rayCount * i * 3.14159265359f / 180);  // Calculate the y-coordinate
        // Define the new vertices for the line
        LineposData.push_back(nextX);
        LineposData.push_back(nextY);

        LinecolorData.insert(LinecolorData.end(), {1.0f, 0.0f, 0.0f});

        LineElems.push_back(0);
        LineElems.push_back(i+1);
    }

    std::vector<RaysData> MyRays;
    MyRays.push_back(RaysData(LineposData, LinecolorData, LineElems));
    ourRayDrawDetails.push_back(UploadRayMesh(
        MyRays[0].LineposData, // points
        MyRays[0].LinecolorData, // colors at points
        MyRays[0].LineElems // indices
        ));


    // Set background color
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    
    // Your rendering loop
    renderLoop(window, ourDrawDetails, ourRayDrawDetails, MyRays, wallsData;);

    // UnloadMesh here
    UnloadMesh(ourDrawDetails);
    UnloadMesh(ourRayDrawDetails);

    glfwTerminate();
    return 0;
}
