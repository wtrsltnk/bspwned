/* 
 * File:   worldrenderer.cpp
 * Author: wouter
 * 
 * Created on April 21, 2009, 9:40 AM
 */

#include "worldrenderer.h"

#include "opengl.h"
#include "math3d.h"
#include <string.h>

void RenderBoundingBox(const short mins[], const short maxs[]);

WorldRenderer::WorldRenderer()
        : mShaderManager(*ShaderManager::createSingleton()), mPvs(NULL), mEntityPvs(NULL), mLeafs(NULL), mPlanes(NULL), mModels(NULL), mNodes(NULL), mMarkSurfaces(NULL)
{
}

WorldRenderer::~WorldRenderer()
{
    if (mPvs != NULL) delete []mPvs;
    if (mEntityPvs != NULL) delete []mEntityPvs;
    if (mLeafs != NULL) delete []mLeafs;
    if (mPlanes != NULL) delete []mPlanes;
    if (mModels != NULL) delete []mModels;
    if (mNodes != NULL) delete []mNodes;
    if (mMarkSurfaces != NULL) delete []mMarkSurfaces;
    ShaderManager::destroySingleton();
}

void WorldRenderer::setup()
{
    this->mStaticRenderer.enableTextures(true);
    this->mStaticRenderer.enableLightmaps(true);
    
    this->mStaticRenderer.setupArrays();
}

void WorldRenderer::render(const float position[3])
{
    mSkyRenderer.render(position);

    glEnable(GL_DEPTH_TEST);
    mStaticRenderer.textureRenderSetup();
    
    static int lastLeaf = -1;
    bool* visibleFaceIndices = new bool[this->mStaticRenderer.getFaceCount()];
    memset(visibleFaceIndices, 0, sizeof(bool) * this->mStaticRenderer.getFaceCount());

    // Find the current leaf of the camera is in
    int currentLeaf = findLeaf(this->mConfig.mViewPoint);
    int faceCount = 0;

    if (currentLeaf != 0)
    {
        // Iterate through all the leafs from the current leaf's possible visible set
        for (int j = 0; j < this->mPvs[currentLeaf].size; j++)
        {
            const tBSPLeaf& leaf = this->mLeafs[this->mPvs[currentLeaf].indices[j]];

            // Cull the leaf from the visible list if it is out the frustum
            if (this->mConfig.mFrustum != NULL)
            {
                float mins[3], maxs[3];
                mins[0] = leaf.mins[0]; mins[1] = leaf.mins[1]; mins[2] = leaf.mins[2];
                maxs[0] = leaf.maxs[0]; maxs[1] = leaf.maxs[1]; maxs[2] = leaf.maxs[2];
                if (this->mConfig.mFrustum->cullBoundingBox(mins, maxs))
                    continue;
            }

            // Iterate all the faces belonging to the leaf from the PVS
            for (unsigned short i = 0; i < leaf.markSurfacesCount; i++)
            {
                unsigned short faceIndex = this->mMarkSurfaces[leaf.firstMarkSurface + i];
                // Test the face for visiblity
                if (testFaceVisibility(faceIndex))
                {
                    visibleFaceIndices[faceIndex] = true;
                    faceCount++;
                }
            }
        }

        for (int i = 1; i < this->mModelCount; i++)
        {
            tBSPModel& model = this->mModels[i];
            for (int k = 0; k < model.faceCount; k++)
            {
                // Test the face for visiblity
                if (testFaceVisibility(model.firstFace + k))
                {
                    visibleFaceIndices[model.firstFace + k] = true;
                }
            }
        }
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();

    // Render all faces if there is no current leaf else render only the visible from the PVS
    if (currentLeaf == 0)
        this->mStaticRenderer.renderAllFaces();
    else
        this->mStaticRenderer.renderVisibleFaces(visibleFaceIndices);

    glPopMatrix();

    // Cleanup
    delete []visibleFaceIndices;
    lastLeaf = currentLeaf;
}

void WorldRenderer::cleanup()
{
}

int WorldRenderer::findLeaf(float position[3], tBSPPlane* plane, float offset)
{
    int i = this->mModels[0].headnode[0];

    while(i >= 0)
    {
        const tBSPNode& node = this->mNodes[i];
        const tBSPPlane& plane = this->mPlanes[node.planeIndex];

        float distance;

        if(plane.type <= PLANE_Z)  // easier for axial planes
        {
            distance = position[plane.type] - plane.distance - offset;
        }
        else
        {
            // f(x,y,z) = Ax+Ay+Az-D
            distance =  plane.normal[0] * position[0]
                       +plane.normal[1] * position[1]
                       +plane.normal[2] * position[2]
                       -plane.distance - offset;
        }

        if(distance >= 0)
            i = node.children[0];
        else
            i = node.children[1];
    }

    return -(i+1);
}

bool WorldRenderer::testFaceVisibility(int faceIndex)
{
    const tFace& face = this->mStaticRenderer.getFace(faceIndex);

    // Determine the distance to the face from the camera viewport
    float distance = DotProduct(mConfig.mViewPoint, face.planeNormal) - face.planeDistance;

    // Skip the face when we see its back
    if (face.planeSide && distance > 0)
        return false;
    else if (!face.planeSide && distance < 0)
        return false;

    return StaticRenderer::testFaceVisibility(face);
}

void WorldRenderer::getPlayerStart(float position[3]) const
{
    mEntityManager.getPlayerStart(position);
}

WorldRenderer::Config::Config()
        : mFrustum(NULL)
{
    this->mViewPoint[0] = this->mViewPoint[1] = this->mViewPoint[2] = 0.0f;
}

WorldRenderer::Config::~Config()
{
}

void RenderBoundingBox(const short mins[], const short maxs[])
{
    glBegin(GL_LINES);
    // bottom
    glVertex3s(mins[0], mins[1], mins[2]);
    glVertex3s(maxs[0], mins[1], mins[2]);

    glVertex3s(maxs[0], mins[1], mins[2]);
    glVertex3s(maxs[0], maxs[1], mins[2]);

    glVertex3s(maxs[0], maxs[1], mins[2]);
    glVertex3s(mins[0], maxs[1], mins[2]);

    glVertex3s(mins[0], maxs[1], mins[2]);
    glVertex3s(mins[0], mins[1], mins[2]);
    // top
    glVertex3s(mins[0], maxs[1], maxs[2]);
    glVertex3s(maxs[0], maxs[1], maxs[2]);

    glVertex3s(maxs[0], maxs[1], maxs[2]);
    glVertex3s(maxs[0], mins[1], maxs[2]);

    glVertex3s(maxs[0], mins[1], maxs[2]);
    glVertex3s(mins[0], mins[1], maxs[2]);

    glVertex3s(mins[0], mins[1], maxs[2]);
    glVertex3s(mins[0], maxs[1], maxs[2]);

    //sides
    glVertex3s(mins[0], maxs[1], mins[2]);
    glVertex3s(mins[0], maxs[1], maxs[2]);

    glVertex3s(maxs[0], maxs[1], mins[2]);
    glVertex3s(maxs[0], maxs[1], maxs[2]);

    glVertex3s(maxs[0], mins[1], mins[2]);
    glVertex3s(maxs[0], mins[1], maxs[2]);

    glVertex3s(mins[0], mins[1], mins[2]);
    glVertex3s(mins[0], mins[1], maxs[2]);
    glEnd();
}
