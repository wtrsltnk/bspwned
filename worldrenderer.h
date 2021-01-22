/* 
 * File:   worldrenderer.h
 * Author: wouter
 *
 * Created on April 21, 2009, 9:41 AM
 */

#ifndef _WORLDRENDERER_H
#define _WORLDRENDERER_H

#include "entitymanager.h"
#include "interfaces.h"
#include "shadermanager.h"
#include "skyrenderer.h"
#include "staticrenderer.h"
#include "types.h"

#include <glm/glm.hpp>

class WorldRenderer
{
    friend class WorldLoader;

public:
    class Config
    {
    public:
        Config();
        virtual ~Config();

        glm::vec3 _viewPoint;
        IFrustum *_frustum;

    } _config;

public:
    WorldRenderer(
        ShaderManager &shaderManager);

    virtual ~WorldRenderer();

    void setup();

    void render(
        const glm::vec3 &position);

    void cleanup();

    int findLeaf(
        const glm::vec3 &position,
        float offset = 0);

    bool testFaceVisibility(
        int faceIndex);

    glm::vec3 getPlayerStart() const;

private:
    StaticRenderer _staticRenderer;
    SkyRenderer _skyRenderer;
    EntityManager _entityManager;
    ShaderManager &_shaderManager;

    tBSPIndexList *_pvs = nullptr;
    tBSPIndexList *_entityPvs = nullptr;

    int _leafCount = 0;
    tBSPLeaf *_leafs = nullptr;

    int _planeCount = 0;
    tBSPPlane *_planes = nullptr;

    int _modelCount = 0;
    tBSPModel *_models = nullptr;

    int _nodeCount = 0;
    tBSPNode *_nodes = nullptr;

    int _markSurfaceCount = 0;
    unsigned short *_markSurfaces = nullptr;

    bool *_visibleFaceIndices = nullptr;
};

#endif /* _WORLDRENDERER_H */
