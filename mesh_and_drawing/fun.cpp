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

#include <iostream>
#include <cmath>

struct Point {
    GLfloat x, y;
};

// Funkcja sprawdzająca, czy dwie linie przecinają się
bool doLinesIntersect(Point p1, Point q1, Point p2, Point q2, Point &intersectionPoint) {
    float orientation1 = (q1.y - p1.y) * (p2.x - q1.x) - (q1.x - p1.x) * (p2.y - q1.y);
    float orientation2 = (q1.y - p1.y) * (q2.x - q1.x) - (q1.x - p1.x) * (q2.y - q1.y);
    float orientation3 = (q2.y - p2.y) * (p1.x - q2.x) - (q2.x - p2.x) * (p1.y - q2.y);
    float orientation4 = (q2.y - p2.y) * (q1.x - q2.x) - (q2.x - p2.x) * (q1.y - q2.y);

    // Check for parallel lines
    if (orientation1 == 0 && orientation2 == 0 && orientation3 == 0 && orientation4 == 0) {
        // Lines are collinear, handle this case as needed
        return false;
    }

    // Check if orientations are different
    if ((orientation1 * orientation2 < 0) && (orientation3 * orientation4 < 0)) {
        // Lines intersect, calculate the intersection point
        float t = ((p2.x - p1.x) * (q1.y - p1.y) - (p2.y - p1.y) * (q1.x - p1.x)) /
              ((q2.y - p2.y) * (q1.x - p1.x) - (q2.x - p2.x) * (q1.y - p1.y));

        intersectionPoint.x = p1.x + t * (q1.x - p1.x);
        intersectionPoint.y = p1.y + t * (q1.y - p1.y);

        return true; // Lines intersect
    } else {
        return false; // Lines do not intersect
    }
}


// Funkcja obliczająca odległość między dwoma punktami
float calculateDistance(Point p1, Point p2) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Function to set the length of a line to a specific amount
void setLineLength(GLfloat &sX, GLfloat &sY, GLfloat &eX, GLfloat &eY, GLfloat targetLength) {
    // Calculate the original length of the line
    Point start = {sX, sY};
    Point end = {eX, eY};
    GLfloat originalLength = calculateDistance(start, end);

    // Scale the coordinates to achieve the target length
    if (originalLength > 0) {
        GLfloat scaleFactor = targetLength / originalLength;
        eX = sX + (eX - sX) * scaleFactor;
        eY = sY + (eY - sY) * scaleFactor;
    }
}