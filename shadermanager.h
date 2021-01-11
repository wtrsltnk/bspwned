/* 
 * File:   shadermanager.h
 * Author: wouter
 *
 * Created on May 13, 2009, 12:20 AM
 */

#ifndef _SHADERMANAGER_H
#define	_SHADERMANAGER_H

#include "types.h"

class Texture;

class ShaderManager
{
public:
    static ShaderManager* createSingleton();
    static ShaderManager* singleton();
    static void destroySingleton();

private:
    static ShaderManager* sSingleton;
    ShaderManager();
    
public:
    virtual ~ShaderManager();

    void setTextureCount(int count);
    int getTextureCount() const;
    Texture* getTexture(int index) const;

    int addShader(int textureIndex, int fxMode, float fxAmount, float fxColor[3]);
    void useShader(int shaderIndex) const;
    const tShader* getShader(int index) const;

private:
    class PIMPL;
    PIMPL* pimpl;


};

#endif	/* _SHADERMANAGER_H */

