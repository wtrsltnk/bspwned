/* 
 * File:   SkyRenderer.cpp
 * Author: wouter
 * 
 * Created on May 25, 2009, 8:28 PM
 */

#include "skyrenderer.h"

#include "opengl.h"

#define SKY_FRONT  (1<<0)
#define SKY_BACK   (1<<1)
#define SKY_LEFT   (1<<2)
#define SKY_RIGHT  (1<<3)
#define SKY_UP     (1<<4)
#define SKY_DOWN   (1<<5)

SkyRenderer::SkyRenderer()
{
}

SkyRenderer::~SkyRenderer()
{
}

void SkyRenderer::render(const float position[3])
{
    float size = 4.0f;
    int renderFlag = SKY_RIGHT | SKY_LEFT | SKY_BACK | SKY_FRONT | SKY_UP | SKY_DOWN;

    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(position[0], position[1], position[2]);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    //"bk", "dn", "ft", "lf", "rt", "up"
    if (renderFlag & SKY_FRONT)
    {
        glBindTexture(GL_TEXTURE_2D, mTextures[2].glIndex);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(size, size, -size);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(size, -size, -size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, -size, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, size, -size);
        glEnd();
    }

    if (renderFlag & SKY_BACK)
    {
        glBindTexture(GL_TEXTURE_2D, mTextures[0].glIndex);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, size, size);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size, size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size, -size, size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size, size, size);
        glEnd();
    }


    if (renderFlag & SKY_LEFT)
    {
        glBindTexture(GL_TEXTURE_2D, mTextures[3].glIndex);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, size, -size);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size, -size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, -size, size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, size, size);
        glEnd();
    }


    if (renderFlag & SKY_RIGHT)
    {
        glBindTexture(GL_TEXTURE_2D, mTextures[4].glIndex);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size, -size, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size, size, -size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(size, size, size);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(size, -size, size);
        glEnd();
    }


    if (renderFlag & SKY_DOWN)
    {
        glBindTexture(GL_TEXTURE_2D, mTextures[1].glIndex);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size, size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, -size, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, -size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, size);
        glEnd();
    }


    if (renderFlag & SKY_UP)
    {
        glBindTexture(GL_TEXTURE_2D, mTextures[5].glIndex);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, size, -size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, size, size);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, -size);
        glEnd();
    }
    glPopMatrix();
}
