/* 
 * File:   odeworldphysics.h
 * Author: wouter
 *
 * Created on May 26, 2009, 9:56 AM
 */

#ifndef _ODEWORLDPHYSICS_H
#define	_ODEWORLDPHYSICS_H

#include "interfaces.h"
#include <ode/ode.h>
#include <vector>

class OdeWorldPhysics : public IPhysics
{
    friend class WorldLoader;
    
public:
    OdeWorldPhysics();
    virtual ~OdeWorldPhysics();

    void setLeafSpacesCount(int count);
    void addToLeafSpace(int leafSpaceIndex, float* vertexData, int vertexCount, int* indexData, int indexCount);
    virtual void createPlane(const float plane[4]);
    void* createSphere(float radius, float mass, const float position[3], const float force[3], const float ang[3]);

    virtual void getGeomPosition(void* geom, float position[3]);

    void step(double time);

private:
    dWorldID mWorld;
    dSpaceID mGlobalSpace;
    int mLeafSpacesCount;
    dSpaceID* mLeafSpaces;
    std::vector<dTriMeshDataID> mTriMeshes;
    dGeomID mBall;
    dReal mStepSize;
    dJointGroupID mContactGroup;

    static void dNearCallback (void *data, dGeomID o1, dGeomID o2);
};

#endif	/* _WORLDPHYSICS_H */

