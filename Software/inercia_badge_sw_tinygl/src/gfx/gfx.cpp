#include "gfx.h"

void initializeTinyGL() {
  // glContext = (TinyGLContext *)malloc(sizeof(TinyGLContext));
  frame_buffer = ZB_open(kOledWidth, kOledHeight, ZB_MODE_5R6G5B, NULL);
  glInit(frame_buffer);
  glViewport(0, 0, kOledWidth, kOledHeight);
  // Set up the projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // glFrustum(
  //     -1.0f * RATIO,
  //     1.0f * RATIO,
  //     -1.0f,
  //     1.0f,
  //     1.0f,
  //     10.0f);
  // Set up the modelview matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set up the dithering map
  ZB_setDitheringMap(frame_buffer, DITHER_MAP_BAYER4);
}

  void GfxHandler(void) { animations[0](); }