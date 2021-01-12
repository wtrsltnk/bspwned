/* 
* File:   worldloader.cpp
* Author: wouter
* 
* Created on April 17, 2009, 1:46 AM
*/

#include "worldloader.h"

#include "lightmap.h"
#include "math3d.h"
#include "opengl.h"
#include "stringcompare.h"
#include "textureloader.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

bool boundingBoxesCollide(float mins1[3], float maxs1[3], short mins2[3], short maxs2[3]);

WorldLoader::WorldLoader(
    const Config &config)
    : mConfig(config),
      mFaceCount(0),
      mFaces(nullptr),
      mShaderIndices(nullptr)
{
}

WorldLoader::~WorldLoader()
{
    if (this->mFaces != nullptr)
    {
        delete[] mFaces;
    }

    if (this->mShaderIndices != nullptr)
    {
        delete[] mShaderIndices;
    }
}

bool WorldLoader::loadBSP(
    const std::string &filename,
    WorldRenderer &renderer)
{
    if (this->mConfig.resourceManager == nullptr)
    {
        setError("No resource manager to load resources from");
        return false;
    }

    IData *bsp = this->mConfig.resourceManager->openFile(filename);

    if (bsp == nullptr)
    {
        setError("Cannot load file");
        return false;
    }

    tBSPHeader *header = (tBSPHeader *)(bsp->Data());

    // We only support BSP files version 30
    if (header->signature != HL1_BSP_SIGNATURE)
    {
        setError("The file is not compatible");
        this->mConfig.resourceManager->closeFile(bsp);
        return false;
    }

    // Load some basic data from the bsp that is necessary for face parsing
    renderer.mPlaneCount = loadLump(&renderer.mPlanes, header->lumps[HL1_BSP_PLANELUMP], bsp);
    renderer.mLeafCount = loadLump(&renderer.mLeafs, header->lumps[HL1_BSP_LEAFLUMP], bsp);
    renderer.mModelCount = loadLump(&renderer.mModels, header->lumps[HL1_BSP_MODELLUMP], bsp);
    renderer.mNodeCount = loadLump(&renderer.mNodes, header->lumps[HL1_BSP_NODELUMP], bsp);
    renderer.mMarkSurfaceCount = loadLump(&renderer.mMarkSurfaces, header->lumps[HL1_BSP_MARKSURFACELUMP], bsp);

    // Loading and parsing entity data can be one of the first things to do because it is not dependent on anything
    if (!loadEntities(bsp, header[0], renderer))
    {
        this->mConfig.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure the entity data is loaded and parsed before we open the textures (to be able to read textures from wad files)
    if (!loadTextures(bsp, header[0], renderer))
    {
        this->mConfig.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure the sahders are loaded before the faces, but after the textures and entities are loaded
    if (!loadShaders(bsp, header[0], renderer))
    {
        this->mConfig.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure the textures are loaded before we start loading faces
    if (!loadFaces(bsp, header[0], renderer))
    {
        this->mConfig.resourceManager->closeFile(bsp);
        return false;
    }

    // Load and decompress visiblity data
    if (!loadVisiblity(bsp, header[0], renderer))
    {
        this->mConfig.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure we close the file
    this->mConfig.resourceManager->closeFile(bsp);

    return true;
}

bool WorldLoader::loadEntities(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // Create a buffer and load the entuty data from the bsp into this buffer
    unsigned char *data = new unsigned char[header.lumps[HL1_BSP_ENTITYLUMP].size];
    bsp->read(data, header.lumps[HL1_BSP_ENTITYLUMP].size, header.lumps[HL1_BSP_ENTITYLUMP].offset);

    // Load the face data from the BSP in a buffer
    this->mFaceCount = loadLump(&this->mFaces, header.lumps[HL1_BSP_FACELUMP], bsp);
    this->mShaderIndices = new int[this->mFaceCount];

    // Parse the raw entity data
    renderer.mEntityManager.parseFromBSPEntityData(data, header.lumps[HL1_BSP_ENTITYLUMP].size);

    // Cleanup
    delete[] data;
    return true;
}

bool WorldLoader::loadTextures(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // We need a texture loader who can create texture object from bsp texture data for us
    TextureLoader tl(this->mConfig.resourceManager, &renderer.mEntityManager);

    // Create a buffer and load the texture data from the bsp into this buffer
    unsigned char *buffer = new unsigned char[header.lumps[HL1_BSP_TEXTURELUMP].size];
    bsp->read(buffer, header.lumps[HL1_BSP_TEXTURELUMP].size, header.lumps[HL1_BSP_TEXTURELUMP].offset);

    int textureCount = ((int *)buffer)[0];
    // The first integer in the buffer contains the number of textures, which we use to build a texture array with
    renderer.mShaderManager.setTextureCount(textureCount);

    // After the first integer, there follow the offsets for each texture header in the BSP file
    int *textureOffsets = &((int *)buffer)[1];

    // Parse the texture data into nice texture objects ready for use by OpenGL
    for (int i = 0; i < textureCount; i++)
    {
        Texture *texture = renderer.mShaderManager.getTexture(i);
        tl.getWadTexture(*texture, buffer + textureOffsets[i]);
    }

    tl.getSkyTextures(renderer.mSkyRenderer.mTextures);

    // Cleanup
    delete[] buffer;
    return true;
}

bool WorldLoader::loadShaders(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // Load the tex info data from the BSP in a buffer
    tBSPTexInfo *texinfos = nullptr;
    loadLump(&texinfos, header.lumps[HL1_BSP_TEXINFOLUMP], bsp);

    // Parse all the value-key pairs in the entity to find render properties for models
    for (int i = 0; i < renderer.mEntityManager.getEntityCount(); i++)
    {
        int modelIndex = -1;
        float fxAmount = 1.0f;
        float fxColor[3] = {1.0f, 1.0f, 1.0f};
        int fxMode = 4;
        float origin[3] = {0};
        const tEntity *entity = renderer.mEntityManager.getEntity(i);

        for (int j = 0; j < entity->valueCount; j++)
        {
            str key = {entity->values[j].key};

            // Save the values of some variables that are needed for rendering
            if (key == "classname")
            {
                str value = {entity->values[j].value};
                if (value == "worldspawn")
                {
                    modelIndex = 0;
                }
            }
            if (key == "model")
                sscanf(entity->values[j].value, "*%d", &modelIndex);
            if (key == "origin")
                sscanf(entity->values[j].value, "%f %f %f", &origin[0], &origin[1], &origin[2]);
            if (key == "rendermode")
                sscanf(entity->values[j].value, "%d", &fxMode);
            if (key == "rendercolor")
            {
                int r, g, b;
                sscanf(entity->values[j].value, "%d %d %d", &r, &g, &b);
                if (r || g || b)
                {
                    fxColor[0] = (float)r / 255.0f;
                    fxColor[1] = (float)g / 255.0f;
                    fxColor[2] = (float)b / 255.0f;
                }
            }
            if (key == "renderamt")
            {
                int renderamt;
                sscanf(entity->values[j].value, "%d", &renderamt);
                fxAmount = (float)renderamt / 255.0f;
            }
        }
        // If there is a model index found within the entity variables, save these model properties in the static redenrer
        if (modelIndex != -1)
        {
            tBSPModel &model = renderer.mModels[modelIndex];
            for (int i = 0; i < model.faceCount; i++)
            {
                tBSPFace &bspFace = this->mFaces[model.firstFace + i];
                tBSPTexInfo &texinfo = texinfos[bspFace.texinfo];

                this->mShaderIndices[model.firstFace + i] = renderer.mShaderManager.addShader(texinfo.miptexIndex, fxMode, fxAmount, fxColor);
            }
        }
    }
    return true;
}

bool WorldLoader::loadFaces(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // Load the face data from the BSP in a buffer
    tBSPFace *faces = nullptr;
    int faceCount = loadLump(&faces, header.lumps[HL1_BSP_FACELUMP], bsp);
    renderer.mStaticRenderer.setFaceCount(faceCount);

    // Load the tex info data from the BSP in a buffer
    tBSPTexInfo *texinfos = nullptr;
    loadLump(&texinfos, header.lumps[HL1_BSP_TEXINFOLUMP], bsp);

    // Load the vertex data from the BSP in a buffer
    tBSPVertex *vertices = nullptr;
    loadLump(&vertices, header.lumps[HL1_BSP_VERTEXLUMP], bsp);

    // Load the surfedges data from the BSP in a buffer
    int *surfedges = nullptr;
    loadLump(&surfedges, header.lumps[HL1_BSP_SURFEDGELUMP], bsp);

    // Load the edge data from the BSP in a buffer
    tBSPEdge *edges = nullptr;
    loadLump(&edges, header.lumps[HL1_BSP_EDGELUMP], bsp);

    // Load the light data from the BSP in a buffer
    unsigned char *lightData = nullptr;
    loadLump(&lightData, header.lumps[HL1_BSP_LIGHTINGLUMP], bsp);

    // Setup al the faces we are going to render for this BSP
    for (int i = 0; i < faceCount; i++)
    {
        const tBSPFace bspFace = faces[i];
        const tBSPPlane bspPlane = renderer.mPlanes[bspFace.planeIndex];
        const tBSPTexInfo bspTexinfo = texinfos[bspFace.texinfo];
        tFace renderFace = {0};
        float is = 1.0f, it = 1.0f;
        float min[2], max[2];
        Texture *texture = renderer.mShaderManager.getTexture(bspTexinfo.miptexIndex);

        // Regular face attributes we can just copy or set
        renderFace.planeSide = bspFace.side;
        renderFace.planeNormal[0] = bspPlane.normal[0];
        renderFace.planeNormal[1] = bspPlane.normal[1];
        renderFace.planeNormal[2] = bspPlane.normal[2];
        renderFace.planeDistance = bspPlane.distance;
        renderFace.firstVertex = renderer.mStaticRenderer.getCurrentVertex();
        renderFace.vertexCount = bspFace.edgeCount;
        renderFace.faceFlags = bspTexinfo.flags;
        renderFace.shaderIndex = this->mShaderIndices[i];

        // Determine the type of this face. This is GL_TRIANGLES, GL_QUADS or GL_POLYGON
        switch (bspFace.edgeCount)
        {
            // TODO: make this from defines
            case 3:
                renderFace.faceType = 4;
                break;
            case 4:
                renderFace.faceType = 7;
                break;
            default:
                renderFace.faceType = 9;
                break;
        }

        // Use the texture(if available) to determine some necessary information
        if (texture != nullptr)
        {
            is = 1.0f / float(texture->width);
            it = 1.0f / float(texture->height);
        }

        // Load the light map for this face and calculate variable for texcoord
        CalcSurfaceExtents(bspFace, bspTexinfo, surfedges, edges, vertices, min, max);
        renderFace.lightmap = ComputeLightmap(bspFace, min, max, lightData);

        // Itterate through all the edges for this face and put the vertices in the static renderer vertex array
        for (int e = 0; e < bspFace.edgeCount; e++)
        {
            tVertex vertex = {0};
            int edgeIndex = surfedges[bspFace.firstEdge + e];
            if (edgeIndex < 0)
            {
                edgeIndex = -edgeIndex;
                const tBSPEdge &edge = edges[edgeIndex];
                vertex.xyz[0] = vertices[edge.vertex[1]].point[0];
                vertex.xyz[1] = vertices[edge.vertex[1]].point[1];
                vertex.xyz[2] = vertices[edge.vertex[1]].point[2];
            }
            else
            {
                const tBSPEdge &edge = edges[edgeIndex];
                vertex.xyz[0] = vertices[edge.vertex[0]].point[0];
                vertex.xyz[1] = vertices[edge.vertex[0]].point[1];
                vertex.xyz[2] = vertices[edge.vertex[0]].point[2];
            }

            // Compute the texture coordinates
            float s = DotProduct(vertex.xyz, bspTexinfo.vecs[0]) + bspTexinfo.vecs[0][3];
            float t = DotProduct(vertex.xyz, bspTexinfo.vecs[1]) + bspTexinfo.vecs[1][3];
            vertex.st[0] = s * is;
            vertex.st[1] = t * it;

            // Compute the lightmap texture coordinates
            float midPolyS = (min[0] + max[0]) / 2.0f;
            float midPolyT = (min[1] + max[1]) / 2.0f;
            float midTexS = (float)renderFace.lightmap->width / 2.0f;
            float midTexT = (float)renderFace.lightmap->height / 2.0f;
            float lightmapS = (midTexS + (s - midPolyS) / 16.0f) / float(renderFace.lightmap->width);
            float lightmapT = (midTexT + (t - midPolyT) / 16.0f) / float(renderFace.lightmap->height);
            vertex.lslt[0] = lightmapS;
            vertex.lslt[1] = lightmapT;

            // Add the vertex to the static renderer vertex array
            renderer.mStaticRenderer.addVertex(vertex);
        }
        // Add the face information to the face list in th static renderer
        renderer.mStaticRenderer.setFace(renderFace, i);
    }

    for (int i = 0; i < renderer.mModelCount; i++)
    {
        tBSPModel &model = renderer.mModels[i];
        for (int j = 0; j < model.faceCount; j++)
        {
            tFace face = renderer.mStaticRenderer.getFace(model.firstFace + j);
            face.modelIndex = i;
            renderer.mStaticRenderer.setFace(face, model.firstFace + j);
        }
    }

    // Cleanup used data
    delete[] faces;
    delete[] texinfos;
    delete[] vertices;
    delete[] surfedges;
    delete[] edges;

    return true;
}

bool WorldLoader::loadVisiblity(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // Load the visibility data into a temporary buffer
    unsigned char *buffer = nullptr;
    loadLump(&buffer, header.lumps[HL1_BSP_VISIBILITYLUMP], bsp);

    // Create the Possible Visible Set list
    renderer.mPvs = new tBSPIndexList[renderer.mLeafCount];
    renderer.mEntityPvs = new tBSPIndexList[renderer.mLeafCount];

    // Iterate through all the leafs for the correct visiblity information
    for (int i = 0; i < renderer.mLeafCount; i++)
    {
        vector<int> leafList;
        vector<int> modelList;
        int visOffset = renderer.mLeafs[i].visofs;

        // Decompress the visibility
        for (int j = 1; j < renderer.mModels[0].visLeafs; visOffset++)
        {
            if (buffer[visOffset] == 0)
            {
                visOffset++;
                j += (buffer[visOffset] << 3);
            }
            else
            {
                for (unsigned char bit = 1; bit; bit <<= 1, j++)
                {
                    if (buffer[visOffset] & bit)
                        leafList.push_back(j);
                }
            }
        }

        // When a vector is collected, create the indexlist and fill it with the indices gather in the previous step
        if (leafList.size() > 0)
        {
            renderer.mPvs[i].size = leafList.size();
            renderer.mPvs[i].indices = new int[renderer.mPvs[i].size];
            for (int j = 0; j < renderer.mPvs[i].size; j++)
                renderer.mPvs[i].indices[j] = leafList[j];
        }
        else
        {
            renderer.mPvs[i].size = 0;
            renderer.mPvs[i].indices = nullptr;
        }

        // Find all the indices of the models colliding this leaf
        for (int j = 1; j < renderer.mModelCount; j++)
        {
            if (boundingBoxesCollide(renderer.mModels[j].mins, renderer.mModels[j].maxs, renderer.mLeafs[i].mins, renderer.mLeafs[i].maxs))
                modelList.push_back(j);
        }

        // When a vector is collected, create the indexlist and fill it with the indices gather in the previous step
        if (modelList.size() > 0)
        {
            renderer.mEntityPvs[i].size = modelList.size();
            renderer.mEntityPvs[i].indices = new int[renderer.mEntityPvs[i].size];
            for (int j = 0; j < renderer.mEntityPvs[i].size; j++)
                renderer.mEntityPvs[i].indices[j] = modelList[j];
        }
        else
        {
            renderer.mEntityPvs[i].size = 0;
            renderer.mEntityPvs[i].indices = nullptr;
        }
    }

    // Cleanup out sjit
    delete[] buffer;

    return true;
}

bool WorldLoader::setupWorld(
    WorldRenderer &renderer)
{
    // Upload all the textures for this BSP in the multi texture layer 0
    glActiveTexture(GL_TEXTURE0);
    for (int i = 0; i < renderer.mShaderManager.getTextureCount(); i++)
    {
        Texture *texture = renderer.mShaderManager.getTexture(i);
        //texture->mipmapping = true;
        texture->glIndex = this->mConfig.resourceManager->addTexture(*texture);
        delete[] texture->data[0];
        texture->data[0] = nullptr;
    }

    for (int i = 0; i < 6; i++)
    {
        renderer.mSkyRenderer.mTextures[i].glIndex = this->mConfig.resourceManager->addTexture(renderer.mSkyRenderer.mTextures[i]);
        delete[] renderer.mSkyRenderer.mTextures[i].data[0];
        renderer.mSkyRenderer.mTextures[i].data[0] = nullptr;
    }

    // Upload all the lightmaps for all the faces in this BSP in the multi texture layer 1
    glActiveTexture(GL_TEXTURE1);
    // This makes sure the lightmap tetxures are packed right. I think that is because the lightmaps have weird sizes
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (int i = 0; i < renderer.mStaticRenderer.getFaceCount(); i++)
    {
        const tFace &face = renderer.mStaticRenderer.getFace(i);
        face.lightmap->glIndex = this->mConfig.resourceManager->addTexture(*face.lightmap);
    }

    renderer.setup();

    return true;
}

WorldLoader::Config::Config()
    : resourceManager(nullptr)
{
}

WorldLoader::Config::~Config()
{
}

bool boundingBoxesCollide(float mins1[3], float maxs1[3], short mins2[3], short maxs2[3])
{
    if (mins1[0] > maxs2[0] || maxs1[0] < mins2[0]) return false;
    if (mins1[1] > maxs2[1] || maxs1[1] < mins2[1]) return false;
    if (mins1[2] > maxs2[2] || maxs1[2] < mins2[2]) return false;

    return true;
}
