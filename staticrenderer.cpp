/* 
 * File:   staticrenderer.cpp
 * Author: wouter
 * 
 * Created on April 16, 2009, 6:06 PM
 */

#include "staticrenderer.h"

#include "opengl.h"
#include "shadermanager.h"
#include <iostream>
#include <vector>

using namespace std;

#define LIST_BLOCK 32

typedef vector<int> IndexList;

class StaticRenderer::PIMPL
{
public:
    int mVertexCount;
    int mAllocVertexCount;
    tVertex *mVertices;

    int mFaceCount;
    tFace *mFaces;

    int mShaderCount;

    bool mTexturesEnabled;
    bool mLightmapEnabled;

    void renderFace(const tFace *face);
};

StaticRenderer::StaticRenderer()
    : pimpl(new StaticRenderer::PIMPL())
{
    pimpl->mVertexCount = 0;
    pimpl->mAllocVertexCount = LIST_BLOCK;
    pimpl->mVertices = new tVertex[pimpl->mAllocVertexCount];

    pimpl->mFaceCount = 0;
    pimpl->mFaces = NULL;

    pimpl->mTexturesEnabled = true;
    pimpl->mLightmapEnabled = false;
}

StaticRenderer::~StaticRenderer()
{
    if (pimpl->mVertices != NULL)
        delete[] pimpl->mVertices;

    if (pimpl->mFaces != NULL)
        delete[] pimpl->mFaces;

    delete pimpl;
}

void StaticRenderer::setupArrays()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(tVertex), &pimpl->mVertices[0].xyz[0]);

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(tVertex), pimpl->mVertices[0].st);

    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(tVertex), pimpl->mVertices[0].lslt);
}

void StaticRenderer::setFaceCount(int count)
{
    pimpl->mFaceCount = count;

    if (pimpl->mFaces != NULL)
        delete[] pimpl->mFaces;
    pimpl->mFaces = NULL;

    if (pimpl->mFaceCount > 0)
        pimpl->mFaces = new tFace[pimpl->mFaceCount];
}

int StaticRenderer::getFaceCount() const
{
    return pimpl->mFaceCount;
}

void StaticRenderer::setShaderCount(int count)
{
    pimpl->mShaderCount = count;
}

int StaticRenderer::getShaderCount() const
{
    return pimpl->mShaderCount;
}

void StaticRenderer::addVertex(const tVertex &vertex)
{
    if (pimpl->mVertexCount + 1 > pimpl->mAllocVertexCount)
    {
        pimpl->mAllocVertexCount += LIST_BLOCK;
        tVertex *tmp = new tVertex[pimpl->mAllocVertexCount];
        for (int i = 0; i < pimpl->mVertexCount; i++)
            tmp[i] = pimpl->mVertices[i];
        delete pimpl->mVertices;
        pimpl->mVertices = tmp;
    }
    pimpl->mVertices[pimpl->mVertexCount++] = vertex;
}

int StaticRenderer::getCurrentVertex() const
{
    return pimpl->mVertexCount;
}

void StaticRenderer::setFace(const tFace &face, int index)
{
    if (index >= 0 && index < pimpl->mFaceCount)
    {
        pimpl->mFaces[index] = face;
    }
}

const tFace &StaticRenderer::getFace(int index) const
{
    return pimpl->mFaces[index];
}

void StaticRenderer::renderAllFaces()
{
    ShaderManager *shaderManager = ShaderManager::singleton();
    for (int i = 0; i < pimpl->mFaceCount; i++)
    {
        tFace &face = pimpl->mFaces[i];
        shaderManager->useShader(face.shaderIndex);
        pimpl->renderFace(&face);
    }
}

void StaticRenderer::renderVisibleFaces(bool visibleFaces[])
{
    ShaderManager *shaderManager = ShaderManager::singleton();
    for (int i = 0; i < pimpl->mFaceCount; i++)
    {
        if (visibleFaces[i])
        {
            tFace &face = pimpl->mFaces[i];
            shaderManager->useShader(face.shaderIndex);
            pimpl->renderFace(&face);
        }
    }
}

void StaticRenderer::renderFace(int face)
{
    ShaderManager::singleton()->useShader(pimpl->mFaces[face].shaderIndex);

    pimpl->renderFace(&pimpl->mFaces[face]);
}

void StaticRenderer::PIMPL::renderFace(const tFace *face)
{
    if (mLightmapEnabled)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, face->lightmap->glIndex);
    }
    glDrawArrays(face->faceType, face->firstVertex, face->vertexCount);
}

void StaticRenderer::enableTextures(bool enable)
{
    pimpl->mTexturesEnabled = enable;
}

void StaticRenderer::enableLightmaps(bool enable)
{
    pimpl->mLightmapEnabled = enable;
}

void StaticRenderer::textureRenderSetup()
{
    glActiveTexture(GL_TEXTURE0);
    if (pimpl->mTexturesEnabled)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    if (pimpl->mLightmapEnabled)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);
}

bool StaticRenderer::testFaceVisibility(const tFace &face)
{
    const tShader *shader = ShaderManager::singleton()->getShader(face.shaderIndex);
    if (shader->flags & SHADER_FLAG_SPECIALTEXTURE)
        return false;

    return true;
}
