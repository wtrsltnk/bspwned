/* 
 * File:   font.cpp
 * Author: wouter
 * 
 * Created on May 4, 2009, 7:29 PM
 */

#include "font.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

Font::Font()
{
}

Font::~Font()
{
}

void Font::print(int x, int y, const char* string, ...)
{
    char text[256] = { 0 };
    va_list ap;
    
    va_start(ap, string);
        vsprintf(text, string, ap);
    va_end(ap);

    glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glListBase(this->mListBase);

    float modelview_matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x,y,0);
    glMultMatrixf(modelview_matrix);

    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
    glPopMatrix();
    
    glCullFace(GL_FRONT);
    glPopAttrib();
    
}
