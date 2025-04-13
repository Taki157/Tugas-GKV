#include <GL/glut.h>
#include <GL/glu.h>
#include <cmath>

GLUquadric* quad;

float cameraAngleX = 20.0f;
float cameraAngleY = 30.0f;
float cameraDistance = 6.0f;

int lastMouseX, lastMouseY;
bool isDragging = false;

float toRadians(float deg) {
    return deg * 3.1415926f / 180.0f;
}

void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    quad = gluNewQuadric();
}

void drawAxes(float length) {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    
    // Sumbu X - Merah
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);
    
    // Sumbu Y - Hijau
    glColor3f(0.0f, 1.0f, 0.0f); 
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);
    
    // Sumbu Z - Biru
    glColor3f(0.0f, 0.0f, 1.0f); 
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();
}

void drawBoxGrid(float size, float step) {
    glColor3f(0.7f, 0.7f, 0.7f); // Warna garis grid (abu-abu muda)
    glLineWidth(1.0f);
    glBegin(GL_LINES);

    // Gambar garis sejajar sumbu X (horizontal)
    for (float z = -size; z <= size; z += step) {
        glVertex3f(-size, 0, z);
        glVertex3f(size, 0, z);
    }

    // Gambar garis sejajar sumbu Z (vertikal)
    for (float x = -size; x <= size; x += step) {
        glVertex3f(x, 0, -size);
        glVertex3f(x, 0, size);
    }

    // Gambar garis sumbu Y (vertikal)
    for (float y = -size; y <= size; y += step) {
        glVertex3f(-size, y, 0);
        glVertex3f(size, y, 0);
    }
    
    glEnd();
}


void drawTabung(float radius, float height, int slices, int stacks) {
    glPushMatrix();
        glTranslatef(0.0f, -height / 2.0f, 0.0f);

        // Alas bawah
        glPushMatrix();
            glRotatef(90, 1, 0, 0);
            gluDisk(quad, 0.0, radius, slices, 1);
        glPopMatrix();

        // Badan tabung
        glPushMatrix();
            glRotatef(-90, 1, 0, 0);
            gluCylinder(quad, radius, radius, height, slices, stacks);
        glPopMatrix();

        // Tutup atas
        glPushMatrix();
            glRotatef(90, 1, 0, 0);
            glTranslatef(0.0f, height, 0.0f);
            gluDisk(quad, 0.0, radius, slices, 1);
        glPopMatrix();
    glPopMatrix();
}

void drawHalfSphere(double radius, int slices, int stacks) {
    // Aktifkan clip plane untuk memotong bola
    double plane_eq[] = { 0.0, -1.0, 0.0, 0.0 }; // y >= 0
    glClipPlane(GL_CLIP_PLANE0, plane_eq);
    glEnable(GL_CLIP_PLANE0);

    gluSphere(quad, radius, slices, stacks);

    glDisable(GL_CLIP_PLANE0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Hitung posisi kamera dari sudut dan jarak
    float camX = cameraDistance * sinf(toRadians(cameraAngleY)) * cosf(toRadians(cameraAngleX));
    float camY = cameraDistance * sinf(toRadians(cameraAngleX));
    float camZ = cameraDistance * cosf(toRadians(cameraAngleY)) * cosf(toRadians(cameraAngleX));

    gluLookAt(camX, camY, camZ,   // Posisi kamera
                0.0, 0.0, 0.0,      // Fokus ke pusat tabung
                0.0, 1.0, 0.0);     // Arah atas (Y axis)

    // Gambar sumbu koordinat
    drawAxes(5.0f);

    // Gambar grid
    glPushMatrix();
        glTranslatef(0.0f, -0.01f, 0.0f); // Geser sedikit ke bawah agar tidak tertutup tabung
        drawBoxGrid(5.0f, 1.0f);
    glPopMatrix();

    // Gambar objek
    glColor3f(0.2f, 0.7f, 0.8f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Gambar garis
    glLineWidth(2.0f); // Lebar garis
    drawTabung(1.0f, 2.0f, 32, 32);
    drawHalfSphere(1.0f, 32, 32); // Gambar kubah di atas tabung
    
    glutSwapBuffers();
}


void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == 3) cameraDistance -= 0.3f; // scroll up
    else if (button == 4) cameraDistance += 0.3f; // scroll down

    if (cameraDistance < 2.0f) cameraDistance = 2.0f;
    if (cameraDistance > 20.0f) cameraDistance = 20.0f;

    if (state == GLUT_DOWN) {
        isDragging = true;
        lastMouseX = x;
        lastMouseY = y;
    } else {
        isDragging = false;
    }

    glutPostRedisplay();
}

void motion(int x, int y) {
    if (isDragging) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;

        cameraAngleY += dx * 0.5f;  // Rotasi horizontal (kanan-kiri)
        cameraAngleX += dy * 0.5f;  // Rotasi vertikal (atas-bawah)

        // Batasi sudut atas-bawah supaya tidak kebalik
        if (cameraAngleX > 89.0f) cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f) cameraAngleX = -89.0f;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tabung dengan Kubah di Atas");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
