/* 
 * File:   font.h
 * Author: wouter
 *
 * Created on May 4, 2009, 7:29 PM
 */

#ifndef _FONT_H
#define	_FONT_H

#include <GL/gl.h>
#include <GL/glu.h>

class Font
{
public:
    Font();
    virtual ~Font();

    void print(int x, int y, const char* string, ...);

    GLuint mTextures[128];
    GLuint mListBase;
};

#endif	/* _FONT_H */

