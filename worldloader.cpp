/* 
* File:   worldloader.cpp
* Author: wouter
* 
* Created on April 17, 2009, 1:46 AM
*/

#include "worldloader.h"

#include "lightmap.h"
#include "opengl.h"
#include "textureloader.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

bool boundingBoxesCollide(
    float mins1[3],
    float maxs1[3],
    short mins2[3],
    short maxs2[3]);

WorldLoader::WorldLoader(
    const Config &config)
    : _config(config),
      _faceCount(0),
      _faces(nullptr),
      _shaderIndices(nullptr)
{
}

WorldLoader::~WorldLoader()
{
    if (this->_faces != nullptr)
    {
        delete[] _faces;
    }

    if (this->_shaderIndices != nullptr)
    {
        delete[] _shaderIndices;
    }
}

bool WorldLoader::loadBSP(
    const std::string &filename,
    WorldRenderer &renderer)
{
    if (this->_config.resourceManager == nullptr)
    {
        spdlog::error("No resource manager to load resources from");

        return false;
    }

    IData *bsp = this->_config.resourceManager->openFile(filename);

    if (bsp == nullptr)
    {
        spdlog::error("Cannot load file");

        return false;
    }

    tBSPHeader *header = (tBSPHeader *)(bsp->Data());

    // We only support BSP files version 30
    if (header->signature != HL1_BSP_SIGNATURE)
    {
        spdlog::error("The file is not compatible");

        this->_config.resourceManager->closeFile(bsp);

        return false;
    }

    // Load some basic data from the bsp that is necessary for face parsing
    renderer._planeCount = loadLump(&renderer._planes, header->lumps[HL1_BSP_PLANELUMP], bsp);
    renderer._leafCount = loadLump(&renderer._leafs, header->lumps[HL1_BSP_LEAFLUMP], bsp);
    renderer._modelCount = loadLump(&renderer._models, header->lumps[HL1_BSP_MODELLUMP], bsp);
    renderer._nodeCount = loadLump(&renderer._nodes, header->lumps[HL1_BSP_NODELUMP], bsp);
    renderer._markSurfaceCount = loadLump(&renderer._markSurfaces, header->lumps[HL1_BSP_MARKSURFACELUMP], bsp);

    // Loading and parsing entity data can be one of the first things to do because it is not dependent on anything
    if (!loadEntities(bsp, header[0], renderer))
    {
        this->_config.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure the entity data is loaded and parsed before we open the textures (to be able to read textures from wad files)
    if (!loadTextures(bsp, header[0], renderer))
    {
        this->_config.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure the sahders are loaded before the faces, but after the textures and entities are loaded
    if (!loadShaders(bsp, header[0], renderer))
    {
        this->_config.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure the textures are loaded before we start loading faces
    if (!loadFaces(bsp, header[0], renderer))
    {
        this->_config.resourceManager->closeFile(bsp);
        return false;
    }

    // Load and decompress visiblity data
    if (!loadVisiblity(bsp, header[0], renderer))
    {
        this->_config.resourceManager->closeFile(bsp);
        return false;
    }

    // Make sure we close the file
    this->_config.resourceManager->closeFile(bsp);

    return true;
}

bool WorldLoader::loadEntities(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // Load the face data from the BSP in a buffer
    this->_faceCount = loadLump(&this->_faces, header.lumps[HL1_BSP_FACELUMP], bsp);
    this->_shaderIndices = new int[this->_faceCount];

    // Create a buffer and load the entuty data from the bsp into this buffer
    auto data = loadLump<unsigned char>(header.lumps[HL1_BSP_ENTITYLUMP], bsp);

    // Parse the raw entity data
    renderer._entityManager.parseFromBSPEntityData(data, header.lumps[HL1_BSP_ENTITYLUMP].size);

    return true;
}

bool WorldLoader::loadTextures(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // We need a texture loader who can create texture object from bsp texture data for us
    TextureLoader tl(this->_config.resourceManager, &renderer._entityManager);

    // Create a buffer and load the texture data from the bsp into this buffer
    auto buffer = loadLump<unsigned char>(header.lumps[HL1_BSP_TEXTURELUMP], bsp);

    int textureCount = ((int *)buffer.get())[0];
    // The first integer in the buffer contains the number of textures, which we use to build a texture array with
    renderer._shaderManager.SetTextureCount(textureCount);

    // After the first integer, there follow the offsets for each texture header in the BSP file
    int *textureOffsets = &((int *)buffer.get())[1];

    // Parse the texture data into nice texture objects ready for use by OpenGL
    for (int i = 0; i < textureCount; i++)
    {
        Texture *texture = renderer._shaderManager.GetTexture(i);
        tl.getWadTexture(*texture, buffer.get() + textureOffsets[i]);
    }

    tl.getSkyTextures(renderer._skyRenderer.mTextures);

    return true;
}

bool WorldLoader::loadShaders(
    const IData *bsp,
    const tBSPHeader &header,
    WorldRenderer &renderer)
{
    // Load the tex info data from the BSP in a buffer
    auto texinfos = loadLump<tBSPTexInfo>(header.lumps[HL1_BSP_TEXINFOLUMP], bsp);

    // Parse all the value-key pairs in the entity to find render properties for models
    for (auto &entity : renderer._entityManager.getEntities())
    {
        int modelIndex = -1;
        float fxAmount = 1.0f;
        float fxColor[3] = {1.0f, 1.0f, 1.0f};
        int fxMode = 4;
        float origin[3] = {0};

        for (auto &value : entity.values)
        {
            std::string key = value.key;

            // Save the values of some variables that are needed for rendering
            if (key == "classname")
            {
                if (value.value == "worldspawn")
                {
                    modelIndex = 0;
                }
            }
            if (key == "model")
            {
                char astrix;
                std::istringstream(value.value) >> astrix >> modelIndex;
                if (modelIndex == 0)
                {
                    modelIndex = -1;
                }
            }
            if (key == "origin")
            {
                std::istringstream(value.value) >> origin[0] >> origin[1] >> origin[2];
            }
            if (key == "rendermode")
            {
                std::istringstream(value.value) >> fxMode;
            }
            if (key == "rendercolor")
            {
                int r, g, b;
                std::istringstream(value.value) >> r >> g >> b;
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
                std::istringstream(value.value) >> renderamt;
                fxAmount = (float)renderamt / 255.0f;
            }
        }

        // If there is a model index found within the entity variables, save these model properties in the static redenrer
        if (modelIndex == -1)
        {
            continue;
        }

        tBSPModel &model = renderer._models[modelIndex];
        for (int i = 0; i < model.faceCount; i++)
        {
            tBSPFace &bspFace = this->_faces[model.firstFace + i];
            tBSPTexInfo &texinfo = texinfos.get()[bspFace.texinfo];

            this->_shaderIndices[model.firstFace + i] = renderer._shaderManager.AddShader(texinfo.miptexIndex, fxMode, fxAmount, fxColor);
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
    int faceCount = 0;
    auto faces = loadLump<tBSPFace>(header.lumps[HL1_BSP_FACELUMP], bsp, &faceCount);

    // Load the tex info data from the BSP in a buffer
    auto texinfos = loadLump<tBSPTexInfo>(header.lumps[HL1_BSP_TEXINFOLUMP], bsp);

    // Load the vertex data from the BSP in a buffer
    auto vertices = loadLump<tBSPVertex>(header.lumps[HL1_BSP_VERTEXLUMP], bsp);

    // Load the surfedges data from the BSP in a buffer
    auto surfedges = loadLump<int>(header.lumps[HL1_BSP_SURFEDGELUMP], bsp);

    // Load the edge data from the BSP in a buffer
    auto edges = loadLump<tBSPEdge>(header.lumps[HL1_BSP_EDGELUMP], bsp);

    // Load the light data from the BSP in a buffer
    auto lightData = loadLump<unsigned char>(header.lumps[HL1_BSP_LIGHTINGLUMP], bsp);

    std::vector<tVertex> loadedVertices;
    std::vector<tFace> loadedFaces;

    // Setup al the faces we are going to render for this BSP
    for (int i = 0; i < faceCount; i++)
    {
        const tBSPFace bspFace = faces.get()[i];
        const tBSPPlane bspPlane = renderer._planes[bspFace.planeIndex];
        const tBSPTexInfo bspTexinfo = texinfos.get()[bspFace.texinfo];
        tFace renderFace = {0, 0, 0, 0, 0, 0, {0, 0, 0}, 0, 0, 0, nullptr};
        float is = 1.0f, it = 1.0f;
        float min[2], max[2];
        Texture *texture = renderer._shaderManager.GetTexture(bspTexinfo.miptexIndex);

        // Regular face attributes we can just copy or set
        renderFace.planeSide = bspFace.side;
        renderFace.planeNormal[0] = bspPlane.normal[0];
        renderFace.planeNormal[1] = bspPlane.normal[1];
        renderFace.planeNormal[2] = bspPlane.normal[2];
        renderFace.planeDistance = bspPlane.distance;
        renderFace.firstVertex = loadedVertices.size();
        renderFace.vertexCount = bspFace.edgeCount;
        renderFace.faceFlags = bspTexinfo.flags;
        renderFace.shaderIndex = this->_shaderIndices[i];

        // Determine the type of this face. This is GL_TRIANGLES, GL_QUADS or GL_POLYGON
        switch (bspFace.edgeCount)
        {
            case 3:
            {
                renderFace.faceType = 4;
                break;
            }
            case 4:
            {
                renderFace.faceType = 7;
                break;
            }
            default:
            {
                renderFace.faceType = 9;
                break;
            }
        }

        // Use the texture(if available) to determine some necessary information
        if (texture != nullptr)
        {
            is = 1.0f / float(texture->width);
            it = 1.0f / float(texture->height);
        }

        // Load the light map for this face and calculate variable for texcoord
        CalcSurfaceExtents(bspFace, bspTexinfo, surfedges.get(), edges.get(), vertices.get(), min, max);
        renderFace.lightmap = ComputeLightmap(bspFace, min, max, lightData.get());

        // Itterate through all the edges for this face and put the vertices in the static renderer vertex array
        for (int e = 0; e < bspFace.edgeCount; e++)
        {
            tVertex vertex = {{0, 0, 0}, {0, 0}, {0, 0}};
            int edgeIndex = surfedges.get()[bspFace.firstEdge + e];
            if (edgeIndex < 0)
            {
                edgeIndex = -edgeIndex;
                const tBSPEdge &edge = edges.get()[edgeIndex];
                vertex.xyz[0] = vertices.get()[edge.vertex[1]].point[0];
                vertex.xyz[1] = vertices.get()[edge.vertex[1]].point[1];
                vertex.xyz[2] = vertices.get()[edge.vertex[1]].point[2];
            }
            else
            {
                const tBSPEdge &edge = edges.get()[edgeIndex];
                vertex.xyz[0] = vertices.get()[edge.vertex[0]].point[0];
                vertex.xyz[1] = vertices.get()[edge.vertex[0]].point[1];
                vertex.xyz[2] = vertices.get()[edge.vertex[0]].point[2];
            }

            // Compute the texture coordinates
            float s = glm::dot(vertex.xyz, glm::vec3(bspTexinfo.vecs[0][0], bspTexinfo.vecs[0][1], bspTexinfo.vecs[0][2])) + bspTexinfo.vecs[0][3];
            float t = glm::dot(vertex.xyz, glm::vec3(bspTexinfo.vecs[1][0], bspTexinfo.vecs[1][1], bspTexinfo.vecs[1][2])) + bspTexinfo.vecs[1][3];
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

            loadedVertices.push_back(vertex);
        }

        loadedFaces.push_back(renderFace);
    }

    if (int(loadedFaces.size()) != faceCount)
    {
        spdlog::error("numer of loaded faces is not equal to the number of faces in the bsp");

        return false;
    }

    for (int i = 0; i < renderer._modelCount; i++)
    {
        tBSPModel &model = renderer._models[i];
        for (int j = 0; j < model.faceCount; j++)
        {
            loadedFaces[model.firstFace + j].modelIndex = i;
        }
    }

    renderer._staticRenderer.setFaceAndVertexData(loadedFaces, loadedVertices);

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
    renderer._pvs = new tBSPIndexList[renderer._leafCount];
    renderer._entityPvs = new tBSPIndexList[renderer._leafCount];

    // Iterate through all the leafs for the correct visiblity information
    for (int i = 0; i < renderer._leafCount; i++)
    {
        vector<int> leafList;
        vector<int> modelList;
        int visOffset = renderer._leafs[i].visofs;

        // Decompress the visibility
        for (int j = 1; j < renderer._models[0].visLeafs; visOffset++)
        {
            if (buffer[visOffset] == 0)
            {
                visOffset++;
                j += (buffer[visOffset] << 3);

                continue;
            }

            for (unsigned char bit = 1; bit; bit <<= 1, j++)
            {
                if (buffer[visOffset] & bit)
                {
                    leafList.push_back(j);
                }
            }
        }

        // When a vector is collected, create the indexlist and fill it with the indices gather in the previous step
        if (leafList.size() > 0)
        {
            renderer._pvs[i].size = static_cast<int>(leafList.size());
            renderer._pvs[i].indices = new int[renderer._pvs[i].size];
            for (int j = 0; j < renderer._pvs[i].size; j++)
            {
                renderer._pvs[i].indices[j] = leafList[j];
            }
        }
        else
        {
            renderer._pvs[i].size = 0;
            renderer._pvs[i].indices = nullptr;
        }

        // Find all the indices of the models colliding this leaf
        for (int j = 1; j < renderer._modelCount; j++)
        {
            if (boundingBoxesCollide(renderer._models[j].mins, renderer._models[j].maxs, renderer._leafs[i].mins, renderer._leafs[i].maxs))
            {
                modelList.push_back(j);
            }
        }

        // When a vector is collected, create the indexlist and fill it with the indices gather in the previous step
        if (modelList.size() > 0)
        {
            renderer._entityPvs[i].size = static_cast<int>(modelList.size());
            renderer._entityPvs[i].indices = new int[renderer._entityPvs[i].size];
            for (int j = 0; j < renderer._entityPvs[i].size; j++)
            {
                renderer._entityPvs[i].indices[j] = modelList[j];
            }
        }
        else
        {
            renderer._entityPvs[i].size = 0;
            renderer._entityPvs[i].indices = nullptr;
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
    for (int i = 0; i < renderer._shaderManager.GetTextureCount(); i++)
    {
        Texture *texture = renderer._shaderManager.GetTexture(i);

        texture->glIndex = this->_config.resourceManager->addTexture(*texture);
        texture->ClearData();
    }

    for (int i = 0; i < 6; i++)
    {
        renderer._skyRenderer.mTextures[i].glIndex = this->_config.resourceManager->addTexture(renderer._skyRenderer.mTextures[i]);
        renderer._skyRenderer.mTextures[i].ClearData();
    }

    // Upload all the lightmaps for all the faces in this BSP in the multi texture layer 1
    glActiveTexture(GL_TEXTURE1);
    // This makes sure the lightmap tetxures are packed right. I think that is because the lightmaps have weird sizes
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (auto &face : renderer._staticRenderer.getFaces())
    {
        face.lightmap->glIndex = this->_config.resourceManager->addTexture(*face.lightmap);
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
