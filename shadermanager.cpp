/*
 * File:   shadermanager.cpp
 * Author: wouter
 * 
 * Created on May 13, 2009, 12:20 AM
 */

#include "shadermanager.h"

#include "interfaces.h"
#include "opengl.h"
#include <stdlib.h>
#include <vector>

using namespace std;

typedef struct sShaderList
{
    int index;
    tShader shader;
    sShaderList *next;

} tShaderList;

ShaderManager *ShaderManager::createSingleton()
{
    if (ShaderManager::sSingleton == nullptr)
    {
        ShaderManager::sSingleton = new ShaderManager();
    }

    return ShaderManager::sSingleton;
}

ShaderManager *ShaderManager::singleton()
{
    return ShaderManager::sSingleton;
}

void ShaderManager::destroySingleton()
{
    if (ShaderManager::sSingleton != nullptr)
    {
        delete ShaderManager::sSingleton;
    }

    ShaderManager::sSingleton = nullptr;
}

ShaderManager *ShaderManager::sSingleton = nullptr;

ShaderManager::ShaderManager()
{
    _textureCount = 0;
    _textures = nullptr;
}

ShaderManager::~ShaderManager()
{
    if (_textures != nullptr)
    {
        delete[] _textures;
        _textures = nullptr;
    }
}

void ShaderManager::SetTextureCount(
    int count)
{
    _textureCount = count;

    if (_textures != nullptr)
    {
        // TODO: assert here, this can not happen when the SetTextureCount() is called only once (which should be the case)
        exit(1);
    }

    if (_textureCount > 0)
    {
        _textures = new Texture[_textureCount];
    }
}

int ShaderManager::GetTextureCount() const
{
    return _textureCount;
}

Texture *ShaderManager::GetTexture(
    int index) const
{
    if (_textures == nullptr)
    {
        // TODO: assert here, this method cannot be called when the texture (and shader) array is not initialized yet. This means the SetTextureCount() has not been called
        exit(1);
    }

    if (index >= 0 && index < _textureCount)
    {
        return &_textures[index];
    }

    return nullptr;
}

int ShaderManager::AddShader(
    int textureIndex,
    int fxMode,
    float fxAmount,
    float fxColor[3])
{
    // Return when the textureindex is not possible
    if (textureIndex < 0 || textureIndex >= _textureCount)
    {
        return -1;
    }

    // Create a shader structure
    tShader shader = {0, 0, {0, 0, 0}, 0, 0, 0, 0};
    shader.textureIndex = textureIndex;
    shader.fxAmount = fxAmount;
    for (int i = 0; i < 3; i++)
    {
        shader.fxColor[i] = fxColor[i];
    }
    shader.alphaTest = 0.8f;

    switch (fxMode)
    {
        case 3: // Glow blending
        case 5: // Additive blending
        {
            shader.flags = SHADER_FLAG_USETEXTURE | SHADER_FLAG_BLEND;
            shader.sourceBlend = GL_SRC_ALPHA;
            shader.destBlend = GL_ONE;
            break;
        }
        case 4: // Solid blending
        {
            shader.flags = SHADER_FLAG_USETEXTURE | SHADER_FLAG_BLEND;
            shader.sourceBlend = GL_SRC_ALPHA;
            shader.destBlend = GL_ONE_MINUS_SRC_ALPHA;
            break;
        }
        case 0: // Normal blending
        case 2: // Texture blending
        {
            shader.flags = SHADER_FLAG_USETEXTURE | SHADER_FLAG_BLEND;
            shader.sourceBlend = GL_SRC_ALPHA;
            shader.destBlend = GL_ONE;
            break;
        }
    }

    if (!_textures[textureIndex].name.empty())
    {
        std::string tName = _textures[textureIndex].name.c_str();

        // Check for transparent textures
        if (tName[0] == '{' || shader.fxAmount < 1.0f)
        {
            shader.flags |= SHADER_FLAG_TRANSPARENTTEXTURE;
        }
        // Check for water textures
        if (tName[0] == '!')
        {
            shader.flags |= SHADER_FLAG_WATERTEXTURE;
        }
        if (tName == "{blue" || tName == "sky" || tName == "aaatrigger" || tName == "clip" || tName == "skip")
        {
            shader.flags |= SHADER_FLAG_SPECIALTEXTURE;
        }
    }

    // First check if there is a shader with the same properties, return the index when this is true
    for (size_t i = 0; i < _shaders.size(); i++)
    {
        if (shaderEquals(_shaders[i], shader))
        {
            return i;
        }
    }

    // Add the shader to the vector
    _shaders.push_back(shader);

    // Return the index of the new shader
    return _shaders.size() - 1;
}

void ShaderManager::UseShader(
    int shaderIndex) const
{
    tShader shader = _shaders[shaderIndex];

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Texture settings
    glActiveTexture(GL_TEXTURE0);
    if (shader.flags & SHADER_FLAG_USETEXTURE)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _textures[shader.textureIndex].glIndex);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    // Alpha testing
    glAlphaFunc(GL_GEQUAL, shader.alphaTest);
    if (shader.flags & SHADER_FLAG_TRANSPARENTTEXTURE)
    {
        glEnable(GL_ALPHA_TEST);
    }
    else
    {
        glDisable(GL_ALPHA_TEST);
    }

    // Blending
    glBlendFunc(shader.sourceBlend, shader.destBlend);
    if (shader.flags & SHADER_FLAG_BLEND)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    glColor4f(shader.fxColor[0], shader.fxColor[1], shader.fxColor[2], shader.fxAmount);
}

const tShader *ShaderManager::GetShader(
    int index) const
{
    return &_shaders[index];
}

bool ShaderManager::shaderEquals(
    const tShader &s1,
    const tShader &s2) const
{
    if (s1.textureIndex != s2.textureIndex) return false;
    if (s1.fxAmount != s2.fxAmount) return false;
    for (int i = 0; i < 3; i++)
    {
        if (s1.fxColor[i] != s2.fxColor[i]) return false;
    }
    if (s1.flags != s2.flags) return false;
    if (s1.sourceBlend != s2.sourceBlend) return false;
    if (s1.destBlend != s2.destBlend) return false;
    if (s1.alphaTest != s2.alphaTest) return false;

    return true;
}
