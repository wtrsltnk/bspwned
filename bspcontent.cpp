/* 
 * File:   bspcontent.cpp
 * Author: Wouter
 * 
 * Created on April 8, 2009, 8:28 PM
 */

#include "bspcontent.h"
#include "bspentity.h"
#include <string.h>

BSPContent::BSPContent()
{
    this->mNodeCount = 0;
    this->mNodes = NULL;

    this->mFaceCount = 0;
    this->mRenderFaces = NULL;

    this->mLightingDataSize = 0;
    this->mLightingData = NULL;

    this->mClipNodeCount = 0;
    this->mClipNodes = NULL;

    this->mLeafCount = 0;
    this->mLeafs = NULL;
    this->mPvs = NULL;

    this->mMarkedSurfacesCount = 0;
    this->mMarkedSurfaces = NULL;

    this->mEdgeCount = 0;
    this->mEdges = NULL;

    this->mModelCount = 0;
    this->mModels = NULL;
}

BSPContent::~BSPContent()
{
    while (!this->mEntities.empty())
    {
        delete this->mEntities.back();
        this->mEntities.pop_back();
    }

    if (mNodes != NULL)
        delete [] mNodes;

    if (mRenderFaces != NULL)
        delete [] mRenderFaces;

    if (mLightingData != NULL)
        delete [] mLightingData;

    if (mClipNodes != NULL)
        delete [] mClipNodes;

    if (mLeafs != NULL)
        delete [] mLeafs;

    for (int i = 0; i < this->mLeafCount; i++)
        delete []this->mPvs[i].indices;
    delete []this->mPvs;
    
    if (mMarkedSurfaces != NULL)
        delete [] mMarkedSurfaces;

    if (mEdges != NULL)
        delete [] mEdges;

    if (mModels != NULL)
        delete [] mModels;
}

int BSPContent::getEntityCount() const
{
    return int(this->mEntities.size());
}

const BSPEntity* BSPContent::getEntity(int index) const
{
    return this->mEntities[index];
}

const BSPEntity* BSPContent::findEntity(const char* name, int indexInResults) const
{
	for (int i = 0; i < this->mEdgeCount; i++)
	{
		if (strcmp(this->mEntities[i]->getClassName(), name) == 0)
		{
			if (indexInResults <= 0)
				return this->mEntities[i];
			else
				indexInResults--;
		}
	}
	return NULL;
}


int BSPContent::getTextureCount() const
{
    return this->mTextureCount;
}

const tBSPTexture* BSPContent::getTextures() const
{
    return this->mTextures;
}

void BSPContent::setTextureRenderIndex(int textureIndex, unsigned int renderIndex)
{
    if (textureIndex < 0 || textureIndex >= this->mTextureCount)
        return;
    
    this->mTextures[textureIndex].renderIndex = renderIndex;
}


int BSPContent::getNodeCount() const
{
    return this->mNodeCount;
}

const tBSPNode* BSPContent::getNodes() const
{
    return this->mNodes;
}


int BSPContent::getFaceCount() const
{
    return this->mFaceCount;
}

const tBSPRenderFace* BSPContent::getRenderFaces() const
{
    return this->mRenderFaces;
}

void BSPContent::setLightmapRenderIndex(int faceIndex, unsigned int renderIndex)
{
    if (faceIndex < 0 || faceIndex >= this->mFaceCount)
        return;

    if (this->mRenderFaces[faceIndex].lightmap != NULL)
        this->mRenderFaces[faceIndex].lightmap->renderIndex = renderIndex;
}


int BSPContent::getLightingDataSize() const
{
    return this->mLightingDataSize;
}

const unsigned char* BSPContent::getLightingData() const
{
    return this->mLightingData;
}


int BSPContent::getClipNodeCount() const
{
    return this->mClipNodeCount;
}

const tBSPClipNode* BSPContent::getClipNodes() const
{
    return this->mClipNodes;
}


int BSPContent::getLeafCount() const
{
    return this->mLeafCount;
}

const tBSPLeaf* BSPContent::getLeafs() const
{
    return this->mLeafs;
}

const tBSPIndexList* BSPContent::getPvs() const
{
    return this->mPvs;
}


int BSPContent::getMarkedSurfacesCount() const
{
    return this->mMarkedSurfacesCount;
}

const unsigned short* BSPContent::getMarkedSurfaces() const
{
    return this->mMarkedSurfaces;
}


int BSPContent::getEdgeCount() const
{
    return this->mEdgeCount;
}

const tBSPEdge* BSPContent::getEdges() const
{
    return this->mEdges;
}


int BSPContent::getModelCount() const
{
    return this->mModelCount;
}

const tBSPModel* BSPContent::getModels() const
{
    return this->mModels;
}
