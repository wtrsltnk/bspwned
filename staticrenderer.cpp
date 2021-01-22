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
    _texturesEnabled = true;
    _lightmapEnabled = false;
}

StaticRenderer::~StaticRenderer()
{}

void StaticRenderer::setupArrays()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(tVertex), &_vertices[0].xyz[0]);

    glClientActiveTexture(GL_TEXTURE0);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(tVertex), &_vertices[0].st[0]);

    glClientActiveTexture(GL_TEXTURE1);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(tVertex), &_vertices[0].lslt[0]);
}

void StaticRenderer::setShaderCount(
    int count)
{
    _shaderCount = count;
}

int StaticRenderer::getShaderCount() const
{
    return _shaderCount;
}

void StaticRenderer::setFaceAndVertexData(
    const std::vector<tFace> &faces,
    const std::vector<tVertex> &vertices)
{
    _faces = faces;
    _vertices = vertices;
}

const std::vector<tFace> &StaticRenderer::getFaces() const
{
    return _faces;
}

const std::vector<tVertex> &StaticRenderer::getVertices() const
{
    return _vertices;
}

void StaticRenderer::renderAllFaces(
    ShaderManager *shaderManager)
{
    for (auto &face : _faces)
    {
        shaderManager->UseShader(face.shaderIndex);
        renderFace(shaderManager, &face);
    }
}

void StaticRenderer::renderVisibleFaces(
    ShaderManager *shaderManager,
    bool visibleFaces[])
{
    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        if (visibleFaces[i])
        {
            tFace &face = _faces[i];
            shaderManager->UseShader(face.shaderIndex);
            renderFace(shaderManager, &face);
        }
    }
}

void StaticRenderer::renderFace(
    ShaderManager *shaderManager,
    int face)
{
    shaderManager->UseShader(_faces[face].shaderIndex);

    renderFace(shaderManager, &_faces[face]);
}

void StaticRenderer::renderFace(
    ShaderManager *shaderManager,
    const tFace *face)
{
    if (_lightmapEnabled)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, face->lightmap->glIndex);
    }
    glDrawArrays(face->faceType, face->firstVertex, face->vertexCount);
}

void StaticRenderer::enableTextures(
    bool enable)
{
    _texturesEnabled = enable;
}

void StaticRenderer::enableLightmaps(
    bool enable)
{
    _lightmapEnabled = enable;
}

void StaticRenderer::textureRenderSetup()
{
    glActiveTexture(GL_TEXTURE0);
    if (_texturesEnabled)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    glActiveTexture(GL_TEXTURE1);
    if (_lightmapEnabled)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }
}

bool StaticRenderer::testFaceVisibility(
    ShaderManager *shaderManager,
    const tFace &face)
{
    const tShader *shader = shaderManager->GetShader(face.shaderIndex);

    return !(shader->flags & SHADER_FLAG_SPECIALTEXTURE);
}
