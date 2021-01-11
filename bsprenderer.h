/* 
 * File:   bsprenderer.h
 * Author: Wouter
 *
 * Created on April 8, 2009, 8:28 PM
 */

#ifndef _BSPRENDERER_H
#define	_BSPRENDERER_H

#include "interfaces.h"
#include "bspcontent.h"

class BSPRenderer
{
public:
    class Configuration
    {
    public:
        Configuration();
        virtual ~Configuration();

        float elapsedTime;
        bool enableLighting;
        bool enableTextures;
        bool enableSky;

        float viewPoint[3];
        IFrustum* frustum;

        IResources* resources;

    };
    
public:
    BSPRenderer(BSPContent* content);
    virtual ~BSPRenderer();

    void setup(const Configuration& config);
    void cleanup();
    
    void update(const Configuration& config);
    void render();

private:
    BSPContent* mContent;
    Configuration mFrameConfig;

};

#endif	/* _BSPRENDERER_H */

