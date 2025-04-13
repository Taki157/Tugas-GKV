#include <GL/glut.h>
#include <GL/glu.h>

void drawTabung(float radius, float height) {
    GLUquadric *quad = gluNewQuadric();

    // Gambar sisi tabung
    glColor3f(0.5f, 0.8f, 1.0f);
    gluCylinder(quad, radius, radius, height, 32, 32);

    // Tutup bawah
    glPushMatrix();
    glRotatef(180, 1, 0, 0);         // Balik ke bawah
    gluDisk(quad, 0, radius, 32, 1); // Tutup bawah
    glPopMatrix();

    // Tutup atas
    glPushMatrix();
    glTranslatef(0, 0, height);      // Pindah ke atas
    gluDisk(quad, 0, radius, 32, 1); // Tutup atas
    glPopMatrix();

    gluDeleteQuadric(quad);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -5.0f);
    glRotatef(45, 1.0f, 1.0f, 0.0f);

    drawTabung(1.0f, 2.0f);

    glutSwapBuffers();
}

void init() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Tabung dengan Tutup - OpenGL");

    init();
    glutDisplayFunc(display);

    glutMainLoop();
    return 0;
}
