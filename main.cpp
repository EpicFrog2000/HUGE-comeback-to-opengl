#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdlib>  // for system function

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include "common/shader.hpp"

#include <vector>

// For mesuring frames
double lastTime = glfwGetTime();
int nbFrames = 0;
struct vertexData{
    GLuint VertexArrayID;
    GLuint vertexbuffer;
    int numVertices;
} MyVertexData; 


// Function to initialize GLFW
int initGLFW() {
    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    return 0;
}

// Function to create an OpenGL window
GLFWwindow* createWindow(int width, int height, const char* title) {
    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window); // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    return window;
}

// Used in renderLoop to draw triangles for now
void Draw(std::vector<vertexData> buffers, GLuint programID){
    for(auto buffer : buffers){
        glUseProgram(programID);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, buffer.numVertices); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);   
    }
}

// Function for the main rendering loop
void renderLoop(GLFWwindow* window, std::vector<vertexData> buffers, GLuint programID) {
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
        // Mesure frames
        double currentTime = glfwGetTime();
        nbFrames++;
        if ( currentTime - lastTime >= 1.0 ){
            printf("%f ms/frame\n", 1000.0/double(nbFrames));
            printf("%f FPS\n", double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        // Draw your scene here
        Draw(buffers, programID);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void genVertexData(std::vector<vertexData> &buffers, GLfloat g_vertex_buffer_data[],int arraySize){
    GLuint VertexArrayID;
    GLuint vertexbuffer;

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * arraySize, g_vertex_buffer_data, GL_STATIC_DRAW);
    MyVertexData.numVertices = arraySize / 3;
    MyVertexData.vertexbuffer = vertexbuffer;
    MyVertexData.VertexArrayID = VertexArrayID;
    buffers.push_back(MyVertexData);
}

int main() {

    if (initGLFW() == -1) {
        return -1;
    }

    GLFWwindow* window = createWindow(1024, 768, "Tutorial 01");
    if (window == NULL) {
        return -1;
    }

    // Vector for all buffers
    std::vector<vertexData> buffers;
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
    // Create trinagle
    GLfloat g_vertex_buffer_data[] = { 
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
    };
    int arraySize = sizeof(g_vertex_buffer_data) / sizeof(g_vertex_buffer_data[0]); // Calculate the size of the array
    genVertexData(buffers, g_vertex_buffer_data, arraySize);
    // Can create multiple above and than append to buffers to ender them all

    //TODO VAO shit

    // Set bg-color
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Your rendering loop
    renderLoop(window,buffers, programID);

    glfwTerminate();
    return 0;
}
