#include <GL/glut.h>
#include <GL/glu.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <functional>
using namespace std;

GLUquadric* quad;

float cameraAngleX = 20.0f;
float cameraAngleY = 30.0f;
float cameraDistance = 6.0f;

int lastMouseX, lastMouseY;
bool isDragging = false;

float toRadians(float deg) {
    return deg * 3.1415926f / 180.0f;
}

int viewMode = 'o'; // Default view mode

void setProjection() {
    // Pastikan viewport sesuai dengan ukuran window
    glViewport(0, 0, 800, 800);  // windowWidth dan windowHeight harus diatur sesuai ukuran window

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Tentukan proyeksi berdasarkan mode tampilan
    if (viewMode == 'o') {
        // Mode orthographic
        glOrtho(-5, 5, -5, 5, -10, 10);
    } else if (viewMode == 'p') {
        // Mode perspective
        gluPerspective(60.0, 1.0, 1.0, 100.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL); // izinkan pewarnaan pakai glColor
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE); // glColor atur dua properti material

    
    // Lighting
    glEnable(GL_LIGHTING);      // Aktifkan lighting
    glEnable(GL_LIGHT0);        // Gunakan satu sumber cahaya (LIGHT0)
    glEnable(GL_NORMALIZE);     // Normalkan vektor normal (wajib untuk lighting)
    glShadeModel(GL_SMOOTH);    // Smooth shading

    // Set posisi cahaya (arah dari mana datangnya cahaya)
    GLfloat light_pos[] = { 3.0f, 5.0f, 5.0f, 1.0f }; // x, y, z, w
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    // Warna cahaya
    GLfloat ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Material objek (biar bereaksi terhadap cahaya)
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

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

    // Grid XZ
    // Gambar garis sejajar sumbu X (horizontal)
    for (float z = -size; z <= size; z += step) {
        glVertex3f(-size, 0, z);
        glVertex3f(size, 0, z);
    }
    // Gambar garis sejajar sumbu Z (depth)
    for (float x = -size; x <= size; x += step) {
        glVertex3f(x, 0, -size);
        glVertex3f(x, 0, size);
    }

    // Grid XY
    // Gambar garis sejajar Y (vertikal)
    for (float y = -size; y <= size; y += step) {
        glVertex3f(-size, y, 0);
        glVertex3f(size, y, 0);
    }
    // Gambar garis sejajar sumbu X (horizontal)
    for (float x = -size; x <= size; x += step) {
        glVertex3f(x, -size, 0);
        glVertex3f(x, size, 0);
    }

    // Grid YZ
    // Gambar garis sejajar sumbu Y (depth)
    for (float z = -size; z <= size; z += step) {
        glVertex3f(0, -size, z);
        glVertex3f(0, size, z);
    }
    // Gambar garis sejajar sumbu Z (horizontal)
    for (float y = -size; y <= size; y += step) {
        glVertex3f(0, y, -size);
        glVertex3f(0, y, size);
    }
    
    glEnd();
}

void drawWithColor(float r, float g, float b, function<void()> drawFunc) {
    // Simpan warna saat ini
    GLfloat prevColor[4];
    glGetFloatv(GL_CURRENT_COLOR, prevColor);

    // Atur warna baru
    glColor3f(r, g, b);

    // Gambar objek yang diberikan
    drawFunc();

    // Kembalikan warna sebelumnya
    glColor4fv(prevColor);
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
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

        cameraAngleY -= dx * 0.5f;  // Rotasi horizontal (kanan-kiri)
        cameraAngleX += dy * 0.5f;  // Rotasi vertikal (atas-bawah)

        // Batasi sudut atas-bawah supaya tidak kebalik
        if (cameraAngleX > 90.0f) cameraAngleX = 90.0f;
        if (cameraAngleX < -90.0f) cameraAngleX = -90.0f;

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'o' || key == 'p') {
        viewMode = key;
        glutPostRedisplay();
    }
}

// -----------------------------------------------------------------
void drawBody(float radius, float height, int slices, int stacks) {
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f); // Pindahkan ke atas agar tabung berada di atas grid
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotasi agar alas menghadap ke atas

        // Alas Bawah
        glPushMatrix();
            gluDisk(quad, 0.0f, radius, slices, 1); // Gambar alas bawah
        glPopMatrix();

        // Badan Tabung
        glPushMatrix();
            gluCylinder(quad, radius, radius, height, slices, stacks); // Gambar badan tabung
        glPopMatrix();

        // Tutup Atas
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, height); // Pindah ke atas
            gluDisk(quad, 0.0f, radius, slices, 1); // Gambar tutup atas
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

void drawHead(float radius, int slices, int stacks) {
    glPushMatrix();
        // Dasar Kepala
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f); // Pindahkan ke atas agar tabung berada di atas grid
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f); // Rotasi agar alas menghadap ke atas
            drawBody(radius, 0.6, 32, 4); // Gambar badan tabung
        glPopMatrix();

        // Gambar setengah bola untuk kepala
        glPushMatrix();
            glTranslatef(0.0f, -0.6f, 0.0f); // Pindah ke atas agar setengah bola berada di atas tabung
            drawHalfSphere(radius, slices, stacks); // Gambar setengah bola
        glPopMatrix();

        // Mata
        glColor3f(1.0f, 1.0f, 1.0f); // Warna putih untuk mata
        // Mata Kiri
        glPushMatrix();
            glTranslatef(0.8f, -0.5f, 0.4f); // Pindah ke posisi mata kanan
            gluSphere(quad, 0.3, 24, 32); // Gambar bola penuh untuk mata
        glPopMatrix();

        // Mata Kanan
        glPushMatrix();
            glTranslatef(0.8f, -0.5f, -0.4f); // Pindah ke posisi mata kiri
            gluSphere(quad, 0.3, 24, 32); // Gambar bola penuh untuk mata
        glPopMatrix();

        // Pupil
        glColor3f(0.0f, 0.0f, 0.0f); // Warna hitam untuk pupil
        // Pupil Kiri
        glPushMatrix();
            glTranslatef(1.075f, -0.5f, 0.4f); // Pindah ke posisi pupil kanan
            glScalef(0.5f, 1.0f, 1.0f); // Skala untuk membuat pupil lebih pipih
            gluSphere(quad, 0.1, 24, 32); // Gambar bola penuh untuk pupil
        glPopMatrix();

        // Pupil Kanan
        glPushMatrix();
            glTranslatef(1.075f, -0.5f, -0.4f); // Pindah ke posisi pupil kanan
            glScalef(0.5f, 1.0f, 1.0f); // Skala untuk membuat pupil lebih pipih
            gluSphere(quad, 0.1, 24, 32); // Gambar bola penuh untuk pupil
        glPopMatrix();
        
    glPopMatrix();
}

void drawHand(float z_pos, float radius, float length, int slices, int stacks) {
    glPushMatrix();
        glTranslatef(0.0f, 0.1f, z_pos); // Pindahkan ke atas agar tabung berada di atas grid
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // Rotasi agar alas menghadap ke atas

        // Badan Tabung Lengan
        glPushMatrix();
            gluCylinder(quad, radius, 0.02, length, slices, stacks); // Gambar badan tabung
        glPopMatrix();

        // Bahu
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f); // Pindah ke bahu
            glRotated(90.0f, 1.0f, 0.0f, 0.0f); // Rotasi agar alas menghadap ke atas
            drawHalfSphere(radius, slices, stacks*2); // Gambar bahu
        glPopMatrix();

        // Ujung Tangan
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, length); // Pindah ke atas
            glRotated(-90.0f, 1.0f, 0.0f, 0.0f); // Rotasi agar searah dengan lengan
            drawHalfSphere(0.02, slices, stacks); // Gambar ujung tangan
        glPopMatrix();

    glPopMatrix();
}

void drawLeg(float z_pos, float radius, float length, int slices, int stacks) {
    glPushMatrix();
        glTranslatef(0.0f, 0.4f, z_pos); // Pindahkan ke atas agar Kaki berada di bawah body
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotasi agar alas menghadap ke atas

        // Badan Tabung Kaki
        glPushMatrix();
            gluCylinder(quad, radius, 0.05, length, slices, stacks); // Gambar badan tabung
        glPopMatrix();

        // Pangkal Kaki
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.0f); // Pindah ke kaki
            glRotated(90.0f, 1.0f, 0.0f, 0.0f); // Rotasi agar alas menghadap ke atas
            drawHalfSphere(radius, slices, stacks*2); // Gambar bahu
        glPopMatrix();

        // Ujung Kaki
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, length); // Pindah ke atas
            // glRotated(0.0f, 1.0f, 0.0f, 0.0f); // Rotasi agar searah dengan kaki
            gluDisk(quad, 0.0f, 0.05, slices, stacks); // Gambar ujung kaki
        glPopMatrix();

glPopMatrix();
}

// -----------------------------------------------------------------

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setProjection();

    // Hitung posisi kamera dari sudut dan jarak
    float camX = cameraDistance * sinf(toRadians(cameraAngleY)) * cosf(toRadians(cameraAngleX));
    float camY = cameraDistance * sinf(toRadians(cameraAngleX));
    float camZ = cameraDistance * cosf(toRadians(cameraAngleY)) * cosf(toRadians(cameraAngleX));

    gluLookAt(camX, camY, camZ,   // Posisi kamera
                0.0, 0.0, 0.0,      // Fokus ke pusat tabung
                0.0, 1.0, 0.0);     // Arah atas (Y axis)

    // Gambar sumbu koordinat
    drawAxes(100.0f);

    // Gambar grid
    drawBoxGrid(100.0f, 1.0f);
    
    // Gambar objek
    glColor3f(0.2f, 0.7f, 0.8f);  
        glLineWidth(2.0f); // Lebar garis
        glRotatef(180.0f, 1.0f, 0.0f, 0.0f); // Rotasi untuk orientasi yang benar
        drawWithColor(1.0f, 0.7569f, 0.0275f, [](){drawBody(1.0f, 0.4f, 32, 4);}); // Gambar body
        glPushMatrix();
            glTranslatef(-1.0f, 0.0f, 0.0f);
            glRotatef(-25.0f, 0.0f, 0.0f, 1.0f);
            glTranslatef(1.0f, 0.0f, 0.0f);
            drawWithColor(1.0f, 0.7569f, 0.0275f, [](){drawHead(1.0f, 32, 32);}); // Gambar head
        glPopMatrix();
        drawWithColor(1.0f, 0.7569f, 0.0275f, [](){drawHand(1.05f, 0.15f, 1.2f, 32, 4), /*Gambar left hand*/
            drawHand(-1.05f, 0.15f, 1.2f, 32, 4), /* Gambar right hand */
            drawLeg(0.5f, 0.25f, 0.4f, 32, 4), /* Gambar left leg */
            drawLeg(-0.5f, 0.25f, 0.4f, 32, 4);}); /* Gambar right leg */

    glutSwapBuffers();
}

// -----------------------------------------------------------------

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
    glutInitWindowSize(800, 800);
    glutCreateWindow("Mini R.E.P.O");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}