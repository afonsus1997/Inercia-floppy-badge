#include "main.h"
extern "C" {
#include "zbuffer.h"
#include "GL/gl.h"
}


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define RATIO (SCREEN_WIDTH / SCREEN_HEIGHT)

// Global variables
float rotationAngle = 0;  // Angle for rotating the cube

void loop_triangles()
{
    int t = 0;

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up light properties with emphasis on intensity
    GLfloat light_intensity = 0.8f;
    GLfloat light_ambient[] = {light_intensity, light_intensity, light_intensity, 1.0f};
    GLfloat light_diffuse[] = {light_intensity, light_intensity, light_intensity, 1.0f};
    GLfloat light_specular[] = {light_intensity, light_intensity, light_intensity, 1.0f};
    GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Enable color material
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_DEPTH_TEST);

    while (true)
    {
        // Clear the buffer using tinygl
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the modelview matrix
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -2.5f); // Move the triangle back so it's visible

        glRotatef(t, 0.2f, 1.0f, 0.005f); // Rotate the triangle

        // Lightsource position
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        glBegin(GL_QUADS);
        {
            // Front face
            glColor3f(0.5f, 0.5f, 0.5f); // Set color to mid-gray for intensity
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            // Back face
            glColor3f(0.5f, 0.5f, 0.5f); // Set color to mid-gray for intensity
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glVertex3f(1.0f, 1.0f, -1.0f);
            glVertex3f(1.0f, -1.0f, -1.0f);
            // Top face
            glColor3f(0.7f, 0.7f, 0.7f); // Set color to lighter gray for intensity
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, -1.0f);
            // Bottom face
            glColor3f(0.2f, 0.2f, 0.2f); // Set color to darker gray for intensity
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glVertex3f(1.0f, -1.0f, -1.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            // Left face
            glColor3f(0.4f, 0.4f, 0.4f); // Set color to mid-dark gray for intensity
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            // Right face
            glColor3f(0.6f, 0.6f, 0.6f); // Set color to mid-light gray for intensity
            glVertex3f(1.0f, -1.0f, -1.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, -1.0f);
        }
        glEnd();

        // Send the buffer to the display
        u8g2.sendBuffer();
        sleep_ms(1000 / 60);
        t = t < 360 ? t + 5 : 0;
    }
}

void loop_cubes()
{
    int width = u8g2.getDisplayWidth();
    int height = u8g2.getDisplayHeight();
    int t = 0;
    while (true)
    {
        sleep_ms(1000 / 60);
        u8g2.clearBuffer();
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                if ((x + t) % 8 < 4 && (y + t) % 8 < 4)
                {
                    u8g2.drawPixel(x, y);
                }
            }
        }
        u8g2.sendBuffer();
        t = t < 7 ? t + 1 : 0;
    }
}
// Function to initialize TinyGL and setup rendering context
void initializeTinyGL() {
    // glContext = (TinyGLContext *)malloc(sizeof(TinyGLContext));
    frame_buffer = ZB_open(kOledWidth, kOledHeight, ZB_MODE_5R6G5B, NULL);
    glInit(frame_buffer);
    glViewport(0, 0, kOledWidth, kOledHeight);
    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(
        -1.0f * RATIO,
        1.0f * RATIO,
        -1.0f,
        1.0f,
        1.0f,
        10.0f);
    // Set up the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set up the dithering map
    ZB_setDitheringMap(frame_buffer, DITHER_MAP_BAYER4);
}



// Setup function (runs once at startup)
void setup() {
  Serial.begin(115200);
  Serial.println("Starting up...");
  
  // Initialize TinyGL and framebuffer
  initializeTinyGL();
  
  // Initialize hardware and I/O (not fully shown here)
  IoInit();
  
  // Initialize the framebuffer for ZBuffer
  
  u8g2.setBufferPtr((uint8_t*)frame_buffer->pbuf);
  u8g2.initDisplay();  // Initialize the display
  u8g2.setPowerSave(0);  // Disable power save mode
  u8g2.setDrawColor(1);  // Set drawing color to white
  u8g2.clearBuffer(); // Clear the display buffer
  u8g2.setFont(u8g2_font_5x8_tr); // Set the font
  u8g2.setCursor(0, 10); // Set the cursor position
  u8g2.print("Hello, World!"); // Print the message
    u8g2.sendBuffer();
  
  // Set up initial 3D transformation (rotation and translation)
  loop_triangles();
  // loop_cubes();
    // Send buffer to the display (render output)
  // u8g2.sendBuffer();
}
  
  // Initialize the player or other components (commented out)
  // PlayerInit();
  // multicore_launch_core1(core1_displayTask);  // Launch display task on core 1


// Main loop (runs repeatedly)
void loop() {
  IoReadButtons();  // Read button inputs
  IoReadAnalogInputs();  // Read analog inputs
  IoReadVolume();  // Read volume (if applicable)
  
  // Handle player actions (not implemented)
  // PlayerHandlePlayback();
  
  IoResetButtonActivations();  // Reset button states
}

// Core 1 display task (runs on a separate core)
void core1_displayTask() {
  while(1) {
    u8g2.clearBuffer();  // Clear the buffer

    // Custom graphics handler function (can update UI here)
    GfxHandler();
    
    // Handle user interface elements
    UiHandleUiElements();
    
    // Send updated buffer to display
    u8g2.sendBuffer();
    
    // Add delay to prevent high CPU usage
    delay(10);
  }
}
