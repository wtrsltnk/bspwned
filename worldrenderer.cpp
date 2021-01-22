/* 
 * File:   worldrenderer.cpp
 * Author: wouter
 * 
 * Created on April 21, 2009, 9:40 AM
 */

#include "worldrenderer.h"

#include "opengl.h"
#include <string.h>

void RenderBoundingBox(
    const short mins[],
    const short maxs[]);

WorldRenderer::WorldRenderer(
    ShaderManager &shaderManager)
    : _shaderManager(shaderManager)
{}

WorldRenderer::~WorldRenderer()
{
    if (_pvs != nullptr) delete[] _pvs;
    if (_entityPvs != nullptr) delete[] _entityPvs;
    if (_leafs != nullptr) delete[] _leafs;
    if (_planes != nullptr) delete[] _planes;
    if (_models != nullptr) delete[] _models;
    if (_nodes != nullptr) delete[] _nodes;
    if (_markSurfaces != nullptr) delete[] _markSurfaces;
    if (_visibleFaceIndices != nullptr) delete[] _visibleFaceIndices;
}

void WorldRenderer::setup()
{
    this->_staticRenderer.enableTextures(true);
    this->_staticRenderer.enableLightmaps(true);

    this->_staticRenderer.setupArrays();

    _visibleFaceIndices = new bool[_staticRenderer.getFaces().size()];
}

void WorldRenderer::render(
    const glm::vec3 &position)
{
    _skyRenderer.render(position);

    glEnable(GL_DEPTH_TEST);
    _staticRenderer.textureRenderSetup();

    static int lastLeaf = -1;
    memset(_visibleFaceIndices, 0, sizeof(bool) * _staticRenderer.getFaces().size());

    // Find the current leaf of the camera is in
    int currentLeaf = findLeaf(this->_config._viewPoint);

    if (currentLeaf != 0)
    {
        int faceCount = 0;

        // Iterate through all the leafs from the current leaf's possible visible set
        for (int j = 0; j < this->_pvs[currentLeaf].size; j++)
        {
            const tBSPLeaf &leaf = this->_leafs[this->_pvs[currentLeaf].indices[j]];

            // Cull the leaf from the visible list if it is out the frustum
            if (this->_config._frustum != NULL)
            {
                float mins[3], maxs[3];

                mins[0] = leaf.mins[0];
                mins[1] = leaf.mins[1];
                mins[2] = leaf.mins[2];
                maxs[0] = leaf.maxs[0];
                maxs[1] = leaf.maxs[1];
                maxs[2] = leaf.maxs[2];

                if (this->_config._frustum->cullBoundingBox(mins, maxs))
                {
                    continue;
                }
            }

            // Iterate all the faces belonging to the leaf from the PVS
            for (unsigned short i = 0; i < leaf.markSurfacesCount; i++)
            {
                unsigned short faceIndex = this->_markSurfaces[leaf.firstMarkSurface + i];
                // Test the face for visiblity
                if (testFaceVisibility(faceIndex))
                {
                    _visibleFaceIndices[faceIndex] = true;
                    faceCount++;
                }
            }
        }

        for (int i = 1; i < this->_modelCount; i++)
        {
            tBSPModel &model = this->_models[i];
            for (int k = 0; k < model.faceCount; k++)
            {
                // Test the face for visiblity
                if (testFaceVisibility(model.firstFace + k))
                {
                    _visibleFaceIndices[model.firstFace + k] = true;
                }
            }
        }
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();

    // Render all faces if there is no current leaf else render only the visible from the PVS
    if (currentLeaf == 0)
    {
        this->_staticRenderer.renderAllFaces(&_shaderManager);
    }
    else
    {
        this->_staticRenderer.renderVisibleFaces(&_shaderManager, _visibleFaceIndices);
    }

    glPopMatrix();

    lastLeaf = currentLeaf;
}

void WorldRenderer::cleanup()
{
}

int WorldRenderer::findLeaf(
    const glm::vec3 &position,
    float offset)
{
    int i = this->_models[0].headnode[0];

    while (i >= 0)
    {
        const tBSPNode &node = this->_nodes[i];
        const tBSPPlane &plane = this->_planes[node.planeIndex];

        float distance;

        if (plane.type <= PLANE_Z) // easier for axial planes
        {
            distance = position[plane.type] - plane.distance - offset;
        }
        else
        {
            // f(x,y,z) = Ax+Ay+Az-D
            distance = plane.normal[0] * position[0] + plane.normal[1] * position[1] + plane.normal[2] * position[2] - plane.distance - offset;
        }

        if (distance >= 0)
        {
            i = node.children[0];
        }
        else
        {
            i = node.children[1];
        }
    }

    return -(i + 1);
}

bool WorldRenderer::testFaceVisibility(
    int faceIndex)
{
    const tFace &face = this->_staticRenderer.getFaces()[faceIndex];

    // Determine the distance to the face from the camera viewport
    float distance = glm::dot(_config._viewPoint, face.planeNormal) - face.planeDistance;

    // Skip the face when we see its back
    if (face.planeSide && distance > 0)
    {
        return false;
    }
    else if (!face.planeSide && distance < 0)
    {
        return false;
    }

    return StaticRenderer::testFaceVisibility(&_shaderManager, face);
}

glm::vec3 WorldRenderer::getPlayerStart() const
{
    return _entityManager.getPlayerStart();
}

WorldRenderer::Config::Config()
    : _frustum(nullptr)
{
    _viewPoint = glm::vec3(0.0f);
}

WorldRenderer::Config::~Config() = default;

void RenderBoundingBox(
    const short mins[],
    const short maxs[])
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
