/* 
 * File:   textureloader.cpp
 * Author: wouter
 * 
 * Created on April 21, 2009, 7:02 PM
 */

#include "textureloader.h"

#include "entitymanager.h"
#include "interfaces.h"
#include "types.h"
#include "wadloader.h"
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

TextureLoader::TextureLoader(
    IResources *resources,
    EntityManager *entities)
{
    mResources = resources;
    mEntities = entities;

    openWadFiles(getWadString(entities));
}

TextureLoader::~TextureLoader()
{
    while (!mWadFiles.empty())
    {
        delete mWadFiles.back();
        mWadFiles.pop_back();
    }
}

bool TextureLoader::getSkyTextures(
    Texture texture[6])
{
    const std::string &skyname = getSkyString(mEntities);

    if (skyname.empty())
    {
        spdlog::error("no sky to load");

        return false;
    }

    const char *shortNames[] = {"bk", "dn", "ft", "lf", "rt", "up"};

    for (int i = 0; i < 6; i++)
    {
        texture[i].repeat = false;

        auto name = fmt::format("{}{}.tga", skyname, shortNames[i]);
        if (mResources->openFileAsTexture(texture[i], name))
        {
            continue;
        }

        name = fmt::format("{}{}.bmp", skyname, shortNames[i]);
        if (!mResources->openFileAsTexture(texture[i], name))
        {
            spdlog::error("failed to load sky texture {}{} as both tga and bmp", skyname, shortNames[i]);

            return false;
        }
    }

    return true;
}

bool TextureLoader::getWadTexture(
    Texture &texture,
    unsigned char *textureData)
{
    tBSPMipTexHeader *header = (tBSPMipTexHeader *)textureData;

    texture.name = header->name;

    if (header->offsets[0] != 0)
    {
        return readTexture(texture, textureData);
    }

    for (vector<WADLoader *>::const_iterator itr = mWadFiles.begin(); itr != mWadFiles.end(); ++itr)
    {
        WADLoader *wadFile = *itr;
        const unsigned char *data = wadFile->getTextureData(header->name);
        if (data != nullptr)
        {
            return readTexture(texture, data);
        }
    }

    return false;
}

bool TextureLoader::readTexture(
    Texture &texture,
    const unsigned char *data)
{
    tBSPMipTexHeader *miptex = (tBSPMipTexHeader *)data;

    int s = miptex->width * miptex->height;
    int paletteOffset = miptex->offsets[0] + s + (s / 4) + (s / 16) + (s / 64) + 2;

    const unsigned char *source0 = data + miptex->offsets[0];
    const unsigned char *source1 = data + miptex->offsets[1];
    const unsigned char *source2 = data + miptex->offsets[2];
    const unsigned char *source3 = data + miptex->offsets[3];
    const unsigned char *palette = data + paletteOffset;

    texture.name = miptex->name;
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
        r = palette[source0[i] * 3];
        g = palette[source0[i] * 3 + 1];
        b = palette[source0[i] * 3 + 2];
        a = 255;
        if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

        texture.data[0][j0++] = r;
        texture.data[0][j0++] = g;
        texture.data[0][j0++] = b;
        texture.data[0][j0++] = a;

        // Level 1
        if (i < (s / 4))
        {
            r = palette[source1[i] * 3];
            g = palette[source1[i] * 3 + 1];
            b = palette[source1[i] * 3 + 2];
            a = 255;
            if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

            texture.data[1][j1++] = r;
            texture.data[1][j1++] = g;
            texture.data[1][j1++] = b;
            texture.data[1][j1++] = a;
        }

        // Level 2
        if (i < (s / 16))
        {
            r = palette[source2[i] * 3];
            g = palette[source2[i] * 3 + 1];
            b = palette[source2[i] * 3 + 2];
            a = 255;
            if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

            texture.data[2][j2++] = r;
            texture.data[2][j2++] = g;
            texture.data[2][j2++] = b;
            texture.data[2][j2++] = a;
        }

        // Level 3
        if (i < (s / 64))
        {
            r = palette[source3[i] * 3];
            g = palette[source3[i] * 3 + 1];
            b = palette[source3[i] * 3 + 2];
            a = 255;
            if (r <= 5 && g <= 5 && b == 255) r = g = b = a = 0;

            texture.data[3][j3++] = r;
            texture.data[3][j3++] = g;
            texture.data[3][j3++] = b;
            texture.data[3][j3++] = a;
        }
    }

    return true;
}

void TextureLoader::openWadFiles(
    const std::string &wadString)
{
    if (mResources == nullptr)
    {
        return;
    }

    int start = 0;
    int hit = wadString.find(';', start);
    while (hit > -1)
    {
        // Pick the full wad file with path from the wads string
        std::string wad = wadString.substr(start, hit - start);
        // Strip the found wadfile of its path
        std::string stripedWad = wad.substr(wad.find_last_of('\\') + 1);
        // Read the wad file
        auto wadFile = mResources->findFile(stripedWad);
        // Add it to the wad list if it is not NULL
        if (!wadFile.empty())
        {
            const IData *data = mResources->openFile(wadFile);
            if (data != NULL)
            {
                mWadFiles.push_back(new WADLoader(data, wadFile.c_str()));
            }
            else
            {
                spdlog::error("Could not open {}", wadFile);
            }
        }

        start = hit + 1;
        hit = wadString.find(';', start);
    }
}

const std::string &TextureLoader::getWadString(
    EntityManager *entities)
{
    int entityCount = entities->getEntityCount();

    for (int i = 0; i < entityCount; i++)
    {
        const tEntity *entity = entities->getEntity(i);

        if (entity->className != "worldspawn")
        {
            continue;
        }

        for (int j = 0; j < entity->valueCount; j++)
        {
            if (entity->values[j].key != "wad")
            {
                continue;
            }

            return entity->values[j].value;
        }
    }

    static std::string empty("");
    return empty;
}

const std::string &TextureLoader::getSkyString(
    EntityManager *entities)
{
    int entityCount = entities->getEntityCount();

    for (int i = 0; i < entityCount; i++)
    {
        const tEntity *entity = entities->getEntity(i);

        if (entity->className != "worldspawn")
        {
            continue;
        }

        for (int j = 0; j < entity->valueCount; j++)
        {
            if (entity->values[j].key != "skyname")
            {
                continue;
            }

            return entity->values[j].value;
        }
    }

    static std::string empty("");
    return empty;
}
