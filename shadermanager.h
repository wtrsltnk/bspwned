/* 
 * File:   shadermanager.h
 * Author: wouter
 *
 * Created on May 13, 2009, 12:20 AM
 */

#ifndef _SHADERMANAGER_H
#define _SHADERMANAGER_H

#include "types.h"
#include <vector>

class Texture;

class ShaderManager
{
public:
    static ShaderManager *createSingleton();
    static ShaderManager *singleton();
    static void destroySingleton();

private:
    static ShaderManager *sSingleton;
    ShaderManager();

public:
    virtual ~ShaderManager();

    void SetTextureCount(
        int count);

    int GetTextureCount() const;

    Texture *GetTexture(
        int index) const;

    int AddShader(
        int textureIndex,
        int fxMode,
        float fxAmount,
        float fxColor[3]);

    void UseShader(
        int shaderIndex) const;

    const tShader *GetShader(
        int index) const;

private:
    int _textureCount;
    Texture *_textures;
    std::vector<tShader> _shaders;

    bool shaderEquals(
        const tShader &s1,
        const tShader &s2) const;
};

#endif /* _SHADERMANAGER_H */
