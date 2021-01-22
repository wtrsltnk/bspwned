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

    void setShaderCount(
        int count);

    int getShaderCount() const;

    void setFaceAndVertexData(
        const std::vector<tFace> &faces,
        const std::vector<tVertex> &vertices);

    const std::vector<tFace> &getFaces() const;

    const std::vector<tVertex> &getVertices() const;

    void renderAllFaces(
        class ShaderManager *shaderManager);

    void renderVisibleFaces(
        ShaderManager *shaderManager,
        bool visibleFaces[]);

    void renderFace(
        ShaderManager *shaderManager,
        int face);

    void enableTextures(
        bool enable);

    void enableLightmaps(
        bool enable);

    void textureRenderSetup();

    static bool testFaceVisibility(
        ShaderManager *shaderManager,
        const tFace &face);

private:
    std::vector<tVertex> _vertices;

    std::vector<tFace> _faces;

    int _shaderCount = 0;

    bool _texturesEnabled = false;
    bool _lightmapEnabled = false;

    void renderFace(
        ShaderManager *shaderManager,
        const tFace *face);
};

#endif /* _STATICRENDERER_H */
