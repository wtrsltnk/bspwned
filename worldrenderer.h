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

        glm::vec3 mViewPoint;
        IFrustum *mFrustum;

    } mConfig;

public:
    WorldRenderer();

    virtual ~WorldRenderer();

    void setup();

    void render(
        const glm::vec3 &position);

    void cleanup();

    int findLeaf(
        const glm::vec3 &position,
        tBSPPlane *plane = NULL,
        float offset = 0);

    bool testFaceVisibility(
        int faceIndex);

    glm::vec3 getPlayerStart() const;

private:
    StaticRenderer mStaticRenderer;
    SkyRenderer mSkyRenderer;
    EntityManager mEntityManager;
    ShaderManager &mShaderManager;

    tBSPIndexList *mPvs;
    tBSPIndexList *mEntityPvs;

    int mLeafCount;
    tBSPLeaf *mLeafs;

    int mPlaneCount;
    tBSPPlane *mPlanes;

    int mModelCount;
    tBSPModel *mModels;

    int mNodeCount;
    tBSPNode *mNodes;

    int mMarkSurfaceCount;
    unsigned short *mMarkSurfaces;
};

#endif /* _WORLDRENDERER_H */
