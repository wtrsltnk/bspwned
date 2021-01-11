/* 
 * File:   bulletworldphysics.h
 * Author: wouter
 *
 * Created on May 28, 2009, 5:09 PM
 */

#ifndef _BULLETWORLDPHYSICS_H
#define	_BULLETWORLDPHYSICS_H

#include "interfaces.h"
#include <btBulletDynamicsCommon.h>

class BulletWorldPhysics : public IPhysics
{
public:
    BulletWorldPhysics();
    virtual ~BulletWorldPhysics();

    virtual void setLeafSpacesCount(int count);
    virtual void addToLeafSpace(int leafSpaceIndex, float* vertexData, int vertexCount, int* indexData, int indexCount);
    virtual void createPlane(const float plane[4]);
    virtual void* createSphere(float radius, float mass, const float position[3], const float force[3], const float ang[3]);

    virtual void getGeomPosition(void* geom, float position[3]);

    virtual void step(double time);

private:
    btDiscreteDynamicsWorld* dynamicsWorld;
    btSequentialImpulseConstraintSolver* solver;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btAxisSweep3* broadphase;

};

#endif	/* _BULLETWORLDPHYSICS_H */

