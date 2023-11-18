#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "fun.cpp"

struct DrawDetails {
    DrawDetails(GLuint v, GLuint e) {
        VAO = v;
        numElements = e;
    }
    GLuint VAO = 0;
    GLuint numElements = 0;
};

struct RaysData{
    RaysData(std::vector<GLfloat> podData, std::vector<GLfloat> colorData, std::vector<GLuint> Elems){
        LineposData = podData;
        LineElems = Elems;
        LinecolorData = colorData;
    }
    std::vector<GLfloat> LineposData = {};
    std::vector<GLfloat>  LinecolorData = {};
    std::vector<GLuint>  LineElems = {};
};

struct WallsData {
    WallsData(GLfloat* pos, GLfloat* cD, GLuint* Elems, size_t posSize, size_t cDSize, size_t elemsSize) {
        posData.assign(pos, pos + posSize);
        colorData.assign(cD, cD + cDSize);
        elems.assign(Elems, Elems + elemsSize);
    }

    std::vector<GLfloat> posData;
    std::vector<GLfloat> colorData;
    std::vector<GLuint> elems;
};

static DrawDetails UploadMesh(GLfloat* verts, GLfloat* colors, int v_count, GLuint* elems, int e_count)
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
    for (const auto& d : details) {
        glDeleteVertexArrays(1, &d.VAO);
    }
    details.clear();
}

static void Draw(std::vector<DrawDetails>& drawDetails) {
    for (const auto& d : drawDetails) {
        glBindVertexArray(d.VAO);
        glDrawElements(GL_TRIANGLES, d.numElements, GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}

static void moveRays(GLFWwindow* window, std::vector<GLfloat>& LineposData, GLfloat& lastX, GLfloat& lastY) {
    // Get cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    GLfloat x = static_cast<GLfloat>((2.0 * xpos) / 1000.0 - 1.0);  // Transform to the range [-1, 1] for X
    GLfloat y = static_cast<GLfloat>(1.0 - (2.0 * ypos) / 1000.0);  // Transform to the range [-1, 1] for Y
    // Calculate the change in x and y
    GLfloat deltaX = x - lastX;
    GLfloat deltaY = y - lastY;
    // Update the first point of the line segment
    LineposData[0] = x;
    LineposData[1] = y;
    // Move the remaining line segments based on the change in cursor position
    for (int i = 2; i < LineposData.size(); i += 2) {
        LineposData[i] += deltaX;
        LineposData[i + 1] += deltaY;
    }
    // Update lastX and lastY for the next iteration
    lastX = x;
    lastY = y;
}

static void rotateRays(GLFWwindow* window, int& oldState, std::vector<GLfloat>& LineposData) {
    int LnewState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int RnewState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

    if (oldState == GLFW_PRESS && (LnewState == GLFW_RELEASE || RnewState == GLFW_RELEASE)) {
        for (int i = 0; i < LineposData.size(); i += 2) {
            float rotationAngle = (LnewState == GLFW_RELEASE) ? -0.1f : 0.1f;
            genRotatedPoints(LineposData[0], LineposData[1], LineposData[i], LineposData[i + 1], rotationAngle);
        }
        oldState = GLFW_RELEASE;
    } else if (LnewState == GLFW_PRESS || RnewState == GLFW_PRESS) {
        oldState = (LnewState == GLFW_PRESS) ? LnewState : RnewState;
    }
}

static DrawDetails UploadRayMesh(
    std::vector<GLfloat>& verts,
    std::vector<GLfloat>& colors,
    std::vector<GLuint>& elems) {

    GLuint vboHandles[2];
    glGenBuffers(2, vboHandles);
    GLuint posBufferHandle = vboHandles[0];
    GLuint colorBufferHandle = vboHandles[1];

    // Populate position buffer
    glBindBuffer(GL_ARRAY_BUFFER, posBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);

    // Populate colors buffer
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);

    // Create and setup vertex array object
    GLuint vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    // Enable attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Bind position buffer to VAO
    glBindVertexBuffer(0, posBufferHandle, 0, sizeof(GLfloat) * 2);
    glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    // Bind color buffer to VAO
    glBindVertexBuffer(1, colorBufferHandle, 0, sizeof(GLfloat) * 3);
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(1, 1);

    // Create and populate element buffer
    GLuint elemHandle;
    glGenBuffers(1, &elemHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elemHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elems.size() * sizeof(GLuint), elems.data(), GL_STATIC_DRAW);

    return DrawDetails(vaoHandle, static_cast<uint32_t>(elems.size()));
}

static void DrawLines(std::vector<DrawDetails>& drawDetails) {
    for (const auto& d : drawDetails) {
        glBindVertexArray(d.VAO);
        glDrawElements(GL_LINES, d.numElements, GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}
