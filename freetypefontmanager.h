/* 
 * File:   freetypefontmanager.h
 * Author: wouter
 *
 * Created on May 4, 2009, 11:01 PM
 */

#ifndef _FREETYPEFONTMANAGER_H
#define	_FREETYPEFONTMANAGER_H

#include "font.h"

class FreeTypeFontManager
{
public:
    FreeTypeFontManager();
    virtual ~FreeTypeFontManager();

    static Font* createFont(const char* name, int height);

private:

};

#endif	/* _FREETYPEFONTMANAGER_H */

