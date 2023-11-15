#include <GL/glew.h>
#include <cmath>


// Assuming genRotatedPoints looks something like this
void genRotatedPoints(float centerX, float centerY, float& x, float& y, float angle) {
    // Translate the point to the origin
    x -= centerX;
    y -= centerY;
    // Rotate the point
    float newX = x * cos(angle) - y * sin(angle);
    float newY = x * sin(angle) + y * cos(angle);
    // Translate the point back to its original position
    x = newX + centerX;
    y = newY + centerY;
}

