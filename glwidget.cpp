#include "glwidget.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "math.h"

float LightPos[] = { 10.0f, 45.0f, 2.0f, 0.2f }; // Light Position Values
float LightAmb[] = { 0.7f, 0.7f, 0.7f, 1.0f }; // Ambient Light Values
float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// Specular Light Values

float MatAmb[] = { 1.0f, 1.0f, 1.0f }; // Blocks material
float MatClean[] = { 0.0f, 0.0f, 0.0f, 1.0 }; // No Material Values

GLuint TextureId[3];
float worldOffsetY = -0.1f;

// Constructor
GLWidget::GLWidget(QWidget *parent)
    :QGLWidget(parent) {
    this->horizontalAngle = 300*M_PI/180.0f;
    this->verticalAngle = 60*M_PI/180.0f;
    this->positionX = 0;
    this->positionZ = 0;
    this->zoomAmount = 140.0f;
    this->wired = false;
    this->levelAmount = 45;

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer.start(16);
}

// Empty destructor
GLWidget::~GLWidget() {}

// Initialize OpenGL
void GLWidget::initializeGL() {
    glShadeModel(GL_SMOOTH); // Enable smooth shading
    glClearColor(0.675f, 0.828f, 1.0f, 1.0f); // Set the clear color to a black background
    glClearDepth(1.0f); // Depth buffer setup
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glDepthFunc(GL_LEQUAL); // Set type of depth test
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really nice perspective calculations

    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);	// Set Light0 Ambience
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);	// Set Light0 Diffuse
    glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc); // Set Light0 Specular
    glEnable(GL_COLOR_MATERIAL); // Enable Color Material
    glEnable(GL_LIGHTING); // Enable Lighting
    glEnable(GL_LIGHT0); // Enable Light0

    QImage grassImg; // Load Sand Texture
    if(!grassImg.load("Assets/sand.png")) {
        exit(1);
    }

    QImage sadstoneImg; // Load Sandstone Texture
    if(!sadstoneImg.load("Assets/cut_sandstone.png")) {
        exit(1);
    }

    QImage sadstoneTopImg; // Load Sandstone Top Texture
    if(!sadstoneTopImg.load("Assets/sandstone_top.png")) {
        exit(1);
    }

    QImage grassFormattedImg; // Convert Grass texture to OpenGL format
    grassFormattedImg = QGLWidget::convertToGLFormat(grassImg);
    if(grassFormattedImg.isNull()) {
        exit(1);
    }

    QImage sandstoneFormattedImg; // Convert Sandstone texture to OpenGL format
    sandstoneFormattedImg = QGLWidget::convertToGLFormat(sadstoneImg);
    if(sandstoneFormattedImg.isNull()) {
        exit(1);
    }

    QImage sandstoneTopFormattedImg; // Convert Sandstone top texture to OpenGL format
    sandstoneTopFormattedImg = QGLWidget::convertToGLFormat(sadstoneTopImg);
    if(sandstoneTopFormattedImg.isNull()) {
        exit(1);
    }

    glEnable(GL_TEXTURE_2D); // Enable Texture Use
    glGenTextures(3, TextureId); // Generate textures
    glBindTexture(GL_TEXTURE_2D, TextureId[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, grassFormattedImg.width(), grassFormattedImg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, grassFormattedImg.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, TextureId[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sandstoneFormattedImg.width(), sandstoneFormattedImg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, sandstoneFormattedImg.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, TextureId[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sandstoneTopFormattedImg.width(), sandstoneTopFormattedImg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, sandstoneTopFormattedImg.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

// This is called when the OpenGL window is resized
void GLWidget::resizeGL(int width, int height)  {
    // Prevent divide by zero (in the gluPerspective call)
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height); // Reset current viewport
    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity(); // Reset projection matrix
    gluPerspective(45.0f, static_cast<GLfloat>(width)/height, 0.1f, 300.0f); // Calculate aspect ratio

    updateGL();
}

// OpenGL painting
void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen and depth buffer

    glMatrixMode(GL_MODELVIEW); // Select matrix matrix
    glLoadIdentity(); // Reset current modelview matrix

    // Change the camera position
    if(verticalAngle < M_PI) {
        gluLookAt(zoomAmount*cos(horizontalAngle)*sin(verticalAngle), zoomAmount*cos(verticalAngle), zoomAmount*sin(horizontalAngle)*sinf(verticalAngle), 0, 0, 0, 0, 1, 0);
    } else {
        gluLookAt(zoomAmount*cos(horizontalAngle)*sin(verticalAngle), zoomAmount*cos(verticalAngle), zoomAmount*sin(horizontalAngle)*sinf(verticalAngle), 0, 0, 0, 0, -1, 0);
    }


    if(wired) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glTranslatef(positionX * cos(horizontalAngle + M_PI/2), 0, positionX * sin(horizontalAngle + M_PI/2));
    glTranslatef(- positionZ * cos(horizontalAngle), 0, - positionZ * sin(horizontalAngle));
    glLightfv(GL_LIGHT0, GL_POSITION, LightPos); // Set Light Position
    printPyramid(levelAmount);
    printFloor(180);
}

void GLWidget::printCube(float offsetX, float offsetY, float offsetZ) {
    glBindTexture(GL_TEXTURE_2D, TextureId[1]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmb);
    glMaterialfv(GL_FRONT, GL_EMISSION, MatClean);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);

    glBegin(GL_QUADS);
    // Front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(offsetX - 0.5f, offsetY + 0.0f, offsetZ - 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(offsetX + 0.5f, offsetY + 0.0f, offsetZ - 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(offsetX + 0.5f, offsetY + 1.0f, offsetZ - 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(offsetX - 0.5f, offsetY + 1.0f, offsetZ - 0.5f);
    // Back face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(offsetX - 0.5f, offsetY + 0.0f, offsetZ + 0.5f);
    glTexCoord2f(-1.0f, 0.0f); glVertex3f(offsetX + 0.5f, offsetY + 0.0f, offsetZ + 0.5f);
    glTexCoord2f(-1.0f, 1.0f); glVertex3f(offsetX + 0.5f, offsetY + 1.0f, offsetZ + 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(offsetX - 0.5f, offsetY + 1.0f, offsetZ + 0.5f);
    // Left face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(offsetX - 0.5f, offsetY + 0.0f, offsetZ - 0.5f);
    glTexCoord2f(-1.0f, 0.0f); glVertex3f(offsetX - 0.5f, offsetY + 0.0f, offsetZ + 0.5f);
    glTexCoord2f(-1.0f, 1.0f); glVertex3f(offsetX - 0.5f, offsetY + 1.0f, offsetZ + 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(offsetX - 0.5f, offsetY + 1.0f, offsetZ - 0.5f);
    // Right face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(offsetX + 0.5f, offsetY + 0.0f, offsetZ - 0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(offsetX + 0.5f, offsetY + 0.0f, offsetZ + 0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(offsetX + 0.5f, offsetY + 1.0f, offsetZ + 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(offsetX + 0.5f, offsetY + 1.0f, offsetZ - 0.5f);
    // Bottom face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(offsetX - 0.5f, offsetY + 0.0f, offsetZ - 0.5f);
    glTexCoord2f(-1.0f, 0.0f); glVertex3f(offsetX + 0.5f, offsetY + 0.0f, offsetZ - 0.5f);
    glTexCoord2f(-1.0f, 1.0f); glVertex3f(offsetX + 0.5f, offsetY + 0.0f, offsetZ + 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(offsetX - 0.5f, offsetY + 0.0f, offsetZ + 0.5f);
    glEnd();

    // Top face separated to use another texture
    glBindTexture(GL_TEXTURE_2D, TextureId[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(offsetX - 0.5f, offsetY + 1.0f, offsetZ - 0.5f);
    glTexCoord2f(-1.0f, 0.0f); glVertex3f(offsetX + 0.5f, offsetY + 1.0f, offsetZ - 0.5f);
    glTexCoord2f(-1.0f, -1.0f); glVertex3f(offsetX + 0.5f, offsetY + 1.0f, offsetZ + 0.5f);
    glTexCoord2f(0.0f, -1.0f); glVertex3f(offsetX - 0.5f, offsetY + 1.0f, offsetZ + 0.5f);
    glEnd();
}

void GLWidget::printLevel(int width, float y) {
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < width; j++) {
            printCube(i - width/2, y, j - width/2);
        }
    }
}

void GLWidget::printPyramid(int height) {
    for(int i = 0; i < height; i++) {
        printLevel(1 + (height - i - 1) * 2, worldOffsetY + i);
    }
}

void GLWidget::printFloor(int width) {
    glBindTexture(GL_TEXTURE_2D, TextureId[0]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, MatAmb);
    glMaterialfv(GL_FRONT, GL_EMISSION, MatClean);
    glColorMaterial(GL_FRONT, GL_EMISSION);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(width * -0.5f, worldOffsetY, width * -0.5f);
    glTexCoord2f(width * -1.0f, 0.0f); glVertex3f(width * 0.5f, worldOffsetY,width * -0.5f);
    glTexCoord2f(width * -1.0f, width * -1.0f); glVertex3f(width * 0.5f, worldOffsetY, width * 0.5f);
    glTexCoord2f(0.0f, width * -1.0f); glVertex3f(width * -0.5f, worldOffsetY, width * 0.5f);
    glEnd();
}
