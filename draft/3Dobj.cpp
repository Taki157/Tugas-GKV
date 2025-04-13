#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>

// Posisi kamera
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 5.0f;

// Target kamera
float targetX = 0.0f;
float targetY = 0.0f;
float targetZ = 0.0f;

// Ukuran window
const int windowWidth = 800;
const int windowHeight = 600;

// Warna untuk kubus
void setColor(int face) {
    switch (face) {
        case 0: glColor3f(1.0f, 0.0f, 0.0f); break; // Merah
        case 1: glColor3f(0.0f, 1.0f, 0.0f); break; // Hijau
        case 2: glColor3f(0.0f, 0.0f, 1.0f); break; // Biru
        case 3: glColor3f(1.0f, 1.0f, 0.0f); break; // Kuning
        case 4: glColor3f(1.0f, 0.0f, 1.0f); break; // Magenta
        case 5: glColor3f(0.0f, 1.0f, 1.0f); break; // Cyan
    }
}

void drawKubah() {
    // Warna kubah
    glColor3f(0.5, 0.5, 1.0);

    // Buat quadric object
    GLUquadric *quad = gluNewQuadric();

    glPushMatrix();
        // Putar agar atas menghadap ke atas
        glRotatef(-90, 1, 0, 0);

        // Hanya render setengah bola dengan scaling di Y
        glScalef(1.0, 0.5, 1.0);  // Scaling sumbu Y -> jadi setengah
        gluSphere(quad, 1.0, 32, 32);  // Radius, slices, stacks
    glPopMatrix();

    gluDeleteQuadric(quad);
}

// Fungsi untuk menggambar kubus
void drawCube() {
    // Ukuran kubus
    float size = 1.0f;
    
    // DEPAN
    setColor(0);
    glBegin(GL_QUADS);
    glVertex3f(-size, -size, size);
    glVertex3f(size, -size, size);
    glVertex3f(size, size, size);
    glVertex3f(-size, size, size);
    glEnd();

    // BELAKANG
    setColor(1);
    glBegin(GL_QUADS);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, -size, -size);
    glEnd();

    // ATAS
    setColor(2);
    glBegin(GL_QUADS);
    glVertex3f(-size, size, -size);
    glVertex3f(-size, size, size);
    glVertex3f(size, size, size);
    glVertex3f(size, size, -size);
    glEnd();

    // BAWAH
    setColor(3);
    glBegin(GL_QUADS);
    glVertex3f(-size, -size, -size);
    glVertex3f(size, -size, -size);
    glVertex3f(size, -size, size);
    glVertex3f(-size, -size, size);
    glEnd();

    // KANAN
    setColor(4);
    glBegin(GL_QUADS);
    glVertex3f(size, -size, -size);
    glVertex3f(size, size, -size);
    glVertex3f(size, size, size);
    glVertex3f(size, -size, size);
    glEnd();

    // KIRI
    setColor(5);
    glBegin(GL_QUADS);
    glVertex3f(-size, -size, -size);
    glVertex3f(-size, -size, size);
    glVertex3f(-size, size, size);
    glVertex3f(-size, size, -size);
    glEnd();
}

// Fungsi untuk menangani input tombol keyboard
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // ESC
            exit(0);
            break;
    }
    glutPostRedisplay();
}

// Fungsi untuk menangani input tombol special keyboard (arrow keys)
void specialKeys(int key, int x, int y) {
    float moveSpeed = 0.1f;
    
    switch (key) {
        case GLUT_KEY_UP:
            cameraY += moveSpeed; // Gerak ke atas
            break;
        case GLUT_KEY_DOWN:
            cameraY -= moveSpeed; // Gerak ke bawah
            break;
        case GLUT_KEY_LEFT:
            cameraX -= moveSpeed; // Gerak ke kiri
            break;
        case GLUT_KEY_RIGHT:
            cameraX += moveSpeed; // Gerak ke kanan
            break;
    }
    
    glutPostRedisplay();
}

// Fungsi untuk menampilkan
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Mengatur posisi kamera
    gluLookAt(
        cameraX, cameraY, cameraZ,  // Posisi kamera
        targetX, targetY, targetZ,  // Target kamera (pusat scene)
        0.0f, 1.0f, 0.0f           // Vektor up
    );
    
    // Menggambar kubus
    drawCube();

    // Menggambar kubah
    drawKubah();
    
    glutSwapBuffers();
}

// Fungsi untuk mengatur ulang ukuran window
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    // Inisialisasi GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Kubus 3D dengan Kamera yang Dapat Digerakkan");
    
    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Set background color
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    
    // Enter the GLUT main loop
    glutMainLoop();
    
    return 0;
}