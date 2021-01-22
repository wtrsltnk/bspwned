/* 
 * File:   textureloader.h
 * Author: wouter
 *
 * Created on April 21, 2009, 7:02 PM
 */

#ifndef _TEXTURELOADER_H
#define	_TEXTURELOADER_H

#include <vector>
#include <string>

class IResources;
class Texture;
class EntityManager;

class TextureLoader
{
public:
    TextureLoader(
        IResources* resources,
        EntityManager* entities);

    virtual ~TextureLoader();

    bool getWadTexture(
        Texture& texture,
        unsigned char* textureData);

    bool getSkyTextures(
        Texture texture[6]);
    
private:
    IResources *mResources;
    EntityManager *mEntities;

    std::vector<class WADLoader *> mWadFiles;

    void openWadFiles(
        const std::string &wadString);

    const std::string &getWadString(
        EntityManager *entities);

    const std::string &getSkyString(
        EntityManager *entities);

    bool readTexture(
        Texture &texture,
        const unsigned char *data);

};

#endif	/* _TEXTURELOADER_H */

