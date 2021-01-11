/* 
 * File:   bspcontent.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 8:28 PM
 */

#ifndef _BSPCONTENT_H
#define	_BSPCONTENT_H

#include "types.h"
#include "interfaces.h"
#include "indexarray.h"
#include <vector>
#include <string>

class BSPEntity;

class BSPContent
{
    friend class BSPReader;
	friend class BSPRenderer;
public:
    BSPContent();
    virtual ~BSPContent();

    int getEntityCount() const;
    const BSPEntity* getEntity(int index) const;
    const BSPEntity* findEntity(const char* name, int indexInResults = 0) const;

    int getTextureCount() const;
    const tBSPTexture* getTextures() const;
    void setTextureRenderIndex(int textureIndex, unsigned int renderIndex);

    int getNodeCount() const;
    const tBSPNode* getNodes() const;

    int getFaceCount() const;
    const tBSPRenderFace* getRenderFaces() const;
    void setLightmapRenderIndex(int faceIndex, unsigned int renderIndex);

    int getLightingDataSize() const;
    const unsigned char* getLightingData() const;

    int getClipNodeCount() const;
    const tBSPClipNode* getClipNodes() const;

    int getLeafCount() const;
    const tBSPLeaf* getLeafs() const;
    const tBSPIndexList* getPvs() const;

    int getMarkedSurfacesCount() const;
    const unsigned short* getMarkedSurfaces() const;

    int getEdgeCount() const;
    const tBSPEdge* getEdges() const;

    int getModelCount() const;
    const tBSPModel* getModels() const;

private:
    std::vector<BSPEntity*> mEntities;
	std::vector<std::string> mWadFiles;
	
    array_t<3> mVertices;
    array_t<2> mTexCoords;
    array_t<2> mLightTexCoords;

    int mTextureCount;
    tBSPTexture* mTextures;

    int mNodeCount;
    tBSPNode* mNodes;

    int mFaceCount;
    tBSPRenderFace* mRenderFaces;

    int mLightingDataSize;
    unsigned char* mLightingData;

    int mClipNodeCount;
    tBSPClipNode* mClipNodes;

    int mLeafCount;
    tBSPLeaf* mLeafs;
    tBSPIndexList* mPvs;

    int mMarkedSurfacesCount;
    unsigned short* mMarkedSurfaces;

    int mEdgeCount;
    tBSPEdge* mEdges;

    int mModelCount;
    tBSPModel* mModels;

};

#endif	/* _BSPCONTENT_H */

