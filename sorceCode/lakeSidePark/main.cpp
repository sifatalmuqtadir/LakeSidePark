#include <windows.h>
#include <GL/glut.h>
#include <math.h>

void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// -------------------------------------------------------------
// Line drawing utility
// -------------------------------------------------------------
void line(float a, float b, float c, float d) {
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(a, b);
    glVertex2f(c, d);
    glEnd();
}

// -------------------------------------------------------------
// Grass Field Parameters & Polygon
// -------------------------------------------------------------
void field() {
    // Upper Field Polygon
    glBegin(GL_POLYGON);
    glColor3f(21.0f/255.0f, 173.0f/255.0f, 52.0f/255.0f);
    glVertex2f(-1,    0.52); // Top Left
    glVertex2f( 1,    0.52); // Top Right
    glVertex2f( 1,    0);    // Bottom Right
    glVertex2f(-1,    0);    // Bottom Left
    glEnd();

    // Lower Field Strip
    glBegin(GL_POLYGON);
    glColor3f(21.0f/255.0f, 173.0f/255.0f, 52.0f/255.0f);
    glVertex2f(-1,  0);
    glVertex2f(-1, -0.1f);
    glVertex2f( 1, -0.1f);
    glVertex2f( 1,  0);
    glEnd();
}

// -------------------------------------------------------------
// Lake Water Parameters & Polygon
// -------------------------------------------------------------
void lake() {
    glBegin(GL_POLYGON);
    glColor3f(125.0f/255.0f, 244.0f/255.0f, 255.0f/255.0f);
    glVertex2f(-1,  -0.1f); // Top Left
    glVertex2f(-1,  -0.6f); // Bottom Left
    glVertex2f( 1,  -0.6f); // Bottom Right
    glVertex2f( 1,  -0.1f); // Top Right
    glEnd();
}

// -------------------------------------------------------------
// Border Lines
// -------------------------------------------------------------
void border() {
    line(-1,  0.52f, 1,  0.52f); // Field Upper Border
    line(-1,  0.00f, 1,  0.00f); // Field Lower Border
    line(-1, -0.10f, 1, -0.10f); // Lake Top Border
    line(-1, -0.60f, 1, -0.60f); // Lake Bottom Border
}

void LakeSidePark_Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    lake();
    field();
    border();

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Lake Side Park");
    glutDisplayFunc(LakeSidePark_Display);
    initGL();
    glutMainLoop();
    return 0;
}
