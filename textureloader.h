/* 
 * File:   textureloader.h
 * Author: wouter
 *
 * Created on April 21, 2009, 7:02 PM
 */

#ifndef _TEXTURELOADER_H
#define	_TEXTURELOADER_H

class IResources;
class Texture;
class EntityManager;

class TextureLoader
{
public:
    TextureLoader(IResources* resources, EntityManager* entities);
    virtual ~TextureLoader();

    bool getWadTexture(Texture& texture, unsigned char* textureData);
    bool getSkyTextures(Texture texture[6]);
    
private:
    class PIMPL;
    PIMPL* pimpl;

};

#endif	/* _TEXTURELOADER_H */

