#include <GL/glut.h>
#include <GL/glu.h>

GLUquadric* quad;

float angleX = 20.0f, angleY = -30.0f;
int lastMouseX, lastMouseY;
bool isDragging = false;

void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    quad = gluNewQuadric();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(2.0, 2.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    glColor3f(0.2f, 0.7f, 0.8f); // Warna tabung

    glPushMatrix();
        glTranslatef(0.0f, -1.0f, 0.0f); // Posisikan ke bawah sedikit agar tabung utuh terlihat

        // Alas (bawah)
        glPushMatrix();
            glRotatef(180, 1, 0, 0); // Putar disk agar menghadap ke atas
            gluDisk(quad, 0.0, 1.0, 32, 1);
        glPopMatrix();

        // Tabung
        gluCylinder(quad, 1.0, 1.0, 2.0, 32, 32);

        // Tutup (atas)
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 2.0f); // Geser ke atas setinggi tabung
            gluDisk(quad, 0.0, 1.0, 32, 1);
        glPopMatrix();

    glPopMatrix();

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
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            isDragging = false;
        }
    }
}

void motion(int x, int y) {
    if (isDragging) {
        angleY += (x - lastMouseX);
        angleX += (y - lastMouseY);
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tabung Lengkap (Dengan Tutup)");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
