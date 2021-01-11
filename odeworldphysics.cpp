/* 
 * File:   odeworldphysics.cpp
 * Author: wouter
 * 
 * Created on May 26, 2009, 9:56 AM
 */

#include "odeworldphysics.h"
#include <iostream>

using namespace std;

OdeWorldPhysics::OdeWorldPhysics()
        : mLeafSpacesCount(0), mLeafSpaces(NULL), mBall(NULL), mStepSize(1.0f / 50.0f)
{
    dInitODE();
    this->mWorld = dWorldCreate();
    dWorldSetGravity (this->mWorld, 0, -9.81f, 0);
    this->mGlobalSpace = dHashSpaceCreate(0);
}

OdeWorldPhysics::~OdeWorldPhysics()
{
    for (int i = 0; i < this->mLeafSpacesCount; i++)
        if (this->mLeafSpaces[i] != NULL)
            dSpaceDestroy(this->mLeafSpaces[i]);
    
    if (this->mLeafSpaces != NULL)
        delete []this->mLeafSpaces;

    for (std::vector<dTriMeshDataID>::iterator itr = this->mTriMeshes.begin(); itr != this->mTriMeshes.end(); ++itr)
        dGeomTriMeshDataDestroy(*itr);
    
    dSpaceDestroy(this->mGlobalSpace);
    dWorldDestroy(this->mWorld);
    dCloseODE();
}

void OdeWorldPhysics::setLeafSpacesCount(int count)
{
    if (this->mLeafSpacesCount == 0)
    {
        this->mLeafSpacesCount = count;
        this->mLeafSpaces = new dSpaceID[this->mLeafSpacesCount];
        for (int i = 0; i < this->mLeafSpacesCount; i++)
            this->mLeafSpaces[i] = NULL;
    }
}

void OdeWorldPhysics::addToLeafSpace(int leafSpaceIndex, float* vertexData, int vertexCount, int* indexData, int indexCount)
{
    if (leafSpaceIndex >= 0 && leafSpaceIndex < this->mLeafSpacesCount)
    {
        dTriMeshDataID tri = dGeomTriMeshDataCreate();
        this->mTriMeshes.push_back(tri);

        dGeomTriMeshDataBuildSingle(tri, vertexData, sizeof(float) * 3, vertexCount, indexData, indexCount, sizeof(int) * 3);
        dCreateTriMesh(this->mGlobalSpace, tri, NULL, NULL, NULL);

    }
}

void OdeWorldPhysics::createPlane(const float plane[4])
{
    dCreatePlane(this->mGlobalSpace, plane[0], plane[1], plane[2], plane[3]);
}

void* OdeWorldPhysics::createSphere(float radius, float mass, const float position[3], const float force[3], const float ang[3])
{
    dMass odeMass;
    if (mBall != NULL)
        dGeomDestroy(mBall);
    
    mBall = dCreateSphere(this->mGlobalSpace, radius);
    dBodyID body = dBodyCreate(this->mWorld);
    dGeomSetBody(mBall, body);
    
    float density = mass / (4.0f * M_PI * pow(radius, 3.f) / 3.0f);
    dMassSetSphere(&odeMass, density, radius);
    dBodySetMass(body, &odeMass);
    dBodySetLinearVel(body, force[0], force[1], force[2]);
    dBodySetAngularVel(body, ang[0], ang[1], ang[2]);
    dGeomSetPosition(mBall, position[0], position[1], position[2]);

    return mBall;
}

void OdeWorldPhysics::step(double time)
{
    static double lastTime = 0;

    if ((time - lastTime) > (this->mStepSize))
    {
        this->mContactGroup = dJointGroupCreate(0);

        for (int i = 0; i < 10; i++)
        {
            dSpaceCollide(this->mGlobalSpace, (void*)this, OdeWorldPhysics::dNearCallback);
            dWorldQuickStep(this->mWorld, this->mStepSize);
        }
        dJointGroupDestroy(this->mContactGroup);

        lastTime = time;
    }
}

#define CONTACT_POINTS 1

void OdeWorldPhysics::dNearCallback(void *data, dGeomID o1, dGeomID o2)
{
    static int i = 0;
    OdeWorldPhysics* thiz = (OdeWorldPhysics*)data;
    if (!dGeomIsSpace(o1) && !dGeomIsSpace(o2))
    {
        dBodyID body1 = dGeomGetBody(o1);
        dBodyID body2 = dGeomGetBody(o2);

        dContact contact[CONTACT_POINTS];

        i++;
        int collisions = dCollide(o1, o2, CONTACT_POINTS, &contact[0].geom, sizeof(dContact));
        if (collisions) {
            for (int i = 0; i < collisions; ++i) {
                contact[i].surface.mode = dContactBounce | dContactSoftCFM | dContactSlip1 | dContactSlip2;

                contact[i].surface.bounce = 0.5f;
                contact[i].surface.soft_cfm = 0.f;

                contact[i].surface.mu = 1.0f;
                contact[i].surface.slip1 = 0;
                contact[i].surface.slip2 = 0;

                dJointID c = dJointCreateContact(thiz->mWorld, thiz->mContactGroup, &contact[i]);
                dJointAttach(c, body1, body2);
            }
        }
    }
}

void OdeWorldPhysics::getGeomPosition(void* geom, float position[3])
{

    const dReal* pos = dGeomGetPosition((dGeomID)geom);
    for (int i = 0; i < 3; i++)
        position[i] = pos[i];
}
