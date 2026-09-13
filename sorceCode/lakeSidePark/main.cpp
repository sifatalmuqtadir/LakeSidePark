/*
===============================================================================
  Project Title: 2D Lakeside Park Scene
  Course: Computer Graphics (AIUB)
  Description: Integrated Day 1 & Day 2 implementation containing base terrain,
               lake geometry, railing posts, park path, and bench structures.
===============================================================================
*/

#include <windows.h>
#include <GL/glut.h>
#include <math.h>

// -------------------------------------------------------------
// Initialization Setup
// -------------------------------------------------------------
void initGL() {
    // Set background clear color to solid white
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// -------------------------------------------------------------
// Primitives & Utility Functions
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
// Environment & Terrain Components (Day 1 Features)
// -------------------------------------------------------------

// Grass Field Parameters & Multi-polygon Mesh
void field() {
    // Main Upper Field Polygon
    glBegin(GL_POLYGON);
    glColor3f(21.0f / 255.0f, 173.0f / 255.0f, 52.0f / 255.0f);
    glVertex2f(-1.0f,  0.52f); // Top Left
    glVertex2f( 1.0f,  0.52f); // Top Right
    glVertex2f( 1.0f,  0.00f); // Bottom Right
    glVertex2f(-1.0f,  0.00f); // Bottom Left
    glEnd();

    // Lower Field Boundary Strip
    glBegin(GL_POLYGON);
    glColor3f(21.0f / 255.0f, 173.0f / 255.0f, 52.0f / 255.0f);
    glVertex2f(-1.0f,  0.00f);
    glVertex2f(-1.0f, -0.10f);
    glVertex2f( 1.0f, -0.10f);
    glVertex2f( 1.0f,  0.00f);
    glEnd();
}

// Lake Water Body Parameters
void lake() {
    glBegin(GL_POLYGON);
    glColor3f(125.0f / 255.0f, 244.0f / 255.0f, 255.0f / 255.0f);
    glVertex2f(-1.0f, -0.10f); // Top Left
    glVertex2f(-1.0f, -0.60f); // Bottom Left
    glVertex2f( 1.0f, -0.60f); // Bottom Right
    glVertex2f( 1.0f, -0.10f); // Top Right
    glEnd();
}

// Structural Border Outlines
void border() {
    line(-1.0f,  0.52f, 1.0f,  0.52f); // Field Upper Border
    line(-1.0f,  0.00f, 1.0f,  0.00f); // Field Lower Divider
    line(-1.0f, -0.10f, 1.0f, -0.10f); // Lake Top Horizon
    line(-1.0f, -0.60f, 1.0f, -0.60f); // Lake Bottom Horizon
}

// -------------------------------------------------------------
// Park Infrastructure Elements (Day 2 Features)
// -------------------------------------------------------------

// Vertical Railing Post Helper
// Coordinates: (a,b)=Top-Left, (c,d)=Bottom-Left, (e,f)=Bottom-Right, (g,h)=Top-Right
void vertical_railing(float a, float b, float c, float d, float e, float f, float g, float h) {
    glBegin(GL_POLYGON);
    glColor3f(87.0f / 255.0f, 87.0f / 255.0f, 87.0f / 255.0f);
    glVertex2f(a, b);
    glVertex2f(c, d);
    glVertex2f(e, f);
    glVertex2f(g, h);
    glEnd();
}

// Complete Railing Structure along the Lake Edge
void railing() {
    // Top Horizontal Bar
    glBegin(GL_POLYGON);
    glColor3f(87.0f / 255.0f, 87.0f / 255.0f, 87.0f / 255.0f);
    glVertex2f(-0.99f,  0.00f);
    glVertex2f(-0.99f, -0.01f);
    glVertex2f( 0.99f, -0.01f);
    glVertex2f( 0.99f,  0.00f);
    glEnd();

    // Bottom Horizontal Bar
    glBegin(GL_POLYGON);
    glColor3f(87.0f / 255.0f, 87.0f / 255.0f, 87.0f / 255.0f);
    glVertex2f( 0.99f, -0.08f);
    glVertex2f( 0.99f, -0.09f);
    glVertex2f(-0.99f, -0.09f);
    glVertex2f(-0.99f, -0.08f);
    glEnd();

    // Spaced Vertical Pillars
    vertical_railing(-0.99f, 0.0f, -0.99f, -0.1f, -0.98f, -0.1f, -0.98f, 0.0f);
    vertical_railing(-0.54f, 0.0f, -0.54f, -0.1f, -0.53f, -0.1f, -0.53f, 0.0f);
    vertical_railing( 0.00f, 0.0f,  0.00f, -0.1f,  0.01f, -0.1f,  0.01f, 0.0f);
    vertical_railing( 0.54f, 0.0f,  0.54f, -0.1f,  0.55f, -0.1f,  0.55f, 0.0f);
    vertical_railing( 0.98f, 0.0f,  0.98f, -0.1f,  0.99f, -0.1f,  0.99f, 0.0f);
}

// Walkway / Park Road Geometry
void park_road() {
    glBegin(GL_POLYGON);
    glColor3f(217.0f / 255.0f, 193.0f / 255.0f, 167.0f / 255.0f);
    glVertex2f(0.88f, 0.00f);
    glVertex2f(0.42f, 0.00f);
    glVertex2f(0.55f, 0.52f);
    glVertex2f(0.75f, 0.52f);
    glEnd();
}

// Wooden Bench Unit Construction
void bench() {
    // Bench Seat Top Surface
    glBegin(GL_POLYGON);
    glColor3f(255.0f / 255.0f, 208.0f / 255.0f, 0.0f / 255.0f);
    glVertex2f(-0.31f, 0.11f);
    glVertex2f( 0.00f, 0.11f);
    glVertex2f(-0.02f, 0.21f);
    glVertex2f(-0.29f, 0.21f);
    glEnd();

    // Bench Wireframe Outlines
    line(-0.31f, 0.11f,  0.00f, 0.11f);
    line( 0.00f, 0.11f, -0.02f, 0.21f);
    line(-0.02f, 0.21f, -0.29f, 0.21f);
    line(-0.29f, 0.21f, -0.31f, 0.11f);
}

// -------------------------------------------------------------
// Display Routine
// -------------------------------------------------------------
void LakeSidePark_Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Render Base Layer (Terrain & Water)
    lake();
    field();
    border();

    // Render Park Structures Layer
    park_road();
    bench();
    railing();

    glutSwapBuffers();
}

// -------------------------------------------------------------
// Main Execution Entry Point
// -------------------------------------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Lakeside Park - 2D Scene Layout");

    glutDisplayFunc(LakeSidePark_Display);
    initGL();

    glutMainLoop();
    return 0;
}
