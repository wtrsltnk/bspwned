/* 
 * File:   textureloader.cpp
 * Author: wouter
 * 
 * Created on April 21, 2009, 7:02 PM
 */

#include "textureloader.h"
#include "interfaces.h"
#include "entitymanager.h"
#include "types.h"
#include "wadloader.h"
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>

using namespace std;

class TextureLoader::PIMPL
{
public:
    IResources* mResources;
    EntityManager* mEntities;

    vector<WADLoader*> mWadFiles;

    void openWadFiles(string wadString);
    const char* getWadString(EntityManager* entities);
    const char* getSkyString(EntityManager* entities);
    bool readTexture(Texture& texture, const unsigned char* data);
};

TextureLoader::TextureLoader(IResources* resources, EntityManager* entities)
        : pimpl(new TextureLoader::PIMPL())
{
    pimpl->mResources = resources;
    pimpl->mEntities = entities;

    pimpl->openWadFiles(pimpl->getWadString(entities));
}

TextureLoader::~TextureLoader()
{
    while (!pimpl->mWadFiles.empty())
    {
        delete pimpl->mWadFiles.back();
        pimpl->mWadFiles.pop_back();
    }
    delete pimpl;
}

bool TextureLoader::getSkyTextures(Texture texture[6])
{
    const char* skyname = pimpl->getSkyString(pimpl->mEntities);

    if (skyname != NULL)
    {
        const char* shortNames[] = { "bk", "dn", "ft", "lf", "rt", "up" };
        char name[32] = { 0 };
        for (int i = 0; i < 6; i++)
        {
            sprintf(name, "%s%s.tga", skyname, shortNames[i]);
            if (!pimpl->mResources->openFileAsTexture(texture[i], name))
                return false;
            texture[i].repeat = false;
        }
        return true;
    }
    
    return false;
}

bool TextureLoader::getWadTexture(Texture& texture, unsigned char* textureData)
{
    tBSPMipTexHeader* header = (tBSPMipTexHeader*)textureData;

    if (header->offsets[0] == 0)
    {
        for (vector<WADLoader*>::const_iterator itr = pimpl->mWadFiles.begin(); itr != pimpl->mWadFiles.end(); ++itr)
        {
            WADLoader* wadFile = *itr;
            const unsigned char* data = wadFile->getTextureData(header->name);
            if (data != NULL)
            {
                return pimpl->readTexture(texture, data);
            }
        }
    }
    else
    {
        return pimpl->readTexture(texture, textureData);
    }
    return false;
}

bool TextureLoader::PIMPL::readTexture(Texture& texture, const unsigned char* data)
{
    tBSPMipTexHeader* miptex = (tBSPMipTexHeader*)data;

    int s = miptex->width * miptex->height;
    int paletteOffset = miptex->offsets[0] + s + (s / 4) + (s / 16) + (s / 64) + 2;

    const unsigned char* source0 = data + miptex->offsets[0];
    const unsigned char* source1 = data + miptex->offsets[1];
    const unsigned char* source2 = data + miptex->offsets[2];
    const unsigned char* source3 = data + miptex->offsets[3];
    const unsigned char* palette = data + paletteOffset;

    texture.name = new char[strlen(miptex->name) + 1];
    strcpy(texture.name, miptex->name);
    texture.width = miptex->width;
    texture.height = miptex->height;
    texture.bpp = 4;
    texture.data[0] = new unsigned char[s * texture.bpp];
    texture.data[1] = new unsigned char[(s / 4) * texture.bpp];
    texture.data[2] = new unsigned char[(s / 16) * texture.bpp];
    texture.data[3] = new unsigned char[(s / 64) * texture.bpp];
    
    int j0 = 0, j1 = 0, j2 = 0, j3 = 0;
    for (int i = 0; i < s; i++)
    {
        unsigned char r, g, b, a;

        // Level 0
        r = palette[source0[i]*3]; g = palette[source0[i]*3 + 1]; b = palette[source0[i]*3 + 2]; a = 255;
        if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

        texture.data[0][j0++] = r;
        texture.data[0][j0++] = g;
        texture.data[0][j0++] = b;
        texture.data[0][j0++] = a;
        
        // Level 1
        if (i < (s / 4))
        {
            r = palette[source1[i]*3]; g = palette[source1[i]*3 + 1]; b = palette[source1[i]*3 + 2]; a = 255;
            if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

            texture.data[1][j1++] = r;
            texture.data[1][j1++] = g;
            texture.data[1][j1++] = b;
            texture.data[1][j1++] = a;
        }

        // Level 2
        if (i < (s / 16))
        {
            r = palette[source2[i]*3]; g = palette[source2[i]*3 + 1]; b = palette[source2[i]*3 + 2]; a = 255;
            if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

            texture.data[2][j2++] = r;
            texture.data[2][j2++] = g;
            texture.data[2][j2++] = b;
            texture.data[2][j2++] = a;
        }

        // Level 3
        if (i < (s / 64))
        {
            r = palette[source3[i]*3]; g = palette[source3[i]*3 + 1]; b = palette[source3[i]*3 + 2]; a = 255;
            if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

            texture.data[3][j3++] = r;
            texture.data[3][j3++] = g;
            texture.data[3][j3++] = b;
            texture.data[3][j3++] = a;
        }
    }

    return true;
}

void TextureLoader::PIMPL::openWadFiles(string wadString)
{
    if (mResources != NULL)
    {
	int start = 0;
	int hit = wadString.find(';', start);
	while (hit > -1)
	{
            // Pick the full wad file with path from the wads string
            string wad = wadString.substr(start, hit-start);
            // Strip the found wadfile of its path
            string stripedWad = wad.substr(wad.find_last_of('\\') + 1);
            // Read the wad file
            const char* wadFile = mResources->findFile(stripedWad.c_str());
            // Add it to the wad list if it is not NULL
            if (wadFile != NULL)
            {
                const IData* data = mResources->openFile(wadFile);
                if (data != NULL)
                {
                    mWadFiles.push_back(new WADLoader(data, wadFile));
                }
                else
                {
                    cout << "Could not open " << wadFile << endl;
                }
            }

            start = hit + 1;
            hit = wadString.find(';', start);
	}
    }
}

const char* TextureLoader::PIMPL::getWadString(EntityManager* entities)
{
    int entityCount = entities->getEntityCount();

    for (int i = 0; i < entityCount; i++)
    {
        const tEntity* entity = entities->getEntity(i);
        if (strcmp(entity->className, "worldspawn") == 0)
        {
            for (int j = 0; j < entity->valueCount; j++)
            {
                if (strcmp(entity->values[j].key, "wad") == 0)
                {
                    return entity->values[j].value;
                }
            }
        }
    }
    return NULL;
}

const char* TextureLoader::PIMPL::getSkyString(EntityManager* entities)
{
    int entityCount = entities->getEntityCount();

    for (int i = 0; i < entityCount; i++)
    {
        const tEntity* entity = entities->getEntity(i);
        if (strcmp(entity->className, "worldspawn") == 0)
        {
            for (int j = 0; j < entity->valueCount; j++)
            {
                if (strcmp(entity->values[j].key, "skyname") == 0)
                {
                    return entity->values[j].value;
                }
            }
        }
    }
    return NULL;
}
