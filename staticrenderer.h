/* 
 * File:   staticrenderer.h
 * Author: wouter
 *
 * Created on April 16, 2009, 6:06 PM
 */

#ifndef _STATICRENDERER_H
#define _STATICRENDERER_H

#include "interfaces.h"
#include "types.h"

#include <array>
#include <vector>

class StaticRenderer
{
public:
    StaticRenderer();

    virtual ~StaticRenderer();

    void setupArrays();

    void setFaceCount(
        int count);

    int getFaceCount() const;

    void setShaderCount(
        int count);

    int getShaderCount() const;

    void addVertex(
        const tVertex &vertex);

    int getCurrentVertex() const;

    void setFace(
        const tFace &face,
        int index);

    const tFace &getFace(
        int index) const;

    void renderAllFaces();

    void renderVisibleFaces(
        bool visibleFaces[]);

    void renderFace(
        int face);

    void enableTextures(
        bool enable);

    void enableLightmaps(
        bool enable);

    void textureRenderSetup();

    static bool testFaceVisibility(
        const tFace &face);

private:
    std::vector<tVertex> mVertices;

    std::vector<tFace> mFaces;

    int mShaderCount = 0;

    bool mTexturesEnabled = false;
    bool mLightmapEnabled = false;

    void renderFace(
        const tFace *face);
};

#endif /* _STATICRENDERER_H */
