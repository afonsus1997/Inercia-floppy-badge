#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../include/GL/gl.h"
#include "../include/zbuffer.h"

#define STBIW_ASSERT(x) /* a comment */
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include-demo/stb_image_write.h"

typedef unsigned char uchar;

#ifndef M_PI
#define M_PI 3.14159265
#endif


void draw_triangle()
{
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glPushMatrix();
    glRotatef(1.5, 0, 0, 1);
    
    glBegin(GL_TRIANGLES);
    
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(-0.9, -0.9, 0.0);
    
    glColor3f(0.0, 0.0, 0.0);
    glVertex3f(0.9, -0.9, 0.0);
    
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0, 0.9, 0.0);
    
    glEnd();
    glPopMatrix();
}

void draw_lines()
{
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glPushMatrix();
    glRotatef(1.5, 0, 0, 1);
    
    glBegin(GL_LINES);

    glColor3f(1.0, 1.0, 1.0);
    
    glVertex3f(-0.9, -0.9, 0.0);
    glVertex3f(0.9, -0.9, 0.0);

    glVertex3f(0.9, -0.9, 0.0);
    glVertex3f(0, 0.9, 0.0);
    
    glVertex3f(0, 0.9, 0.0);
    glVertex3f(-0.9, -0.9, 0.0);
    
    glEnd();
    glPopMatrix();
}


void draw_points()
{
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glPushMatrix();
    glRotatef(1.5, 0, 0, 1);
    
    glBegin(GL_POINTS);

    glColor3f(1.0, 1.0, 1.0);
    
    glVertex3f(-0.9, -0.9, 0.0);
    glVertex3f(0.9, -0.9, 0.0);    
    glVertex3f(0, 0.9, 0.0);
    
    glEnd();
    glPopMatrix();
}


void initScene() {
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glTextSize(GL_TEXT_SIZE24x24);
    glEnable(GL_NORMALIZE);
}

int main(int argc, char** argv) {
    int winSizeX = 320;
    int winSizeY = 256;
    PIXEL* imbuf = NULL;
    uchar* pbuf = NULL;
    
    ZBuffer* frameBuffer = NULL;
    
    switch (TGL_FEATURE_RENDER_BITS)
    {
    case 32:
        frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_RGBA, 0);
        break;

    case 16:
        frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_5R6G5B, 0);
        break;

    case 1:
        frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_INDEX, 0);
        break;

    default:;
    }

    if (!frameBuffer)
    {
        printf("\nZB_open failed!");
        exit(1);
    }
    
    glInit(frameBuffer);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, winSizeX, winSizeY);
    
    // glShadeModel(GL_FLAT);
    glShadeModel(GL_SMOOTH);
    //glDisable(GL_LIGHTING);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    GLfloat h = (GLfloat)winSizeY / (GLfloat)winSizeX;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glFrustum(-1.0, 1.0, -h, h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, 0.0);
    // glTranslatef(-1,-0.5,-10.0);

    initScene();

    // glSetEnableSpecular(GL_TRUE);
    glSetEnableSpecular(GL_FALSE);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ZB_setDitheringMap(frameBuffer, DITHER_MAP_BAYER2);
    // draw_lines();
    // draw_points();
    draw_triangle();


    if (TGL_FEATURE_RENDER_BITS == 1)
    {
        imbuf = calloc(1, (winSizeX / 8) * winSizeY);
    }
    else
    {
        imbuf = calloc(1, sizeof(PIXEL) * winSizeX * winSizeY);
    }


    ZB_copyFrameBuffer(frameBuffer, imbuf, winSizeX * sizeof(PIXEL));


    if (TGL_FEATURE_RENDER_BITS == 32)
    {
        pbuf = malloc(3 * winSizeX * winSizeY);
        
        for(int i = 0; i < winSizeX * winSizeY; i++)
        {
            pbuf[3 * i + 0] = GET_RED(imbuf[i]);
            pbuf[3 * i + 1] = GET_GREEN(imbuf[i]);
            pbuf[3 * i + 2] = GET_BLUE(imbuf[i]);
        }
        stbi_write_png("render.png", winSizeX, winSizeY, 3, pbuf, 0);
        free(imbuf);
        free(pbuf);
    } 
    else if(TGL_FEATURE_RENDER_BITS == 16)
    {
        pbuf = malloc(3 * winSizeX * winSizeY);
        for (int i = 0; i < winSizeX * winSizeY; i++)
        {
            pbuf[3 * i + 0] = GET_RED(imbuf[i]);
            pbuf[3 * i + 1] = GET_GREEN(imbuf[i]);
            pbuf[3 * i + 2] = GET_BLUE(imbuf[i]);
        }
        stbi_write_png("render.png", winSizeX, winSizeY, 3, pbuf, 0);
        free(imbuf);
        free(pbuf);
    }
    else if(TGL_FEATURE_RENDER_BITS == 1)
    {
        pbuf = malloc(3 * winSizeX * winSizeY);
        GLint ls = frameBuffer->linesize;

        for (int x = 0; x < winSizeX; x++)
        {
            for (int y = 0; y < winSizeY; y++)
            {
                GLbyte val = (imbuf[y * ls + x / 8] >> (x % 8)) & 1;
                val = val ? 0xff : 0x00;
                pbuf[(y * winSizeX + x) * 3 + 0] = val;
                pbuf[(y * winSizeX + x) * 3 + 1] = val;
                pbuf[(y * winSizeX + x) * 3 + 2] = val;
            }
        }

        stbi_write_png("render_1b.png", winSizeX, winSizeY, 3, pbuf, 0);
        free(imbuf);
        free(pbuf);
    }

    ZB_close(frameBuffer);
    glClose();

    return 0;
}
