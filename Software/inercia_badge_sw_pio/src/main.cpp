#include "main.h"
extern "C" {
#include "zbuffer.h"
#include "GL/gl.h"
}

ZBuffer *frameBuffer;  // ZBuffer for framebuffer

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Global variables
float rotationAngle = 0;  // Angle for rotating the cube

uint8_t testbuf[512];

// Set up the perspective projection
void setPerspective(float fov, float aspect, float near, float far) {
  float top = near * tan(fov * M_PI / 360.0);
  float bottom = -top;
  float right = top * aspect;
  float left = -right;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glFrustum(left, right, bottom, top, near, far);  // Set up the frustum (viewing volume)
  glMatrixMode(GL_MODELVIEW);  // Return to modelview matrix
}

// Function to initialize TinyGL and setup rendering context
void initializeTinyGL() {
    // glContext = (TinyGLContext *)malloc(sizeof(TinyGLContext));
    frameBuffer = ZB_open(SCREEN_WIDTH, SCREEN_HEIGHT, ZB_MODE_5R6G5B, 0);
    glInit(frameBuffer);

    // Set the viewport for rendering
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
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
  frameBuffer = ZB_open(kOledWidth, kOledHeight, ZB_MODE_5R6G5B, NULL);
  u8g2.setBufferPtr((uint8_t*)frameBuffer->pbuf);  // Set framebuffer pointer to u8g2testbuf
  // u8g2.setBufferPtr((uint8_t*)testbuf);  // Set framebuffer pointer to u8g2testbuf
  u8g2.initDisplay();  // Initialize the display
  u8g2.setPowerSave(0);  // Disable power save mode
  u8g2.setDrawColor(1);  // Set drawing color to white
  u8g2.clearBuffer(); // Clear the display buffer
  u8g2.setFont(u8g2_font_5x8_tr); // Set the font
  u8g2.setCursor(0, 10); // Set the cursor position
  u8g2.print("Hello, World!"); // Print the message
    u8g2.sendBuffer();
  
  // Set up initial 3D transformation (rotation and translation)
  glMatrixMode(GL_PROJECTION_MATRIX);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW_MATRIX);
  glLoadIdentity();
  while(1){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(0,  1);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-1, -1);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(1, -1);
    glEnd();
    glFinish();
    ZB_setDitheringMap(frame_buffer, 3);
    // Send buffer to the display (render output)
    u8g2.sendBuffer();
  }
  
  // Initialize the player or other components (commented out)
  // PlayerInit();
  // multicore_launch_core1(core1_displayTask);  // Launch display task on core 1
}

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
