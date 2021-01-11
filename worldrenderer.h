/* 
 * File:   worldrenderer.h
 * Author: wouter
 *
 * Created on April 21, 2009, 9:41 AM
 */

#ifndef _WORLDRENDERER_H
#define	_WORLDRENDERER_H

#include "staticrenderer.h"
#include "entitymanager.h"
#include "shadermanager.h"
#include "skyrenderer.h"
#include "types.h"
#include "interfaces.h"

class WorldRenderer
{
    friend class WorldLoader;

public:
    class Config
    {
    public:
        Config();
        virtual ~Config();

        float mViewPoint[3];
        IFrustum* mFrustum;
        
    } mConfig;
public:
    WorldRenderer();
    virtual ~WorldRenderer();

    void setup();
    void render(const float position[3]);
    void cleanup();
    
    int findLeaf(float position[3], tBSPPlane* plane = NULL, float offset = 0);
    bool testFaceVisibility(int faceIndex);

    void getPlayerStart(float position[3]) const;

private:
    StaticRenderer mStaticRenderer;
    SkyRenderer mSkyRenderer;
    EntityManager mEntityManager;
    ShaderManager& mShaderManager;

    tBSPIndexList* mPvs;
    tBSPIndexList* mEntityPvs;

    int mLeafCount;
    tBSPLeaf* mLeafs;

    int mPlaneCount;
    tBSPPlane* mPlanes;

    int mModelCount;
    tBSPModel* mModels;

    int mNodeCount;
    tBSPNode* mNodes;

    int mMarkSurfaceCount;
    unsigned short* mMarkSurfaces;

};

#endif	/* _WORLDRENDERER_H */

