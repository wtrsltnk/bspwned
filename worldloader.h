/* 
 * File:   worldloader.h
 * Author: wouter
 *
 * Created on April 17, 2009, 1:46 AM
 */

#ifndef _WORLDLOADER_H
#define	_WORLDLOADER_H

#include "types.h"
#include "interfaces.h"
#include "error.h"
#include "worldrenderer.h"

class WorldLoader : public Error
{
public:
    class Config
    {
    public:
        Config();
        virtual ~Config();

        IResources* resourceManager;
    };
public:
    WorldLoader(const Config& config);
    virtual ~WorldLoader();

    bool loadBSP(const std::string& filename, WorldRenderer& renderer);
    bool loadEntities(const IData* bsp, const tBSPHeader& header, WorldRenderer& renderer);
    bool loadFaces(const IData* bsp, const tBSPHeader& header, WorldRenderer& renderer);
    bool loadVisiblity(const IData* bsp, const tBSPHeader& header, WorldRenderer& renderer);
    bool loadTextures(const IData* bsp, const tBSPHeader& header, WorldRenderer& renderer);
    bool loadShaders(const IData* bsp, const tBSPHeader& header, WorldRenderer& renderer);

    bool setupWorld(WorldRenderer& renderer);
    
private:
    const Config& mConfig;

    int mFaceCount;
    tBSPFace* mFaces;
    int* mShaderIndices;

    /// Creates the array accoording to the lump given. Don't forget to remove the data block!
    template <typename T>
    int loadLump(T** array, const tBSPLump& lump, const IData* data)
    {
        int count = lump.size / sizeof(T);
        *array = new T[count];
        data->read(*array, count, lump.offset);
        return count;
    }

};

#endif	/* _WORLDLOADER_H */

