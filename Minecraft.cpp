#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>

// Konstanta untuk ukuran dunia
#define WORLD_SIZE 100
#define WORLD_HEIGHT 10

// Tipe blok
enum BlockType {
    AIR,
    GRASS,
    DIRT,
    STONE,
    WATER,
    WOOD,
    LEAVES,
    SAND,
    BLOCK_TYPES_COUNT
};

// Struktur untuk posisi
struct Position {
    float x, y, z;
};

// Struktur untuk blok
struct Block {
    BlockType type;
    bool visible;
};

// Variabel untuk dunia
Block world[WORLD_SIZE][WORLD_HEIGHT][WORLD_SIZE];

// Posisi dan orientasi kamera
Position camera = {WORLD_SIZE / 2.0f, WORLD_HEIGHT, WORLD_SIZE / 2.0f};
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
const float CAMERA_SPEED = 0.1f;
const float MOUSE_SENSITIVITY = 0.2f;

// Variabel untuk mouse
int lastMouseX = 0, lastMouseY = 0;
bool firstMouse = true;
bool mouseActive = false;

// Variabel untuk pergerakan
bool keyStates[256] = {false};

// Block yang sedang dipilih
BlockType selectedBlockType = GRASS;

// Fungsi untuk mengecek apakah blok terlihat dari luar
bool isBlockVisible(int x, int y, int z) {
    if (x <= 0 || x >= WORLD_SIZE - 1 ||
        y <= 0 || y >= WORLD_HEIGHT - 1 ||
        z <= 0 || z >= WORLD_SIZE - 1)
        return true;
    
    return world[x-1][y][z].type == AIR ||
           world[x+1][y][z].type == AIR ||
           world[x][y-1][z].type == AIR ||
           world[x][y+1][z].type == AIR ||
           world[x][y][z-1].type == AIR ||
           world[x][y][z+1].type == AIR;
}

// Generate dunia prosedural sederhana
void generateWorld() {
    srand(time(NULL));
    
    // Inisialisasi dunia dengan udara
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                world[x][y][z].type = AIR;
                world[x][y][z].visible = false;
            }
        }
    }
    
    // Generate terrain dasar
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int z = 0; z < WORLD_SIZE; z++) {
            // Tinggi terrain sederhana
            int height = 2 + rand() % 2; // 2-3 blocks height
            
            // Set blok tanah
            for (int y = 0; y < height; y++) {
                world[x][y][z].type = DIRT;
            }
            
            // Set block paling atas sebagai rumput
            world[x][height-1][z].type = GRASS;
            
            // Tambahkan air di bagian bawah
            for (int y = 0; y < 1; y++) {
                if (rand() % 10 == 0) { // 10% kemungkinan air
                    world[x][y][z].type = WATER;
                }
            }
            
            // Tambahkan pohon acak
            if (rand() % 50 == 0 && height > 0) { // 2% kemungkinan pohon
                int treeHeight = 3 + rand() % 2; // 3-4 blok tinggi
                
                // Batang pohon
                for (int y = height; y < height + treeHeight; y++) {
                    if (y < WORLD_HEIGHT) {
                        world[x][y][z].type = WOOD;
                    }
                }
                
                // Dedaunan
                if (height + treeHeight < WORLD_HEIGHT) {
                    for (int dx = -1; dx <= 1; dx++) {
                        for (int dy = 0; dy <= 1; dy++) {
                            for (int dz = -1; dz <= 1; dz++) {
                                int lx = x + dx;
                                int ly = height + treeHeight + dy;
                                int lz = z + dz;
                                
                                if (lx >= 0 && lx < WORLD_SIZE &&
                                    ly >= 0 && ly < WORLD_HEIGHT &&
                                    lz >= 0 && lz < WORLD_SIZE) {
                                    world[lx][ly][lz].type = LEAVES;
                                }
                            }
                        }
                    }
                }
            }
            
            // Tambahkan batu acak
            for (int y = 0; y < height - 1; y++) {
                if (rand() % 3 == 0) { // 33% kemungkinan batu di bawah permukaan
                    world[x][y][z].type = STONE;
                }
            }
        }
    }
    
    // Tentukan blok mana yang terlihat
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                if (world[x][y][z].type != AIR) {
                    world[x][y][z].visible = isBlockVisible(x, y, z);
                }
            }
        }
    }
}

// Fungsi untuk mendapatkan warna blok
void getBlockColor(BlockType type, float &r, float &g, float &b) {
    switch (type) {
        case GRASS:
            r = 0.0f; g = 0.8f; b = 0.0f;
            break;
        case DIRT:
            r = 0.6f; g = 0.3f; b = 0.0f;
            break;
        case STONE:
            r = 0.5f; g = 0.5f; b = 0.5f;
            break;
        case WATER:
            r = 0.0f; g = 0.0f; b = 0.8f;
            break;
        case WOOD:
            r = 0.5f; g = 0.3f; b = 0.1f;
            break;
        case LEAVES:
            r = 0.0f; g = 0.5f; b = 0.0f;
            break;
        case SAND:
            r = 0.9f; g = 0.9f; b = 0.2f;
            break;
        default:
            r = 1.0f; g = 1.0f; b = 1.0f;
    }
}

// Fungsi untuk menggambar kubus
void drawCube(float x, float y, float z, BlockType type) {
    float r, g, b;
    getBlockColor(type, r, g, b);
    
    glColor3f(r, g, b);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidCube(1.0);
    glPopMatrix();
}

// Ray casting untuk memilih blok
bool rayCast(float startX, float startY, float startZ, 
             float dirX, float dirY, float dirZ,
             int &hitX, int &hitY, int &hitZ,
             int &prevX, int &prevY, int &prevZ) {
    
    float maxDistance = 5.0f; // Maksimum jarak untuk ray cast
    float step = 0.1f;        // Ukuran langkah
    
    for (float t = 0; t < maxDistance; t += step) {
        float x = startX + dirX * t;
        float y = startY + dirY * t;
        float z = startZ + dirZ * t;
        
        int blockX = (int)floorf(x);
        int blockY = (int)floorf(y);
        int blockZ = (int)floorf(z);
        
        prevX = (int)floorf(startX + dirX * (t - step));
        prevY = (int)floorf(startY + dirY * (t - step));
        prevZ = (int)floorf(startZ + dirZ * (t - step));
        
        if (blockX >= 0 && blockX < WORLD_SIZE &&
            blockY >= 0 && blockY < WORLD_HEIGHT &&
            blockZ >= 0 && blockZ < WORLD_SIZE) {
            
            if (world[blockX][blockY][blockZ].type != AIR) {
                hitX = blockX;
                hitY = blockY;
                hitZ = blockZ;
                return true;
            }
        }
    }
    
    return false;
}

// Fungsi untuk menggambar dunia
void drawWorld() {
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int z = 0; z < WORLD_SIZE; z++) {
                if (world[x][y][z].type != AIR && world[x][y][z].visible) {
                    drawCube(x + 0.5f, y + 0.5f, z + 0.5f, world[x][y][z].type);
                }
            }
        }
    }
}

// Fungsi callback untuk menggambar
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set kamera
    float lookX = camera.x + cos(cameraYaw) * cos(cameraPitch);
    float lookY = camera.y + sin(cameraPitch);
    float lookZ = camera.z + sin(cameraYaw) * cos(cameraPitch);
    
    gluLookAt(camera.x, camera.y, camera.z, 
              lookX, lookY, lookZ, 
              0.0f, 1.0f, 0.0f);
    
    // Gambar dunia
    drawWorld();
    
    // Gambar crosshair di tengah
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
    glVertex2i(centerX - 10, centerY);
    glVertex2i(centerX + 10, centerY);
    glVertex2i(centerX, centerY - 10);
    glVertex2i(centerX, centerY + 10);
    glEnd();
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glutSwapBuffers();
}

// Fungsi untuk mengatur proyeksi
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, (float)width / (float)height, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}

// Fungsi untuk merespon tombol keyboard yang ditekan
void keyDown(unsigned char key, int x, int y) {
    keyStates[key] = true;
    
    // Tekan R untuk meregenerasi dunia
    if (key == 'r' || key == 'R') {
        generateWorld();
    }
    
    // Tekan 1-7 untuk memilih blok
    if (key >= '1' && key <= '7') {
        selectedBlockType = (BlockType)(key - '0');
    }
    
    // Tekan ESC untuk keluar
    if (key == 27) {
        exit(0);
    }
}

// Fungsi untuk merespon tombol keyboard yang dilepas
void keyUp(unsigned char key, int x, int y) {
    keyStates[key] = false;
}

// Fungsi untuk handle pergerakan
void movement() {
    float lookX = cos(cameraYaw) * cos(cameraPitch);
    float lookY = sin(cameraPitch);
    float lookZ = sin(cameraYaw) * cos(cameraPitch);
    
    float strafeX = cos(cameraYaw - 3.14159f / 2);
    float strafeZ = sin(cameraYaw - 3.14159f / 2);
    
    // Maju (W)
    if (keyStates['w'] || keyStates['W']) {
        camera.x += lookX * CAMERA_SPEED;
        camera.y += lookY * CAMERA_SPEED;
        camera.z += lookZ * CAMERA_SPEED;
    }
    
    // Mundur (S)
    if (keyStates['s'] || keyStates['S']) {
        camera.x -= lookX * CAMERA_SPEED;
        camera.y -= lookY * CAMERA_SPEED;
        camera.z -= lookZ * CAMERA_SPEED;
    }
    
    // Kiri (A)
    if (keyStates['a'] || keyStates['A']) {
        camera.x += strafeX * CAMERA_SPEED;
        camera.z += strafeZ * CAMERA_SPEED;
    }
    
    // Kanan (D)
    if (keyStates['d'] || keyStates['D']) {
        camera.x -= strafeX * CAMERA_SPEED;
        camera.z -= strafeZ * CAMERA_SPEED;
    }
    
    // Naik (Space)
    if (keyStates[' ']) {
        camera.y += CAMERA_SPEED;
    }
    
    // Turun (Shift)
    if (keyStates['c'] || keyStates['C']) {
        camera.y -= CAMERA_SPEED;
    }
    
    // Tambahkan blok (klik kiri)
    if (keyStates['e'] || keyStates['E']) {
        int hitX, hitY, hitZ, prevX, prevY, prevZ;
        if (rayCast(camera.x, camera.y, camera.z, lookX, lookY, lookZ, hitX, hitY, hitZ, prevX, prevY, prevZ)) {
            if (prevX >= 0 && prevX < WORLD_SIZE &&
                prevY >= 0 && prevY < WORLD_HEIGHT &&
                prevZ >= 0 && prevZ < WORLD_SIZE) {
                
                world[prevX][prevY][prevZ].type = selectedBlockType;
                world[prevX][prevY][prevZ].visible = true;
                
                // Update visibilitas blok sekitarnya
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dz = -1; dz <= 1; dz++) {
                            int nx = prevX + dx;
                            int ny = prevY + dy;
                            int nz = prevZ + dz;
                            
                            if (nx >= 0 && nx < WORLD_SIZE &&
                                ny >= 0 && ny < WORLD_HEIGHT &&
                                nz >= 0 && nz < WORLD_SIZE && 
                                world[nx][ny][nz].type != AIR) {
                                
                                world[nx][ny][nz].visible = isBlockVisible(nx, ny, nz);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Hapus blok (klik kanan)
    if (keyStates['q'] || keyStates['Q']) {
        int hitX, hitY, hitZ, prevX, prevY, prevZ;
        if (rayCast(camera.x, camera.y, camera.z, lookX, lookY, lookZ, hitX, hitY, hitZ, prevX, prevY, prevZ)) {
            world[hitX][hitY][hitZ].type = AIR;
            world[hitX][hitY][hitZ].visible = false;
            
            // Update visibilitas blok sekitarnya
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dz = -1; dz <= 1; dz++) {
                        int nx = hitX + dx;
                        int ny = hitY + dy;
                        int nz = hitZ + dz;
                        
                        if (nx >= 0 && nx < WORLD_SIZE &&
                            ny >= 0 && ny < WORLD_HEIGHT &&
                            nz >= 0 && nz < WORLD_SIZE && 
                            world[nx][ny][nz].type != AIR) {
                            
                            world[nx][ny][nz].visible = isBlockVisible(nx, ny, nz);
                        }
                    }
                }
            }
        }
    }
    
    glutPostRedisplay();
}

// Handle mouse motion
void mouseMotion(int x, int y) {
    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
        return;
    }
    
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    
    lastMouseX = x;
    lastMouseY = y;
    
    cameraYaw += (float)deltaX * 0.01f;
    cameraPitch -= (float)deltaY * 0.01f;
    
    // Batasi pitch agar tidak melihat terlalu atas atau bawah
    if (cameraPitch > 1.5f) cameraPitch = 1.5f;
    if (cameraPitch < -1.5f) cameraPitch = -1.5f;
}

// Mouse button callback
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            keyStates['e'] = true;
        } else {
            keyStates['e'] = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            keyStates['q'] = true;
        } else {
            keyStates['q'] = false;
        }
    }
}

// Fungsi idle untuk memperbarui game
void idle() {
    movement();
    glutPostRedisplay();
}

// Fungsi utama
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Minecraft dengan GLUT");
    
    // Inisialisasi OpenGL
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f); // Warna langit biru muda
    
    // Generate dunia
    generateWorld();
    
    // Set callback
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMotionFunc(mouseMotion);
    glutPassiveMotionFunc(mouseMotion);
    glutMouseFunc(mouseButton);
    glutIdleFunc(idle);
    
    // Atur mode mouse
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
    
    glutMainLoop();
    return 0;
}