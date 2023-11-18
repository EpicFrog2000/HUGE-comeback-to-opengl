#include <GL/glew.h>
#include <cmath>
#include <iostream>
#include <cmath>

struct Point {
    GLfloat x, y;
};

struct Line {
    Point start, end;
};

GLfloat distance(const Point& p1, const Point& p2) {
    GLfloat dx = p2.x - p1.x;
    GLfloat dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}

GLfloat length(const Line& line) {
    return distance(line.start, line.end);
}

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

// Helper function to find the orientation of three points
int orientation(Point p, Point q, Point r) {
    float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;  // Collinear
    return (val > 0) ? 1 : -1; // Clockwise or counterclockwise
}

// Check if two line segments intersect
bool doIntersect(Line l1, Line l2, Point& intersectionPoint) {
    // Find the 4 orientations needed for general and special cases
    int o1 = orientation(l1.start, l1.end, l2.start);
    int o2 = orientation(l1.start, l1.end, l2.end);
    int o3 = orientation(l2.start, l2.end, l1.start);
    int o4 = orientation(l2.start, l2.end, l1.end);

    // General case
    if (o1 != o2 && o3 != o4) {
        // Lines intersect, calculate the intersection point
        float x1 = l1.start.x, y1 = l1.start.y;
        float x2 = l1.end.x, y2 = l1.end.y;
        float x3 = l2.start.x, y3 = l2.start.y;
        float x4 = l2.end.x, y4 = l2.end.y;

        float determinant = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
        if (determinant == 0) {
            // Lines are parallel, no unique intersection point
            return false;
        }

        intersectionPoint.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / determinant;
        intersectionPoint.y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / determinant;

        return true;
    }

    return false;
}

// Function to check if a point is on a line segment
bool isPointOnLine(const Point& p, const Line& line) {
    GLfloat crossProduct = (p.y - line.start.y) * (line.end.x - line.start.x) - (p.x - line.start.x) * (line.end.y - line.start.y);

    // Check if the point is on the line segment
    if (fabs(crossProduct) > 0.00001) {
        return false;
    }

    GLfloat dotProduct = (p.x - line.start.x) * (line.end.x - line.start.x) + (p.y - line.start.y) * (line.end.y - line.start.y);

    if (dotProduct < 0) {
        return false;
    }

    GLfloat squaredLength = (line.end.x - line.start.x) * (line.end.x - line.start.x) + (line.end.y - line.start.y) * (line.end.y - line.start.y);

    if (dotProduct > squaredLength) {
        return false;
    }

    return true;
}