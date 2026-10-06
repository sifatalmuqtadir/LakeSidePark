/*
===========================================================
                PROJECT: Lake Side Park
===========================================================

SCENE COORDINATE STRUCTURE (GeoGebra Reference):
-----------------------------------------------------------
Park   : A(-1, 0.52) B(1, 0.52) C(1, 0) D(-1, 0)
Railing: E(-1, 0)    F(-1,-0.1) G(1, 0) H(1,-0.1)
Lake   : I(-1,-0.1)  J(-1,-0.6) K(1,-0.1) L(1,-0.6)

CONTROLS:
-----------------------------------------------------------
[B] - Start Boat movement (Left to Right).
[N] - Stop  Boat movement.

[Left Mouse Click]  - Start Ferris Wheel rotation.
[Right Mouse Click] - Stop  Ferris Wheel rotation.
[H] - Increase Ferris Wheel Speed.
[L] - Decrease Ferris Wheel Speed.

[1] - Overcast: rain starts, lamp lights turn golden.
[2] - Clear   : rain stops,  lamp lights turn grey.

===========================================================
*/

#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define MAX_DROPS 10000

// Rain drop data
float dropX[MAX_DROPS];
float dropY[MAX_DROPS];
float dropSpeed[MAX_DROPS];
int   dropCount = 0;

const float rainGroundY = -0.6f; // Bottom of lake = bottom of rain

float boat_move_x    = 0.0f; // Boat initial X position
float boat_speed     = 0.0f; // Boat initially stopped

float nagordola_angle = 0.0f;
float nagordola_speed = 0.0f; // Ferris wheel initially stopped

bool is_overcast_sky = false; // Initially clear sky

// ============================================================
//  INIT
// ============================================================

void initGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

// ============================================================
//  UTILITY
// ============================================================

void line(float a, float b, float c, float d)
{
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f); // Black
    glVertex2f(a, b);
    glVertex2f(c, d);
    glEnd();
}

// ============================================================
//  RAIN
// ============================================================

void drawDrops()
{
    glColor3f(81.0f/255.0f, 245.0f/255.0f, 252.0f/255.0f); // Rain colour
    for (int i = 0; i < dropCount; ++i)
    {
        glBegin(GL_LINES);
        glVertex2f(dropX[i], dropY[i]);          // Drop top
        glVertex2f(dropX[i], dropY[i] - 0.04f);  // Drop bottom
        glEnd();
    }
}

void rain_animation(int value)
{
    if (dropCount < MAX_DROPS && (rand() % 5) == 0)
    {
        dropX[dropCount]     = (rand() % 2000 / 1000.0f) - 1.0f; // -1 to 1
        dropY[dropCount]     = 0.52f;                              // Start at park top
        dropSpeed[dropCount] = 0.01f + (rand() % 100 / 10000.0f);
        dropCount++;
    }

    for (int i = 0; i < dropCount; )
    {
        dropY[i] -= dropSpeed[i];
        if (dropY[i] <= rainGroundY)
        {
            // Replace with last drop
            dropX[i]     = dropX[dropCount - 1];
            dropY[i]     = dropY[dropCount - 1];
            dropSpeed[i] = dropSpeed[dropCount - 1];
            dropCount--;
        }
        else { i++; }
    }

    glutPostRedisplay();
    glutTimerFunc(10, rain_animation, 0);
}

// ============================================================
//  PARK FIELD   [A(-1,0.52) B(1,0.52) C(1,0) D(-1,0)]
// ============================================================

void field()
{
    // Main Park Grass Field
    glBegin(GL_POLYGON);
    glColor3f(21.0f/255.0f, 173.0f/255.0f, 52.0f/255.0f); // Grass green
    glVertex2f(-1,    0.52); // A
    glVertex2f( 1,    0.52); // B
    glVertex2f( 1,    0);    // C
    glVertex2f(-1,    0);    // D
    glEnd();

    // Thin grass strip between Railing and Lake
    // (fills the gap at the very bottom of the railing zone)
    glBegin(GL_POLYGON);
    glColor3f(21.0f/255.0f, 173.0f/255.0f, 52.0f/255.0f); // Grass green
    glVertex2f(-1,  0);    // E
    glVertex2f(-1, -0.1f); // F
    glVertex2f( 1, -0.1f); // H  (note: E->F->H->G order draws correctly)
    glVertex2f( 1,  0);    // G
    glEnd();
}

// ============================================================
//  RAILING   [E(-1,0)  F(-1,-0.1)  G(1,0)  H(1,-0.1)]
// ============================================================

void vertical_railing(float a, float b, float c, float d,
                      float e, float f, float g, float h)
{
    glBegin(GL_POLYGON);
    glColor3f(87.0f/255.0f, 87.0f/255.0f, 87.0f/255.0f); // Dark grey
    glVertex2f(a, b); // top-left
    glVertex2f(c, d); // bottom-left
    glVertex2f(e, f); // bottom-right
    glVertex2f(g, h); // top-right
    glEnd();
}

void railing()
{
    // Upper horizontal rail  (sits just below E-G line, y = 0 to -0.02)
    glBegin(GL_POLYGON);
    glColor3f(87.0f/255.0f, 87.0f/255.0f, 87.0f/255.0f); // Dark grey
    glVertex2f(-0.99f,  0.0f);   // E1
    glVertex2f(-0.99f, -0.01f);  // F1
    glVertex2f( 0.99f, -0.01f);  // G1
    glVertex2f( 0.99f,  0.0f);   // H1
    glEnd();

    // Lower horizontal rail  (sits near bottom of railing zone, y = -0.08 to -0.09)
    glBegin(GL_POLYGON);
    glColor3f(87.0f/255.0f, 87.0f/255.0f, 87.0f/255.0f); // Dark grey
    glVertex2f( 0.99f, -0.08f);  // I1
    glVertex2f( 0.99f, -0.09f);  // J1
    glVertex2f(-0.99f, -0.09f);  // K1
    glVertex2f(-0.99f, -0.08f);  // L1
    glEnd();

    // Vertical posts  (span from y=0 down to y=-0.1, inside E-F-G-H zone)
    //  Each call: top-left, bottom-left, bottom-right, top-right
    vertical_railing(-0.99f,  0.0f,  -0.99f, -0.1f,  -0.98f, -0.1f,  -0.98f,  0.0f); // M1 N1 O1 P1
    vertical_railing(-0.90f,  0.0f,  -0.90f, -0.1f,  -0.89f, -0.1f,  -0.89f,  0.0f); // Q1 R1 S1 T1
    vertical_railing(-0.81f,  0.0f,  -0.81f, -0.1f,  -0.80f, -0.1f,  -0.80f,  0.0f); // U1 V1 W1 X1
    vertical_railing(-0.72f,  0.0f,  -0.72f, -0.1f,  -0.71f, -0.1f,  -0.71f,  0.0f); // Y1 Z1 A2 B2
    vertical_railing(-0.63f,  0.0f,  -0.63f, -0.1f,  -0.62f, -0.1f,  -0.62f,  0.0f); // C2 D2 E2 F2
    vertical_railing(-0.54f,  0.0f,  -0.54f, -0.1f,  -0.53f, -0.1f,  -0.53f,  0.0f); // G2 H2 I2 J2
    vertical_railing(-0.45f,  0.0f,  -0.45f, -0.1f,  -0.44f, -0.1f,  -0.44f,  0.0f); // K2 L2 M2 N2
    vertical_railing(-0.36f,  0.0f,  -0.36f, -0.1f,  -0.35f, -0.1f,  -0.35f,  0.0f); // O2 P2 Q2 R2
    vertical_railing(-0.27f,  0.0f,  -0.27f, -0.1f,  -0.26f, -0.1f,  -0.26f,  0.0f); // S2 T2 U2 V2
    vertical_railing(-0.18f,  0.0f,  -0.18f, -0.1f,  -0.17f, -0.1f,  -0.17f,  0.0f); // W2 X2 Y2 Z2
    vertical_railing(-0.09f,  0.0f,  -0.09f, -0.1f,  -0.08f, -0.1f,  -0.08f,  0.0f); // A3 B3 C3 D3
    vertical_railing( 0.00f,  0.0f,   0.00f, -0.1f,   0.01f, -0.1f,   0.01f,  0.0f); // E3 F3 G3 H3
    vertical_railing( 0.09f,  0.0f,   0.09f, -0.1f,   0.10f, -0.1f,   0.10f,  0.0f); // I3 J3 K3 L3
    vertical_railing( 0.18f,  0.0f,   0.18f, -0.1f,   0.19f, -0.1f,   0.19f,  0.0f); // M3 N3 O3 P3
    vertical_railing( 0.27f,  0.0f,   0.27f, -0.1f,   0.28f, -0.1f,   0.28f,  0.0f); // Q3 R3 S3 T3
    vertical_railing( 0.36f,  0.0f,   0.36f, -0.1f,   0.37f, -0.1f,   0.37f,  0.0f); // U3 V3 W3 X3
    vertical_railing( 0.45f,  0.0f,   0.45f, -0.1f,   0.46f, -0.1f,   0.46f,  0.0f); // Y3 Z3 A4 B4
    vertical_railing( 0.54f,  0.0f,   0.54f, -0.1f,   0.55f, -0.1f,   0.55f,  0.0f); // C4 D4 E4 F4
    vertical_railing( 0.63f,  0.0f,   0.63f, -0.1f,   0.64f, -0.1f,   0.64f,  0.0f); // G4 H4 I4 J4
    vertical_railing( 0.72f,  0.0f,   0.72f, -0.1f,   0.73f, -0.1f,   0.73f,  0.0f); // K4 L4 M4 N4
    vertical_railing( 0.81f,  0.0f,   0.81f, -0.1f,   0.82f, -0.1f,   0.82f,  0.0f); // O4 P4 Q4 R4
    vertical_railing( 0.90f,  0.0f,   0.90f, -0.1f,   0.91f, -0.1f,   0.91f,  0.0f); // S4 T4 U4 V4
    vertical_railing( 0.98f,  0.0f,   0.98f, -0.1f,   0.99f, -0.1f,   0.99f,  0.0f); // W4 X4 Y4 Z4
}

// ============================================================
//  LAKE   [I(-1,-0.1)  J(-1,-0.6)  K(1,-0.1)  L(1,-0.6)]
// ============================================================

void lake()
{
    glBegin(GL_POLYGON);
    glColor3f(125.0f/255.0f, 244.0f/255.0f, 255.0f/255.0f); // Water blue
    glVertex2f(-1,  -0.1f); // I
    glVertex2f(-1,  -0.6f); // J
    glVertex2f( 1,  -0.6f); // L
    glVertex2f( 1,  -0.1f); // K
    glEnd();
}

// ============================================================
//  BOAT   (floats inside Lake, centred at y ≈ -0.3)
// ============================================================

void boat()
{
    glPushMatrix();
    glTranslatef(boat_move_x, 0.0f, 0.0f);

    // Boat Hull (chocolate brown trapezoid)
    glBegin(GL_POLYGON);
    glColor3f(171.0f/255.0f, 69.0f/255.0f, 12.0f/255.0f); // Brown
    glVertex2f(-0.80f, -0.27f); // A5
    glVertex2f(-0.72f, -0.33f); // B5
    glVertex2f(-0.48f, -0.33f); // C5
    glVertex2f(-0.40f, -0.27f); // D5
    glEnd();

    // Boat Cabin (golden rectangle on deck)
    glBegin(GL_POLYGON);
    glColor3f(224.0f/255.0f, 182.0f/255.0f, 47.0f/255.0f); // Gold
    glVertex2f(-0.70f, -0.27f); // E5
    glVertex2f(-0.70f, -0.22f); // F5
    glVertex2f(-0.50f, -0.22f); // G5
    glVertex2f(-0.50f, -0.27f); // H5
    glEnd();

    // Left Wheel Cover (grey half-circle)
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 200; i++)
    {
        glColor3f(176.0f/255.0f, 176.0f/255.0f, 176.0f/255.0f); // Grey
        float pi = 3.1416f;
        float A  = (i * pi) / 200;
        float r  = 0.05f;
        glVertex2f(r * cosf(A) - 0.70f, r * sinf(A) - 0.27f); // I5 (centre E5)
    }
    glEnd();

    // Right Wheel Cover (grey half-circle)
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 200; i++)
    {
        glColor3f(176.0f/255.0f, 176.0f/255.0f, 176.0f/255.0f); // Grey
        float pi = 3.1416f;
        float A  = (i * pi) / 200;
        float r  = 0.05f;
        glVertex2f(r * cosf(A) - 0.50f, r * sinf(A) - 0.27f); // J5 (centre H5)
    }
    glEnd();

    // Hull border lines
    line(-0.80f, -0.27f, -0.40f, -0.27f); // A5 D5  (top edge)
    line(-0.72f, -0.33f, -0.48f, -0.33f); // B5 C5  (bottom edge)
    line(-0.80f, -0.27f, -0.72f, -0.33f); // A5 B5  (left side)
    line(-0.48f, -0.33f, -0.40f, -0.27f); // C5 D5  (right side)

    // Cabin top border
    line(-0.70f, -0.22f, -0.50f, -0.22f); // F5 G5

    // Left wheel cover border
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 200; i++)
    {
        glColor3f(0.0f, 0.0f, 0.0f); // Black
        float pi = 3.1416f;
        float A  = (i * pi) / 200;
        float r  = 0.05f;
        glVertex2f(r * cosf(A) - 0.70f, r * sinf(A) - 0.27f); // I5 arc
    }
    glEnd();

    // Right wheel cover border
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 200; i++)
    {
        glColor3f(0.0f, 0.0f, 0.0f); // Black
        float pi = 3.1416f;
        float A  = (i * pi) / 200;
        float r  = 0.05f;
        glVertex2f(r * cosf(A) - 0.50f, r * sinf(A) - 0.27f); // J5 arc
    }
    glEnd();

    glPopMatrix();
}

void boat_animation(int value)
{
    boat_move_x += boat_speed;
    if (boat_move_x > 1.8f)
        boat_move_x = -1.03f; // Wrap to left
    glutPostRedisplay();
    glutTimerFunc(25, boat_animation, 0);
}

// ============================================================
//  FERRIS WHEEL / NAGORDOLA
// ============================================================

void nagordola_Wheel(float cx, float cy, float radius, float R, float G, float B)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < 200; i++)
    {
        glColor3f(R, G, B);
        float pi = 3.1416f;
        float A  = (i * 2 * pi) / 200;
        glVertex2f(radius * cosf(A) + cx, radius * sinf(A) + cy); // Rim vertex
    }
    glEnd();
}

void nagordola_spoke(float a, float b, float c, float d,
                     float e, float f, float g, float h)
{
    // White rectangular spoke
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glVertex2f(a, b); // K5
    glVertex2f(c, d); // L5
    glVertex2f(e, f); // M5
    glVertex2f(g, h); // N5
    glEnd();
}

void cabin_upper(float a, float b, float c, float d,
                 float e, float f, float g, float h)
{
    // Blue top of cabin gondola
    glBegin(GL_POLYGON);
    glColor3f(29.0f/255.0f, 0.0f/255.0f, 255.0f/255.0f); // Blue
    glVertex2f(a, b); // O5
    glVertex2f(c, d); // P5
    glVertex2f(e, f); // Q5
    glVertex2f(g, h); // R5
    glEnd();
}

void cabin_lower(float a, float b, float c, float d,
                 float e, float f, float g, float h)
{
    // Red bottom of cabin gondola
    glBegin(GL_POLYGON);
    glColor3f(255.0f/255.0f, 0.0f/255.0f, 0.0f/255.0f); // Red
    glVertex2f(a, b); // S5
    glVertex2f(c, d); // T5
    glVertex2f(e, f); // U5
    glVertex2f(g, h); // V5
    glEnd();
}

void nagordola()
{
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    // --- Rotate whole wheel ---
    glPushMatrix();
    glTranslatef(-0.75f, 0.35f, 0.0f);
    glRotatef(nagordola_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.75f, -0.35f, 0.0f);

    // Outer rim (white)
    nagordola_Wheel(-0.75f, 0.35f, 0.15f,
                    255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f);
    // Inner disc (green)
    nagordola_Wheel(-0.75f, 0.35f, 0.13f,
                    21.0f/255.0f, 173.0f/255.0f, 52.0f/255.0f);

    // Horizontal spoke
    nagordola_spoke(-0.89f, 0.36f,  // W5
                    -0.89f, 0.34f,  // X5
                    -0.61f, 0.34f,  // Y5
                    -0.61f, 0.36f); // Z5

    // Vertical spoke
    nagordola_spoke(-0.74f, 0.49f,  // A6
                    -0.76f, 0.49f,  // B6
                    -0.76f, 0.21f,  // C6
                    -0.74f, 0.21f); // D6

    // --- Top Cabin (counter-rotates to stay upright) ---
    glPushMatrix();
    glTranslatef(-0.75f, 0.49f, 0.0f);
    glRotatef(-nagordola_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.75f, -0.49f, 0.0f);
    nagordola_spoke(-0.745f, 0.490f,  // E6
                    -0.755f, 0.490f,  // F6
                    -0.755f, 0.425f,  // G6
                    -0.745f, 0.425f); // H6
    cabin_upper(-0.72f, 0.425f,  // I6
                -0.78f, 0.425f,  // J6
                -0.78f, 0.390f,  // K6
                -0.72f, 0.390f); // L6
    cabin_lower(-0.78f, 0.390f,  // M6
                -0.72f, 0.390f,  // N6
                -0.72f, 0.365f,  // O6
                -0.78f, 0.365f); // P6
    glPopMatrix();

    // --- Right Cabin ---
    glPushMatrix();
    glTranslatef(-0.61f, 0.35f, 0.0f);
    glRotatef(-nagordola_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.61f, -0.35f, 0.0f);
    nagordola_spoke(-0.615f, 0.350f,  // Q6
                    -0.605f, 0.350f,  // R6
                    -0.605f, 0.285f,  // S6
                    -0.615f, 0.285f); // T6
    cabin_upper(-0.58f, 0.285f,  // U6
                -0.64f, 0.285f,  // V6
                -0.64f, 0.250f,  // W6
                -0.58f, 0.250f); // X6
    cabin_lower(-0.64f, 0.250f,  // Y6
                -0.58f, 0.250f,  // Z6
                -0.58f, 0.225f,  // A7
                -0.64f, 0.225f); // B7
    glPopMatrix();

    // --- Left Cabin ---
    glPushMatrix();
    glTranslatef(-0.89f, 0.35f, 0.0f);
    glRotatef(-nagordola_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.89f, -0.35f, 0.0f);
    nagordola_spoke(-0.885f, 0.350f,  // C7
                    -0.895f, 0.350f,  // D7
                    -0.895f, 0.285f,  // E7
                    -0.885f, 0.285f); // F7
    cabin_upper(-0.86f, 0.285f,  // G7
                -0.92f, 0.285f,  // H7
                -0.92f, 0.250f,  // I7
                -0.86f, 0.250f); // J7
    cabin_lower(-0.92f, 0.250f,  // K7
                -0.86f, 0.250f,  // L7
                -0.86f, 0.225f,  // M7
                -0.92f, 0.225f); // N7
    glPopMatrix();

    // --- Bottom Cabin ---
    glPushMatrix();
    glTranslatef(-0.75f, 0.21f, 0.0f);
    glRotatef(-nagordola_angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.75f, -0.21f, 0.0f);
    nagordola_spoke(-0.745f, 0.210f,  // O7
                    -0.755f, 0.210f,  // P7
                    -0.755f, 0.145f,  // Q7
                    -0.745f, 0.145f); // R7
    cabin_upper(-0.72f, 0.145f,  // S7
                -0.78f, 0.145f,  // T7
                -0.78f, 0.110f,  // U7
                -0.72f, 0.110f); // V7
    cabin_lower(-0.78f, 0.110f,  // W7
                -0.72f, 0.110f,  // X7
                -0.72f, 0.085f,  // Y7
                -0.78f, 0.085f); // Z7
    glPopMatrix();

    glPopMatrix(); // End wheel rotation

    // Support Stand - Right Leg
    glBegin(GL_POLYGON);
    glColor3f(122.0f/255.0f, 80.0f/255.0f, 75.0f/255.0f); // Brown
    glVertex2f(-0.75f, 0.35f); // A8
    glVertex2f(-0.65f, 0.05f); // B8
    glVertex2f(-0.62f, 0.05f); // C8
    glEnd();

    // Support Stand - Left Leg
    glBegin(GL_POLYGON);
    glColor3f(122.0f/255.0f, 80.0f/255.0f, 75.0f/255.0f); // Brown
    glVertex2f(-0.75f, 0.35f); // D8
    glVertex2f(-0.85f, 0.05f); // E8
    glVertex2f(-0.88f, 0.05f); // F8
    glEnd();

    // Hub circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < 200; i++)
    {
        glColor3f(122.0f/255.0f, 80.0f/255.0f, 75.0f/255.0f); // Brown
        float pi = 3.1416f;
        float A  = (i * 2 * pi) / 200;
        float r  = 0.015f;
        glVertex2f(r * cosf(A) - 0.75f, r * sinf(A) + 0.35f); // G8 (hub centre)
    }
    glEnd();
}

void nagordola_rotation(int value)
{
    nagordola_angle += nagordola_speed;
    glutPostRedisplay();
    glutTimerFunc(20, nagordola_rotation, 0);
}

// ============================================================
//  BENCH
// ============================================================

void bench()
{
    // Left Leg
    glBegin(GL_POLYGON);
    glColor3f(217.0f/255.0f, 140.0f/255.0f, 0.0f/255.0f); // Orange-brown
    glVertex2f(-0.27f, 0.05f); // H8
    glVertex2f(-0.25f, 0.05f); // I8
    glVertex2f(-0.25f, 0.11f); // J8
    glVertex2f(-0.27f, 0.11f); // K8
    glEnd();
    line(-0.27f, 0.11f, -0.27f, 0.05f); // K8 H8
    line(-0.25f, 0.05f, -0.25f, 0.11f); // I8 J8

    // Right Leg
    glBegin(GL_POLYGON);
    glColor3f(217.0f/255.0f, 140.0f/255.0f, 0.0f/255.0f); // Orange-brown
    glVertex2f(-0.06f, 0.05f); // L8
    glVertex2f(-0.04f, 0.05f); // M8
    glVertex2f(-0.04f, 0.11f); // N8
    glVertex2f(-0.06f, 0.11f); // O8
    glEnd();
    line(-0.06f, 0.11f, -0.06f, 0.05f); // O8 L8
    line(-0.04f, 0.05f, -0.04f, 0.11f); // M8 N8

    // Seat
    glBegin(GL_POLYGON);
    glColor3f(255.0f/255.0f, 208.0f/255.0f, 0.0f/255.0f); // Yellow
    glVertex2f(-0.31f, 0.11f); // P8
    glVertex2f( 0.00f, 0.11f); // Q8
    glVertex2f(-0.02f, 0.21f); // R8
    glVertex2f(-0.29f, 0.21f); // S8
    glEnd();
    line(-0.31f, 0.11f,  0.00f, 0.11f); // P8 Q8
    line( 0.00f, 0.11f, -0.02f, 0.21f); // Q8 R8
    line(-0.02f, 0.21f, -0.29f, 0.21f); // R8 S8
    line(-0.29f, 0.21f, -0.31f, 0.11f); // S8 P8

    // Back-support Left Post
    glBegin(GL_POLYGON);
    glColor3f(217.0f/255.0f, 140.0f/255.0f, 0.0f/255.0f); // Orange-brown
    glVertex2f(-0.26f, 0.21f); // T8
    glVertex2f(-0.24f, 0.21f); // U8
    glVertex2f(-0.24f, 0.24f); // V8
    glVertex2f(-0.26f, 0.24f); // W8
    glEnd();
    line(-0.24f, 0.21f, -0.24f, 0.24f); // U8 V8
    line(-0.26f, 0.24f, -0.26f, 0.21f); // W8 T8

    // Back-support Right Post
    glBegin(GL_POLYGON);
    glColor3f(217.0f/255.0f, 140.0f/255.0f, 0.0f/255.0f); // Orange-brown
    glVertex2f(-0.05f, 0.21f); // X8
    glVertex2f(-0.07f, 0.21f); // Y8
    glVertex2f(-0.07f, 0.24f); // Z8
    glVertex2f(-0.05f, 0.24f); // A9
    glEnd();
    line(-0.07f, 0.21f, -0.07f, 0.24f); // Y8 Z8
    line(-0.05f, 0.24f, -0.05f, 0.21f); // A9 X8

    // Back-support Frame (grey)
    glBegin(GL_POLYGON);
    glColor3f(189.0f/255.0f, 189.0f/255.0f, 189.0f/255.0f); // Grey
    glVertex2f(-0.29f, 0.24f); // B9
    glVertex2f(-0.02f, 0.24f); // C9
    glVertex2f(-0.02f, 0.33f); // D9
    glVertex2f(-0.29f, 0.33f); // E9
    glEnd();
    line(-0.29f, 0.24f, -0.02f, 0.24f); // B9 C9
    line(-0.02f, 0.24f, -0.02f, 0.33f); // C9 D9
    line(-0.02f, 0.33f, -0.29f, 0.33f); // D9 E9
    line(-0.29f, 0.33f, -0.29f, 0.24f); // E9 B9

    // Back Slat (golden)
    glBegin(GL_POLYGON);
    glColor3f(255.0f/255.0f, 208.0f/255.0f, 0.0f/255.0f); // Yellow
    glVertex2f(-0.27f, 0.26f); // F9
    glVertex2f(-0.04f, 0.26f); // G9
    glVertex2f(-0.04f, 0.31f); // H9
    glVertex2f(-0.27f, 0.31f); // I9
    glEnd();
    line(-0.27f, 0.26f, -0.04f, 0.26f); // F9 G9
    line(-0.04f, 0.26f, -0.04f, 0.31f); // G9 H9
    line(-0.04f, 0.31f, -0.27f, 0.31f); // H9 I9
    line(-0.27f, 0.31f, -0.27f, 0.26f); // I9 F9
}

// ============================================================
//  LAMP POST
// ============================================================

void lp_box(float a, float b, float c, float d,
            float e, float f, float g, float h)
{
    // Solid black segment
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.0f, 0.0f); // Black
    glVertex2f(a, b); // J9
    glVertex2f(c, d); // K9
    glVertex2f(e, f); // L9
    glVertex2f(g, h); // M9
    glEnd();
}

void lp_light(float cx, float cy)
{
    // Globe bulb (grey day / golden night)
    glBegin(GL_POLYGON);
    for (int i = 0; i < 200; i++)
    {
        if (is_overcast_sky)
            glColor3f(255.0f/255.0f, 223.0f/255.0f,   0.0f/255.0f); // Golden
        else
            glColor3f(224.0f/255.0f, 224.0f/255.0f, 224.0f/255.0f); // Grey
        float pi = 3.1416f;
        float A  = (i * 2 * pi) / 200;
        float r  = 0.02f;
        glVertex2f(r * cosf(A) + cx, r * sinf(A) + cy); // N9 (bulb centre)
    }
    glEnd();
}

void lamp_post()
{
    // ---- Left Lamp Post ----
    // Base plate
    glBegin(GL_POLYGON);
    glColor3f(245.0f/255.0f, 76.0f/255.0f, 0.0f/255.0f); // Orange-red
    glVertex2f(-0.45f, 0.07f); // O9
    glVertex2f(-0.45f, 0.05f); // P9
    glVertex2f(-0.38f, 0.05f); // Q9
    glVertex2f(-0.38f, 0.07f); // R9
    glEnd();
    line(-0.45f, 0.07f, -0.45f, 0.05f); // O9 P9
    line(-0.38f, 0.05f, -0.38f, 0.07f); // Q9 R9
    line(-0.38f, 0.07f, -0.45f, 0.07f); // R9 O9

    lp_box(-0.40f, 0.07f, -0.43f, 0.07f, -0.43f, 0.30f, -0.40f, 0.30f);  // S9 T9 U9 V9  (pole)
    lp_box(-0.36f, 0.30f, -0.36f, 0.31f, -0.47f, 0.31f, -0.47f, 0.30f);  // W9 X9 Y9 Z9  (top bar)
    lp_box(-0.47f, 0.33f, -0.47f, 0.31f, -0.46f, 0.31f, -0.46f, 0.33f);  // A10 Z9 B10 C10
    lp_box(-0.36f, 0.31f, -0.37f, 0.31f, -0.37f, 0.33f, -0.36f, 0.33f);  // X9 D10 E10 F10
    lp_box(-0.41f, 0.31f, -0.42f, 0.31f, -0.42f, 0.36f, -0.41f, 0.36f);  // G10 H10 I10 J10 (centre arm)
    lp_box(-0.46f, 0.34f, -0.46f, 0.33f, -0.49f, 0.33f, -0.49f, 0.34f);  // K10 C10 L10 M10
    lp_box(-0.37f, 0.34f, -0.37f, 0.33f, -0.34f, 0.33f, -0.34f, 0.34f);  // N10 E10 O10 P10
    lp_box(-0.49f, 0.34f, -0.49f, 0.36f, -0.48f, 0.36f, -0.48f, 0.34f);  // M10 Q10 R10 S10
    lp_box(-0.34f, 0.34f, -0.34f, 0.36f, -0.35f, 0.36f, -0.35f, 0.34f);  // P10 T10 U10 V10
    lp_light(-0.485f, 0.375f); // W10  left  bulb
    lp_light(-0.415f, 0.375f); // X10  centre bulb
    lp_light(-0.345f, 0.375f); // Y10  right  bulb

    // ---- Right Lamp Post ----
    // Base plate
    glBegin(GL_POLYGON);
    glColor3f(245.0f/255.0f, 76.0f/255.0f, 0.0f/255.0f); // Orange-red
    glVertex2f(0.07f, 0.05f); // Z10
    glVertex2f(0.14f, 0.05f); // A11
    glVertex2f(0.14f, 0.07f); // B11
    glVertex2f(0.07f, 0.07f); // C11
    glEnd();
    line(0.07f, 0.07f, 0.14f, 0.07f); // C11 B11
    line(0.07f, 0.07f, 0.07f, 0.05f); // C11 Z10
    line(0.14f, 0.07f, 0.14f, 0.05f); // B11 A11

    lp_box(0.12f, 0.07f,  0.09f, 0.07f,  0.09f, 0.30f,  0.12f, 0.30f); // D11 E11 F11 G11 (pole)
    lp_box(0.16f, 0.30f,  0.16f, 0.31f,  0.05f, 0.31f,  0.05f, 0.30f); // H11 I11 J11 K11 (top bar)
    lp_box(0.06f, 0.31f,  0.06f, 0.33f,  0.05f, 0.33f,  0.05f, 0.31f); // L11 M11 N11 K11
    lp_box(0.15f, 0.31f,  0.15f, 0.33f,  0.16f, 0.33f,  0.16f, 0.31f); // O11 P11 Q11 I11
    lp_box(0.11f, 0.31f,  0.10f, 0.31f,  0.10f, 0.36f,  0.11f, 0.36f); // R11 S11 T11 U11 (centre arm)
    lp_box(0.06f, 0.33f,  0.06f, 0.34f,  0.03f, 0.34f,  0.03f, 0.33f); // L11 V11 W11 X11
    lp_box(0.15f, 0.33f,  0.15f, 0.34f,  0.18f, 0.34f,  0.18f, 0.33f); // P11 Y11 Z11 A12
    lp_box(0.03f, 0.34f,  0.03f, 0.36f,  0.04f, 0.36f,  0.04f, 0.34f); // W11 B12 C12 D12
    lp_box(0.18f, 0.34f,  0.18f, 0.36f,  0.17f, 0.36f,  0.17f, 0.34f); // Z11 E12 F12 G12
    lp_light(0.035f, 0.375f); // H12  left  bulb
    lp_light(0.105f, 0.375f); // I12  centre bulb
    lp_light(0.175f, 0.375f); // J12  right  bulb
}

// ============================================================
//  CHRISTMAS TREES
// ============================================================

void c_trunk(float a, float b, float c, float d,
             float e, float f, float g, float h)
{
    glBegin(GL_POLYGON);
    glColor3f(245.0f/255.0f, 76.0f/255.0f, 0.0f/255.0f); // Orange trunk
    glVertex2f(a, b); // K12
    glVertex2f(c, d); // L12
    glVertex2f(e, f); // M12
    glVertex2f(g, h); // N12
    glEnd();
}

void c_leaf(float a, float b, float c, float d, float e, float f)
{
    glBegin(GL_POLYGON);
    glColor3f(58.0f/255.0f, 227.0f/255.0f, 57.0f/255.0f); // Bright green
    glVertex2f(a, b); // O12
    glVertex2f(c, d); // P12
    glVertex2f(e, f); // Q12
    glEnd();
}

void chrismas_tree()
{
    // ---- Right ground-level tree ----
    c_trunk(0.92f, 0.05f,   // R12
            0.94f, 0.05f,   // S12
            0.94f, 0.18f,   // T12
            0.92f, 0.18f);  // U12
    c_leaf(0.87f, 0.18f,  0.99f, 0.18f,  0.93f, 0.23f); // V12 W12 X12
    c_leaf(0.88f, 0.21f,  0.98f, 0.21f,  0.93f, 0.26f); // Y12 Z12 A13
    c_leaf(0.89f, 0.24f,  0.97f, 0.24f,  0.93f, 0.29f); // B13 C13 D13

    line(0.906f, 0.21f,  0.87f, 0.18f);  // E13 V12
    line(0.87f,  0.18f,  0.99f, 0.18f);  // V12 W12
    line(0.99f,  0.18f,  0.954f,0.21f);  // W12 F13
    line(0.906f, 0.21f,  0.88f, 0.21f);  // E13 Y12
    line(0.954f, 0.21f,  0.98f, 0.21f);  // F13 Z12
    line(0.88f,  0.21f,  0.91f, 0.24f);  // Y12 G13
    line(0.98f,  0.21f,  0.95f, 0.24f);  // Z12 H13
    line(0.91f,  0.24f,  0.89f, 0.24f);  // G13 B13
    line(0.95f,  0.24f,  0.97f, 0.24f);  // H13 C13
    line(0.89f,  0.24f,  0.93f, 0.29f);  // B13 D13
    line(0.97f,  0.24f,  0.93f, 0.29f);  // C13 D13
    line(0.92f,  0.18f,  0.92f, 0.05f);  // U12 R12
    line(0.94f,  0.05f,  0.94f, 0.18f);  // S12 T12

    // ---- Left ground-level tree ----
    c_trunk(0.38f, 0.05f,   // I13
            0.36f, 0.05f,   // J13
            0.36f, 0.18f,   // K13
            0.38f, 0.18f);  // L13
    c_leaf(0.31f, 0.18f,  0.43f, 0.18f,  0.37f, 0.23f); // M13 N13 O13
    c_leaf(0.32f, 0.21f,  0.42f, 0.21f,  0.37f, 0.26f); // P13 Q13 R13
    c_leaf(0.33f, 0.24f,  0.41f, 0.24f,  0.37f, 0.29f); // S13 T13 U13

    line(0.31f, 0.18f,  0.43f,  0.18f);  // M13 N13
    line(0.31f, 0.18f,  0.346f, 0.21f);  // M13 V13
    line(0.43f, 0.18f,  0.394f, 0.21f);  // N13 W13
    line(0.346f,0.21f,  0.32f,  0.21f);  // V13 P13
    line(0.394f,0.21f,  0.42f,  0.21f);  // W13 Q13
    line(0.32f, 0.21f,  0.35f,  0.24f);  // P13 X13
    line(0.42f, 0.21f,  0.39f,  0.24f);  // Q13 Y13
    line(0.35f, 0.24f,  0.33f,  0.24f);  // X13 S13
    line(0.39f, 0.24f,  0.41f,  0.24f);  // Y13 T13
    line(0.33f, 0.24f,  0.37f,  0.29f);  // S13 U13
    line(0.41f, 0.24f,  0.37f,  0.29f);  // T13 U13
    line(0.36f, 0.05f,  0.36f,  0.18f);  // J13 K13
    line(0.38f, 0.05f,  0.38f,  0.18f);  // I13 L13

    // ---- Left upper tree (park level) ----
    c_trunk(0.402f, 0.32f,   // Z13
            0.418f, 0.32f,   // A14
            0.418f, 0.41f,   // B14
            0.402f, 0.41f);  // C14
    c_leaf(0.37f, 0.41f,  0.45f, 0.41f,  0.41f, 0.44f);   // D14 E14 F14
    c_leaf(0.38f, 0.43f,  0.44f, 0.43f,  0.41f, 0.46f);   // G14 H14 I14
    c_leaf(0.39f, 0.45f,  0.43f, 0.45f,  0.41f, 0.475f);  // J14 K14 L14

    line(0.37f,     0.41f, 0.45f,     0.41f); // D14 E14
    line(0.37f,     0.41f, 0.397f,    0.43f); // D14 M14
    line(0.45f,     0.41f, 0.4233f,   0.43f); // E14 N14
    line(0.397f,    0.43f, 0.38f,     0.43f); // M14 G14
    line(0.4233f,   0.43f, 0.44f,     0.43f); // N14 H14
    line(0.38f,     0.43f, 0.40f,     0.45f); // G14 O14
    line(0.44f,     0.43f, 0.42f,     0.45f); // H14 P14
    line(0.40f,     0.45f, 0.39f,     0.45f); // O14 J14
    line(0.42f,     0.45f, 0.43f,     0.45f); // P14 K14
    line(0.39f,     0.45f, 0.41f,     0.475f);// J14 L14
    line(0.43f,     0.45f, 0.41f,     0.475f);// K14 L14
    line(0.402f,    0.32f, 0.402f,    0.41f); // Z13 C14
    line(0.418f,    0.32f, 0.418f,    0.41f); // A14 B14

    // ---- Right upper tree (park level) ----
    c_trunk(0.898f, 0.32f,   // Q14
            0.882f, 0.32f,   // R14
            0.882f, 0.41f,   // S14
            0.898f, 0.41f);  // T14
    c_leaf(0.85f, 0.41f,  0.93f, 0.41f,  0.89f, 0.44f);   // U14 V14 W14
    c_leaf(0.86f, 0.43f,  0.92f, 0.43f,  0.89f, 0.46f);   // X14 Y14 Z14
    c_leaf(0.87f, 0.45f,  0.91f, 0.45f,  0.89f, 0.475f);  // A15 B15 C15

    line(0.85f,        0.41f, 0.93f,        0.41f); // U14 V14
    line(0.85f,        0.41f, 0.876666f,    0.43f); // U14 D15
    line(0.93f,        0.41f, 0.903333f,    0.43f); // V14 E15
    line(0.876666f,    0.43f, 0.86f,        0.43f); // D15 X14
    line(0.903333f,    0.43f, 0.92f,        0.43f); // E15 Y14
    line(0.86f,        0.43f, 0.88f,        0.45f); // X14 F15
    line(0.92f,        0.43f, 0.90f,        0.45f); // Y14 G15
    line(0.88f,        0.45f, 0.87f,        0.45f); // F15 A15
    line(0.90f,        0.45f, 0.91f,        0.45f); // G15 B15
    line(0.87f,        0.45f, 0.89f,        0.475f);// A15 C15
    line(0.91f,        0.45f, 0.89f,        0.475f);// B15 C15
    line(0.882f,       0.41f, 0.882f,       0.32f); // S14 R14
    line(0.898f,       0.41f, 0.898f,       0.32f); // T14 Q14
}

// ============================================================
//  PARK ROAD
// ============================================================

void park_road()
{
    glBegin(GL_POLYGON);
    glColor3f(217.0f/255.0f, 193.0f/255.0f, 167.0f/255.0f); // Sandy beige
    glVertex2f(0.88f, 0.00f);  // H15
    glVertex2f(0.42f, 0.00f);  // I15
    glVertex2f(0.55f, 0.52f);  // J15
    glVertex2f(0.75f, 0.52f);  // K15
    glEnd();
}

// ============================================================
//  HEDGE BUSHES
// ============================================================

void hedge(float ax, float ay,   // Left  half-circle centre
           float bx, float by,   // Right half-circle centre
           float cx, float cy,   // Middle full-circle centre
           float r)              // Radius
{
    // Left half-dome
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 200; i++)
    {
        glColor3f(0.0f/255.0f, 145.0f/255.0f, 7.0f/255.0f); // Dark green
        float pi = 3.1416f;
        float A  = (i * pi) / 200;
        glVertex2f(r * cosf(A) + ax, r * sinf(A) + ay); // L15 (left centre)
    }
    glEnd();

    // Right half-dome
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 200; i++)
    {
        glColor3f(0.0f/255.0f, 145.0f/255.0f, 7.0f/255.0f); // Dark green
        float pi = 3.1416f;
        float A  = (i * pi) / 200;
        glVertex2f(r * cosf(A) + bx, r * sinf(A) + by); // M15 (right centre)
    }
    glEnd();

    // Middle full circle
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 200; i++)
    {
        glColor3f(0.0f/255.0f, 145.0f/255.0f, 7.0f/255.0f); // Dark green
        float pi = 3.1416f;
        float A  = (i * 2 * pi) / 200;
        glVertex2f(r * cosf(A) + cx, r * sinf(A) + cy); // N15 (middle centre)
    }
    glEnd();
}

void hedge_tree()
{
    // Park path hedge pairs (left side, right side of path)
    hedge(0.48f, 0.04f,  0.52f, 0.04f,  0.50f, 0.06f,  0.020f); // O15 P15 Q15
    hedge(0.82f, 0.04f,  0.78f, 0.04f,  0.80f, 0.06f,  0.020f); // R15 S15 T15
    hedge(0.50f, 0.14f,  0.54f, 0.14f,  0.52f, 0.16f,  0.020f); // U15 V15 W15
    hedge(0.80f, 0.14f,  0.76f, 0.14f,  0.78f, 0.16f,  0.020f); // X15 Y15 Z15
    hedge(0.52f, 0.24f,  0.55f, 0.24f,  0.535f,0.255f, 0.015f); // A16 B16 C16
    hedge(0.78f, 0.24f,  0.75f, 0.24f,  0.765f,0.255f, 0.015f); // D16 E16 F16
    hedge(0.55f, 0.34f,  0.58f, 0.34f,  0.565f,0.355f, 0.015f); // G16 H16 I16
    hedge(0.72f, 0.34f,  0.75f, 0.34f,  0.735f,0.355f, 0.015f); // J16 K16 L16
    hedge(0.575f,0.44f,  0.595f,0.44f,  0.585f,0.450f, 0.010f); // M16 N16 O16
    hedge(0.725f,0.44f,  0.705f,0.44f,  0.715f,0.450f, 0.010f); // P16 Q16 R16
}

// ============================================================
//  BOUNDARY LINES
// ============================================================

void border()
{
    line(-1,  0.52f, 1,  0.52f); // S16 T16  park top   (A-B)
    line(-1,  0.00f, 1,  0.00f); // U16 V16  park bottom / railing top   (D-C / E-G)
    line(-1, -0.10f, 1, -0.10f); // W16 X16  railing bottom / lake top   (F-H / I-K)
    line(-1, -0.60f, 1, -0.60f); // Y16 Z16  lake bottom   (J-L)
}

// ============================================================
//  KEYBOARD & MOUSE
// ============================================================

void handleKeypress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'b': // Boat start
            boat_speed = 0.002f;
            break;
        case 'n': // Boat stop
            boat_speed = 0.0f;
            break;

        case 'h': // Ferris wheel faster
            nagordola_speed += 0.5f;
            break;
        case 'l': // Ferris wheel slower
            nagordola_speed -= 0.5f;
            if (nagordola_speed < 0.0f) nagordola_speed = 0.0f;
            break;

        case '1': // Overcast — rain on, lights golden
            is_overcast_sky = true;
            break;
        case '2': // Clear — rain off, lights grey
            is_overcast_sky = false;
            dropCount = 0;
            break;
    }
    glutPostRedisplay();
}

void handleMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
        nagordola_speed = 1.0f; // Start ferris wheel
    if (button == GLUT_RIGHT_BUTTON)
        nagordola_speed = 0.0f; // Stop ferris wheel
}

// ============================================================
//  DISPLAY
// ============================================================

void LakeSidePark_Display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Layer 1 — Lake (bottom)
    lake();
    boat();

    // Layer 2 — Railing (between park and lake)


    // Layer 3 — Park (top)
    field();
    railing();
    nagordola();
    bench();
    lamp_post();
    chrismas_tree();
    park_road();
    hedge_tree();

    // Rain overlay (overcast only)
    if (is_overcast_sky)
        drawDrops();

    // Scene boundary lines
    border();

    glutSwapBuffers();
}

// ============================================================
//  MAIN
// ============================================================

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(900, 800);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Lake Side Park");
    glutDisplayFunc(LakeSidePark_Display);
    initGL();
    glutTimerFunc(25, boat_animation,    0);
    glutTimerFunc(20, nagordola_rotation,0);
    glutTimerFunc(10, rain_animation,    0);
    glutKeyboardFunc(handleKeypress);
    glutMouseFunc(handleMouse);
    glutMainLoop();
    return 0;
}
