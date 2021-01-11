/* 
 * File:   freetypefontmanager.cpp
 * Author: wouter
 * 
 * Created on May 4, 2009, 11:01 PM
 */

#include "freetypefontmanager.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <GL/gl.h>
#include <stdexcept>

void make_dlist(FT_Face& face, int ch, Font* font);

FreeTypeFontManager::FreeTypeFontManager()
{
}

FreeTypeFontManager::~FreeTypeFontManager()
{
}

Font* FreeTypeFontManager::createFont(const char* name, int height)
{
    FT_Library library;
    if (FT_Init_FreeType(&library))
        return NULL;

    FT_Face face;
    if (FT_New_Face(library, name, 0, &face))
        return NULL;

    FT_Set_Char_Size( face, height << 6, height << 6, 96, 96);

    Font* font = new Font();

    font->mListBase = glGenLists(128);
    glGenTextures(128, font->mTextures);

    try
    {
        for(unsigned char i = 0; i < 128; i++)
            make_dlist(face, i,font);
    }
    catch (std::runtime_error& e)
    {
	glDeleteLists(font->mListBase, 128);
	glDeleteTextures(128, font->mTextures);
        delete font;
        font = 0;
    }

    // Cleanup
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return font;
}
inline int next_p2 ( int a )
{
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}
void make_dlist(FT_Face& face, int ch, Font* font)
{
    if(FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT))
        throw std::runtime_error("FT_Load_Glyph failed");

    FT_Glyph glyph;
    if(FT_Get_Glyph(face->glyph, &glyph))
        throw std::runtime_error("FT_Get_Glyph failed");

    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

    FT_Bitmap& bitmap=bitmap_glyph->bitmap;

    int width = next_p2( bitmap.width );
    int height = next_p2( bitmap.rows );

    GLubyte* expanded_data = new GLubyte[ 2 * width * height];

    for(int j=0; j <height;j++) {
        for(int i=0; i < width; i++){
            expanded_data[2*(i+j*width)]= expanded_data[2*(i+j*width)+1] =
            (i>=bitmap.width || j>=bitmap.rows) ?
            0 : bitmap.buffer[i + bitmap.width*j];
        }
    }

    glBindTexture( GL_TEXTURE_2D, font->mTextures[ch]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data);
    delete [] expanded_data;

    glNewList(font->mListBase + ch, GL_COMPILE);

    glBindTexture(GL_TEXTURE_2D, font->mTextures[ch]);

    glTranslatef(bitmap_glyph->left, 0, 0);

    glPushMatrix();
    glTranslatef(0, bitmap_glyph->top-bitmap.rows, 0);

    float x = float(bitmap.width) / float(width);
    float y = float(bitmap.rows) / float(height);

    glBegin(GL_QUADS);
        glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
        glTexCoord2d(0,y); glVertex2f(0,0);
        glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
        glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
    glEnd();
    glPopMatrix();
    glTranslatef(face->glyph->advance.x >> 6 ,0,0);

    glEndList();
}
