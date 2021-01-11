/* 
 * File:   SkyRenderer.h
 * Author: wouter
 *
 * Created on May 25, 2009, 8:28 PM
 */

#ifndef _SKYRENDERER_H
#define	_SKYRENDERER_H

#include "interfaces.h"

class SkyRenderer
{
    friend class WorldLoader;
public:
    SkyRenderer();
    virtual ~SkyRenderer();

    void render(const float position[3]);
    
private:
    Texture mTextures[6];

};

#endif	/* _SKYRENDERER_H */

