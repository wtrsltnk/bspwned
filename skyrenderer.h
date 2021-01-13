/* 
 * File:   SkyRenderer.h
 * Author: wouter
 *
 * Created on May 25, 2009, 8:28 PM
 */

#ifndef _SKYRENDERER_H
#define _SKYRENDERER_H

#include "interfaces.h"

#include <glm/glm.hpp>

class SkyRenderer
{
    friend class WorldLoader;

public:
    SkyRenderer();

    virtual ~SkyRenderer();

    void render(
        const glm::vec3 &position);

private:
    Texture mTextures[6];
};

#endif /* _SKYRENDERER_H */
