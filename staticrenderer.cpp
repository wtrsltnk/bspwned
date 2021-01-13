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

StaticRenderer::StaticRenderer()
{
    mTexturesEnabled = true;
    mLightmapEnabled = false;
}

StaticRenderer::~StaticRenderer()
{}

void StaticRenderer::setupArrays()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(tVertex), &mVertices[0].xyz[0]);

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(tVertex), &mVertices[0].st[0]);

    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(tVertex), &mVertices[0].lslt[0]);
}

void StaticRenderer::setFaceCount(
    int count)
{
    mFaces = std::vector<tFace>(count);
}

int StaticRenderer::getFaceCount() const
{
    return static_cast<int>(mFaces.size());
}

void StaticRenderer::setShaderCount(
    int count)
{
    mShaderCount = count;
}

int StaticRenderer::getShaderCount() const
{
    return mShaderCount;
}

void StaticRenderer::addVertex(
    const tVertex &vertex)
{
    mVertices.push_back(vertex);
}

int StaticRenderer::getCurrentVertex() const
{
    return static_cast<int>(mVertices.size());
}

void StaticRenderer::setFace(
    const tFace &face,
    int index)
{
    if (index >= 0 && index < getFaceCount())
    {
        mFaces[index] = face;
    }
}

const tFace &StaticRenderer::getFace(
    int index) const
{
    return mFaces[index];
}

void StaticRenderer::renderAllFaces()
{
    ShaderManager *shaderManager = ShaderManager::singleton();
    for (int i = 0; i < getFaceCount(); i++)
    {
        tFace &face = mFaces[i];
        shaderManager->UseShader(face.shaderIndex);
        renderFace(&face);
    }
}

void StaticRenderer::renderVisibleFaces(
    bool visibleFaces[])
{
    ShaderManager *shaderManager = ShaderManager::singleton();
    for (int i = 0; i < getFaceCount(); i++)
    {
        if (visibleFaces[i])
        {
            tFace &face = mFaces[i];
            shaderManager->UseShader(face.shaderIndex);
            renderFace(&face);
        }
    }
}

void StaticRenderer::renderFace(
    int face)
{
    ShaderManager::singleton()->UseShader(mFaces[face].shaderIndex);

    renderFace(&mFaces[face]);
}

void StaticRenderer::renderFace(
    const tFace *face)
{
    if (mLightmapEnabled)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, face->lightmap->glIndex);
    }
    glDrawArrays(face->faceType, face->firstVertex, face->vertexCount);
}

void StaticRenderer::enableTextures(
    bool enable)
{
    mTexturesEnabled = enable;
}

void StaticRenderer::enableLightmaps(
    bool enable)
{
    mLightmapEnabled = enable;
}

void StaticRenderer::textureRenderSetup()
{
    glActiveTexture(GL_TEXTURE0);
    if (mTexturesEnabled)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    glActiveTexture(GL_TEXTURE1);
    if (mLightmapEnabled)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}

bool StaticRenderer::testFaceVisibility(
    const tFace &face)
{
    const tShader *shader = ShaderManager::singleton()->GetShader(face.shaderIndex);

    return !(shader->flags & SHADER_FLAG_SPECIALTEXTURE);
}
