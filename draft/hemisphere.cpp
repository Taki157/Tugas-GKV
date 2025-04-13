#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Set background color
    glEnable(GL_DEPTH_TEST); // Enable depth testing for 3D effects
    glEnable(GL_LIGHTING);    // Enable lighting
    glEnable(GL_LIGHT0);      // Enable light source 0

    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0}; // Position of the light
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Position the camera
    gluLookAt(0.0, 0.0, 5.0,  // Camera position
              0.0, 0.0, 0.0,  // Target position
              0.0, 1.0, 0.0); // Up vector
    
    // Draw the hemisphere (setengah bola)
    glPushMatrix();
    
    // Rotate for better view
    glRotatef(45.0, 1.0, 0.0, 0.0); // Rotate along the X-axis
    
    // Set color for the hemisphere
    glColor3f(1.0, 0.0, 0.0); // Red color

    // Draw the upper half of the sphere (hemisphere)
    glutSolidSphere(30.0, 50, 50); // Sphere with radius 1, 50 slices and stacks
    
    glPopMatrix();
    
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Enable depth buffer
    glutInitWindowSize(500, 500);
    glutCreateWindow("Half Sphere (Hemisphere)");

    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
