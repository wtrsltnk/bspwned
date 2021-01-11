/* 
 * File:   bsprenderer.cpp
 * Author: Wouter
 * 
 * Created on April 8, 2009, 8:28 PM
 */

#include "bsprenderer.h"
#ifdef _WIN32_
#include <windows.h>
#endif
#include <GL/gl.h>

BSPRenderer::BSPRenderer(BSPContent* content)
        : mContent(content)
{
}

BSPRenderer::~BSPRenderer()
{
}

void BSPRenderer::setup(const Configuration& config)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, this->mContent->mVertices);
    
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, this->mContent->mTexCoords);

    if (config.resources != NULL)
    {
        for (int i = 0; i < this->mContent->getTextureCount(); i++)
        {
            const tBSPTexture& texture = this->mContent->getTextures()[i];

            int renderIndex = config.resources->addTexture(texture);
            this->mContent->setTextureRenderIndex(i, renderIndex);
        }
//        for (int i = 0; i < this->mContent->getFaceCount(); i++)
//        {
//            const tBSPRenderFace& face = this->mContent->getRenderFaces()[i];
//
//            int renderIndex = config.resources->addTexture(*face.lightmap);
//            this->mContent->setLightmapRenderIndex(i, renderIndex);
//        }
    }
}

void BSPRenderer::cleanup()
{
}

void BSPRenderer::update(const Configuration& config)
{
    this->mFrameConfig = config;

    // Collect visible faces here
}

void BSPRenderer::render()
{
    const tBSPRenderFace* faces = this->mContent->getRenderFaces();

    glPushMatrix();
    glScalef(0.2f, 0.2f, 0.2f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    for (int i = 0; i < this->mContent->getFaceCount(); i++)
    {
		if (faces[i].texture->flags == 3)
			continue;
        glBindTexture(GL_TEXTURE_2D, faces[i].texture->renderIndex);
		glDrawArrays(faces[i].faceType, faces[i].firstVertexIndex, faces[i].vertexCount);
    }
    glPopMatrix();
}

BSPRenderer::Configuration::Configuration()
        : elapsedTime(0), enableLighting(true), enableTextures(true), enableSky(true), frustum(NULL), resources(NULL)
{
    this->viewPoint[0] = 0;
    this->viewPoint[1] = 0;
    this->viewPoint[2] = 0;
}

BSPRenderer::Configuration::~Configuration()
{
}
